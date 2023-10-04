/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "data_structure.h"

void Heap::wash(){
    while(!heap.empty()){
        auto x = heap.top();
        auto p = iterator.find(x.second.first);
        if(p == iterator.end() || x.second.second != p->second.second){
            heap.pop();
        }else{
            break;
        }
    }
}

void Heap::push(const int key, const ld value){
    iterator[key] = std::make_pair(value, 0);
    heap.push(std::make_pair(value, std::make_pair(key, 0)));
}

int Heap::top(){
    wash();
    return heap.top().second.first;
}

void Heap::pop(){
    int key = top();
    erase(key);
}

void Heap::modify(const int key, const ld value){
    auto p = iterator.find(key);
    p->second.first = value;
    p->second.second = p->second.second + 1;
    heap.push(std::make_pair(value, std::make_pair(key, p->second.second)));
}

void Heap::erase(const int key){
    auto p = iterator.find(key);
    iterator.erase(p);
}