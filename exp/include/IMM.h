//
// Created by asuka on 2022/7/24.
//

#ifndef SIMULATION_H_IMM_H
#define SIMULATION_H_IMM_H

#include "graph.h"

///set of RI sets
vector<vector<int>> R;

///covered[u] marks which RI sets the node u is covered by
vector<int> covered[MAX_NODE_SIZE];
///coveredNum[u] marks how many RI sets the node u is covered by
int coveredNum[MAX_NODE_SIZE];

/*!
 * @brief Empty the set R.
 */
void init_R() {
    R.clear();
    for (auto &i : covered) i.clear();
    memset(coveredNum, 0, sizeof(coveredNum));
}

/*!
 * @brief insert a RR set into R.
 * @param RR
 */
void insert_R(vector<int> &RR) {
    R.emplace_back(RR);
    for (int u : RR) {
        covered[u].emplace_back(RR.size() - 1);
        coveredNum[u]++;
    }
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
bool RIsetCovered[MAX_NODE_SIZE];
bool nodeRemain[MAX_NODE_SIZE];
int coveredNum_tmp[MAX_NODE_SIZE];

/*!
 * @warning This function is not tested!
 * @brief Select a set S of size k that covers the maximum RI sets in R
 * @param graph : the graph
 * @param k : the size of S
 * @param S : returns S as an passed parameter
 * @return : the fraction of RI sets in R that are covered by S
 */
double IMM_NodeSelection(Graph &graph, int k, vector<int> &S) {
    S.clear();
    for (int i = 0; i < R.size(); i++) RIsetCovered[i] = false;
    for (int i = 0; i < graph.n; i++) nodeRemain[i] = true;
    memcpy(coveredNum_tmp, coveredNum, graph.n * sizeof(int));
    priority_queue<pair<int, int>> Q;
    for (int i = 0; i < graph.n; i++) Q.push(make_pair(coveredNum_tmp[i], i));
    int influence = 0;

    while (S.size() < k) {
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
            for (int u : R[RIIndex]) {
                if (nodeRemain[u]) coveredNum_tmp[u]--;
            }
            RIsetCovered[RIIndex] = true;
        }
    }
    return (double) influence / R.size();
}

#endif //SIMULATION_H_IMM_H
