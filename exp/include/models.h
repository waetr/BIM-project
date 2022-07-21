//
// Created by asuka on 2022/7/15.
//

#ifndef EXP_MODELS_H
#define EXP_MODELS_H
#include <vector>
#include <random>
#include <iostream>

#define verbose_flag 1

#define MAX_NODE_SIZE 20000
#define graph_type int
#define DIRECTED_G 0
#define UNDIRECTED_G 1

#define model_type int
#define NONE 0
#define IC 1
#define LT 2
#define IC_M 3

#define IM_solver int
#define ENUMERATION 0
#define DEGREE 1
#define PAGERANK 2
#define CELF_NORMAL 3
#define DEGREE_ADVANCED 4
#define PAGERANK_ADVANCED 5
#define CELF_ADVANCED 6

std::random_device rd__;
std::minstd_rand random_engine(rd__());
std::uniform_real_distribution<double> distrib(0.0, 1.0);

bool CELF_out_of_time = false;

/*!
 * @brief Random number generator that generates real number between [0,1)
 * @return A random real number between [0,1)
 */
inline double random_real() {
    return distrib(random_engine);
}

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
