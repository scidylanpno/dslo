/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */
 
#ifndef __ALGORITHMS__
#define __ALGORITHMS__

#include "hypergraph.h"

Hypergraph* TryRemove(const int, const Hypergraph*, const ld);

Hypergraph* TryEnhance(const int, const Hypergraph*, const ld);

Hypergraph* FindMinimal(const Hypergraph*);

vector<Hypergraph*> FindAllMinimal(const Hypergraph*);

// vector<Hypergraph*> ApproxMinAndRemove(const Hypergraph*, const int, const ld, const bool);

vector<Hypergraph*> ApproxMinAndRemove(const Hypergraph*, const int, const ld, const bool, const int way_to_remove_nodes, Output&);

namespace NaiveAlgorithm{
    Hypergraph* FindDensestByDp(const Hypergraph*);
    vector<Hypergraph*> NaiveApprox(const Hypergraph*, const int, Output&);
    vector<Hypergraph*> NaiveDensest(const Hypergraph*, const int, Output&);
}

namespace UniquenessAlgorithm{
    Hypergraph* FindDensestSubgraph(const Hypergraph*);
    bool CheckByFindMinimal(const Hypergraph*, Hypergraph*&);
    bool CheckByPruning(const Hypergraph*, const set, const ld);
    bool Check(const Hypergraph*);
}

bool CheckUniqueness(const Hypergraph*);

// #include "algorithms.cc"

#endif // __ALGORITHMS__
