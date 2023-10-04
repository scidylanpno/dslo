/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "hypergraph.h"
#include "data_structure.h"
#include "utility.h"

#include <sstream>
#include <iterator>
#include <cassert>

/**********Node**********/

bool Node::Read(Input& input, int& id, ld& weight){
    std::stringstream ss;
    if(!input.NextLine(ss)){
        return false;
    }
    if(!(ss >> id >> weight)){
        return false;
    }
    #ifdef UNWEIGHTED
    weight = 1;
    #endif
    return true;
}

Node::Node(const int id): id_(id){weight_ = weight_of_edges_ = 0;}

int Node::get_id() const{
    return id_;
}

ld Node::get_weight() const{
    return weight_;
}

void Node::set_weight(const ld weight){
    weight_ = weight;
}

bool Node::has_edge(const int edge_id) const{
    return edges_.find(edge_id) != edges_.end();
}

void Node::Connect(const int edge_id, const ld edge_weight){
    #ifndef NVAL
    if(has_edge(edge_id)){
        return;
    }
    #endif // NVAL
    edges_.insert(edge_id);
    weight_of_edges_ += edge_weight;
}

void Node::Connect(const Edge& edge){
    Connect(edge.get_id(), edge.get_weight());
}

void Node::Cut(const int edge_id, const ld edge_weight){
    #ifndef NVAL
    if(!has_edge(edge_id)){
        return;
    }
    #endif // NVAL
    edges_.erase(edge_id);
    weight_of_edges_ -= edge_weight;
}

void Node::Cut(const Edge&edge){
    Cut(edge.get_id(), edge.get_weight());
}

ld Node::get_degree() const{
    if(cmp(weight_, 0) == 0){
        return 0;
    }
    return weight_of_edges_ / weight_;
}

string Node::Show() const{
    string info = "node(" + Int2String(id_) +"){ ";
    for(const auto& x:edges_){
        info += Int2String(x) + " ";
    }
    info += "} - ";
    info += Double2String(weight_);
    return info;
}

string Node::ToFile() const{
    string info = Int2String(id_) + " " + Int2String(weight_);
    return info;
}

int Node::get_unweighted_degree() const{
    return edges_.size();
}

/**********Edge**********/

bool Edge::Read(Input& input, ld& weight, vector<int>& ids){
    std::stringstream ss;
    if(!input.NextLine(ss)){
        return false;
    }

    string str;
    if(!(ss >> str >> weight)){
        return false;
    }

    #ifdef UNWEIGHTED
    weight = 1;
    #endif

    for(auto& x:str){
        if(x==','){
            x=' ';
        }
    }

    ss.clear();
    ss.str(str);

    ids.clear();
    int node_id;
    while(ss >> node_id){
        ids.push_back(node_id);
    }
    return true;
}

Edge::Edge(const int id): id_(id){weight_ = 0;}

int Edge::get_id() const{
    return id_;
}

ld Edge::get_weight() const{
    return weight_;
}

void Edge::set_weight(const ld weight){
    weight_ = weight;
}

bool Edge::has_node(const int node_id) const{
    return nodes_.find(node_id) != nodes_.end();
}

void Edge::Connect(const int node_id){
    #ifndef NVAL
    if(has_node(node_id)){
        return;
    }
    #endif // NVAL
    nodes_.insert(node_id);
}

void Edge::Connect(const Node& node){
    Connect(node.get_id());
}

void Edge::Cut(const int node_id){
    #ifndef NVAL
    if(!has_node(node_id)){
        return;
    }
    #endif // NVAL
    nodes_.erase(node_id);
}

void Edge::Cut(const Node& node){
    Cut(node.get_id());
}

string Edge::Show() const{
    string ans = "edge(" + Int2String(id_) + "){ ";
    for(const auto& x:nodes_){
        ans += Int2String(x) + " ";
    }
    ans += "} - ";
    ans += Double2String(weight_);
    return ans;
}

string Edge::ToFile() const{
    string ans = "";
    for(const auto& x:nodes_){
        ans += Int2String(x) + ",";
    }
    ans[ans.length()-1]=' ';
    ans += Int2String(weight_);
    return ans;
}

vector<int> Edge::get_nodes_id() const{
    vector<int> vec;
    for(const auto& x:nodes_)
        vec.push_back(x);
    return vec;
}

/**********Hypergraph**********/

void Hypergraph::Clear(){
    number_of_nodes_ = number_of_edges_ = 0;
    avl_edge_id = 0;
    for(auto& x:node_set_){
        delete x.second;
    }
    for(auto& x:edge_set_){
        delete x.second;
    }
    node_set_.clear();  edge_set_.clear();
    weight_of_nodes_ = weight_of_edges_ = 0;
}

Node* Hypergraph::AddNode(const int node_id, const ld node_weight){
    //assert(has_node(node_id) == false);
    Node* u = new Node(node_id);
    u->set_weight(node_weight);
    node_set_[node_id] = u;
    weight_of_nodes_ += node_weight;
    ++number_of_nodes_;
    return u;
}

Edge* Hypergraph::AddEdge(const vector<int>& nodes, const ld edge_weight, const int edge_id){
    assert(edge_id >= 0);
    Edge* e = new Edge(edge_id);
    e->set_weight(edge_weight);
    edge_set_[edge_id] = e;
    weight_of_edges_ += edge_weight;
    ++number_of_edges_;

    for(auto node_id:nodes){
        #ifndef NVAL
        if(!has_node(node_id))  Console::Error("Undefined Node.");
        #endif
        e->Connect(*node_set_[node_id]);
        node_set_[node_id]->Connect(*e);
    }
    return e;
}

int Hypergraph::get_assigned_edge_id(){
    while(has_edge(avl_edge_id))    ++avl_edge_id;
    return avl_edge_id;
}

void Hypergraph::EraseNode(const int node_id){
    #ifndef NVAL
    if(!has_node(node_id)){
        return;
    }
    #endif
    auto u = node_set_[node_id];
    set edge_set_to_be_deleted = u->edges_;
    for(const auto& edge_id:edge_set_to_be_deleted){
        EraseEdge(edge_id);
    }
    weight_of_nodes_ -= u->get_weight();
    node_set_.erase(node_id);
    --number_of_nodes_;
    delete u;
}

void Hypergraph::EraseEdge(const int edge_id){
    #ifndef NVAL
    if(!has_edge(edge_id)){
        return;
    }
    #endif
    auto e = edge_set_[edge_id];
    for(const auto& node_id:e->nodes_){
        node_set_[node_id]->Cut(*e);
    }
    weight_of_edges_ -= e->get_weight();
    edge_set_.erase(edge_id);
    --number_of_edges_;
    delete e;
}

Hypergraph::Hypergraph(Input node_input, Input edge_input){
    Clear();

    int node_id;
    ld node_weight;
    while(Node::Read(node_input, node_id, node_weight)){
        AddNode(node_id, node_weight);
    }

    ld edge_weight;
    vector<int> nodes;
    while(Edge::Read(edge_input, edge_weight, nodes)){
        AddEdge(nodes, edge_weight, get_assigned_edge_id());
    }
}

Hypergraph::Hypergraph(const Hypergraph& graph){
    Clear();

    /*for(const auto& x:graph.node_set_){
        AddNode(x.first, x.second->weight_);
    }

    for(const auto& x:graph.edge_set_){
        AddEdge(x.second->get_nodes_id(), x.second->weight_, x.first);
        if(x.first > avl_edge_id)   avl_edge_id = x.first + 1;
    }*/

    number_of_nodes_ = graph.number_of_nodes_;
    number_of_edges_ = graph.number_of_edges_;
    node_set_ = graph.node_set_;
    for(auto& x:node_set_){
        x.second = new Node(*x.second);
    }
    edge_set_ = graph.edge_set_;
    for(auto& x:edge_set_){
        x.second = new Edge(*x.second);
        avl_edge_id = x.first + 1;
    }
    weight_of_edges_ = graph.weight_of_edges_;
    weight_of_nodes_ = graph.weight_of_nodes_;
}

Hypergraph::~Hypergraph(){
    Clear();
}

bool Hypergraph::is_empty() const{
    return number_of_nodes_ == 0 && number_of_edges_ == 0;
}

bool Hypergraph::has_node(const int node_id) const{
    return node_set_.find(node_id) != node_set_.end();
}

bool Hypergraph::has_edge(const int edge_id) const{
    return edge_set_.find(edge_id) != edge_set_.end();
}

int Hypergraph::get_number_of_nodes() const{
    return number_of_nodes_;
}

int Hypergraph::get_number_of_edges() const{
    return number_of_edges_;
}

ld Hypergraph::get_degree_of_node(const int node_id){
    return node_set_[node_id]->get_degree();
}

ld Hypergraph::get_weight_of_nodes() const{
    return weight_of_nodes_;
}

ld Hypergraph::get_density() const{
    if(cmp(weight_of_nodes_, 0) == 0){
        return 0;
    }
    return weight_of_edges_ / weight_of_nodes_;
}

set Hypergraph::get_node_set() const{
    set ans;
    for(const auto& x:node_set_){
        ans.insert(x.first);
    }
    return ans;
}

int Hypergraph::get_random_node_id() const{
    assert(number_of_nodes_ > 0);
    int rank = get_random_value(0, number_of_nodes_ - 1);
    auto x = node_set_.begin();
    std::advance(x, rank);
    return x->first;
}

void Hypergraph::RemoveNodeSet(const set node_set){
    for(const auto& node_id:node_set){
        EraseNode(node_id);
    }
}

Hypergraph* Hypergraph::GenerateComplement(const Hypergraph& subgraph, const Hypergraph& graph){
    Hypergraph* complement = new Hypergraph(subgraph);
    auto edge_set_to_be_deleted = complement->edge_set_;
    for(const auto& x:edge_set_to_be_deleted){
        complement->EraseEdge(x.first);
    }
    for(const auto& x:graph.edge_set_){
        int edge_id = x.first;
        if(subgraph.has_edge(edge_id)){
            continue;
        }
        Edge* e = x.second;
        vector<int> node_set;
        for(const auto& node_id:e->nodes_){
            if(subgraph.has_node(node_id)){
                complement->AddEdge({node_id}, e->get_weight(), complement->get_assigned_edge_id());
            }
        }
        /*if(!node_set.empty()){
            complement->AddEdge(node_set, e->get_weight());
        }*/
    }
    return complement;
}

Hypergraph* Hypergraph::GenerateInducement(const Hypergraph& subgraph, const Hypergraph& graph){
    Hypergraph* inducement = new Hypergraph(subgraph);
    auto edge_set_to_be_deleted = inducement->edge_set_;
    for(const auto& x:edge_set_to_be_deleted){
        inducement->EraseEdge(x.first);
    }
    for(const auto& x:graph.edge_set_){
        Edge* e = x.second;
        vector<int> node_set;
        for(const auto& node_id:e->nodes_){
            if(subgraph.has_node(node_id)){
                node_set.push_back(node_id);
            }
        }
        if(!node_set.empty()){
            inducement->AddEdge(node_set, e->get_weight(), inducement->get_assigned_edge_id());
        }
    }
    return inducement;
}

string Hypergraph::Show() const{
    string ans = "hypergraph{\n";
    for(const auto& x:node_set_){
        ans += x.second->Show() + "\n";
    }
    for(const auto& x:edge_set_){
        ans += x.second->Show() + "\n";
    }
    return ans + "}";
}

string Hypergraph::NodeSetToFile() const{
    string ans = "";
    for(const auto& x:node_set_){
        ans += x.second->ToFile() + "\n";
    }
    return ans;
}

string Hypergraph::EdgeSetToFile() const{
    string ans = "";
    for(const auto& x:edge_set_){
        ans += x.second->ToFile() + "\n";
    }
    return ans;
}

string Hypergraph::get_unweighted_degree_distribution() const{
    vector<int> vi;
    for(const auto& x:node_set_){
        vi.push_back(x.second->get_unweighted_degree());
    }

    Map<int, int> cnt;
    for(auto x:vi){
        if(cnt.count(x)==0) cnt[x]=0;
        cnt[x]++;
    }
    
    string ans="";
    for(auto x:cnt){
        ans += "(" + Int2String(x.first) + " x " + Int2String(x.second) + ") ";
    }
    return ans;
}

void Hypergraph::get_normal_graph(std::vector<set>& graph) const{
    Map<int, int> index_of_nodes;
    int idx = 0;
    graph.clear();
    for(const auto& e: edge_set_){
        const set& node_set = e.second->nodes_;
        std::vector<int> nodes;
        for(int x:node_set){
            if(index_of_nodes.count(x) == 0){
                index_of_nodes[x] = idx++;
                graph.push_back({});
            }
            nodes.push_back(x);
        }
        for(int i = nodes.size() - 1; i >= 0; --i){
            for(int j = i - 1; j >= 0; --j){
                graph[index_of_nodes[nodes[i]]].insert(index_of_nodes[nodes[j]]);
                graph[index_of_nodes[nodes[j]]].insert(index_of_nodes[nodes[i]]);
            }
        }
    }
}

/**********HypergraphWithHeap**********/

void HypergraphWithHeap::EraseNode(const int node_id){
    Hypergraph::EraseNode(node_id);
    heap_.erase(node_id);
}

void HypergraphWithHeap::EraseEdge(const int edge_id){
    #ifndef NVAL
    if(!has_edge(edge_id)){
        return;
    }
    #endif
    auto e = edge_set_[edge_id];
    for(const auto& node_id:e->nodes_){
        node_set_[node_id]->Cut(*e);
        heap_.modify(node_id, node_set_[node_id]->get_degree());
    }
    weight_of_edges_ -= e->get_weight();
    edge_set_.erase(edge_id);
    --number_of_edges_;
    delete e;
}

HypergraphWithHeap::HypergraphWithHeap(const Hypergraph& graph):Hypergraph(graph){
    for(const auto& x:node_set_){
        heap_.push(x.first, x.second->get_degree());
    }
}

int HypergraphWithHeap::Top(){
    return heap_.top();
}

void HypergraphWithHeap::Pop(){
    int node_id = heap_.top();
    EraseNode(node_id);
}

/**********HypergraphWithPruningEdge**********/

void HypergraphWithPruningEdge::EraseNode(const int node_id){
    #ifdef DEBUG
    LOG("");
    #endif
    
    #ifndef NVAL
    if(!has_node(node_id)){
        return;
    }
    #endif
    auto u = node_set_[node_id];
    set edge_set_to_be_deleted = u->edges_;
    for(const auto& edge_id:edge_set_to_be_deleted){
        #ifndef NVAL
        if(!has_edge(edge_id) || !edge_set_[edge_id]->has_node(node_id)){
            continue;
        }
        #endif
        edge_set_[edge_id]->Cut(node_id);
        if(edge_set_[edge_id]->nodes_.size() == 0){
            EraseEdge(edge_id);
        }
    }
    weight_of_nodes_ -= u->get_weight();
    node_set_.erase(node_id);
    --number_of_nodes_;
    delete u;
}

HypergraphWithPruningEdge::HypergraphWithPruningEdge(const Hypergraph& g):Hypergraph(g){}
