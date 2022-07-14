//
// Created by asuka on 22-7-14.
//

#ifndef UNTITLED_GRAPH_H
#define UNTITLED_GRAPH_H
#include<boost/graph/adjacency_list.hpp>
#include<cstdio>
#include<iostream>
#include <fstream>
using namespace boost;
typedef boost::adjacency_list<listS, vecS, undirectedS> Graph;
typedef property_map<Graph, vertex_index_t>::type IndexMap;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;



void openGraph(Graph &g) {
    std::ifstream inFile("data/edges.csv", std::ios::in);
    std::string lineStr;
    while(std::getline(inFile, lineStr)) {
        std::stringstream ss(lineStr);
        std::string str;
        int x = -1, y;
        while(getline(ss,str,','))
            if(x == -1) x = stoi(str);
            else {
                y  = stoi(str);
                add_edge(x,y,g);
                x = -1;
            }
    }
    inFile.close();
}
#endif //UNTITLED_GRAPH_H
