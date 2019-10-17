//
//  common.h
//  Society
//
//  Created by Arthur Sun on 10/12/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef common_h
#define common_h

#include <iostream>
#include <vector>
#include <unistd.h>
#include <thread>
#include <random>
#include <cmath>

#include "brain.h"

#define diameter_limit 2.0f

#define genome_new_node_prob 0.01f

#define genome_input_size 2

#define genome_output_size 2

#define hash_origin 0x1p32f

#define hash_half_bits 32

#define restitution 0.2f

struct vec2 {
    float x;
    float y;
    
    vec2() {}
    
    vec2(float scl) : x(scl), y(scl) {}
    
    vec2(float x, float y) : x(x), y(y) {}
    
    size_t hash() const {
        return (((size_t)(floorf(y / diameter_limit) + hash_origin)) << hash_half_bits) | (size_t)(floorf(x / diameter_limit) + hash_origin);
    }
};

#define make_vec2_operator(o1, o2) \
inline vec2 operator o1 (const vec2& a, const vec2& b) {return vec2(a.x o1 b.x, a.y o1 b.y);}\
inline vec2& operator o2 (vec2& a, const vec2& b) {a.x o2 b.x;a.y o2 b.y;return a;}

make_vec2_operator(+, +=)
make_vec2_operator(-, -=)
make_vec2_operator(*, *=)
make_vec2_operator(/, /=)

inline float dot(const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}

inline vec2 mul (const vec2& a, const vec2& b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

struct color {
    float r;
    float g;
    float b;
    
    color() {}
    
    color(float r, float g, float b) : r(r), g(g), b(b) {}
};

enum obj_type {
    obj_item,
    obj_creature
};

struct obj {
    obj_type type;
    vec2 position;
    vec2 velocity;
    float radius;
    float density;
    color color;
};

struct item : obj {
    bool fixed;
    
    item() {
        type = obj_item;
    }
};

#endif /* common_h */
