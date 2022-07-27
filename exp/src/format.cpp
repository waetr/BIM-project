#include "top.h"

int64 reIndex[MAX_NODE_SIZE], cnt = 0;

int main(int argc, char const *argv[]){
    memset(reIndex, -1, sizeof (reIndex));
    init_commandLine(argc, argv);
    Graph G(graphFilePath, DIRECTED_G);
    out.open("../data/soc-LiveJournal1.csv", ios::app);
    for(int i = 0; i < G.n; i++){
        if(!G.deg_in[i] && !G.deg_out[i]) continue;
        reIndex[i] = cnt++;
    }
    for(int i = 0; i < G.n; i++){
        if(reIndex[i] == -1) continue;
        for(auto &e : G.g[i]) {
            out << reIndex[i] << "," << reIndex[e.v] << endl;
        }
    }
    return 0;
}