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
void CELF(Graph &graph, int32 k, vector<node> &candidate, vector<node> &seeds) {
    if (k >= candidate.size()) {
        seeds = candidate;
        if(verbose_flag) printf("Nodes are not exceeding k. All selected.\n");
        return;
    }
    double cur = clock();
    int64 r = 0;
    /// first double : magimal influence
    /// first node : index
    /// second int64 : iteration round
    typedef pair<double, pair<node, int64> > node0;
    priority_queue<node0> Q;
    seeds.resize(1);
    for (node u : candidate) {
        seeds[0] = u;
        if(!local_mg) Q.push(make_pair(MC_simulation(graph, seeds), make_pair(u, 0)));
        else Q.push(make_pair(MG0[u], make_pair(u, 0)));
    }
    double current_spread = 0;
    seeds.clear();
    if(verbose_flag) printf("\tInitialization time = %.5f\n", time_by(cur));
    while (seeds.size() < k) {
        r++;
        node0 u = Q.top();
        Q.pop();
        if (u.second.second == seeds.size()) {
            if(verbose_flag) {
                cout << "\tnode = " << u.second.first << "\tround = " << r << "\ttime = " <<time_by(cur) << endl;
            }
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
        for (node id = 0; id < graph.n; ++id) {
            node degree = graph.deg_out[id];
            double alpha_residual = alpha * residuals[id];
            pi[id] += alpha_residual;
            r_sum -= alpha_residual;
            double increment = (residuals[id] - alpha_residual) / degree;
            residuals[id] = 0;
            for (auto &edges:graph.g[id]) {
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
void pgrank_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    vector<double> pi(graph.n, 0);
    power_iteration(graph, pi, 0.2);
    vector<pair<double, node> > pg_rank;
    set<node> seeds_reorder;
    for (node u : A) {
        pg_rank.clear();
        for (auto &edge : graph.g[u]) {
            node v = edge.v;
            if(find(A.begin(), A.end(), v) == A.end()) {
                pg_rank.emplace_back(make_pair(pi[v], v));
            }
        }
        sort(pg_rank.begin(), pg_rank.end());
        reverse(pg_rank.begin(), pg_rank.end());
        for (int32 i = 0; i < k && i < pg_rank.size(); i++)
            seeds_reorder.insert(pg_rank[i].second);
    }
    for (node w : seeds_reorder) seeds.emplace_back(w);
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
void degree_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    vector<pair<node , node> > degree_rank;
    set<node> seeds_reorder;
    for (node u : A) {
        degree_rank.clear();
        for (auto &edge : graph.g[u]) {
            node v = edge.v;
            if(find(A.begin(), A.end(), v) == A.end()) {
                degree_rank.emplace_back(make_pair(graph.deg_out[v], v));
            }
        }
        sort(degree_rank.begin(), degree_rank.end());
        reverse(degree_rank.begin(), degree_rank.end());
        for (int32 i = 0; i < k && i < degree_rank.size(); i++)
            seeds_reorder.insert(degree_rank[i].second);
    }
    for (node w : seeds_reorder) seeds.emplace_back(w);
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
void CELF_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    double cur = clock();
    set<node> seeds_reorder;
    for (node u : A) {
        vector<node> neighbours, one_seed;
        for (auto &edge : graph.g[u]) {
            if(find(A.begin(), A.end(), edge.v) == A.end()) {
                neighbours.emplace_back(edge.v);
            }
        }
        CELF(graph, k, neighbours, one_seed);
        for (node w : one_seed)
            seeds_reorder.insert(w);
    }
    for (node w : seeds_reorder) seeds.emplace_back(w);
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
void enumeration_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    vector<vector<node> > V_n;
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
    for (int64 i = 1; i < value.size(); i++) if (value[i] > value[max_value]) max_value = i;
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
node source_participant(node v, vector<node> f[], const int32 k, const node num_neighbours[]) {
    if(f[v].empty()) return -1;
    node u = f[v][0];
    for(int i = 1; i < f[v].size(); i++) {
        if(num_neighbours[f[v][i]] < num_neighbours[u]) u = f[v][i];
    }
    if(num_neighbours[u] >= k) return -1;
    return u;
}

/*!
 * @brief Encapsulated operations for advanced version of IM solver : pagerank
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void advanced_pgrank_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    vector<double> pi(graph.n, 0);
    power_iteration(graph, pi, 0.2);
    set<node> S; //candidate neighbour set
    vector<pair<double, node> > S_ordered;
    auto *num_neighbours = new node [graph.n]();
    auto *f = new vector<node>[graph.n](); //f[v] means in-coming active participant of v
    for (node u : A)
        for (auto &edge : graph.g[u]) {
            if(find(A.begin(), A.end(), edge.v) == A.end()) {
                S.insert(edge.v);
                f[edge.v].emplace_back(u);
            }
        }
    for (node w : S) shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    for (node w : S) S_ordered.emplace_back(make_pair(pi[w], w));
    //S_ordered is ordered by pageRank
    sort(S_ordered.begin(), S_ordered.end());
    reverse(S_ordered.begin(), S_ordered.end());
    for (int64 i = 0; i < S_ordered.size(); i++) {
        node v = S_ordered[i].second;
        node u = source_participant(v, f, k, num_neighbours);
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
void advanced_degree_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    set<node> S; //candidate neighbour set
    vector<pair<double, node> > S_ordered;
    auto *num_neighbours = new node [graph.n]();
    auto *f = new vector<node>[graph.n](); //f[v] means in-coming active participant of v
    for (node u : A)
        for (auto &edge : graph.g[u]) {
            if(find(A.begin(), A.end(), edge.v) == A.end()) {
                S.insert(edge.v);
                f[edge.v].emplace_back(u);
            }
        }
    for (node w : S) shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    for (node w : S) S_ordered.emplace_back(make_pair(graph.deg_out[w], w));
    //S_ordered is ordered by degree
    sort(S_ordered.begin(), S_ordered.end());
    reverse(S_ordered.begin(), S_ordered.end());
    for (int64 i = 0; i < S_ordered.size(); i++) {
        node v = S_ordered[i].second;
        node u = source_participant(v, f, k, num_neighbours);
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
void advanced_CELF_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    double cur = clock();
    int64 r = 0;
    set<node> S; //candidate neighbour set
    auto *num_neighbours = new node [graph.n]();
    auto *f = new vector<node>[graph.n](); //f[v] means in-coming active participant of v
    //push all candidate neighbour to S, and update f
    for (node u : A){
        for (auto &edge : graph.g[u]) {
            if(find(A.begin(), A.end(), edge.v) == A.end()) {
                S.insert(edge.v);
                f[edge.v].emplace_back(u);
            }
        }
    }
    for (node w : S) {
        shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    }
    typedef pair<double, pair<node, int64> > node0;
    priority_queue<node0> Q;
    seeds.resize(1); //Add a temporary space
    for (node u : S) {
        seeds[0] = u;
        if(!local_mg) {
            Q.push(make_pair(MC_simulation(graph, seeds), make_pair(u, 0)));
        }
        else {
            Q.push(make_pair(MG0[u], make_pair(u, 0)));
        }
    }
    double current_spread = 0;
    seeds.pop_back(); //Clear the temporary space
    if(verbose_flag) printf("\tInitialization time = %.5f\n", time_by(cur));
    while (!Q.empty()) {
        node0 Tp = Q.top();
        node v = Tp.second.first;
        int64 it_round = Tp.second.second;
        double mg = Tp.first;
        Q.pop();
        node u = source_participant(v, f, k, num_neighbours);
        if (u == -1) continue;
        r++;
        if (it_round == seeds.size()) {
            num_neighbours[u]++;
            seeds.emplace_back(v);
            current_spread += mg;
            if(verbose_flag) {
                cout << "\tnode = " << v << "\tround = " << r << "\ttime = " <<time_by(cur) << endl;
            }
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
