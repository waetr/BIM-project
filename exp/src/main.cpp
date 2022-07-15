
#include "simulation.h"

int main() {
    //option 1
    //set k
    k0 = 1;

    //load graph from absolute path
    Graph g(R"(D:\BIM-project\BIM-project\exp\data\edges.csv)", UNDIRECTED_G);

    //set diffusion model
    g.set_diffusion_model(IC);

    //set the active participant set A
    set<int> A;
    generate_seed(g, A, 1);

    //V_n to store all possible neighbour set S
    vector<set<int> > V_n;

    //generate all collections recursively
    select_neighbours(g, A, V_n, 0, 0, A.begin(), true);
    cout << "number of S_n : " << V_n.size() << endl;

    double answer = 0;
    for (auto &S_n : V_n) {
        cout << "set S = {";
        for (auto u : S_n) cout << u << ",";
        cout << "} ";
        //for each set, calculate its influence spread
        double x = MC_simulation(g, S_n);
        cout << "simulation result:" << x << endl;
        answer = max(answer, x);
    }
    cout << "\nfinal answer = " << answer << endl;
    return 0;
}