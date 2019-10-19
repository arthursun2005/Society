//
//  society.cpp
//  Society
//
//  Created by Arthur Sun on 10/13/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "society.h"

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
