//
// Created by lenovo on 2022/7/15.
//
#include "graph.h"

#ifndef EXP_SIMULATION_H
#define EXP_SIMULATION_H

#include <random>
#include <set>
#include <random>
#include <algorithm>

/*!
 * @brief generate a random node set of graph.
 * @param graph : the graph
 * @param S : stores the node set. Suppose it is initialized as empty.
 * @param size : the size of the node set
 */
void generate_seed(Graph &graph, set<int> &S, int size = 1) {
    S.clear();
    int *tmp = new int[graph.n + 1];
    for (int i = 0; i <= graph.n; i++) tmp[i] = i;
    srand((unsigned) time(nullptr));
    shuffle(tmp, tmp + graph.n + 1, std::mt19937(std::random_device()()));
    for (int i = 0; i < size; i++) S.insert(tmp[i]);
    delete[] tmp;
}

/*!
 * @brief run MC simulation to evaluate the influence spread.
 * @param graph : the graph that define propagation models(IC)
 * @param S : the seed set
 * @param iteration_times : the number of rounds for MC simulations
 * @return the estimated value of influence spread
 */
double MC_simulation(Graph &graph, set<int> &S, int iteration_rounds = 100) {
    random_device seed;
    ranlux48 engine(seed());
    uniform_real_distribution<> distrib(0.0, 1.0);
    set<int> new_active, A, new_ones;
    vector<int> spread;
    for (int i = 1; i <= iteration_rounds; i++) {
        new_active = S;
        A = S;
        new_ones.clear();
        while (!new_active.empty()) {
            for (int u : new_active) {
                for (auto edge : graph.g[u]) {
                    bool success = (distrib(engine) < edge.second);
                    if (success) new_ones.insert(edge.first);
                }
            }
            new_active.clear();
            for (int u : new_ones) if (A.find(u) == A.end()) new_active.insert(u);
            for (int u : new_ones) A.insert(u);
            new_ones.clear();
        }
        spread.emplace_back(A.size());
    }
    double res = 0;
    for (int a : spread) res += (double) a / iteration_rounds;
    return res;
}

/*!
 * @brief These global variables are used to assist the recursive functions.
 * Except for k0, nothing else needs to be initialized.
 */
int k0;
int selected[MAX_NODE_SIZE];
int neighbour_selected[MAX_NODE_SIZE];
int stack_kS[MAX_NODE_SIZE], stack_kS_top;

/*!
 * @brief A recursive method for enumerating all possible S.
 * @param graph : the graph
 * @param S : the active participant set
 * @param V_n : a container to store all possible S
 * @param k_now : Integer internal parameters, should initialize as 0
 * @param i_now : Integer internal parameters, should initialize as 0
 * @param it : Iterator internal parameters, should initialize as S.begin()
 * @param is_new : Boolean internal parameters, should initialize as true
 */
void select_neighbours(Graph &graph, set<int> &S, vector<set<int> > &V_n, int k_now, int i_now, set<int>::iterator it,
                       bool is_new) {
    if (it == S.end()) {
        set<int> set_tmp;
        V_n.emplace_back(set_tmp);
        for (int i = 1; i <= stack_kS_top; i++) V_n[V_n.size()-1].insert(stack_kS[i]);
        return;
    }
    int u = *it;
    if (is_new) {
        if (it == S.begin())
            for (int w : S) selected[w] = 2;
        neighbour_selected[u] = 0;
        for (int i = 0; i < graph.g[u].size(); i++) {
            int v = graph.g[u][i].first;
            if (selected[v] == 1) neighbour_selected[u]++;
        }
    }
    if (k_now == k0 || neighbour_selected[u] == graph.g[u].size()) {
        select_neighbours(graph, S, V_n, 0, 0, ++it, true);
    } else {
        for (int i = i_now; i < graph.g[u].size(); i++) {
            int v = graph.g[u][i].first;
            if (!selected[v]) {
                selected[v] = 1;
                k_now++;
                neighbour_selected[u]++;
                stack_kS[++stack_kS_top] = v;
                select_neighbours(graph, S, V_n, k_now, i + 1, it, false);
                selected[v] = 0;
                k_now--;
                neighbour_selected[u]--;
                --stack_kS_top;
            }
        }
    }
    if (is_new && it == S.begin()) for (int w : S) selected[w] = 0;
}

#endif //EXP_SIMULATION_H