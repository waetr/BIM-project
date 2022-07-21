//
// Created by asuka on 2022/7/16.
// IM Solvers

#ifndef EXP_IMS_H
#define EXP_IMS_H

#include "simulation.h"
#include <queue>
#include <stack>

/*!
 * @brief MG0[u] stores influence spread of {u}
 */
double MG0[MAX_NODE_SIZE];

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
        if(verbose_flag) printf("Nodes are not exceeding k. All selected.\n");
        return;
    }
    double cur = clock();
    int r = 0;
    /// first double : magimal influence
    /// first int : index
    /// second int : iteration round
    typedef pair<double, pair<int, int> > node0;
    priority_queue<node0> Q;
    seeds.resize(1);
    for (int u : candidate) {
        seeds[0] = u;
        if(MG0[u] == 0) MG0[u] = MC_simulation(graph, seeds);
        Q.push(make_pair(MG0[u], make_pair(u, 0)));
    }
    double current_spread = 0;
    seeds.clear();
    if(verbose_flag) printf("\tInitialization time = %.5f\n", time_by(cur));
    while (seeds.size() < k) {
        r++;
        node0 u = Q.top();
        Q.pop();
        if (u.second.second == seeds.size()) {
            if(verbose_flag) printf("\tnode = %d\tround = %d\ttime = %.5f\n", u.second.first, r, time_by(cur));
            seeds.emplace_back(u.second.first);
            current_spread += u.first;
        } else {
            seeds.emplace_back(u.second.first);
            u.first = MC_simulation(graph, seeds) - current_spread;
            seeds.pop_back();
            u.second.second = seeds.size();
            Q.push(u);
        }
    }
    if(verbose_flag) printf("CELF done. total time = %.3f\n", time_by(cur));
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
    pg_rank.clear();
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
    degree_rank.clear();
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : CELF
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void CELF_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    double cur = clock();
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
    if(verbose_flag) printf("CELF method done. total time = %.3f\n", time_by(cur));
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
    if(verbose_flag) cout << "The size of the solution space : " << V_n.size() << endl;
    for (auto &S_n : V_n) {
        if(verbose_flag) print_set(S_n, "set S = ");
        double x = MC_simulation(graph, S_n);
        value.emplace_back(x);
        if(verbose_flag) printf(" simulation result = %.5f\n", x);
    }
    double max_value = 0;
    for (int i = 1; i < value.size(); i++) if (value[i] > value[max_value]) max_value = i;
    seeds = V_n[max_value];
}

/*!
 * @brief Select an active participant u of node v, and remove u from f[v] if u does not meet the criteria.
 * @param v : the candidate neighbour node
 * @param f : the source active participant set of v
 * @param k : Predefined k
 * @param num_neighbours : num_neighbours[u] means how many neighbours that active participant u has been selected
 * @return : the source active participant of v, and -1 if no node meets the criteria.
 */
int source_participant(int v, vector<int> f[], const int k, const int num_neighbours[]) {
    if(f[v].empty()) return -1;
    int u = f[v][f[v].size() - 1];
    while (num_neighbours[u] == k) {
        f[v].pop_back();
        if (f[v].empty()) {
            u = -1;
            break;
        }
        u = f[v][f[v].size() - 1];
    }
    return u;
}

/*!
 * @brief Encapsulated operations for advanced version of IM solver : pagerank
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void advanced_pgrank_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    vector<double> pi(graph.n, 0);
    power_iteration(graph, pi, 0.2);
    set<int> S; //candidate neighbour set
    vector<pair<double, int> > S_ordered;
    int *num_neighbours = new int[graph.n]();
    vector<int> *f = new vector<int>[graph.n](); //f[v] means in-coming active participant of v
    for (int u : A)
        for (auto edge : graph.g[u]) {
            S.insert(edge.v);
            f[edge.v].emplace_back(u);
        }
    for (int w : S) shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    for (int w : S) S_ordered.emplace_back(make_pair(pi[w], w));
    //S_ordered is ordered by pageRank
    sort(S_ordered.begin(), S_ordered.end());
    reverse(S_ordered.begin(), S_ordered.end());
    for (int i = 0; i < S_ordered.size(); i++) {
        int v = S_ordered[i].second;
        int u = source_participant(v, f, k, num_neighbours);
        if (u == -1) continue;
        num_neighbours[u]++;
        seeds.emplace_back(v);
    }
    delete[] num_neighbours;
    delete[] f;
}

/*!
 * @brief Encapsulated operations for advanced version of IM solver : degree
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void advanced_degree_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    set<int> S; //candidate neighbour set
    vector<pair<double, int> > S_ordered;
    int *num_neighbours = new int[graph.n]();
    vector<int> *f = new vector<int>[graph.n](); //f[v] means in-coming active participant of v
    for (int u : A)
        for (auto edge : graph.g[u]) {
            S.insert(edge.v);
            f[edge.v].emplace_back(u);
        }
    for (int w : S) shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    for (int w : S) S_ordered.emplace_back(make_pair(graph.deg_out[w], w));
    //S_ordered is ordered by degree
    sort(S_ordered.begin(), S_ordered.end());
    reverse(S_ordered.begin(), S_ordered.end());
    for (int i = 0; i < S_ordered.size(); i++) {
        int v = S_ordered[i].second;
        int u = source_participant(v, f, k, num_neighbours);
        if (u == -1) continue;
        num_neighbours[u]++;
        seeds.emplace_back(v);
    }
    delete[] num_neighbours;
    delete[] f;
}

/*!
 * @brief Encapsulated operations for advanced version of IM solver : CELF
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void advanced_CELF_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    double cur = clock();
    int r = 0;
    set<int> S; //candidate neighbour set
    int *num_neighbours = new int[graph.n]();
    vector<int> *f = new vector<int>[graph.n](); //f[v] means in-coming active participant of v
    for (int u : A)
        for (auto edge : graph.g[u]) {
            S.insert(edge.v);
            f[edge.v].emplace_back(u);
        }
    for (int w : S) shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    typedef pair<double, pair<int, int> > node0;
    priority_queue<node0> Q;
    seeds.resize(1);
    for (int u : S) {
        seeds[0] = u;
        if(MG0[u] == 0) MG0[u] = MC_simulation(graph, seeds);
        Q.push(make_pair(MG0[u], make_pair(u, 0)));
    }
    double current_spread = 0;
    seeds.clear();
    if(verbose_flag) printf("\tInitialization time = %.5f\n", time_by(cur));
    while (!Q.empty()) {
        node0 Tp = Q.top();
        int v = Tp.second.first, it_round = Tp.second.second;
        double mg = Tp.first;
        Q.pop();
        int u = source_participant(v, f, k, num_neighbours);
        if (u == -1) continue;
        r++;
        if (it_round == seeds.size()) {
            num_neighbours[u]++;
            seeds.emplace_back(v);
            current_spread += mg;
            if(verbose_flag) printf("\tnode = %d\tround = %d\ttime = %.5f\n", v, r, time_by(cur));
        } else {
            seeds.emplace_back(v);
            Tp.first = MC_simulation(graph, seeds) - current_spread;
            seeds.pop_back();
            Tp.second.second = seeds.size();
            Q.push(Tp);
        }
    }
    delete[] num_neighbours;
    delete[] f;
    if(verbose_flag) printf("CELF advanced done. total time = %.3f\n", time_by(cur));
}

#endif //EXP_IMS_H
