#include "top.h"

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    vector<node> A_batch = {1000, 2000, 5000};
    vector<int32> k_batch = {10};
    vector<IM_solver> solver_batch = {DEGREE, PAGERANK, IMM_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, IMM_ADVANCED};
    Run_simulation(A_batch, k_batch, solver_batch, IC_M, 3);
    return 0;
}