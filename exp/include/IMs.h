//
// Created by asuka on 2022/7/16.
// IM Solvers

#ifndef EXP_IMS_H
#define EXP_IMS_H

#include "simulation.h"
#include <queue>

/*!
 * @brief CELF algorithm is used to select k most influential nodes at a given candidate.
 * @param graph : the graph
 * @param k : the number of nodes to be selected
 * @param candidate : the candidate node set
 * @param seeds : returns the most influential nodes set
 */
void CELF(Graph &graph, int k, vector<int> &candidate, vector<int> &seeds) {
    if (k >= candidate.size()) {
        seeds = candidate;
        printf("Nodes are not exceeding k. All selected.");
        return;
    }
    priority_queue<Node> nonactive;
    seeds.resize(1);
    for (int c : candidate) {
        seeds[0] = c;
        nonactive.push(Node(c, MC_simulation(graph, seeds)));
    }
    Node current_node;
    double current_spread = 0;
    seeds.clear();
    double cur;
    int count;

    for (int i = 0; i < k; i++) {
        cur = clock();
        Node best_node(-1, -1);
        seeds.push_back(nonactive.top().vertex);
        count = 0;
        while (nonactive.top() != best_node) {
            current_node = nonactive.top();
            seeds[i] = current_node.vertex;
            current_node.value = MC_simulation(graph, seeds) - current_spread;
            count++;
            nonactive.pop();
            nonactive.push(current_node);
            if (best_node < current_node)
                best_node = current_node;
        }
        nonactive.pop();
        seeds[i] = best_node.vertex;
        current_spread += best_node.value;
        printf("seed: %d\t%d\t%f\t%d\n", seeds[i], i + 1, time_by(cur), count);
    }
}

/*!
 * @brief Use power iteration method to calculate pagerank values of nodes in graph.
 * @remarks Note that if there are isolated nodes in the graph, the iteration will not stop causing an error.
 * @param graph : the graph
 * @param pi : returns a size-n vector, containing pagerank values of all nodes
 * @param alpha : initialized usually as 0.15 or 0.2
 * @param l1_error : The precision that needs to be achieved
 */
void power_iteration(Graph &graph, vector<double> &pi, double alpha, double l1_error = 1e-9) {
    vector<double> residuals(graph.n, 1.0 / graph.n);
    vector<double> new_residuals(graph.n, 0);
    double r_sum = 1;
    for (uint32_t num_iter = 0; r_sum > l1_error; ++num_iter) {
        for (int id = 0; id < graph.n; ++id) {
            int degree = graph.deg_out[id];
            double alpha_residual = alpha * residuals[id];
            pi[id] += alpha_residual;
            r_sum -= alpha_residual;
            double increment = (residuals[id] - alpha_residual) / degree;
            residuals[id] = 0;
            for (auto edges:graph.g[id]) {
                int nid = edges.v;
                new_residuals[nid] += increment;
            }
        }
        residuals.swap(new_residuals);
    }
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : pagerank
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void pgrank_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    vector<double> pi(graph.n, 0);
    power_iteration(graph, pi, 0.2);
    vector<pair<double, int> > pg_rank;
    set<int> seeds_reorder;
    for (int u : A) {
        pg_rank.clear();
        for (auto edge : graph.g[u]) {
            int v = edge.v;
            pg_rank.emplace_back(make_pair(pi[v], v));
        }
        sort(pg_rank.begin(), pg_rank.end());
        reverse(pg_rank.begin(), pg_rank.end());
        for (int i = 0; i < k && i < pg_rank.size(); i++)
            seeds_reorder.insert(pg_rank[i].second);
    }
    for (int w : seeds_reorder) seeds.emplace_back(w);
    seeds_reorder.clear();
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : degree
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void degree_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    vector<pair<int, int> > degree_rank;
    set<int> seeds_reorder;
    for (int u : A) {
        degree_rank.clear();
        for (auto edge : graph.g[u]) {
            int v = edge.v;
            degree_rank.emplace_back(make_pair(graph.deg_out[v], v));
        }
        sort(degree_rank.begin(), degree_rank.end());
        reverse(degree_rank.begin(), degree_rank.end());
        for (int i = 0; i < k && i < degree_rank.size(); i++)
            seeds_reorder.insert(degree_rank[i].second);
    }
    for (int w : seeds_reorder) seeds.emplace_back(w);
    seeds_reorder.clear();
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : CELF
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void CELF_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    set<int> seeds_reorder;
    for (int u : A) {
        vector<int> neighbours, one_seed;
        for (auto edge : graph.g[u])
            neighbours.emplace_back(edge.v);
        CELF(graph, k, neighbours, one_seed);
        for (int w : one_seed)
            seeds_reorder.insert(w);
    }
    for (int w : seeds_reorder) seeds.emplace_back(w);
    seeds_reorder.clear();
}

/*!
 * @brief Encapsulated operations for Option 1
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void enumeration_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    vector<vector<int> > V_n;
    vector<double> value;
    select_neighbours(graph, A, V_n, k, 0, 0, A.begin(), true);
    cout << "The size of the solution space : " << V_n.size() << endl;
    for (auto &S_n : V_n) {
        print_set(S_n, "set S = ");
        double x = MC_simulation(graph, S_n);
        value.emplace_back(x);
        printf(" simulation result = %.5f\n", x);
    }
    double max_value = 0;
    for (int i = 1; i < value.size(); i++) if (value[i] > value[max_value]) max_value = i;
    seeds = V_n[max_value];
}

#endif //EXP_IMS_H
