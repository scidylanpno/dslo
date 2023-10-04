/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#ifndef __HYPERGRAPH__
#define __HYPERGRAPH__

#include "macro.h"
#include "utility.h"
#include "data_structure.h"

class node;
class Edge;
class Hypergraph;
class HypergraphWithHeap;

/**********Node**********/

class Node{

friend class Hypergraph;
friend class HypergraphWithHeap;
friend class HypergraphWithPruningEdge;

private:
int id_;
ld weight_;
ld weight_of_edges_;
set edges_;

public:
static bool Read(Input&, int&, ld&);
Node(const int);
int get_id() const;
ld get_weight() const;
void set_weight(const ld);
bool has_edge(const int) const;
void Connect(const int, const ld);
void Connect(const Edge&);
void Cut(const int, const ld);
void Cut(const Edge&);
ld get_degree() const;
string Show() const;
string ToFile() const;
int get_unweighted_degree() const;

friend class GurobiHypergraphMethod;
friend class RemoveSmallComponent;

};

/**********Edge**********/

class Edge{

friend class Hypergraph;
friend class HypergraphWithHeap;
friend class HypergraphWithPruningEdge;

private:
int id_;
ld weight_;
set nodes_;

public:
static bool Read(Input&, ld&, vector<int>&);
Edge(const int);
int get_id() const;
ld get_weight() const;
void set_weight(const ld);
bool has_node(const int) const;
void Connect(const int);
void Connect(const Node&);
void Cut(const int);
void Cut(const Node&);
string Show() const;
string ToFile() const;
vector<int> get_nodes_id() const;

friend class GurobiHypergraphMethod;
friend class RemoveSmallComponent;

};

/**********Hypergraph**********/

class Hypergraph{
protected:
int number_of_nodes_, number_of_edges_;
int avl_edge_id;
HashMap<int, Node*> node_set_;
HashMap<int, Edge*> edge_set_;
ld weight_of_nodes_, weight_of_edges_;
void Clear();
Node* AddNode(const int, const ld);
Edge* AddEdge(const vector<int>&, const ld, const int);
int get_assigned_edge_id();
virtual void EraseNode(const int);
virtual void EraseEdge(const int);

public:
Hypergraph(Input, Input);
Hypergraph(const Hypergraph&);
~Hypergraph();
bool is_empty() const;
bool has_node(const int) const;
bool has_edge(const int) const;
int get_number_of_nodes() const;
int get_number_of_edges() const;
ld get_degree_of_node(const int);
ld get_weight_of_nodes() const;
ld get_density() const;
set get_node_set() const;
int get_random_node_id() const;
void RemoveNodeSet(const set);
static Hypergraph* GenerateComplement(const Hypergraph&, const Hypergraph&);
static Hypergraph* GenerateInducement(const Hypergraph&, const Hypergraph&);
string Show() const;
string NodeSetToFile() const;
string EdgeSetToFile() const;
string get_unweighted_degree_distribution() const;
void get_normal_graph(std::vector<set>&) const;

friend class GurobiHypergraphMethod;
friend class RemoveSmallComponent;

};

/**********HypergraphWithHeap**********/

class HypergraphWithHeap: public Hypergraph{
protected:
Heap heap_;
void EraseNode(const int) override;
void EraseEdge(const int) override;

public:
HypergraphWithHeap(const Hypergraph&);
int Top();
void Pop();
};

/**********HypergraphWithPruningEdge**********/

class HypergraphWithPruningEdge: public Hypergraph{
protected:
void EraseNode(const int) override;

public:
HypergraphWithPruningEdge(const Hypergraph&);

};

// #include "hypergraph.cc"

#endif // __HYPERGRAPH__
