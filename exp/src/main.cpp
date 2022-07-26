#include "top.h"

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    vector<node> A_batch = {500};
    vector<int32> k_batch = {10};
    vector<IM_solver> solver_batch = {PAGERANK, PAGERANK_ADVANCED, IMM_NORMAL, IMM_ADVANCED};
    Run_simulation(A_batch, k_batch, solver_batch, IC_M);
    return 0;
}