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
#include "proxy.h"

struct society {
    std::vector<creature*> C;
    
    std::vector<item*> I;
    
    proxy_set set;
    
    ~society() {
        for(creature* q : C)
            delete q;
        
        for(item* q : I)
            delete q;
    }
    
    void add(creature* x) {
        C.push_back(x);
        set.add(new proxy(C.back()));
    }
    
    void add(item* x) {
        I.push_back(x);
        set.add(new proxy(I.back()));
    }
    
    void solve(obj* a, obj* b);
    
    void step(float dt);
};

#endif /* society_h */
