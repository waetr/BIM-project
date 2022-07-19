#include "IMs.h"

int main() {
    //set k
    int k0 = 2;

    //load graph from absolute path
    Graph g(R"(D:\BIM-project\BIM-project\exp\data\edges.csv)", UNDIRECTED_G);

    //set diffusion model
//    g.set_diffusion_model(IC);
    g.set_diffusion_model(IC_M, 15);



    //set the active participant set A
    vector<int> A = {132, 578, 1919};

    cout << "overlap ratio = " << estimate_neighbor_overlap(g,A) << endl;

    //Instantiate an empty container to store the result
    vector<int> seeds;

    //option_1
    enumeration_method(g, k0, A, seeds);
    print_set(seeds, "final seed set enumerated = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    //option 2
    degree_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using degree = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    pgrank_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using pagerank = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    CELF_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using CELF = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    advanced_degree_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using advanced degree = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    advanced_pgrank_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using advanced pagerank = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    advanced_CELF_method(g, k0, A, seeds);
    print_set(seeds, "final seed set using advanced CELF = ");
    cout << "spread = " << MC_simulation(g, seeds) << endl;
    seeds.clear();

    return 0;
}