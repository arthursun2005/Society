//
//  wall.h
//  Society
//
//  Created by Arthur Sun on 10/20/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef wall_h
#define wall_h

#include "../common.h"

struct wall : obj {
    wall() {
        type = obj_wall;
        
        color.r = 0.75;
        color.g = 0.75;
        color.b = 0.75;
    }
};

#endif /* wall_h */
