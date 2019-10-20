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

/// maximum diameter any object can have
#define diameter_limit 2.0

#define genome_new_node_prob 0.01

#define hash_origin 0x1p16

#define hash_half_bits 32

#define hash_y_step (1lu << hash_half_bits)

#define hash_x_step 1

#define restitution 0.2

/// how far a creature can see
#define sight_limit 32.0

/// sight accuracy
#define sight_lines 8

#define sight_half_angle 0.5

/// creatures' maximum acceleration
#define creature_acceleration 8.0

/// creatures' maximum angular velocity
#define creature_rotation 2.0

/// creatures' maximum lifespan
#define creature_lifespan 86400.0

#define inputs_before_sight 5

#define genome_input_size (inputs_before_sight + 4 * sight_lines)

#define genome_output_size 6

#define maximum_energy 6000.0

#define minimum_energy 60.0

#define food_amount 120.0

#define food_life 300.0

#define food_left 0.25

#define food_left_amount (food_left * food_left * food_amount)

struct vec2 {
    double x;
    double y;
    
    vec2() {}
    
    vec2(double scl) : x(scl), y(scl) {}
    
    vec2(double x, double y) : x(x), y(y) {}
    
    size_t hash() const {
        return (((size_t)(floor(y / diameter_limit) + hash_origin)) << hash_half_bits) + (size_t)(floor(x / diameter_limit) + hash_origin);
    }
};

extern vec2 sight_left;

extern double sight_step_angle;

extern vec2 sight_step;

extern double initial_states[2];

#define make_vec2_operator(o1, o2) \
inline vec2 operator o1 (const vec2& a, const vec2& b) {return vec2(a.x o1 b.x, a.y o1 b.y);}\
inline vec2& operator o2 (vec2& a, const vec2& b) {a.x o2 b.x;a.y o2 b.y;return a;}

make_vec2_operator(+, +=)
make_vec2_operator(-, -=)
make_vec2_operator(*, *=)
make_vec2_operator(/, /=)

inline double dot(const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}

inline vec2 mul (const vec2& a, const vec2& b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

struct color {
    double r;
    double g;
    double b;
    
    color() {}
    
    color(double r, double g, double b) : r(r), g(g), b(b) {}
};

enum obj_type {
    obj_creature,
    obj_wall,
    obj_food,
    obj_source
};

struct obj {
    obj_type type;
    vec2 position;
    vec2 velocity;
    vec2 dir;
    double radius;
    double density;
    color color;
    std::string name;
    bool dead;
    
    obj() : dead(false) {}
    
    void randomlize_color() {
        color.r = ne_random(0.0, 1.0);
        color.g = ne_random(0.0, 1.0);
        color.b = ne_random(0.0, 1.0);
    }
    
    void randomlize_name() {
        size_t size = ne_random(3, 12);
        name.resize(size);
        for(char& q : name) {
            q = ne_random('a', 'z');
        }
    }
};

double raycast(const vec2& p, const vec2& n, const vec2& q, double r);

#endif /* common_h */
