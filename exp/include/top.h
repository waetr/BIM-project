//
// Created by lenovo on 2022/7/19.
//

#ifndef EXP_TOP_H
#define EXP_TOP_H

#include "IMs.h"
#include "argparse.h"
#include "IMM.h"

string graphFilePath;

void init_commandLine(int argc, char const *argv[]) {
    auto args = util::argparser("The experiment of BIM.");
    args.set_program_name("exp")
            .add_help_option()
            .add_argument<std::string>("input", "initialize graph file")
            .add_option("-v", "--verbose", "output verbose message or not")
            .add_option<std::string>("-l", "--local", "use local value as single spread or not", "")
            .add_option<int64>("-r", "--rounds", "number of MC simulation iterations per time, default is 10000", 10000)
            .parse(argc, argv);
    graphFilePath = "../data/" + args.get_argument_string("input");
    if (args.has_option("--verbose")) {
        verbose_flag = 1;
        cout << "verbose flag set to 1\n";
    }
    if (!args.get_option_string("--local").empty()) {
        local_mg = 1;
        string MGPath = "../data/" + args.get_option_string("--local");
        cout << "local spread flag set to 1, file path = " << MGPath << endl;
        ifstream inFile(MGPath, ios::in);
        if (!inFile.is_open()) {
            std::cerr << "(get error) local file not found: " << args.get_option_string("--local") << std::endl;
            std::exit(-1);
        }
        int64 cnt = 0;
        while (inFile.good()) inFile >> MG0[cnt++];
        inFile.close();
    }
    MC_iteration_rounds = args.get_option_int64("--rounds");
    cout << "MC_iteration_rounds set to " << MC_iteration_rounds << endl;
}

double solvers(Graph &graph, int32 k, vector<node> &A, vector<node> &seeds, IM_solver solver) {
    double cur = clock();
    seeds.clear();
    switch (solver) {
        case ENUMERATION:
            enumeration_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using enumeration: ");
            //print_set_f(seeds, " Seed set using enumeration: ");
            break;
        case DEGREE:
            degree_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using degree: ");
            //print_set_f(seeds, " Seed set using degree: ");
            break;
        case PAGERANK:
            pgrank_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using pagerank: ");
            //print_set_f(seeds, " Seed set using pagerank: ");
            break;
        case CELF_NORMAL:
            CELF_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using CELF: ");
            //print_set_f(seeds, " Seed set using CELF: ");
            break;
        case DEGREE_ADVANCED:
            advanced_degree_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED degree: ");
            //print_set_f(seeds, " Seed set using ADVANCED degree: ");
            break;
        case PAGERANK_ADVANCED:
            advanced_pgrank_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED pagerank: ");
            //print_set_f(seeds, " Seed set using ADVANCED pagerank: ");
            break;
        case CELF_ADVANCED:
            advanced_CELF_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED CELF: ");
            //print_set_f(seeds, " Seed set using ADVANCED CELF: ");
            break;
        case IMM_NORMAL:
            IMM_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using IMM: ");
            //print_set_f(seeds, " Seed set using IMM: ");
            break;
        case IMM_ADVANCED:
            advanced_IMM_method(graph, k, A, seeds);
            print_set(seeds, " Seed set using ADVANCED IMM: ");
            //print_set_f(seeds, " Seed set using ADVANCED IMM: ");
            break;
        default:
            break;
    }
    if (verbose_flag) printf(" total time = %.3f\n", time_by(cur));
    else puts("");
    out << '\n';
    return time_by(cur);
}

void Run_simulation(vector<node> &A_batch, vector<int32> &k_batch, vector<IM_solver> &solver_batch, model_type type,
                    int32 rounds = 3) {
    fstream file_eraser("../output/result.out", ios::out);
    file_eraser.close();
    out.open("../output/result.out", ios::app);
    //load graph from absolute path
    Graph G(graphFilePath, DIRECTED_G);

    //set diffusion model
    G.set_diffusion_model(type, 15);

    //Instantiate the active participant set A and seed set
    vector<node> A, seeds;

    double result[12][500], timer[12][500], seedSize[12][500];

    string solver_name[] = {"ENUMERATION", "DEGREE", "PAGERANK", "CELF", "DEGREE_ADVANCED", "PAGERANK_ADVANCED",
                            "CELF_ADVANCED", "IMM_NORMAL", "IMM_ADVANCED"};


    for (node A_size : A_batch) {
        double overlap_ratio = 0;
        memset(result, 0, sizeof(result));
        memset(timer, 0, sizeof(timer));
        for (int32 r_ = 1; r_ <= rounds; r_++) {
            generate_seed(G, A, A_size);
            print_set(A, "active participant: "), puts("");
            print_set_f(A, "active participant: "), out << '\n';
            overlap_ratio += estimate_neighbor_overlap(G, A) / rounds;
            for (int32 k : k_batch) {
                cout << "Working on A_size = " << A_size << ", round = " << r_ << ", k = " << k << endl;
                out << "Working on A_size = " << A_size << ", round = " << r_ << ", k = " << k << endl;
                for (IM_solver solver_used : solver_batch) {
                    timer[solver_used][k] += solvers(G, k, A, seeds, solver_used) / rounds;
                    result[solver_used][k] += FI_simulation(G, seeds) / rounds;
                    seedSize[solver_used][k] += (double) seeds.size() / rounds;
                }
            }
        }
        cout << "\"*************\\nSize " << A_size << " DONE!\n*************\n";

        cout << "Active participant size = " << A_size << endl;
        cout << "Mean overlap ratio = " << overlap_ratio << endl;
        for (int32 k : k_batch) {
            cout << "\tk = " << k << endl;
            for (IM_solver solver_used : solver_batch) {
                cout << "\t\tseed quality of " << solver_name[solver_used] << " = " << result[solver_used][k];
                cout << " time = " << timer[solver_used][k];
                cout << " size = " << seedSize[solver_used][k] << endl;
            }
        }

        out << "Active participant size = " << A_size << endl;
        out << "Mean overlap ratio = " << overlap_ratio << endl;
        for (int32 k : k_batch) {
            out << "\tk = " << k << endl;
            for (IM_solver solver_used : solver_batch) {
                out << "\t\tseed quality of " << solver_name[solver_used] << " = " << result[solver_used][k];
                out << " time = " << timer[solver_used][k];
                out << " size = " << seedSize[solver_used][k] << endl;
            }
        }
    }
    out.close();
}

#endif //EXP_TOP_H
