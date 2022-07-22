#include "argparse.h"
#include "top.h"

int main(int argc, char const *argv[]){
    init_commandLine(argc, argv);
    Graph G("../data/test.csv", UNDIRECTED_G);
    G.set_diffusion_model(IC_M, 15);
    vector<int> A={3,6,5,2,1,0,8,7},seeds;
    CELF_method(G,2,A,seeds);
    print_set(seeds);
    return 0;
}