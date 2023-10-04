/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#ifndef __MACRO__
#define __MACRO__

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

using string = std::string;

template<class T>
using vector = std::vector<T>;

using set = std::unordered_set<int>;

using ld = long double;

const ld kEps = 1e-7;
const ld kInf = 1e366l;

inline int cmp(const ld, const ld);
inline int get_random_value(const int, const int);
//template<class T1>
//inline string count_vector(const std::vector<T1>&, bool (*)(const T1&, const T1&), string (*get_value)(const T1&)){


#include "macro.cc"

#endif // __MACRO__
