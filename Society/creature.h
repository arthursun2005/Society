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
    ne_brain brain;
    
    double states[2];
    double age;
    
    creature() : brain(genome_input_size, genome_output_size), age(0.0) {
        type = obj_creature;
        brain.mutate_add_link();
        brain.flush();
        
        std::copy_n(initial_states, 2, states);
    }
    
    creature(const creature& q) : brain(q.brain), age(0.0) {
        type = obj_creature;
        brain.mutate(genome_new_node_prob);
        brain.flush();
    }
    
};

#endif /* creature_h */
