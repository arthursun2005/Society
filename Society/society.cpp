//
//  society.cpp
//  Society
//
//  Created by Arthur Sun on 10/13/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"

vec2 sight_left = vec2(cosf(sight_half_angle), -sinf(sight_half_angle));

double sight_step_angle = 2.0 * sight_half_angle / (double) (sight_lines - 1);

vec2 sight_step = vec2(cosf(sight_step_angle), sinf(sight_step_angle));

double initial_states[2] = {60.0, 90000.0};

double raycast(const vec2& p, const vec2& n, const vec2& q, double r) {
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
    double b = 2.0 * dot(n, p - q);
    double c = dot(q, q) + dot(p, p - 2.0 * q) - r * r;
    double e = b * b - 4.0 * c;
    double d1 = (-b + sqrtf(e)) / 2.0;
    double d2 = (-b - sqrtf(e)) / 2.0;
    
    if(d1 < 0.0 && d2 < 0.0) return FLT_MAX;
    
    if(d1 < 0.0) {
        return d2;
    }else if(d2 < 0.0) {
        return d1;
    }else{
        return fminf(d1, d2);
    }
}

void society::step(double dt, void (callback)(obj*)) {
    std::vector<obj*> new_objs;
    
    {
        std::vector<proxy*>::iterator i = P.end();
        std::vector<proxy*>::iterator begin = P.begin();
        while(i-- != begin) {
            if((*i)->obj->dead) {
                callback((*i)->obj);
                delete (*i)->obj;
                P.erase(i);
                continue;
            }
            (*i)->hash = (*i)->obj->position.hash();
        }
    }
    
    for(obj* q : O) {
        if(q->type == obj_creature) {
            ne_node** inputs = ((creature*)q)->brain.inputs();
            for(int i = 0; i != sight_lines; ++i) {
                inputs[inputs_before_sight + 4 * i]->value = sight_limit;
                inputs[inputs_before_sight + 1 + 4 * i]->value = 0.0;
                inputs[inputs_before_sight + 2 + 4 * i]->value = 0.0;
                inputs[inputs_before_sight + 3 + 4 * i]->value = 0.0;
            }
        }
    }
    
    std::sort(P.begin(), P.end(), [] (const proxy* a, const proxy* b) {
        return a->hash < b->hash;
    });
    
    size_t si = 0;
    
    for(proxy* p : P) {
        if(p->obj->type == obj_creature) {
            creature* q = (creature*)p->obj;
            vec2 n = mul(q->dir, sight_left);
            double bx = q->position.x;
            double ex = q->position.x;
            double by = q->position.y;
            double ey = q->position.y;
            for(int k = 0; k != sight_lines; ++k) {
                vec2 e = q->position + n * sight_limit;
                bx = fminf(bx, e.x);
                ex = fmaxf(ex, e.x);
                by = fminf(by, e.y);
                ey = fmaxf(ey, e.y);
                n = mul(n, sight_step);
            }
            bx -= diameter_limit;
            ex += diameter_limit;
            by -= diameter_limit;
            ey += diameter_limit;
            while(by < ey) {
                sight_segment* ss;
                if(si == S.size()) {
                    ss = new sight_segment();
                    S.push_back(ss);
                }else{
                    ss = S[si];
                }
                ss->lower_bound = vec2(bx, by).hash();
                ss->upper_bound = vec2(ex, by).hash();
                ss->q = q;
                by += diameter_limit;
                ++si;
            }
        }
    }
    
    std::sort(S.begin(), S.begin() + si, [] (const sight_segment* a, const sight_segment* b) {
        return a->lower_bound < b->lower_bound;
    });
    
    {
        std::vector<sight_segment*>::iterator i = S.begin();
        std::vector<sight_segment*>::iterator end = S.begin() + si;
        std::vector<sight_segment*>::iterator n;
        
        std::vector<proxy*>::iterator j = P.begin();
        std::vector<proxy*>::iterator e = P.end();
        std::vector<proxy*>::iterator k = j;
        
        ne_node** inputs;
        while(i != end) {
            inputs = (*i)->q->brain.inputs();
            j = k;
            while(j != e && (*j)->hash < (*i)->lower_bound) {
                ++j;
            }
            k = j;
            
            n = i;
            ++n;
            
            while(j != e && (*j)->hash <= (*i)->upper_bound) {
                if((*j)->obj != (*i)->q) {
                    for(int k = 0; k != sight_lines; ++k) {
                        double a = k * sight_step_angle;
                        vec2 p = mul(mul((*i)->q->dir, sight_left), vec2(cosf(a), sinf(a)));
                        double d = raycast((*i)->q->position, p, (*j)->obj->position, (*j)->obj->radius);
                        if(d < inputs[inputs_before_sight + 4 * k]->value) {
                            inputs[inputs_before_sight + 4 * k]->value = d;
                            inputs[inputs_before_sight + 1 + 4 * k]->value = (*j)->obj->color.r;
                            inputs[inputs_before_sight + 2 + 4 * k]->value = (*j)->obj->color.g;
                            inputs[inputs_before_sight + 3 + 4 * k]->value = (*j)->obj->color.b;
                        }
                    }
                }
                ++j;
                
                if(n != end && k != e && (*k)->hash < (*n)->lower_bound)
                    ++k;
            }
            ++i;
        }
    }
    
    for(obj* q : O) {
        if(q->type == obj_creature) {
            ne_node** inputs = ((creature*)q)->brain.inputs();
            ne_node** outputs = ((creature*)q)->brain.outputs();
            
            inputs[0]->value = 1.0;
            inputs[1]->value = ((creature*)q)->states[0];
            inputs[2]->value = ((creature*)q)->states[1];
            
            ((creature*)q)->brain.activate();
            
            vec2 v = creature_acceleration * vec2(outputs[0]->value, outputs[1]->value);
            double v2 = dot(v, v);
            
            if(v2 > creature_acceleration * creature_acceleration)
                v *= sqrtf(creature_acceleration/v2);
            
            q->velocity += v * dt;
            
            double a = outputs[2]->value;
            
            if(a < -creature_rotation)
                a = -creature_rotation;
            else if(a > creature_rotation)
                a = creature_rotation;
            
            a *= dt;
            q->dir = mul(q->dir, vec2(cosf(a), sinf(a)));
            
            ((creature*)q)->states[0] -= dt;
            ((creature*)q)->states[1] -= dt;
            ((creature*)q)->age += dt;
            if(((creature*)q)->age > creature_lifespan || ((creature*)q)->states[0] < 1.0 || ((creature*)q)->states[1] < 1.0) {
                q->dead = true;
            }else{
                if(outputs[4]->value > 0.0) {
                    creature* n = new creature(*(creature*)q);
                    n->position = q->position + 2.0 * q->dir * q->radius;
                    n->radius = q->radius;
                    n->density = 1.0;
                    n->randomlize_name();
                    n->randomlize_color();
                    n->velocity = q->dir;
                    n->dir = q->dir;
                    n->states[0] = ((creature*)q)->states[0] * 0.5;
                    n->states[1] = ((creature*)q)->states[1] * 0.5;
                    std::copy_n(n->states, 2, ((creature*)q)->states);
                    new_objs.push_back(n);
                }
            }
        }else if(q->type == obj_source) {
            ((source*)q)->time += dt;
            double a = dt * ((source*)q)->rotation;
            q->dir = mul(q->dir, vec2(cosf(a), sinf(a)));
            if(((source*)q)->time >= ((source*)q)->period) {
                food* n = new food();
                n->position = q->position + 2.0 * q->dir * q->radius;
                n->radius = q->radius;
                n->density = 1.0;
                n->name = "food";
                n->velocity = q->dir;
                new_objs.push_back(n);
                ((source*)q)->time -= ((source*)q)->period;
            }
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
                solve((*i)->obj, (*j)->obj);
                ++j;
            }
            
            while(k != end && (*k)->hash < h2) {
                ++k;
            }
            
            j = k;
            while(j != end && (*j)->hash <= h3) {
                solve((*i)->obj, (*j)->obj);
                ++j;
            }
            
            ++i;
        }
    }
    
    {
        std::vector<obj*>::iterator i = O.end();
        std::vector<obj*>::iterator begin = O.begin();
        while(i-- != begin) {
            if((*i)->dead) {
                O.erase(i);
                continue;
            }
            (*i)->position += dt * (*i)->velocity;
        }
    }
    
    for(obj* q : new_objs) {
        add(q);
    }
}

void society::solve(obj *a, obj *b) {
    if(a->dead || b->dead) return;
    
    vec2 d = b->position - a->position;
    double l = dot(d, d);
    double radius = a->radius + b->radius;
    if(l < radius * radius) {
        double w = sqrtf(l);
        d /= w;
        double m1 = a->radius * a->radius * a->density;
        double m2 = b->radius * b->radius * b->density;
        double inv_sum = 1.0 / (m1 + m2);
        double q = dot(a->velocity - b->velocity, d);
        if(q > 0.0) {
            vec2 impulse = (1.0 + restitution) * q * m1 * m2 * inv_sum * d;
            a->velocity -= impulse / m1;
            b->velocity += impulse / m2;
        }
        
        if(b->type == obj_food && a->type == obj_creature) {
            if(((creature*)a)->brain.outputs()[3]->value > 0.0) {
                ((creature*)a)->states[0] += food_amount;
                b->dead = true;
            }
        }else if(a->type == obj_food && b->type == obj_creature) {
            if(((creature*)b)->brain.outputs()[3]->value > 0.0) {
                ((creature*)b)->states[0] += food_amount;
                a->dead = true;
            }
        }
    }
}
