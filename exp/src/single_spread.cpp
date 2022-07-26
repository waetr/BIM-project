//
// Created by asuka on 2022/7/22.
//

#include <iomanip>
#include "top.h"

int main(int argc, char const *argv[]) {
    init_commandLine(argc, argv);
    out.open("../data/dblp_mg.txt");
    Graph G("../data/formal-com-dblp.csv", DIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<node> seeds;
    seeds.resize(1);
    for (node u = 0; u < G.n; u++) {
        seeds[0] = u;
        out << setprecision(10) << FI_simulation(G, seeds) << endl;
        cout << "Calculating : " << u << endl;
    }
    out.close();
    return 0;
}