//
//  society.h
//  Society
//
//  Created by Arthur Sun on 10/12/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef society_h
#define society_h

#include "common.h"
#include "creature.h"

struct proxy {
    size_t hash;
    obj* obj;
    
    proxy(struct obj* obj) : obj(obj) {}
};

struct sight_segment {
    size_t lower_bound;
    size_t upper_bound;
    creature* q;
    int i;
};

struct society {
    std::vector<creature*> C;
    
    std::vector<item*> I;
    
    std::vector<proxy*> P;
    std::vector<sight_segment*> S;
    
    ~society() {
        for(proxy* p : P)
            delete p;
        
        for(creature* q : C)
            delete q;
        
        for(item* q : I)
            delete q;
        
        for(sight_segment* q : S)
            delete q;
    }
    
    void add(creature* x) {
        C.push_back(x);
        P.push_back(new proxy(C.back()));
    }
    
    void add(item* x) {
        I.push_back(x);
        P.push_back(new proxy(I.back()));
    }
    
    void solve(obj* a, obj* b, float dt);
    
    void step(float dt);
};

#endif /* society_h */
