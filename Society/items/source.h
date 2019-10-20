//
//  source.h
//  Society
//
//  Created by Arthur Sun on 10/20/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef source_h
#define source_h

#include "../common.h"

struct food : obj {
    double age;
    
    food() : age(0.0) {
        type = obj_food;
        
        color.r = 1.0;
        color.g = 1.0;
        color.b = 0.0;
    }
};

struct source : obj {
    double period;
    double time;
    double rotation;
    
    source() : time(0.0) {
        type = obj_source;
        
        color.r = 1.0;
        color.g = 1.0;
        color.b = 1.0;
    }
};

#endif /* source_h */
