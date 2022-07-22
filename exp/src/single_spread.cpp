//
// Created by asuka on 2022/7/22.
//

#include <iomanip>
#include "top.h"

int main() {
    out.open("../data/edges_mg.txt");
    Graph G("../data/edges.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> seeds;
    seeds.resize(1);
    for (int u = 0; u < G.n; u++) {
        seeds[0] = u;
        out << setprecision(10) << MC_simulation(G, seeds, 20000) << endl;
        cout << "Calculating : " << u << endl;
    }
    out.close();
    return 0;
}