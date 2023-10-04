/*
 *
 * For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'
 *
 * Copyright reserved.
 *
 */

#include "utility.h"
#include "hypergraph.h"
#include "algorithms.h"

string info = R"(
The program takes 5+1 arguments:
- the location of the nodes file;
- the location of the hypergraph file;
- a parameter which can be 0, 1 or 2 if we want to run NaiveDensest(0), NaiveApprox(1), MinAndRemove(2) or ApproxMinAndRemove(3), and the default is 2;
- the number of dense subgraphs we want to extract, where the default is 10;
- the parameter alpha which sets the threshold for the overlapping, where the default is 0.0 (no overlap) [this parameter will be ignored in Naive algorithm];
- (will be deleted later) the way we use to remove nodes in Line 7 Algorithm 5, where 1 (default) denote the first kind, and 2 denote the second kind [this parameter will be ignored in Naive algorithm].
)";

void RunNaiveDenest(const Hypergraph* g, const int k, vector<Hypergraph*> &ans, Output& output){
    ans = NaiveAlgorithm::NaiveDensest(g, k, output);
}

void RunNaiveApprox(const Hypergraph* g, const int k, vector<Hypergraph*> &ans, Output& output){
    ans = NaiveAlgorithm::NaiveApprox(g, k, output);
}

void RunMinAndRemove(const Hypergraph* g, const int k, const ld alpha, const int way_overlapness, vector<Hypergraph*> &ans, Output& output){
    ans = ApproxMinAndRemove(g, k, alpha, false, way_overlapness, output);
}

void RunApproxMinAndRemove(const Hypergraph* g, const int k, const ld alpha, const int way_overlapness, vector<Hypergraph*> &ans, Output& output){
    ans = ApproxMinAndRemove(g, k, alpha, true, way_overlapness, output);
}

int main(const int argc, const char* argv[]){
    int algo = 2;
    int k = 10;
    double alpha = 0;
    int way_to_remove_nodes = 1;
    Hypergraph* g = nullptr;
    tik
    switch(argc){
        case 7:
            way_to_remove_nodes = String2Int(argv[6]);
        case 6:
            alpha = String2Double(argv[5]);
        case 5:
            k = String2Int(argv[4]);
        case 4:
            algo = String2Int(argv[3]);
        case 3:
            Console::Show("Reading hypergraph...");
            g = new Hypergraph(argv[1], argv[2]);
            Console::Show("Done.");
            break;
        default:
            Console::Error(info);
    }
    int n = g->get_number_of_nodes();
    int m = g->get_number_of_edges();
    string output_file_name = "./result/" + RetrieveFileName(argv[1]);
    if(algo == 0 || algo == 1){
        if(algo == 0){
            output_file_name += "_[NaiveDensest(0)]";
        }else{
            output_file_name += "_[NaiveApprox(1)]";
        }
        output_file_name += "_k=" + Int2String(k);
    }else if(algo == 2 || algo == 3){
        if(algo == 2){
            output_file_name += "_[MinAndRemove(2)]";
        }else{
            output_file_name += "_[ApproxMinAndRemove(3)]";
        }
        output_file_name += "_k=" + Int2String(k) + "_alpha=" + Double2String(alpha) + "_removalway=" + Int2String(way_to_remove_nodes);
    }else{
		Console::Error(info);
	}
    Output output(output_file_name.c_str());
    vector<Hypergraph*> ans;
    if(algo == 0){
        Console::Show("We perform [NaiveDensest(0)] on a dataset with " + Int2String(n) + " nodes and " + Int2String(m) + " edges with [k = " + Int2String(k) + "].");
        RunNaiveDenest(g, k, ans, output);
    }else if(algo == 1){
        Console::Show("We perform [NaiveApprox(1)] on a dataset with " + Int2String(n) + " nodes and " + Int2String(m) + " edges with [k = " + Int2String(k) + "].");
        RunNaiveApprox(g, k, ans, output);
    }else if(algo == 2){
        Console::Show("We perform [MinAndRemove(2)] on a dataset with " + Int2String(n) + " nodes and " + Int2String(m) + " edges with [k = " + Int2String(k) + "], [alpha = " + Double2String(alpha) + "].");
        RunMinAndRemove(g, k, alpha, way_to_remove_nodes, ans, output);
    }else if(algo == 3){
        Console::Show("We perform [ApproxMinAndRemove(3)] on a dataset with " + Int2String(n) + " nodes and " + Int2String(m) + " edges with [k = " + Int2String(k) + "], [alpha = " + Double2String(alpha) + "].");
        RunApproxMinAndRemove(g, k, alpha, way_to_remove_nodes, ans, output);
    }
    tok
    ld sum = 0.0;
    for(const auto& g:ans){
        sum += g->get_density();
    }
    Console::Show("Total value is " + Double2String(sum) + ".");
    output.PrintLine("Total value is " + Double2String(sum) + ".");

    Console::Show(elapsed);
    output.PrintLine(elapsed);
    return 0;
}
