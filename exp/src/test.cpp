#include "top.h"

int main(int argc, char const *argv[]){
    init_commandLine(argc, argv);
    Graph G("../data/edges.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> A;
    for(int i = 5; i <= 30; i+=5) {
        generate_seed(G, A, i);
        double cur = clock();
        cout << "ap size = " << i << endl;
        cout << "MC simulation result = " << MC_simulation(G, A) << " time = " << time_by(cur) << endl;
        cur = clock();
        cout << "FI sketch simulation result = " << FI_simulation(G, A) << " time = " << time_by(cur) << endl;
    }
    return 0;
}