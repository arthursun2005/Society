//
//  creature.h
//  Society
//
//  Created by Arthur Sun on 10/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef creature_h
#define creature_h

struct creature : obj {
    std::string str;
    std::string name;
    ne_brain brain;
    
    creature() : brain(genome_input_size, genome_output_size) {
        type = obj_creature;
        
        color.r = ne_random(0.0f, 1.0f);
        color.g = ne_random(0.0f, 1.0f);
        color.b = ne_random(0.0f, 1.0f);
    }
};

#endif /* creature_h */
