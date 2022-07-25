#include "top.h"

int main(int argc, char const *argv[]){
    init_commandLine(argc, argv);
    Graph G("../data/edges.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> A, seed;
    A = {537,1552,3182,6172,7976};

    vector<int> B={175,232,342,644,738,1008,1463,1571,3338,3560,4983,5449,6548,6790,8156,8269,8858,8928,9187,9918};
    cout << MC_simulation(G, B);

//    double cur = clock();
//    seed.clear();
//    IMM_method(G, 5, A, seed);
//    print_set(seed, "IMM set = ");
//    cout << "IMM result = " << MC_simulation(G, seed) << " time = " << time_by(cur) << endl;


//    double cur = clock();
//    CELF(G, 10, B, seed);
//    print_set(seed, "CLEF set = ");
//    cout << "CELF result = " << MC_simulation(G, seed) << " time = " << time_by(cur) << endl;
//
//    cur = clock();
//    seed.clear();
//    IMM(G, B, 10, 0.5, 1, seed);
//    print_set(seed, "IMM set = ");
//    cout << "IMM result = " << MC_simulation(G, seed) << " time = " << time_by(cur) << endl;
    return 0;
}