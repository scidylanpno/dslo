/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#ifndef __DATA_STRUCTURE__
#define __DATA_STRUCTURE__

#include "macro.h"

#include <unordered_map>
#include <map>
#include <queue>

template<class T1, class T2>
using HashMap = std::unordered_map<T1, T2>;

template<class T1, class T2>
using Map = std::map<T1, T2>;

struct Heap{
std::priority_queue<std::pair<ld, std::pair<ld, int> >, vector<std::pair<ld, std::pair<ld, int> > >, std::greater<std::pair<ld, std::pair<ld, int> > > > heap;
HashMap<int, std::pair<ld, int> > iterator;
void wash();
void push(const int, const ld);
int top();
void pop();
void modify(const int, const ld);
void erase(const int);
};



// #include "data_structure.cc"

#endif // __DATA_STRUCTURE__
