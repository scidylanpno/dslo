/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "lp_algorithms.h"
#include "hypergraph.h"
#include "utility.h"

#include "gurobi_c++.h"


#define testget(uid) Console::Show("get "+Int2String(uid)+":"); node_variable[uid].get(GRB_DoubleAttr_X); Console::Show("ok");

/**********GurobiHypergraphMethod**********/

int GurobiHypergraphMethod::GenerateNodeVariable(const Hypergraph* g, GRBModel& model, HashMap<int, GRBVar>& node_variable){
    int cnt = 0;
    for(const auto& x:g->node_set_){
        node_variable[x.first] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, string("y" + Int2String(x.first)));    ++cnt;
    }
    return cnt;
}

int GurobiHypergraphMethod::GenerateEdgeVariable(const Hypergraph* g, GRBModel& model, HashMap<int, GRBVar>& edge_variable){
    int cnt = 0;
    for(const auto& x:g->edge_set_){
        edge_variable[x.first] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "x" + Int2String(x.first));    ++cnt;
    }
    return cnt;
}

int GurobiHypergraphMethod::AddEdgeNodeConstraints(const Hypergraph* g, HashMap<int, GRBVar>& node_variable, HashMap<int, GRBVar>& edge_variable, GRBModel& model){
    int cnt = 0;
    for(const auto& x:g->edge_set_){
        int edge_id = x.first;
        Edge* e = x.second;
        for(const auto& node_id:e->nodes_){
            model.addConstr(edge_variable[edge_id] - node_variable[node_id] <= 0, "x" + Int2String(edge_id) + " <= y" + Int2String(node_id));   ++cnt;
        }
    }
    return cnt;
}

void GurobiHypergraphMethod::GetNodeWeightedSumExpression(const Hypergraph* g, HashMap<int, GRBVar>& node_variable, GRBLinExpr& node_weighted_sum){
    for(const auto& x:g->node_set_){
        node_weighted_sum += (double)x.second->get_weight() * node_variable[x.first];
    }
}

void GurobiHypergraphMethod::GetEdgeWeightedSumExpression(const Hypergraph* g, HashMap<int, GRBVar>& edge_variable, GRBLinExpr& edge_weighted_sum){
    for(const auto& x:g->edge_set_){
        edge_weighted_sum += (double)x.second->get_weight() * edge_variable[x.first];
    }
}

/**********LpAlgorithm**********/

void LpAlgorithm::FastLp(HashMap<int, GRBVar>& node_variable, Hypergraph* ans){
    ld max_value = 0.0;
    for(const auto& x:node_variable){
        if(cmp(x.second.get(GRB_DoubleAttr_X), max_value) >= 0){
            max_value = x.second.get(GRB_DoubleAttr_X);
        }
    }
    for(const auto& x:node_variable){
        if(cmp(x.second.get(GRB_DoubleAttr_X), max_value) < 0){
            ans->RemoveNodeSet({x.first});
        }
    }
}

void LpAlgorithm::FastLpForModified(HashMap<int, GRBVar>& node_variable, Hypergraph* ans){
    for(const auto& x:node_variable){
        //testget(x.first);
        if(cmp(x.second.get(GRB_DoubleAttr_X), 0) == 0){
            ans->RemoveNodeSet({x.first});
        }
    }
}



Hypergraph* LpAlgorithm::BasicLp(const Hypergraph* g, const bool run_fast = false){
    #ifdef DEBUG
    LOG(FEATURE("Nodes", Int2String(g->get_number_of_nodes())) + FEATURE("Edges", Int2String(g->get_number_of_nodes())) + FEATURE("Density", Double2String(g->get_density())));
    #endif
    
    Hypergraph* ans = new Hypergraph(*g);

    try{
        GRBEnv env(true);
        env.set(GRB_IntParam_OutputFlag, 0);
        env.set(GRB_IntParam_Method, 1);
        env.start();
        GRBModel model(env);

        HashMap<int, GRBVar> node_variable;
        HashMap<int, GRBVar> edge_variable;

        int number_of_nodes = GurobiHypergraphMethod::GenerateNodeVariable(g, model, node_variable);
        int number_of_edges = GurobiHypergraphMethod::GenerateEdgeVariable(g, model, edge_variable);

        GRBLinExpr node_weighted_sum;
	    GRBLinExpr edge_weighted_sum;
    
        GurobiHypergraphMethod::GetNodeWeightedSumExpression(g, node_variable, node_weighted_sum);
        GurobiHypergraphMethod::GetEdgeWeightedSumExpression(g, edge_variable, edge_weighted_sum);

        int number_of_node_edge_pairs = GurobiHypergraphMethod::AddEdgeNodeConstraints(g, node_variable, edge_variable, model);
        model.addConstr(node_weighted_sum <= 1, "nodeconstraints");

        model.setObjective(edge_weighted_sum, GRB_MAXIMIZE);

        Console::Show("........Running BasicLp:");
        Console::Show("........      " + Int2String(number_of_nodes) + " nodes;");
        Console::Show("........      " + Int2String(number_of_edges) + " edges;");
        Console::Show("........      " + Int2String(number_of_node_edge_pairs) + " pairs of (u, e);");
        tik
        model.optimize();
        tok;
        

        if(run_fast){
            FastLp(node_variable, ans);
            Console::Show("........      density = " + Double2String(ans->get_density()));
        }else{
            Console::Error("FastLp is assumed.");
        }
        Console::Show("........Done. " + elapsed);

    }catch(GRBException e){
        string info = "Error code = " + Int2String(e.getErrorCode()) + "\n" + e.getMessage();
        Console::Error(info);
    }
    return ans;
}

Hypergraph* LpAlgorithm::ModifiedLp(const Hypergraph* g, const int node_id, const ld rho_max, const bool run_fast = false){
    #ifdef DEBUG
    LOG("");
    #endif
    
    Hypergraph* ans = new Hypergraph(*g);

    if(!g->has_node(node_id)){
        Console::Error("Target Node Does Not Exist!");
    }

    try{
        GRBEnv env(true);
        env.set(GRB_IntParam_OutputFlag, 0);
        env.set(GRB_IntParam_Method, 1);
        env.start();
        GRBModel model(env);

        HashMap<int, GRBVar> node_variable;
        HashMap<int, GRBVar> edge_variable;

        int number_of_nodes = GurobiHypergraphMethod::GenerateNodeVariable(g, model, node_variable);
        int number_of_edges = GurobiHypergraphMethod::GenerateEdgeVariable(g, model, edge_variable);

        GRBLinExpr node_weighted_sum;
	    GRBLinExpr edge_weighted_sum;
    
        GurobiHypergraphMethod::GetNodeWeightedSumExpression(g, node_variable, node_weighted_sum);
        GurobiHypergraphMethod::GetEdgeWeightedSumExpression(g, edge_variable, edge_weighted_sum);

        int number_of_node_edge_pairs = GurobiHypergraphMethod::AddEdgeNodeConstraints(g, node_variable, edge_variable, model);
        model.addConstr(node_weighted_sum <= 1.0, "nodeconstraints");
        model.addConstr(edge_weighted_sum >= (double)(rho_max - kEps), "edgeconstraints");

        GRBLinExpr objective_expression;
        objective_expression += 1.0 * node_variable[node_id];
        

        model.setObjective(objective_expression, GRB_MAXIMIZE);

        Console::Show("........Running ModifiedLp:");
        Console::Show("........      " + Int2String(number_of_nodes) + " nodes;");
        Console::Show("........      " + Int2String(number_of_edges) + " edges;");
        Console::Show("........      " + Int2String(number_of_node_edge_pairs) + " pairs of (u, e);");
        tik
        model.optimize();
        tok;

        if(run_fast){
            int optimize_status = model.get(GRB_IntAttr_Status);
            if(optimize_status != GRB_INF_OR_UNBD && optimize_status != GRB_INFEASIBLE && optimize_status != GRB_UNBOUNDED){
                //testget(node_id);
                if(cmp(node_variable[node_id].get(GRB_DoubleAttr_X), 0) == 0){
                    delete ans;
                    ans = nullptr;
                }else{
                    FastLpForModified(node_variable, ans);
                    if(cmp(ans->get_density(), rho_max) < 0){
                        delete ans;
                        ans = new Hypergraph(*g);
                    }
                }
            }
            if(ans != nullptr)            
                Console::Show("........      density = " + Double2String(ans->get_density()));
        }else{
            Console::Error("FastLp is assumed.");
        }
        Console::Show("........Done. " + elapsed);

    }catch(GRBException e){
        string info = "Error code = " + Int2String(e.getErrorCode()) + "\n" + e.getMessage();
        Console::Error(info);
    }
    return ans;
}

/**********LpWithUniquenessCheck**********/
void UniquenessAlgorithm::LpWithUniquenessCheck::FastLp(HashMap<int, GRBVar>& node_variable, Hypergraph* ans){
    #ifdef DEBUG
    LOG("");
    #endif
    Console::Show("Running FastLp for Uniqueness Check.");
    
    ld max_value = 0.0;
    for(const auto& x:node_variable){
        if(cmp(x.second.get(GRB_DoubleAttr_X), max_value) >= 0){
            max_value = x.second.get(GRB_DoubleAttr_X);
        }
    }
    for(const auto& x:node_variable){
        if(cmp(x.second.get(GRB_DoubleAttr_X), max_value) < 0){
            ans->RemoveNodeSet({x.first});
            if(cmp(x.second.get(GRB_DoubleAttr_X), 0) != 0){
                throw "NOT_UNIQUE";
            }
        }
    }
}


bool UniquenessAlgorithm::LpWithUniquenessCheck::BasicLp(const Hypergraph* g, const bool run_fast, Hypergraph*& ans){
    #ifdef DEBUG
    LOG("");
    #endif
    
    try{
        ans = LpAlgorithm::BasicLp(g, run_fast);
    }catch(const char* info){
        if(string(info) == string("NOT_UNIQUE")){
            return false;
        }
    }
    return true;
}

bool UniquenessAlgorithm::LpWithUniquenessCheck::ModifiedLp(const Hypergraph* g, const int node_id, const ld rho_max, const bool run_fast, Hypergraph*& ans){
    #ifdef DEBUG
    LOG("");
    #endif
    
    
    ans = LpAlgorithm::ModifiedLp(g, node_id, rho_max, run_fast);
    return true;
}
