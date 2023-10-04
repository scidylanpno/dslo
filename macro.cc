/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "macro.h"

#include <cmath>

inline int cmp(const ld a, const ld b){
    if(fabs(a - b) < kEps)   return 0;
    if(a < b) return -1;
    return 1;
}

inline int get_random_value(const int lower_bound, const int upper_bound){
    if(lower_bound == upper_bound){
        return lower_bound;
    }
    return lower_bound + rand() % (upper_bound - lower_bound);
}


