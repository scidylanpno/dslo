/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "algorithms.h"
#include "lp_algorithms.h"
#include "macro.h"
#include "utility.h"

#include <cassert>

LpAlgorithm lp_algorithm;
UniquenessAlgorithm::LpWithUniquenessCheck lp_with_uniqueness_check;

Hypergraph* GreedyAlgorithm(const Hypergraph*);
Hypergraph* SeiveByNodeDegree(const Hypergraph*, const ld);
void NodesRemovalForLimitedOverlap(const Hypergraph*, Hypergraph*, const ld, const int way_to_remove_nodes);

Hypergraph* TryRemove(const int node_id, const Hypergraph* g, const ld rho_max){
    #ifdef DEBUG
    LOG("");
    #endif

    Hypergraph* g_minus_u = new Hypergraph(*g);
    g_minus_u->RemoveNodeSet({node_id});
    Hypergraph* h = lp_algorithm.BasicLp(g_minus_u, true);
    delete g_minus_u;
    if(cmp(h->get_density(), rho_max) >= 0)  return h;
    delete h;
    return nullptr;
}

Hypergraph* TryEnhance(const int node_id, const Hypergraph* g, const ld rho_max){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* h = lp_algorithm.ModifiedLp(g, node_id, rho_max, true);
    return h;
}

Hypergraph* FindMinimal(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* greedy_denest_subgraph = GreedyAlgorithm(g);
    ld rho_apx = greedy_denest_subgraph->get_density();
    delete greedy_denest_subgraph;
    Hypergraph* g_bar = SeiveByNodeDegree(g, rho_apx);
    Console::Show("........Graph after pruning: " + Int2String(g_bar->get_number_of_nodes()) + " nodes, " + Int2String(g_bar->get_number_of_edges()) + " edges, density = " + Double2String(g_bar->get_density()) + ".");
    Hypergraph* h = lp_algorithm.BasicLp(g_bar, true);
    delete g_bar;
    ld rho_max = h->get_density();
    Hypergraph* ans = nullptr;
    while(true){
        if(h->get_number_of_nodes() == 1){
            ans = new Hypergraph(*h);
            break;
        }
        int u_id = h->get_random_node_id();
        Hypergraph* h1 = TryRemove(u_id, h, rho_max);
        Hypergraph* h2 = TryEnhance(u_id, h, rho_max);
        if(h1 == nullptr){
            ans = h2;
            break;
        }
        delete h;
        if(cmp(h1->get_weight_of_nodes(), h2->get_weight_of_nodes()) < 0){
            delete h2;
            h = h1;
        }else{
            delete h1;
            h = h2;
        }
    }
    delete h;
    return ans;
}

vector<Hypergraph*> FindAllMinimal(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    vector<Hypergraph*> l;
    Hypergraph* g_bar = new Hypergraph(*g);
    while(true){
        Hypergraph* h = FindMinimal(g_bar);
        if(l.empty()){
            l.push_back(h);
        }else if(cmp(h->get_density(), l[0]->get_density()) >= 0){
            l.push_back(h);
        }else{
            delete h;
            break;
        }
        g_bar->RemoveNodeSet(h->get_node_set());
    }
    delete g_bar;
    return l;
}

vector<Hypergraph*> ApproxMinAndRemove(const Hypergraph* g, const int k, const ld alpha, const bool is_approx, const int way_to_remove_nodes, Output& output){
    #ifdef DEBUG
    LOG("");
    #endif
    
    vector<Hypergraph*> l;
    Hypergraph* h = new Hypergraph(*g);
    while((l.size() < k) && !h->is_empty()){
        Hypergraph* g_i = nullptr;
        if(is_approx){
            g_i = GreedyAlgorithm(h);
        }else{
            g_i = FindMinimal(h);
        }
        l.push_back(g_i);
        //Console::Log("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("");
        #ifndef TURNOFFGRAPHOUTPUT
        output.PrintLine(g_i->Show() + FEATURE("Density", Double2String(g_i->get_density())));
        #endif // TURNOFFGRAPHOUTPUT
        NodesRemovalForLimitedOverlap(g_i, h, alpha, way_to_remove_nodes);
    }
    return l;
}

Hypergraph* NaiveAlgorithm::FindDensestByDp(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* greedy_denest_subgraph = GreedyAlgorithm(g);
    ld rho_apx = greedy_denest_subgraph->get_density();
    delete greedy_denest_subgraph;
    Hypergraph* g_bar = SeiveByNodeDegree(g, rho_apx);
    Hypergraph* h = lp_algorithm.BasicLp(g_bar, true);
    delete g_bar;
    return h;
}

vector<Hypergraph*> NaiveAlgorithm::NaiveDensest(const Hypergraph* g, const int k, Output& output){
    #ifdef DEBUG
    LOG("");
    #endif
    
    vector<Hypergraph*> l;
    Hypergraph* h = new Hypergraph(*g);
    while((l.size() < k) && !h->is_empty()){
        Hypergraph* g_i = FindDensestByDp(h);
        l.push_back(g_i);
        //Console::Log("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("");
        #ifndef TURNOFFGRAPHOUTPUT
        output.PrintLine(g_i->Show() + FEATURE("Density", Double2String(g_i->get_density())));
        #endif // TURNOFFGRAPHOUTPUT
        h->RemoveNodeSet(g_i->get_node_set());
    }
    return l;
}

vector<Hypergraph*> NaiveAlgorithm::NaiveApprox(const Hypergraph* g, const int k, Output& output){
    #ifdef DEBUG
    LOG("");
    #endif
    
    vector<Hypergraph*> l;
    Hypergraph* h = new Hypergraph(*g);
    while((l.size() < k) && !h->is_empty()){
        Hypergraph* g_i = GreedyAlgorithm(h);
        l.push_back(g_i);
        //Console::Log("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("[" + Int2String(l.size()) + "]\t" + Int2String(g_i->get_number_of_nodes()) + " nodes, " + Int2String(g_i->get_number_of_edges()) + " edges, density = " + Double2String(g_i->get_density()));
        Console::Show("");
        #ifndef TURNOFFGRAPHOUTPUT
        output.PrintLine(g_i->Show() + FEATURE("Density", Double2String(g_i->get_density())));
        #endif // TURNOFFGRAPHOUTPUT
        h->RemoveNodeSet(g_i->get_node_set());
    }
    return l;
}

Hypergraph* UniquenessAlgorithm::FindDensestSubgraph(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* greedy_denest_subgraph = GreedyAlgorithm(g);
    ld rho_apx = greedy_denest_subgraph->get_density();
    delete greedy_denest_subgraph;
    Hypergraph* g_bar = SeiveByNodeDegree(g, rho_apx);
    Hypergraph* h = lp_algorithm.BasicLp(g_bar, true);
    delete g_bar;
    return h;
}

bool UniquenessAlgorithm::CheckByFindMinimal(const Hypergraph* g, Hypergraph*& ans){
    #ifdef DEBUG
    LOG("");
    #endif

    Hypergraph* greedy_denest_subgraph = GreedyAlgorithm(g);
    ld rho_apx = greedy_denest_subgraph->get_density();
    delete greedy_denest_subgraph;
    Hypergraph* g_bar = SeiveByNodeDegree(g, rho_apx);
    Hypergraph* h = nullptr;
    bool flag = lp_with_uniqueness_check.BasicLp(g_bar, true, h);
    delete g_bar;
    if(!flag){
        delete h;
        return false;
    }
    ld rho_max = h->get_density();
    if(h->get_number_of_nodes() == 1){
        ans = h;
        return true;
    }
    int u_id = h->get_random_node_id();
    Hypergraph* h1 = TryRemove(u_id, h, rho_max);
    Hypergraph* h2 = TryEnhance(u_id, h, rho_max);
    if(h1 == nullptr){
        ans = h2;
        delete h1;
    }else{
        flag = false;
        ans = h1;
        delete h2;
    }
    delete h;
    return flag;
}

bool UniquenessAlgorithm::CheckByPruning(const Hypergraph* g, const set densest_part, const ld rho_max){
    #ifdef DEBUG
    LOG("");
    #endif
    
    HypergraphWithPruningEdge* h = new HypergraphWithPruningEdge(*g);
    h->RemoveNodeSet(densest_part);
    if(h->get_number_of_edges()==0) return true;
    Hypergraph* second_minimal = FindDensestSubgraph((Hypergraph*)h);
    return cmp(rho_max, second_minimal->get_density()) > 0;
}

bool UniquenessAlgorithm::Check(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* h = nullptr;
    if(!CheckByFindMinimal(g, h)){
        return false;
    }
    return CheckByPruning(g, h->get_node_set(), h->get_density());
}

/**********GreedyAlgorithm**********/

Hypergraph* GreedyAlgorithm(const Hypergraph* g){
    #ifdef DEBUG
    LOG("");
    #endif
    
    vector<int> node_set;
    vector<ld> weight_before_removal;
    Hypergraph* ans = new Hypergraph(*g);
    HypergraphWithHeap* h = new HypergraphWithHeap(*g);
    while(!h->is_empty()){
        weight_before_removal.push_back(h->get_density());
        int u_id = h->Top();
        //Console::Log("u.id: " + Int2String(u_id) + "; degree: " + Double2String(h->get_degree_of_node(u_id)) + "; h->density: " + Double2String(h->get_density()));
        assert(h->has_node(u_id));
        h->Pop();
        node_set.push_back(u_id);
    }
    int number_of_nodes_to_be_removed = 0;
    for(int i = 0; i < g->get_number_of_nodes(); ++i){
        if(cmp(weight_before_removal[i], weight_before_removal[number_of_nodes_to_be_removed]) > 0){
            number_of_nodes_to_be_removed = i;
        }
    }
    Console::Log("number of nodes to be removed = " + Int2String(number_of_nodes_to_be_removed));
    node_set.resize(number_of_nodes_to_be_removed);
    set node_set_to_be_deleted(node_set.begin(), node_set.end());
    ans->RemoveNodeSet(node_set_to_be_deleted);
    Console::Log("density = " + Double2String(ans->get_density()));
    delete h;
    return ans;
}

/**********SeiveByNodeDengree**********/

Hypergraph* SeiveByNodeDegree(const Hypergraph* g, const ld rho_apx){
    #ifdef DEBUG
    LOG("");
    #endif

    Console::Show("........Rho_apx = " + Double2String(rho_apx));
    
    HypergraphWithHeap* h = new HypergraphWithHeap(*g);
    while(true){
        int u_id = h->Top();
        if(cmp(h->get_degree_of_node(u_id), rho_apx) < 0){
            h->Pop();
        }else{
            break;
        }
    }
    Hypergraph* g_bar = new Hypergraph(*(Hypergraph*)h);
    delete h;
    return g_bar;
}

/**********NodesRemovalForLimitedOverlap**********/

void NodesRemovalForLimitedOverlap(const Hypergraph* g_i, Hypergraph* h, const ld alpha, const int way_to_remove_nodes){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* temp;
    if(way_to_remove_nodes == 1){
        temp = Hypergraph::GenerateInducement(*g_i, *h);
    }else{
        temp = Hypergraph::GenerateComplement(*g_i, *h);
    }
    HypergraphWithHeap* s = new HypergraphWithHeap(*temp);
    delete temp;

    while(cmp(s->get_weight_of_nodes(), alpha * g_i->get_weight_of_nodes()) > 0){
        int u_id = s->Top();
        s->Pop();
        h->RemoveNodeSet({u_id});
    }
    delete s;
}

