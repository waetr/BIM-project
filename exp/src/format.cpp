//
// Created by lenovo on 2022/7/26.
//

#include <iomanip>
#include "top.h"
#include <map>

map<pair<node, node>, bool> h;

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    out.open("../data/blog-catalog.csv");
    Graph G("../data/edges.csv", UNDIRECTED_G);
    for (node u = 0; u < G.n; u++) {
        for(auto &edge : G.g[u]) {
                out << u << "," << edge.v << endl;
        }
    }
    out.close();
    return 0;
}