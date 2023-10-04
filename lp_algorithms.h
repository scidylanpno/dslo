/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#ifndef __LP_ALGORITHMS__
#define __LP_ALGORITHMS__

#include "hypergraph.h"

#include "gurobi_c++.h"


/**********GurobiHypergraphMethod**********/

class GurobiHypergraphMethod{
public:
static int GenerateNodeVariable(const Hypergraph*, GRBModel&, HashMap<int, GRBVar>&);
static int GenerateEdgeVariable(const Hypergraph*, GRBModel&, HashMap<int, GRBVar>&);
static int AddEdgeNodeConstraints(const Hypergraph*, HashMap<int, GRBVar>&, HashMap<int, GRBVar>&, GRBModel&);
static void GetNodeWeightedSumExpression(const Hypergraph*, HashMap<int, GRBVar>&, GRBLinExpr&);
static void GetEdgeWeightedSumExpression(const Hypergraph*, HashMap<int, GRBVar>&, GRBLinExpr&);

};

/**********LpAlgorithm**********/

class LpAlgorithm{
public:
virtual void FastLp(HashMap<int, GRBVar>&, Hypergraph*);
virtual void FastLpForModified(HashMap<int, GRBVar>&, Hypergraph*);
Hypergraph* BasicLp(const Hypergraph*, const bool);
Hypergraph* ModifiedLp(const Hypergraph*, const int, const ld, const bool);
};

/**********LpWithUniquenessCheck**********/

namespace UniquenessAlgorithm{
class LpWithUniquenessCheck: public LpAlgorithm{
public:
void FastLp(HashMap<int, GRBVar>&, Hypergraph*) override;
bool BasicLp(const Hypergraph*, const bool, Hypergraph*&);
bool ModifiedLp(const Hypergraph*, const int, const ld, const bool, Hypergraph*&);
};
}


// #include "lp_algorithms.cc"

#endif // __LP_ALGORITHMS__
