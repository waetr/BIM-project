//
// Created by asuka on 22-7-14.
//

#ifndef UNTITLED_GRAPH_H
#define UNTITLED_GRAPH_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include "models.h"

using namespace std;
const vector<pair<int, double> > gg(0);

class Graph {
public:
    /*!
     * @param n : maximum index of node
     * @param m : number of edges
     * @param g : adjacency list
     */
    int n, m;
    vector<vector<pair<int, double> > > g;
    vector<int> deg_in, deg_out;
    model_type diff_model;

    /*!
     * @brief Init the graph for a default size.
     */
    Graph() {
        n = m = 0;
        g.clear();
        diff_model = NONE;
    }

    /*!
     * @brief A destructor for graph.
    */
    ~Graph() {}

    /*!
     * @brief Copy constructor.
     */
    Graph(Graph &g) {
        n = g.n;
        m = g.m;
        diff_model = g.diff_model;
        this->g = g.g;
        this->deg_in = g.deg_in;
        this->deg_out = g.deg_out;
    }

    /*!
     * @brief add an weighted edge into graph.
     * @param source : source node
     * @param target : destination node
     * @param weight : weight of edge, 1.0 as default
     */
    void add_edge(int source, int target, double weight = 1.0) {
        n = max(n, max(source, target) + 1);
        while (g.size() < n) {
            g.emplace_back(gg);
            deg_in.emplace_back(0);
            deg_out.emplace_back(0);
        }
        m++;
        deg_in[target]++;
        deg_out[source]++;
        g[source].emplace_back(make_pair(target, weight));
    }

    /*!
     * load graph through a file
     * @param filename : the name of loading file
     */
    Graph(const string& filename, graph_type type) : Graph() {
        vector<pair<int, int>> edges;
        ifstream inFile(filename, ios::in);
        string lineStr;
        while (getline(inFile, lineStr)) {
            stringstream ss(lineStr);
            string str;
            int x = -1, y;
            while (getline(ss, str, ','))
                if (x == -1) x = stoi(str);
                else {
                    y = stoi(str);
                    edges.emplace_back(x, y);
                    x = -1;
                }
        }
        inFile.close();
        for (pair<int, int> e:edges) {
            add_edge(e.first, e.second);
            if(type == UNDIRECTED_G) add_edge(e.second, e.first);
        }
    }

    /*!
     * @brief Set the diffusion model to IC/LT. If you modify the graph later, you need to set it again.
     * @param new_type : the name of the diffusion model.
     */
    void set_diffusion_model(model_type new_type) {
        if(new_type == IC) {
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < g[i].size(); j++)
                    g[i][j].second = 1.0 / deg_in[g[i][j].first];
            }
        }
    }
};

struct Node{
public:
    int vertex;
    double value;
    Node() = default;
    Node(int v, double p) : vertex(v), value(p) {}
    bool operator < (const Node &other) const {
        if(value == other.value)
            return vertex < other.vertex;
        else
            return value < other.value;
    }
    bool operator > (const Node &other) const {
        if(value == other.value)
            return vertex > other.vertex;
        else
            return value > other.value;
    }
    bool operator != (const Node &a) const {
        return a.vertex != vertex;
    }
};


#endif //UNTITLED_GRAPH_H
