//
// Created by asuka on 2022/7/15.
//

#ifndef EXP_MODELS_H
#define EXP_MODELS_H
#include <vector>

#define MAX_NODE_SIZE 2000000
#define graph_type int
#define DIRECTED_G 0
#define UNDIRECTED_G 1

#define model_type int
#define NONE 0
#define IC 1
#define LT 2
#define IC_M 3

/*!
 * @brief calculate the interval from start time.
 * @param start : start timestamp
 * @return the length of the interval
 */
double time_by(double start) {
    return (clock() - start) / CLOCKS_PER_SEC;
}

/*!
 * @brief Print all elements in a vector.
 * @param S : the set
 */
void print_set(std::vector<int> &S, const std::string &Prefix = "") {
    std::cout << Prefix;
    std::cout << "{";
    for (int i = 0; i < S.size(); i++) {
        std::cout << S[i];
        if (i != S.size() - 1) std::cout << ",";
    }
    std::cout << "} ";
}

#endif //EXP_MODELS_H
