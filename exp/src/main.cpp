#include "top.h"

int main() {
    vector<int> A_batch = {1};
    vector<int> k_batch = {20};
    vector<IM_solver> solver_batch = {DEGREE, PAGERANK, CELF_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, CELF_ADVANCED};
    Run_simulation(A_batch, k_batch, solver_batch, IC_M, 3);
    return 0;
}