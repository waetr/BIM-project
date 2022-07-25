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
        return ans;
    }

    template<class T>
    static T sqr(T x) {
        return x * x;
    }
};

///set of nodes that can be selected as seed set
vector<int> IMMCandidate;

///set of RI sets
vector<vector<int>> RRI;

///covered[u] marks which RI sets the node u is covered by
vector<int> covered[MAX_NODE_SIZE];
///coveredNum[u] marks how many RI sets the node u is covered by
int coveredNum[MAX_NODE_SIZE];

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
int dist[MAX_NODE_SIZE + 1];
bool DijkstraVis[MAX_NODE_SIZE];

/*!
 * @brief Algorithm for CTIC to generate Reserve-Influence or Forward-Influence set of IMM.
 * @param graph : the graph
 * @param uStart : the starting nodes of this RI/FI set
 * @param RR : returns the RI/FI set as an passed parameter
 * @param RI_flag : determine which type of set to generate. true as RI, false as FI.
 *
 */
void RI_Gen(Graph &graph, vector<int> &uStart, vector<int> &RR, bool RI_flag) {
    if (graph.diff_model != IC_M) {
        cerr << "RI_Gen must be used for IC-M model!\n";
        exit(-1);
    }
    if (dist[MAX_NODE_SIZE] != -1) {
        //Initialize dist[] to be all -1 at the first time
        memset(dist, -1, sizeof(dist));
    }
    const int Inf = graph.deadline + 1;
    auto *edge_list = RI_flag ? &graph.gT : &graph.g;
    for (int u : uStart)
        dist[u] = 0;
    RR.clear();
    priority_queue<pair<int, int>> Q;
    for (int u : uStart)
        Q.push(make_pair(0, u));
    while (!Q.empty()) { //Dijkstra Algorithm
        int u = Q.top().second;
        Q.pop();
        if (DijkstraVis[u]) continue;
        DijkstraVis[u] = true;
        RR.emplace_back(u);
        for (auto &edgeT : (*edge_list)[u]) {
            geometric_distribution<int> distribution(edgeT.m);
            bool activate_success = (random_real() < edgeT.p);
            int randomWeight = activate_success ? (distribution(random_engine) + 1) : Inf;
            if ((dist[edgeT.v] == -1 || dist[edgeT.v] > dist[u] + randomWeight) &&
                dist[u] + randomWeight <= graph.deadline) {
                dist[edgeT.v] = dist[u] + randomWeight;
                Q.push(make_pair(-dist[edgeT.v], edgeT.v));
            }
        }
    }
    for (auto u : RR)
        dist[u] = -1, DijkstraVis[u] = false;
}

/*!
 * @brief generate a random RI set and insert it into R.
 * @param G : the graph
 */
void insert_R(Graph &G) {
    vector<int> RR;
    uniform_int_distribution<int> uniformIntDistribution(0, G.n - 1);
    vector<int> vStart = {uniformIntDistribution(random_engine)};
    RI_Gen(G, vStart, RR, true);
    RRI.emplace_back(RR);
    for (int u : RR) {
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
double FI_simulation(Graph &graph, vector<int> &S) {
    vector<int> RR;
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
bool RIsetCovered[3000010];
bool nodeRemain[MAX_NODE_SIZE];
int coveredNum_tmp[MAX_NODE_SIZE];

/*!
 * @brief Selection phase of IMM : Select a set S of size k that covers the maximum RI sets in R
 * @param graph : the graph
 * @param k : the size of S
 * @param S : returns S as an passed parameter
 * @return : the fraction of RI sets in R that are covered by S
 */
double IMMNodeSelection(Graph &graph, int k, vector<int> &S) {
    S.clear();
    for (int i = 0; i < RRI.size(); i++) RIsetCovered[i] = false;
    for (int i : IMMCandidate) nodeRemain[i] = true;
    memcpy(coveredNum_tmp, coveredNum, graph.n * sizeof(int));
    priority_queue<pair<int, int>> Q;
    for (int i : IMMCandidate) Q.push(make_pair(coveredNum_tmp[i], i));
    int influence = 0;

    while (S.size() < k && !Q.empty()) {
        int value = Q.top().first, maxInd = Q.top().second;
        Q.pop();
        if (value > coveredNum_tmp[maxInd]) {
            Q.push(make_pair(coveredNum_tmp[maxInd], maxInd));
            continue;
        }
        influence += coveredNum_tmp[maxInd];
        S.emplace_back(maxInd);
        nodeRemain[maxInd] = false;
        for (int RIIndex : covered[maxInd]) {
            if (RIsetCovered[RIIndex]) continue;
            for (int u : RRI[RIIndex]) {
                if (nodeRemain[u]) coveredNum_tmp[u]--;
            }
            RIsetCovered[RIIndex] = true;
        }
    }
    for (int i : IMMCandidate) nodeRemain[i] = false;
    return (double) influence / RRI.size();
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets into R.
 * @param graph : the graph
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy.
 * @param iota : argument related to accuracy.
 */
void IMMSampling(Graph &graph, int k, double eps, double iota) {
    double epsilon_prime = eps * sqrt(2);
    double LB = 1;
    vector<int> S_tmp;
    auto End = (int) (log2(graph.n) + 1e-9 - 1);
    for (int i = 1; i <= End; i++) {
        auto ci = (long long) ((2.0 + 2.0 / 3.0 * epsilon_prime) *
                               (iota * log(graph.n) + Math::logcnk(graph.n, k) + log(Math::log2(graph.n))) *
                               pow(2.0, i) / Math::sqr(epsilon_prime));
        while (RRI.size() < ci)
            insert_R(graph);

        double ept = IMMNodeSelection(graph, k, S_tmp);
        if (ept > 1 / pow(2.0, i)) {
            LB = ept * graph.n / (1.0 + epsilon_prime);
            break;
        }
    }
    double e = exp(1);
    double alpha = sqrt(iota * log(graph.n) + log(2));
    double beta = sqrt((1.0 - 1.0 / e) * (Math::logcnk(graph.n, k) + iota * log(graph.n) + log(2)));
    auto C = (long long) (2.0 * graph.n * Math::sqr((1.0 - 1.0 / e) * alpha + beta) / (LB * Math::sqr(eps)));
    while (RRI.size() < C)
        insert_R(graph);
}

/*!
 * @brief Sampling phase of IMM : generate sufficient RI sets.
 * @param graph : the graph
 * @param candidate : candidate node set in which seed set can choose nodes
 * @param k : the size of the seed set
 * @param eps : argument related to accuracy. default as 0.5.
 * @param iota : argument related to accuracy. default as 1.
 */
void IMM(Graph &G, vector<int> &candidate, int k, double eps, double iota, vector<int> &S) {
    IMMCandidate = candidate;
    init_R();
    double iota_new = iota * (1.0 + log(2) / log(G.n));
    IMMSampling(G, k, eps, iota_new);
    IMMNodeSelection(G, k, S);
}

/*!
 * @brief Encapsulated operations for Option 2 using IM solver : IMM
 * @param graph : the graph
 * @param k : the number in the problem definition
 * @param A : the active participant set A
 * @param seeds : returns the seed set S = {S_1, S_2, ..., S_n}
 */
void IMM_method(Graph &graph, int k, vector<int> &A, vector<int> &seeds) {
    double cur = clock();
    set<int> seeds_reorder;
    for (int u : A) {
        vector<int> neighbours, one_seed;
        for (auto &edge : graph.g[u]) {
            if(find(A.begin(), A.end(), edge.v) == A.end()) {
                neighbours.emplace_back(edge.v);
            }
        }
        IMM(graph, neighbours, k, 0.5, 1, one_seed);
        for (int w : one_seed)
            seeds_reorder.insert(w);
    }
    for (int w : seeds_reorder) seeds.emplace_back(w);
    seeds_reorder.clear();
    if(verbose_flag) printf("IMM method done. total time = %.3f\n", time_by(cur));
}

#endif //SIMULATION_H_IMM_H
