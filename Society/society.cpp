//
//  society.cpp
//  Society
//
//  Created by Arthur Sun on 10/13/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"

vec2 sight_left = vec2(cosf(sight_half_angle), -sinf(sight_half_angle));

float sight_step_angle = 2.0f * sight_half_angle / (float) (sight_lines - 1);

vec2 sight_step = vec2(cosf(sight_step_angle), sinf(sight_step_angle));

float raycast(const vec2& p, const vec2& n, const vec2& q, float r) {
    /**
     circle: (x - q.x)^2 + (y - q.y)^2 = r^2
     line: (p.x + n.x * d, p.y + n.y * d) (d <= m)
     intersection: (p.x + n.x * d - q.x)^2 + (p.y + n.y * d - q.y)^2 = r^2 =
     p.x * (p.x - 2 * q.x) + 2 * n.x * d * (p.x - q.x) + n.x * d * n.x * d + q.x * q.x + ['.x -> .y']
     .
     .
     .
     2 * dot(n, p - q) * d + dot(n, n) * d * d = r^2 - dot(q, q) - dot(p, p - 2 * q)
     quadratic formula ad^2 + bd + c = 0
     with:
     a = dot(n, n)
     b = 2 * dot(n, p - q)
     c = dot(q, q) + dot(p, p - 2 * q) - r^2
     take the smallest non-negative (>=0) value of d
     
     dot(n, n) = 1 (unit circle vector definition)
     */
    float b = 2.0f * dot(n, p - q);
    float c = dot(q, q) + dot(p, p - 2.0f * q) - r * r;
    float e = b * b - 4.0f * c;
    float d1 = (-b + sqrtf(e)) / 2.0f;
    float d2 = (-b - sqrtf(e)) / 2.0f;
    
    if(d1 < 0.0f && d2 < 0.0f) return FLT_MAX;
    
    if(d1 < 0.0f) {
        return d2;
    }else if(d2 < 0.0f) {
        return d1;
    }else{
        return fminf(d1, d2);
    }
}

void society::step(float dt) {
    for(proxy* p : P) {
        p->hash = p->obj->position.hash();
    }
    
    for(creature* q : C) {
        ne_node** inputs = q->brain.inputs();
        for(int i = 0; i != sight_lines; ++i) {
            inputs[1 + 4 * i]->value = sight_limit;
            inputs[2 + 4 * i]->value = 0.0f;
            inputs[3 + 4 * i]->value = 0.0f;
            inputs[4 + 4 * i]->value = 0.0f;
        }
    }
    
    std::sort(P.begin(), P.end(), [] (const proxy* a, const proxy* b) {
        return a->hash < b->hash;
    });
    
    for(sight_segment* q : S)
        delete q;
    
    S.clear();
    
    for(proxy* p : P) {
        if(p->obj->type == obj_creature) {
            creature* q = (creature*)p->obj;
            vec2 n = mul(q->dir, sight_left);
            for(int i = 0; i != sight_lines; ++i) {
                vec2 p = q->position + n * sight_limit;
                float bx = fminf(q->position.x, p.x) - diameter_limit;
                float ex = fmaxf(q->position.x, p.x) + diameter_limit;
                float by = fminf(q->position.y, p.y) - diameter_limit;
                float ey = fmaxf(q->position.y, p.y) + diameter_limit;
                while(by < ey) {
                    sight_segment* ss = new sight_segment();
                    ss->lower_bound = vec2(bx, by).hash();
                    ss->upper_bound = vec2(ex, by).hash();
                    ss->q = q;
                    ss->i = i;
                    S.push_back(ss);
                    by += diameter_limit;
                }
                n = mul(n, sight_step);
            }
        }
    }
    
    std::sort(S.begin(), S.end(), [] (const sight_segment* a, const sight_segment* b) {
        return a->lower_bound < b->lower_bound;
    });
    
    {
        std::vector<sight_segment*>::iterator i = S.begin();
        std::vector<sight_segment*>::iterator end = S.end();
        
        std::vector<proxy*>::iterator j = P.begin();
        std::vector<proxy*>::iterator e = P.end();
        std::vector<proxy*>::iterator k = j;
        
        ne_node** inputs;
        while(i != end) {
            float a = (*i)->i * sight_step_angle;
            vec2 p = mul(mul((*i)->q->dir, sight_left), vec2(cosf(a), sinf(a)));
            inputs = (*i)->q->brain.inputs();
            j = k;
            while(j != e && (*j)->hash < (*i)->lower_bound) {
                ++j;
            }
            k = j;
            while(j != e && (*j)->hash <= (*i)->upper_bound) {
                if((*j)->obj != (*i)->q) {
                    float d = raycast((*i)->q->position, p, (*j)->obj->position, (*j)->obj->radius);
                    if(d < inputs[1 + 4 * (*i)->i]->value) {
                        inputs[1 + 4 * (*i)->i]->value = d;
                        inputs[2 + 4 * (*i)->i]->value = (*j)->obj->color.r;
                        inputs[3 + 4 * (*i)->i]->value = (*j)->obj->color.g;
                        inputs[4 + 4 * (*i)->i]->value = (*j)->obj->color.b;
                    }
                }
                ++j;
            }
            ++i;
        }
    }
    
    {
        std::vector<proxy*>::iterator i = P.begin();
        std::vector<proxy*>::iterator end = P.end();
        
        std::vector<proxy*>::iterator j;
        std::vector<proxy*>::iterator k = i;
        
        size_t h1, h2, h3;
        
        while(i != end) {
            h1 = (*i)->hash + hash_x_step;
            h2 = (*i)->hash + hash_y_step - hash_x_step;
            h3 = h2 + 2 * hash_x_step;
            
            j = i + 1;
            while(j != end && (*j)->hash <= h1) {
                solve((*i)->obj, (*j)->obj, dt);
                ++j;
            }
            
            while(k != end && (*k)->hash < h2) {
                ++k;
            }
            
            j = k;
            while(j != end && (*j)->hash <= h3) {
                solve((*i)->obj, (*j)->obj, dt);
                ++j;
            }
            
            ++i;
        }
    }
    
    for(creature* q : C) {
        q->position += dt * q->velocity;
    }
    
    for(item* q : I) {
        q->position += dt * q->velocity;
    }
}

void society::solve(obj *a, obj *b, float dt) {
    vec2 d = b->position - a->position;
    float l = dot(d, d);
    float radius = a->radius + b->radius;
    if(l < radius * radius) {
        float w = sqrtf(l);
        d /= w;
        float m1 = a->radius * a->radius * a->density;
        float m2 = b->radius * b->radius * b->density;
        float inv_sum = 1.0f / (m1 + m2);
        float q = dot(a->velocity - b->velocity, d);
        if(q > 0.0f) {
            vec2 impulse = (1.0f + restitution) * q * m1 * m2 * inv_sum * d;
            a->velocity -= impulse / m1;
            b->velocity += impulse / m2;
        }
        float f = inv_sum * (radius - w) / dt;
        a->velocity -= m2 * f * d;
        b->velocity += m1 * f * d;
    }
}
