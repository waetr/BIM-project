//
// Created by lenovo on 2022/7/19.
//

#ifndef EXP_TOP_H
#define EXP_TOP_H

#include "IMs.h"

double solvers(Graph &graph, int k, vector<int> &A, vector<int> &seeds, IM_solver solver) {
    double cur = clock();
    seeds.clear();
    switch (solver) {
        case ENUMERATION:
            enumeration_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using enumeration: ");
            break;
        case DEGREE:
            degree_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using degree: ");
            break;
        case PAGERANK:
            pgrank_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using pagerank: ");
            break;
        case CELF_NORMAL:
            CELF_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using CELF: ");
            break;
        case DEGREE_ADVANCED:
            advanced_degree_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED degree: ");
            break;
        case PAGERANK_ADVANCED:
            advanced_pgrank_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED pagerank: ");
            break;
        case CELF_ADVANCED:
            advanced_CELF_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED CELF: ");
            break;
        default:
            break;
    }
    if (verbose_flag) printf(" total time = %.3f\n", time_by(cur));
    else puts("");
    return time_by(cur);
}

bool tooLarge(Graph &g, vector<int> &A) {
    set<int> S;
    for (int u : A)
        for (auto e : g.g[u]) S.insert(e.v);
    return (S.size() >= 50 * A.size());
}

void Run_simulation(vector<int> &A_batch, vector<int> &k_batch, model_type type, int rounds = 3) {
    ofstream out("../output/result.out", ios::app);
    //load graph from absolute path
    Graph G("../data/edges.csv", UNDIRECTED_G);

    //set diffusion model
    G.set_diffusion_model(type, 15);

    //Instantiate the active participant set A and seed set
    vector<int> A, seeds;

    double result[7][100], timer[7][100];

    string solver_name[] = {"ENUMERATION", "DEGREE", "PAGERANK", "CELF", "DEGREE_ADVANCED", "PAGERANK_ADVANCED",
                            "CELF_ADVANCED"};

    vector<IM_solver> solver_ = {DEGREE, PAGERANK, CELF_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, CELF_ADVANCED};

    int random_set[rounds][G.n];
    for (int i = 0; i < rounds; i++) {
        for (int j = 0; j < G.n; j++) random_set[i][j] = j;
        shuffle(random_set[i], random_set[i] + G.n, std::mt19937(std::random_device()()));
    }

    for (int A_size : A_batch) {
        double overlap_ratio = 0;
        memset(result, 0, sizeof(result));
        memset(timer, 0, sizeof(timer));
        for (int r_ = 1; r_ <= rounds; r_++) {
            A.clear();
            for(int j = 0; j < A_size; j++) A.emplace_back(random_set[r_-1][j]);
            print_set(A, "active participant: "), puts("");
            overlap_ratio += estimate_neighbor_overlap(G, A) / rounds;
            for (int k : k_batch) {
                printf("Working on A_size = %d, round = %d, k = %d\n", A_size, r_, k);
                for (IM_solver solver_used : solver_) {
                    timer[solver_used][k] += solvers(G, k, A, seeds, solver_used) / rounds;
                    result[solver_used][k] += MC_simulation(G, seeds) / rounds;
                }
            }
        }
        printf("*************\nSize %d DONE!\n*************\n", A_size);

        cout << "Active participant size = " << A_size << endl;
        cout << "Mean overlap ratio = " << overlap_ratio << endl;
        for (int k : k_batch) {
            cout << "\tk = " << k << endl;
            for (IM_solver solver_used : solver_) {
                cout << "\t\tseed quality of " << solver_name[solver_used] << " = " << result[solver_used][k];
                cout << " mean time = " << timer[solver_used][k] << endl;
            }
        }

        out << "Active participant size = " << A_size << endl;
        out << "Mean overlap ratio = " << overlap_ratio << endl;
        for (int k : k_batch) {
            out << "\tk = " << k << endl;
            for (IM_solver solver_used : solver_) {
                out << "\t\tseed quality of " << solver_name[solver_used] << " = " << result[solver_used][k];
                out << " mean time = " << timer[solver_used][k] << endl;
            }
        }
    }
    out.close();
}

#endif //EXP_TOP_H
