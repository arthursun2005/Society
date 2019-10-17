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
    
    while(i != end) {
        size_t h1 = (*i)->hash + 1;
        size_t h2 = (*i)->hash + (1lu << hash_half_bits) - 1;
        size_t h3 = h2 + 2;
        
        j = i + 1;
        while(j != end && (*i)->hash <= h1) {
            solve((*i)->obj, (*j)->obj);
            ++j;
        }
        
        while(k != end && (*k)->hash < h2) {
            ++k;
        }
        
        j = k;
        while(j != end && (*j)->hash <= h3) {
            solve((*i)->obj, (*j)->obj);
        }
        
        ++i;
    }
}

void society::solve(obj *a, obj *b) {
}
