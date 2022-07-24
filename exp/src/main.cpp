#include "top.h"

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    vector<int> A_batch = {5};
    vector<int> k_batch = {3};
    vector<IM_solver> solver_batch = {DEGREE, PAGERANK, CELF_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, CELF_ADVANCED};
    Run_simulation(A_batch, k_batch, solver_batch, IC_M);
    return 0;
}