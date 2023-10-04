# dslo

For paper 'Finding Subgraphs with Maximum Total Density and Limited Overlap in Weighted Hypergraphs'

---

The instructions are for the following enviroments:
- Operating System: Ubuntu 20.04.4 LTS
- Kernel: Linux 5.4.0-155-generic
- Architecture: x86-64

You may need to change some in case any is different.

---

In order to run the code you need to install [Gurobi](https://www.gurobi.com/), and add gurobi home path (`GUROBI_HOME`) in the `Makefile`. To compile the code, open the terminal and type

```bath
make
mkdir result
```

 `./dslo` is program we will use. 

 The program takes 5 arguments:
- the location of the nodes file;
- the location of the hypergraph file;
- a parameter which can be 0, 1 or 2 if we want to run NaiveDenest(0), NaiveApprox(1), MinAndRemove(2) or ApproxMinAndRemove(3), and the default is 2;
- the number of dense subgraphs we want to extract, where the default is 10;
- the parameter alpha which sets the threshold for the overlapping, where the default is 0.0 (no overlap) [this parameter will be ignored in Naive algorithm];


Example:

```bath
./dslo xx.nodes_weight xx.hypergraph

./dslo xx.nodes_weight xx.hypergraph 2 10 

./dslo xx.nodes_weight xx.hypergraph 2 10 0
```

The console output will contain the sum of density and the running time.
The output graph could be found in folder `result`.