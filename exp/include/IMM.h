//
// Created by asuka on 2022/7/24.
//

#ifndef SIMULATION_H_IMM_H
#define SIMULATION_H_IMM_H

#include "graph.h"

/*!
 * @brief Some predefined mathematical functions.
 */
class Math {
public:
    static double log2(int n) {
        return log(n) / log(2);
    }

    static double logcnk(int n, int k) {
        double ans = 0;
        for (int i = n - k + 1; i <= n; i++) {
            ans += log(i);
        }
        for (int i = 1; i <= k; i++) {
            ans -= log(i);
        }
        return max(ans, 0.0);
    }

    template<class T>
    static T sqr(T x) {
        return x * x;
    }
};

///set of RI sets
vector<vector<node>> RRI;

///covered[u] marks which RI sets the node u is covered by
vector<int64> covered[MAX_NODE_SIZE];
///coveredNum[u] marks how many RI sets the node u is covered by
int64 coveredNum[MAX_NODE_SIZE];

/*!
 * @brief Empty the set R.
 */
void init_R() {
    RRI.clear();
    for (auto &i : covered) i.clear();
    memset(coveredNum, 0, sizeof(coveredNum));
}

///@brief temporary array for RI_Gen
///
/// no need to initialize
int64 dist[MAX_NODE_SIZE + 1];
bool DijkstraVis[MAX_NODE_SIZE];

/*!
 * @brief Algorithm for CTIC to generate Reserve-Influence or Forward-Influence set of IMM.
 * @param graph : the graph
 * @param uStart : the starting nodes of this RI/FI set
 * @param RR : returns the RI/FI set as an passed parameter
 * @param RI_flag : determine which type of set to generate. true as RI, false as FI.
 *
 */
void RI_Gen(Graph &graph, vector<node> &uStart, vector<node> &RR, bool RI_flag) {
    if (dist[MAX_NODE_SIZE] != -1) {
        //Initialize dist[] to be all -1 at the first time
        memset(dist, -1, sizeof(dist));
    }
    auto *edge_list = RI_flag ? &graph.gT : &graph.g;
    for (node u : uStart)
        dist[u] = 0;
    RR.clear();
    priority_queue<pair<int64, node>> Q;
    for (node u : uStart)
        Q.push(make_pair(0, u));
    while (!Q.empty()) { //Dijkstra Algorithm
        node u = Q.top().second;
        Q.pop();
        if (DijkstraVis[u]) continue;
        DijkstraVis[u] = true;
        RR.emplace_back(u);
        for (auto &edgeT : (*edge_list)[u]) {
            bool activate_success = (random_real() < edgeT.p);
            if (activate_success) {
                int randomWeight;
                if (graph.diff_model == IC) {
                    randomWeight = 1;
                } else if (graph.diff_model == IC_M) {
                    geometric_distribution<int> distribution(edgeT.m);
                    randomWeight = distribution(random_engine) + 1;
                }
                if ((dist[edgeT.v] == -1 || dist[edgeT.v] > dist[u] + randomWeight) &&
                    dist[u] + randomWeight <= graph.deadline) {
                    dist[edgeT.v] = dist[u] + randomWeight;
                    Q.push(make_pair(-dist[edgeT.v], edgeT.v));
                }
            }

        }
    }
    for (node u : RR)
        dist[u] = -1, DijkstraVis[u] = false;
}

/*!
 * @brief generate a random RI set and insert it into R.
 * @param G : the graph
 */
void insert_R(Graph &G) {
    vector<node> RR;
    uniform_int_distribution<node> uniformIntDistribution(0, G.n - 1);
    vector<node> vStart = {uniformIntDistribution(random_engine)};
    RI_Gen(G, vStart, RR, true);
    RRI.emplace_back(RR);
    for (node u : RR) {
        covered[u].emplace_back(RR.size() - 1);
        coveredNum[u]++;
    }
}

/*!
 * @brief generate FI sketches to evaluate the influence spread.
 * @param graph : the graph that define propagation models(IC-M)
 * @param S : the seed set
 * @return the estimated value of influence spread
 */
double FI_simulation(Graph &graph, vector<node> &S) {
    vector<node> RR;
    double res = 0, cur = clock();
    for (int i = 1; i <= MC_iteration_rounds; i++) {
        RI_Gen(graph, S, RR, false);
        res += (double) RR.size() / MC_iteration_rounds;
    }
    if (verbose_flag) printf("\t\tresult=%.3f time=%.3f\n", res, time_by(cur));
    return res;
}

/*!
 * @brief temporary array for IMM_NodeSelection
 *
 * RIsetCovered : marking the RI sets that has been covered by S
 * nodeRemain : check if an node is remained in the heap
 * covered_num_tmp : a copy of covered_num
 */
bool nodeRemain[MAX_NODE_SIZE];
int64 coveredNum_tmp[MAX_NODE_SIZE];

/*!
 * @brief Selection phase of IMM : Select a set S of size k that covers the maximum RI sets in R
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of S
 * @param S : returns S as an passed parameter
 * @return : the fraction of RI sets in R that are covered by S
 */
double IMMNodeSelection(Graph &graph, vector<node> &candidate, int32 k, vector<node> &S) {
    S.clear();
    vector<bool> RIsetCovered(RRI.size(), false);
    for (node i : candidate) nodeRemain[i] = true;
    memcpy(coveredNum_tmp, coveredNum, graph.n * sizeof(int64));
    priority_queue<pair<int64, node>> Q;
    for (node i : candidate) Q.push(make_pair(coveredNum_tmp[i], i));
    int64 influence = 0;

    while (S.size() < k && !Q.empty()) {
        int64 value = Q.top().first;
        node maxInd = Q.top().second;
        Q.pop();
        if (value > coveredNum_tmp[maxInd]) {
            Q.push(make_pair(coveredNum_tmp[maxInd], maxInd));
            continue;
        }
        influence += coveredNum_tmp[maxInd];
        S.emplace_back(maxInd);
        nodeRemain[maxInd] = false;
        for (int64 RIIndex : covered[maxInd]) {
            if (RIsetCovered[RIIndex]) continue;
            for (node u : RRI[RIIndex]) {
                if (nodeRemain[u]) coveredNum_tmp[u]--;
            }
            RIsetCovered[RIIndex] = true;
        }
    }
    for (node i : candidate) nodeRemain[i] = false;
    return (double) influence / RRI.size();
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets into R.
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy.
 * @param iota : argument related to accuracy.
 */
void IMMSampling(Graph &graph, vector<node> &candidate, int32 k, double eps, double iota) {
    double epsilon_prime = eps * sqrt(2);
    double LB = 1;
    vector<node> S_tmp;
    auto End = (int) (log2(graph.n) + 1e-9 - 1);
    for (int i = 1; i <= End; i++) {
        auto ci = (int64) ((2.0 + 2.0 / 3.0 * epsilon_prime) *
                           (iota * log(graph.n) + Math::logcnk(graph.n, k) + log(Math::log2(graph.n))) / Math::sqr(epsilon_prime) *
                           pow(2.0, i));
        if(ci > (int64)100000000) break;
        while (RRI.size() < ci)
            insert_R(graph);

        double ept = IMMNodeSelection(graph, candidate, k, S_tmp);
        if (ept > 1.0 / pow(2.0, i)) {
            LB = ept * graph.n / (1.0 + epsilon_prime);
            break;
        }
    }
    double e = exp(1);
    double alpha = sqrt(iota * log(graph.n) + log(2));
    double beta = sqrt((1.0 - 1.0 / e) * (Math::logcnk(graph.n, k) + iota * log(graph.n) + log(2)));
    auto C = (int64) (2.0 * graph.n * Math::sqr((1.0 - 1.0 / e) * alpha + beta) / LB / Math::sqr(eps));
    C = min(C, (int64)100000000);
    while (RRI.size() < C)
        insert_R(graph);
    if (verbose_flag) {
        cout << "\tfinal C = " << C << endl;
    }
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets.
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy. default as 0.5.
 * @param iota : argument related to accuracy. default as 1.
 * @param G_flag : determine if it is a version of IMM-G
 */
void IMM(Graph &G, vector<node> &candidate, int32 k, double eps, double iota, vector<node> &S) {
    //init_R();
    double iota_new = iota * (1.0 + log(2) / log(G.n));
//    while (RRI.size() < 200000)
//        insert_R(G);
    IMMSampling(G, candidate, k, eps, iota_new);
    IMMNodeSelection(G, candidate, k, S);
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : IMM
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void IMM_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    double cur = clock();
    set<node> seeds_reorder;
    for (node u : A) {
        vector<node> neighbours, one_seed;
        for (auto &edge : graph.g[u]) {
            if (find(A.begin(), A.end(), edge.v) == A.end()) {
                neighbours.emplace_back(edge.v);
            }
        }
        IMM(graph, neighbours, k, 0.5, 1, one_seed);
        for (node w : one_seed)
            seeds_reorder.insert(w);
    }
    for (node w : seeds_reorder) seeds.emplace_back(w);
    seeds_reorder.clear();
    if (verbose_flag) printf("IMM method done. total time = %.3f\n", time_by(cur));
}


/*!
 * @brief Selection phase of advanced IMM.
 * @param graph : the graph
 * @param A : active participant set
 * @param k : the size of S
 * @param S : returns S as an passed parameter
 * @return : the fraction of RI sets in R that are covered by S
 */
double IMMNodeSelection_advanced(Graph &graph, vector<node> &A, int32 k, vector<node> &S) {
    S.clear();
    set<node> N; //candidate neighbour set
    auto *num_neighbours = new node[graph.n]();
    auto *f = new vector<node>[graph.n](); //f[v] means in-coming active participant of v
    //push all candidate neighbour to S, and update f
    for (node u : A) {
        for (auto &edge : graph.g[u]) {
            if (find(A.begin(), A.end(), edge.v) == A.end()) {
                N.insert(edge.v);
                f[edge.v].emplace_back(u);
            }
        }
    }
    for (node w : S) {
        shuffle(f[w].begin(), f[w].end(), std::mt19937(std::random_device()()));
    }
    vector<bool> RIsetCovered(RRI.size(), false);
    for (node i : N) nodeRemain[i] = true;
    memcpy(coveredNum_tmp, coveredNum, graph.n * sizeof(int64));
    priority_queue<pair<int64, node>> Q;
    for (node i : N) Q.push(make_pair(coveredNum_tmp[i], i));
    int64 influence = 0;

    while (!Q.empty()) {
        int64 value = Q.top().first;
        node maxInd = Q.top().second;
        Q.pop();

        node u0 = source_participant(maxInd, f, k, num_neighbours);
        if (u0 == -1) continue;

        if (value > coveredNum_tmp[maxInd]) {
            Q.push(make_pair(coveredNum_tmp[maxInd], maxInd));
            continue;
        }
        influence += coveredNum_tmp[maxInd];
        S.emplace_back(maxInd);
        num_neighbours[u0]++;
        nodeRemain[maxInd] = false;
        for (int64 RIIndex : covered[maxInd]) {
            if (RIsetCovered[RIIndex]) continue;
            for (node u : RRI[RIIndex]) {
                if (nodeRemain[u]) coveredNum_tmp[u]--;
            }
            RIsetCovered[RIIndex] = true;
        }
    }
    for (node i : N) nodeRemain[i] = false;
    delete[] num_neighbours;
    delete[] f;
    return (double) influence / RRI.size();
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets into R.
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy.
 * @param iota : argument related to accuracy.
 */
void IMMSampling_advanced(Graph &graph, vector<node> &A, int32 k, double eps, double iota) {
    int32 kA = 0;
    for (node u : A) kA += min(k, (int32) graph.g[u].size());
    kA = min(kA, (int32)graph.n);
    double epsilon_prime = eps * sqrt(2);
    double LB = 1;
    vector<node> S_tmp;
    auto End = (int) (log2(graph.n) + 1e-9 - 1);
    for (int i = 1; i <= End; i++) {
        auto ci = (int64) ((2.0 + 2.0 / 3.0 * epsilon_prime) *
                           (iota * log(graph.n) + Math::logcnk(graph.n, kA) + log(Math::log2(graph.n))) *
                           pow(2.0, i) / Math::sqr(epsilon_prime));
        if (verbose_flag) cout << "\tci = " << ci << endl;
        while (RRI.size() < ci)
            insert_R(graph);

        double ept = IMMNodeSelection_advanced(graph, A, k, S_tmp);
        if (ept > 1.0 / pow(2.0, i)) {
            LB = ept * graph.n / (1.0 + epsilon_prime);
            break;
        }
    }
    double e = exp(1);
    double alpha = sqrt(iota * log(graph.n) + log(2));
    double beta = sqrt((1.0 - 1.0 / e) * (Math::logcnk(graph.n, kA) + iota * log(graph.n) + log(2)));
    auto C = (int64) (2.0 * graph.n * Math::sqr((1.0 - 1.0 / e) * alpha + beta) / LB / Math::sqr(eps));
    while (RRI.size() < C)
        insert_R(graph);
    if (verbose_flag) cout << "\tfinal C = " << C << endl;
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets.
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy. default as 0.5.
 * @param iota : argument related to accuracy. default as 1.
 * @param G_flag : determine if it is a version of IMM-G
 */
void IMM_advanced(Graph &G, vector<node> &A, int32 k, double eps, double iota, vector<node> &S) {
    //init_R();
    double iota_new = iota * (1.0 + log(2) / log(G.n));
    IMMSampling_advanced(G, A, k, eps, iota_new);
    IMMNodeSelection_advanced(G, A, k, S);
}

void advanced_IMM_method(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds) {
    double cur = clock();
    IMM_advanced(graph, A, k, 0.5, 1, seeds);
    if(verbose_flag) printf("IMM advanced done. total time = %.3f\n", time_by(cur));
}

#endif //SIMULATION_H_IMM_H
