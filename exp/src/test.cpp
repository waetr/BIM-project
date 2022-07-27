#include "top.h"

int main(int argc, char const *argv[]){
    init_commandLine(argc, argv);
    Graph G(graphFilePath, DIRECTED_G);
    cout <<"!\n";
    G.set_diffusion_model(IC_M, 15);

    cout << G.n << " " << G.m << endl;

    vector<node> A, seeds;
    generate_seed(G, A, 100);
    int32 k = 50;

    pgrank_method(G, k, A, seeds);
    print_set(seeds, "pgrank = ");
    FI_simulation(G, seeds);
    seeds.clear();

    IMM_method(G, k, A, seeds);
    print_set(seeds, "IMM = ");
    FI_simulation(G, seeds);
    seeds.clear();

    advanced_pgrank_method(G, k, A, seeds);
    print_set(seeds, "pgrank-G = ");
    FI_simulation(G, seeds);
    seeds.clear();

    advanced_IMM_method(G, k, A, seeds);
    print_set(seeds, "IMM-G = ");
    FI_simulation(G, seeds);
    seeds.clear();

    return 0;
}