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
#include "items/source.h"
#include "items/wall.h"

struct proxy {
    size_t hash;
    obj* obj;
    
    proxy(struct obj* obj) : obj(obj) {}
};

struct sight_segment {
    size_t lower_bound;
    size_t upper_bound;
    creature* q;
};

struct society {
    std::vector<obj*> O;
    
    std::vector<proxy*> P;
    std::vector<sight_segment*> S;
    
    ~society() {
        for(proxy* p : P)
            delete p;
        
        for(obj* q : O)
            delete q;
        
        for(sight_segment* q : S)
            delete q;
    }
    
    void add(obj* x) {
        O.push_back(x);
        P.push_back(new proxy(O.back()));
    }
    
    void solve(obj* a, obj* b);
    
    void step(double dt, void (callback)(obj*));
};

#endif /* society_h */
