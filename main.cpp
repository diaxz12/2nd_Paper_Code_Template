// -------------------------------------------------------------- -*- C++ -*-
// File: Luis second paper
// Version 12.9.0


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <ilcplex/ilocplex.h>

//bibliotecas pessoais feita por LD
#include "modelo.h"

ILOSTLBEGIN


int main(int argc, char **argv)
{
    //Declarar variaveis de leitura das instancias e variaveis auxiliares
    IloEnv env;
    int numero_cenarios;
    int numero_ativos;
    int janela_temporal;
    int instances;
    int Time_Limit;
    int model_version;
    int memory,threads;
    int sublengthTW;
    float alpha;
    float beta;
    float Gap;
    char *path = new char[200];
    char *results_string = new char[200];
    char *solution_string = new char[200];
    char *results_summed_string = new char[200];

    //definir os parametros da corrida
    if(argc >= 2){
        path = argv[1];
        numero_cenarios = atoi(argv[2]);
        numero_ativos = atoi(argv[3]);
        janela_temporal = atoi(argv[4]);
        instances = atoi(argv[5]);
        Time_Limit = atoi(argv[6]);
        Gap = atof(argv[7]);
        model_version = atoi(argv[8]);
        memory = atoi(argv[9]);
        threads = atoi(argv[10]);
        sublengthTW = atoi(argv[11]);
        alpha = atof(argv[12]);
        beta = atof(argv[13]);
        cout << "Combination\n" << "Instance Folder" << path << "\nNumero cenarios\t" << numero_cenarios << "\tNumero ativos\t" << numero_ativos << "\tTime window\t" << janela_temporal << "\tInstance number\t" << instances
             << "\tTime Limit\t" << Time_Limit << "\tMIP GAP\t" << Gap << "\tCPU Memory\t" << memory << "\tThreads count\t" << threads << "\tModel Version\t" << model_version;
    }else{
        cerr << "File not found for combination \n";
        exit (EXIT_FAILURE);
    }

    //comeca aqui o modelo cplex em concert
    try {
        //final path para carregar o ficheiro
        const char *filename = build_instance_path(model_version,path,numero_cenarios,numero_ativos,janela_temporal,instances);

        //esta linha de codigo permite fazer append aos resultados obtidos
        build_results_path(results_string,numero_ativos,janela_temporal,model_version);

        //terminal print
        cout << "\nFile Path\n" << filename << endl;

        //esta linha de codigo permite fazer append as solucoes obtidas
        build_solution_path(solution_string,numero_ativos,janela_temporal,numero_cenarios,instances,model_version);
        cout << "PATH \t" << solution_string << endl;

        //esta linha de codigo permite fazer append aos resultados resumidos obtidas
        sprintf(results_summed_string,"resultados/todos_resumidos/N%dTW%d/%s",numero_ativos,janela_temporal,"results_summed.csv");

        //resolver o modelo CPLEX
        solve_selected_model(model_version,env,filename,instances, results_string, solution_string, results_summed_string, Time_Limit, Gap, memory, threads, sublengthTW, alpha, beta);
    }
    catch (IloException &ex) {
        cerr << "Error: " << ex << endl;
    }
    catch (...) {
        cerr << "Error: Unknown exception caught!" << endl;
    }

    env.end();

    return 0;
} // END main
