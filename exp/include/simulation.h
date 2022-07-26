//
// Created by lenovo on 2022/7/16.
//

#ifndef EXP_SIMULATION_H
#define EXP_SIMULATION_H

#include "graph.h"
#include <set>
#include <algorithm>

/*!
 * @brief generate a random node set of graph.
 * @param graph : the graph
 * @param S : stores the node set. Suppose it is initialized as empty.
 * @param size : the size of the node set
 */
void generate_seed(Graph &graph, vector<node> &S, node size = 1) {
    S.clear();
    auto *tmp = new node[graph.n];
    for (node i = 0; i < graph.n; i++) {
        tmp[i] = i;
    }
    shuffle(tmp, tmp + graph.n, std::mt19937(std::random_device()()));
    for (node i = 0; i < size; i++) S.emplace_back(tmp[i]);
    delete[] tmp;
}

/// @brief Marks the point that was activated in the MC simulation
bool active[MAX_NODE_SIZE];

/*!
 * @brief run MC simulation to evaluate the influence spread.
 * @param graph : the graph that define propagation models(IC)
 * @param S : the seed set
 * @return the estimated value of influence spread
 */
double MC_simulation(Graph &graph, vector<node> &S) {
    double cur = clock();
    int64 meet_time = 0; //Too large for int!
    vector<node> new_active, A, new_ones;
    vector<Edge> meet_nodes, meet_nodes_tmp;
    double res = 0;
    for (int64 i = 1; i <= MC_iteration_rounds; i++) {
        if (graph.diff_model == IC) {
            new_active = S, A = S;
            for (node w : S) active[w] = true;
            new_ones.clear();
            while (!new_active.empty()) {
                for (node u : new_active) {
                    for (auto &edge : graph.g[u]) {
                        node v = edge.v;
                        if (active[v]) continue;
                        bool success = (random_real() < edge.p);
                        if (success) new_ones.emplace_back(v), active[v] = true;
                    }
                }
                new_active = new_ones;
                for (node u : new_ones) A.emplace_back(u);
                new_ones.clear();
            }
            for (node u : A) active[u] = false;
            res += (double) A.size() / MC_iteration_rounds;
            A.clear();
        } else if (graph.diff_model == IC_M) {
            meet_nodes.clear();
            new_active = S;
            for (node w : S) active[w] = true;
            for (int64 spread_rounds = 0; spread_rounds < graph.deadline; spread_rounds++) {
                for (node u : new_active) {
                    for (auto &edge : graph.g[u]) {
                        if (active[edge.v]) continue;
                        meet_nodes.emplace_back(edge);
                    }
                }
                for (node u : new_active) A.emplace_back(u);
                new_active.clear();
                if (meet_nodes.empty()) break;
                meet_nodes_tmp.clear();
                for (auto &edge : meet_nodes) {
                    if (!active[edge.v]) {
                        meet_time++;
                        bool meet_success = (random_real() < edge.m);
                        if (meet_success) {
                            bool activate_success = (random_real() < edge.p);
                            if (activate_success) {
                                new_active.emplace_back(edge.v);
                                active[edge.v] = true;
                            }
                        } else {
                            meet_nodes_tmp.emplace_back(edge);
                        }
                    }
                }
                meet_nodes = meet_nodes_tmp;
            }
            for (node u : new_active) A.emplace_back(u);
            for (node u : A) active[u] = false;
            res += (double) A.size() / MC_iteration_rounds;
            A.clear();
        }
    }
    double end = time_by(cur);
    if (verbose_flag) printf("\t\tresult=%.3f time=%.3f meet time=%lld\n", res, end, meet_time);
    return res;
}

/*!
 * @brief These global variables are used to assist the recursive functions.
 * No needs to be initialized.
 */
int64 node_selected[MAX_NODE_SIZE];
int64 neighbour_selected[MAX_NODE_SIZE];
int64 stack_kS[MAX_NODE_SIZE], stack_kS_top;

/*!
 * @brief A recursive method for enumerating all possible S.
 * @param graph : the graph
 * @param S : the active participant set
 * @param V_n : a container to store all possible S
 * @param k0 : number of neighbours k of each node
 * @param k_now : Integer internal parameters, should initialize as 0
 * @param i_now : Integer internal parameters, should initialize as 0
 * @param it : Iterator internal parameters, should initialize as S.begin()
 * @param is_new : Boolean internal parameters, should initialize as true
 */
void select_neighbours(Graph &graph, vector<node> &S, vector<vector<node> > &V_n, int64 k0, int64 k_now, int64 i_now,
                       vector<node>::iterator it,
                       bool is_new) {
    if (it == S.end()) {
        vector<node> set_tmp;
        for (int64 i = 1; i <= stack_kS_top; i++) set_tmp.emplace_back(stack_kS[i]);
        V_n.emplace_back(set_tmp);
        return;
    }
    int u = *it;
    if (is_new) {
        if (it == S.begin())
            for (auto w : S) node_selected[w] = 2;
        neighbour_selected[u] = 0;
        for (int64 i = 0; i < graph.g[u].size(); i++) {
            node v = graph.g[u][i].v;
            if (node_selected[v] == 1) neighbour_selected[u]++;
        }
    }
    if (k_now == k0 || neighbour_selected[u] == graph.g[u].size()) {
        select_neighbours(graph, S, V_n, k0, 0, 0, ++it, true);
    } else {
        for (int64 i = i_now; i < graph.g[u].size(); i++) {
            int v = graph.g[u][i].v;
            if (!node_selected[v]) {
                node_selected[v] = 1;
                k_now++;
                neighbour_selected[u]++;
                stack_kS[++stack_kS_top] = v;
                select_neighbours(graph, S, V_n, k0, k_now, i + 1, it, false);
                node_selected[v] = 0;
                k_now--;
                neighbour_selected[u]--;
                --stack_kS_top;
            }
        }
    }
    if (is_new && it == S.begin()) for (int w : S) node_selected[w] = 0;
}

/*!
 * @brief Calculate the degree of neighbor overlap at active participant.
 * @param graph : the graph
 * @param seeds : the active participant set
 * @param iteration_rounds : The number of selection
 * @return the mean overlap ratio
 */
double estimate_neighbor_overlap(Graph &graph, vector<node> &seeds) {
    auto *num = new node[graph.n]();
    node tot = 0, overlap = 0;
    for (node u : seeds)
        for (auto &e : graph.g[u])
            num[e.v]++;
    for (node u : seeds) {
        for (auto &e : graph.g[u]) {
            if (num[e.v] > 0) tot++;
            if (num[e.v] > 1) overlap++;
            num[e.v] = 0;
        }
    }
    delete[] num;
    return (double) overlap / tot;
}

#endif //EXP_SIMULATION_H
