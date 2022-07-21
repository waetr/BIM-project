#include "top.h"

int main() {
    Graph G("../data/edges.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> A = {1463, 1823, 644, 232, 5449}, seeds;
    solvers(G, 2, A, seeds, CELF_NORMAL);
    solvers(G, 2, A, seeds, CELF_ADVANCED);
    return 0;
}