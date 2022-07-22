#include "argparse.h"
#include "top.h"

int main(int argc, char const *argv[]){
    init_commandLine(argc, argv);
    Graph G("../data/edges.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> A={1010,2940,3148,6952,7668},seeds;
    double cur = 0;
    advanced_CELF_method(G,10,A,seeds);
    cout << time_by(cur) << endl;
    print_set(seeds);
    return 0;
}