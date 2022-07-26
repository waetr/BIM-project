#include "top.h"

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    vector<node> A_batch = {3};
    vector<int32> k_batch = {3};
    vector<IM_solver> solver_batch = {DEGREE, PAGERANK, CELF_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, CELF_ADVANCED, IMM_NORMAL};
//    vector<IM_solver> solver_batch = {IMM_NORMAL};
    Run_simulation(A_batch, k_batch, solver_batch, IC_M);
    return 0;
}