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
            if (verbose_flag) print_set(seeds, "Seed set using enumeration: "), puts("");
            break;
        case DEGREE:
            degree_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using degree: "), puts("");
            break;
        case PAGERANK:
            pgrank_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using pagerank: "), puts("");
            break;
        case CELF_NORMAL:
            CELF_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using CELF: "), puts("");
            break;
        case DEGREE_ADVANCED:
            advanced_degree_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using ADVANCED degree: "), puts("");
            break;
        case PAGERANK_ADVANCED:
            advanced_pgrank_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using ADVANCED pagerank: "), puts("");
            break;
        case CELF_ADVANCED:
            advanced_CELF_method(graph, k, A, seeds);
            if (verbose_flag) print_set(seeds, "Seed set using ADVANCED CELF: "), puts("");
            break;
        default:
            break;
    }
    return time_by(cur);
}

bool tooLarge(Graph &g, vector<int> &A) {
    set<int> S;
    for (int u : A)
        for (auto e : g.g[u]) S.insert(e.v);
    return (S.size() >= 30 * A.size());
}

void Run_simulation(vector<int> &A_batch, vector<int> &k_batch, model_type type, int rounds = 3) {
    ofstream out;
    out.open("../output/result.out");
    //load graph from absolute path
    Graph G("../data/edges.csv", UNDIRECTED_G);

    //set diffusion model
    G.set_diffusion_model(type, 15);

    //Instantiate the active participant set A and seed set
    vector<int> A, seeds;

    double result[7][10], timer[7][10];

    string solver_name[] = {"ENUMERATION", "DEGREE", "PAGERANK", "CELF", "DEGREE_ADVANCED", "PAGERANK_ADVANCED",
                            "CELF_ADVANCED"};

    vector<IM_solver> solver_ = {DEGREE, PAGERANK, CELF_NORMAL, DEGREE_ADVANCED, PAGERANK_ADVANCED, CELF_ADVANCED};

    int BATCH_k_SIZE[] = {1, 2, 5, 10, 20};

    for (int A_size : A_batch) {
        double overlap_ratio = 0;
        memset(result, 0, sizeof(result));
        memset(timer, 0, sizeof(timer));
        for (int r_ = 1; r_ <= rounds; r_++) {
            cout << "round = " << r_ << endl;
            generate_seed(G, A, A_size);
            while(tooLarge(G, A) || estimate_neighbor_overlap(G, A) < 0.2) generate_seed(G, A, A_size);
            overlap_ratio += estimate_neighbor_overlap(G, A) / rounds;
            for (int k : k_batch) {
                for (IM_solver solver_used : solver_) {
                    timer[solver_used][k] += solvers(G, k, A, seeds, solver_used) / rounds;
                    result[solver_used][k] += MC_simulation(G, seeds) / rounds;
                }
            }
        }
        cout << "******DONE! " << A_size << endl;
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
