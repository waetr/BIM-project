#include "IMs.h"

int main() {
    //set k
    int k0 = 3;

    //load graph from absolute path
    Graph g(R"(D:\BIM-project\BIM-project\exp\data\edges.csv)", UNDIRECTED_G);

    //set diffusion model
    g.set_diffusion_model(IC);

    //set the active participant set A
    vector<int> A;
    //generate_seed(g, A, 3);
    A.push_back(100);

    //Instantiate an empty container to store the results
    vector<int> seeds;

//    //option_1
//    enumeration_method(g, k0, A, seeds);
//    print_set(seeds, "final seed set enumerated = ");
//    seeds.clear();

    //option 2
    degree_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using degree = ");
    printf("\n");
    seeds.clear();

    pgrank_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using pagerank = ");
    printf("\n");
    seeds.clear();

    CELF_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using CELF = ");
    printf("\n");
    seeds.clear();

    return 0;
}