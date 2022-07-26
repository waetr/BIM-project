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

/*!
 * @brief triple<int, double, double> to represent an edge
 * first argument : index of outgoing node
 * second argument : p_{u,v} in IC/WC model
 * third argument : m_{u,v} in IC-M model
 */
struct Edge {
    node v;
    double p, m;

    Edge() {}

    Edge(node v, double p, double m) : v(v), p(p), m(m) {}
};

const vector<Edge> gg(0);

class Graph {
public:
    /*!
     * @param n : maximum index of node
     * @param m : number of edges
     * @param g : adjacency list
     */
    node n;
    int64 m, deadline;
    vector<vector<Edge> > g, gT;
    vector<node> deg_in, deg_out;
    model_type diff_model;

    /*!
     * @brief Init the graph for a default size.
     */
    Graph() {
        n = m = deadline = 0;
        g.clear();
        gT.clear();
        diff_model = NONE;
    }

    /*!
     * @brief A destructor for graph.
    */
    ~Graph() = default;

    /*!
     * @brief Copy constructor.
     */
    Graph(Graph &g) {
        n = g.n;
        m = g.m;
        deadline = g.deadline;
        diff_model = g.diff_model;
        this->g = g.g;
        this->gT = g.gT;
        this->deg_in = g.deg_in;
        this->deg_out = g.deg_out;
    }

    /*!
     * @brief add an weighted edge into graph.
     * @param source : source node
     * @param target : destination node
     * @param weight : weight of edge, 1.0 as default
     */
    void add_edge(node source, node target, double weight = 1.0) {
        n = max(n, max(source, target) + 1);
        while (g.size() < n) {
            g.emplace_back(gg);
            gT.emplace_back(gg);
            deg_in.emplace_back(0);
            deg_out.emplace_back(0);
        }
        m++;
        deg_in[target]++;
        deg_out[source]++;
        g[source].emplace_back(Edge(target, weight, weight));
        gT[target].emplace_back(Edge(source, weight, weight));
    }

/*!
 * @brief load a graph through file
 * @param filename : the path of the file
 * @param type : detrmine the graph type is directed or undirected
 */
    void open(const string &filename, graph_type type) {
        vector<pair<node, node>> edges;
        ifstream inFile(filename, ios::in);
        if (!inFile.is_open()) {
            std::cerr << "(get error) graph file not found: " << filename << std::endl;
            std::exit(-1);
        }
        string lineStr;
        while (getline(inFile, lineStr)) {
            stringstream ss(lineStr);
            string str;
            node x = -1, y;
            while (getline(ss, str, ','))
                if (x == -1) x = stoi(str);
                else {
                    y = stoi(str);
                    edges.emplace_back(x, y);
                    x = -1;
                }
        }
        inFile.close();
        for (auto e : edges) {
            add_edge(e.first, e.second);
            if (type == UNDIRECTED_G) add_edge(e.second, e.first);
        }
    }

    Graph(const string &filename, graph_type type) : Graph() {
        this->open(filename, type);
    }

    /*!
     * @brief Set the diffusion model to IC/LT. If you modify the graph later, you need to set it again.
     * @param new_type : the name of the diffusion model.
     */
    void set_diffusion_model(model_type new_type, int64 new_deadline = 0) {
        diff_model = new_type;
        if (new_type == IC) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < g[i].size(); j++)
                    g[i][j].p = 1.0 / deg_in[g[i][j].v];
            }
        } else if (new_type == IC_M) {
            double sum_m = 0, sum_p = 0;
            int64 num_edges = 0;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < g[i].size(); j++) {
                    g[i][j].p = 1.0 / deg_in[g[i][j].v];
                    g[i][j].m = 5.0 / (5.0 + deg_out[i]);
                    sum_m += g[i][j].m;
                    sum_p += g[i][j].p;
                    num_edges++;
                }
            }
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < gT[i].size(); j++) {
                    gT[i][j].p = 1.0 / deg_in[i];
                    gT[i][j].m = 5.0 / (5.0 + deg_out[g[i][j].v]);
                }
            }
            deadline = new_deadline;
            if (verbose_flag) {
                cout << "average activate probability = " << sum_p / num_edges << endl;
                cout << "average meeting probability = " << sum_m / num_edges << endl;
            }
        }
    }
};

#endif //UNTITLED_GRAPH_H
