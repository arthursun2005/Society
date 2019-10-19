//
//  society.cpp
//  Society
//
//  Created by Arthur Sun on 10/13/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"

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
     */
    float a = dot(n, n);
    float b = 2.0f * dot(n, p - q);
    float c = dot(q, q) + dot(p, p - 2.0f * q) - r * r;
    float e = b * b - 4.0f * a * c;
    float i = 2.0f * a;
    float d1 = (-b + sqrtf(e)) / i;
    float d2 = (-b - sqrtf(e)) / i;
    
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
    for(proxy* p : set.proxies) {
        p->hash = p->obj->position.hash();
    }
    
    set.sort();
    
    std::vector<proxy*>::iterator i = set.proxies.begin();
    std::vector<proxy*>::iterator end = set.proxies.end();
    
    std::vector<proxy*>::iterator j;
    std::vector<proxy*>::iterator k = i;
    
    
    size_t h1, h2, h3;
    
    while(i != end) {
        h1 = (*i)->hash + 1;
        h2 = (*i)->hash + (1lu << hash_half_bits) - 1;
        h3 = h2 + 2;
        
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
    
    for(creature* q : C) {
        q->position += dt * q->velocity;
    }
    
    for(item* q : I) {
        q->position += dt * q->velocity;
    }
}

void society::solve(obj *a, obj *b) {
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
        float f = inv_sum * (radius - w);
        a->velocity -= m2 * f * d;
        b->velocity += m1 * f * d;
    }
}
