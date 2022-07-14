
#include "graph.h"

int main() {
    Graph g;
    openGraph(g);
    std::cout<<"Number of vertices: " << num_vertices(g) << "\n";
    std::cout<<"Number of edges: " << num_edges(g) << "\n";
    return 0;
}