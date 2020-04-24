#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <random>
#include <ilcplex/ilocplex.h>


#ifndef MODELO
#define MODELO

using namespace std;

typedef IloArray<IloNumVarArray> IloNumVarArray2;
typedef IloArray<IloNumVarArray2> IloNumVarArray3;
typedef IloArray<IloNumVarArray3> IloNumVarArray4;

//Define default values for solve_select_model function
vector<double> DefinedBudget_default(5, 0);
vector<vector<int>> ReplaceAsset_default(5);


//funcao para criar o path para a instancia a ser lida
char *build_instance_path(int model_version, char *path, int numero_cenarios, int numero_ativos, int janela_temporal,
                          int instances) {

    //converter para string as variaveis
    char instances_cenarios[10];
    sprintf(instances_cenarios, "%d", numero_cenarios);
    char instances_ativos[10];
    sprintf(instances_ativos, "%d", numero_ativos);
    char instances_janela_temporal[10];
    sprintf(instances_janela_temporal, "%d", janela_temporal);

    //converte variaveis int para *char
    char instances_string[10];
    sprintf(instances_string, "%d", instances);
    //faz o concatenate com a variavel int e a variavel char

    //associar a pasta de dados adequada
    switch (model_version) {
        case 1: //"CVAR_budget_planning"
            strcat(path, "/data/Stochastic Data/N");
            break;
        case 2: //"CVAR_budget_planning_Stochastic_solution"
            strcat(path, "/data/Stochastic solution Data/N");
            break;
        case 3: //"CVAR_budget_planning_Deterministic_solution"
            strcat(path, "/data/Deterministic solution Data/N");
            break;
        case 4: //"CVAR_replacement_planning"
            strcat(path, "/data/Stochastic Data/N");
            break;
        case 5: //"CVAR_replacement_planning_Stochastic_solution"
            strcat(path, "/data/Stochastic solution Data/N");
            break;
        case 6: //"CVAR_replacement_planning_Deterministic_solution"
            strcat(path, "/data/Deterministic solution Data/N");
            break;
        case 7: //"Deterministic_model"
            strcat(path, "/data/Deterministic Data/N");
            break;
        case 8: //"Master_model"
            sprintf(path, "data/Stochastic Data/N");
            break;
        default: //optional
            cout << "Failed to find the Folder data!";
            exit(EXIT_FAILURE);
    }

    //concatenar resto do path com base nos argumentos
    strcat(path, instances_ativos);
    strcat(path, "TW");
    strcat(path, instances_janela_temporal);
    //caso especial para o deterministico
    if (model_version == 7) {
        strcat(path, "/data_deterministico_");
    } else {
        strcat(path, "/data_estastico_");
    }
    strcat(path, instances_ativos);
    strcat(path, "_TW_");
    strcat(path, instances_janela_temporal);
    //so coloca-se os cenarios caso nao seja deterministico
    if (model_version != 7) {
        strcat(path, "_numscenarios_");
        strcat(path, instances_cenarios);
    }
    strcat(path, "_instancia_");
    strcat(path, instances_string);
    strcat(path, ".txt");

    return path;
}

//funcao para verificar existencia do ficheiro dos resultados resumidos
bool check_results_output(int numero_ativos, int janela_temporal) {
    char *results_summed_string = new char[200];
    sprintf(results_summed_string, "resultados/todos_resumidos/N%dTW%d/%s", numero_ativos, janela_temporal,
            "results_summed.csv");
    ifstream results_summed_aux(results_summed_string);
    bool existe;

    if (results_summed_aux.fail()) {
        existe = false;
    } else {
        existe = true;
    }

    results_summed_aux.close();

    return existe;
}

//funcao para verificar existencia do ficheiro dos resultados com solucao completa
void build_results_path(char *&results_string, int numero_ativos, int janela_temporal, int model_version) {

    //esta linha de codigo permite fazer append aos resultados obtidos

    switch (model_version) {
        case 2: //"CVAR_budget_planning_Stochastic_solution"
            sprintf(results_string, "resultados/Solved_Stochastic_solution/N%dTW%d/%s", numero_ativos, janela_temporal,
                    "results.csv");
            break;
        case 3: //"CVAR_budget_planning_Deterministic_solution"
            sprintf(results_string, "resultados/Solved_Deterministic_solution/N%dTW%d/%s", numero_ativos,
                    janela_temporal, "results.csv");
            break;
        case 5: //"CVAR_replacement_planning_Stochastic_solution"
            sprintf(results_string, "resultados/Solved_Stochastic_solution/N%dTW%d/%s", numero_ativos, janela_temporal,
                    "results.csv");
            break;
        case 6: //"CVAR_replacement_planning_Deterministic_solution"
            sprintf(results_string, "resultados/Solved_Deterministic_solution/N%dTW%d/%s", numero_ativos,
                    janela_temporal, "results.csv");
            break;
        default: //optional
            sprintf(results_string, "resultados/Solved/N%dTW%d/%s", numero_ativos, janela_temporal, "results.csv");
    }
}

//funcao para verificar existencia do ficheiro dos resultados com solucao para ser inserida noutras instancias
void
build_solution_path(char *&solution_string, int numero_ativos, int janela_temporal, int numero_cenarios, int instances,
                    int model_version) {

    //esta linha de codigo permite fazer append aos resultados obtidos para a solucao

    switch (model_version) {
        case 1: //"CVAR_budget_planning"
            sprintf(solution_string, "resultados/CVAR_budget_planning_generate_solution/N%dTW%d/%s%d%s%d%s%d%s%d%s",
                    numero_ativos, janela_temporal, "data_estastico_", numero_ativos, "_TW_", janela_temporal,
                    "_numscenarios_", 50, "_instancia_", instances, ".txt");
            break;
        case 7: //"Deterministic_model"
            sprintf(solution_string, "resultados/Deterministic_model_generate_solution/N%dTW%d/%s%d%s%d%s%d%s%d%s",
                    numero_ativos, janela_temporal, "data_estastico_", numero_ativos, "_TW_", janela_temporal,
                    "_numscenarios_", 50, "_instancia_", instances, ".txt");
            break;
        case 4: //"CVAR_replacement_planning"
            sprintf(solution_string,
                    "resultados/CVAR_replacement_planning_generate_solution/N%dTW%d/%s%d%s%d%s%d%s%d%s", numero_ativos,
                    janela_temporal, "data_estastico_", numero_ativos, "_TW_", janela_temporal, "_numscenarios_", 50,
                    "_instancia_", instances, ".txt");
            break;
        default: //optional
            cout << "\n!!No solution will be printed for this model configuration!!\n";
    }
}

//funcao para processar solucao resultados de cada modelo
//#####################//
//#####Process solution//
//#####################//

//inicio#######################################################################################################################################################################################################################################
void process_solution(int inicio_tempo, int fim_tempo, vector<int> &variable_master, vector<int> variable_sub_problem) {

    int k = 0;

    for (int t = inicio_tempo; t < fim_tempo; ++t) {
        variable_master[t] = variable_sub_problem[k];
        k++;
    }
}

void process_solution(int inicio_tempo, int fim_tempo, vector<double> &variable_master,
                      vector<double> variable_sub_problem) {

    int k = 0;

    for (int t = inicio_tempo; t < fim_tempo; ++t) {
        variable_master[t] = variable_sub_problem[k];
        k++;
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, vector<vector<int>> &variable_master,
                      vector<vector<int>> variable_sub_problem, bool stochastic = false) {

    //indice auxilar para contabilizar o tempo
    int k;

    if (stochastic == true) {
        for (int i = 0; i < second_dimension; i++) {
            k = 0;
            for (int t = inicio_tempo; t < fim_tempo; ++t) {
                variable_master[t][i] = variable_sub_problem[k][i];
                k++;
            }
        }
    } else {
        for (int i = 0; i < second_dimension; i++) {
            k = 0;
            for (int t = inicio_tempo; t < fim_tempo; ++t) {
                variable_master[i][t] = variable_sub_problem[i][k];
                k++;
            }
        }
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, vector<vector<double>> &variable_master,
                      vector<vector<double>> variable_sub_problem, bool stochastic = false) {

    //indice auxilar para contabilizar o tempo
    int k;

    if (stochastic == true) {
        for (int i = 0; i < second_dimension; i++) {
            k = 0;
            for (int t = inicio_tempo; t < fim_tempo; ++t) {
                variable_master[t][i] = variable_sub_problem[k][i];
                k++;
            }
        }
    } else {
        for (int i = 0; i < second_dimension; i++) {
            k = 0;
            for (int t = inicio_tempo; t < fim_tempo; ++t) {
                variable_master[i][t] = variable_sub_problem[i][k];
                k++;
            }
        }
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, int third_dimension,
                      vector<vector<vector<int>>> &variable_master, vector<vector<vector<int>>> variable_sub_problem,
                      bool stochastic = false) {
    //indice auxilar para contabilizar o tempo
    int k;

    if (stochastic == true) {
        for (int i = 0; i < third_dimension; i++) {
            for (int j = 0; j < second_dimension; j++) {
                k = 0;
                for (int t = inicio_tempo; t < fim_tempo; ++t) {
                    variable_master[i][t][j] = variable_sub_problem[i][k][j];
                    k++;
                }
            }
        }
    } else {
        for (int i = 0; i < third_dimension; i++) {
            for (int j = 0; j < second_dimension; j++) {
                k = 0;
                for (int t = inicio_tempo; t < fim_tempo; ++t) {
                    variable_master[i][j][t] = variable_sub_problem[i][j][k];
                    k++;
                }
            }
        }
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, int third_dimension,
                      vector<vector<vector<double>>> &variable_master,
                      vector<vector<vector<double>>> variable_sub_problem, bool stochastic = false) {
    //indice auxilar para contabilizar o tempo
    int k;

    if (stochastic == true) {
        for (int i = 0; i < third_dimension; i++) {
            for (int j = 0; j < second_dimension; j++) {
                k = 0;
                for (int t = inicio_tempo; t < fim_tempo; ++t) {
                    variable_master[i][t][j] = variable_sub_problem[i][k][j];
                    k++;
                }
            }
        }
    } else {
        for (int i = 0; i < third_dimension; i++) {
            for (int j = 0; j < second_dimension; j++) {
                k = 0;
                for (int t = inicio_tempo; t < fim_tempo; ++t) {
                    variable_master[i][j][t] = variable_sub_problem[i][j][k];
                    k++;
                }
            }
        }
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, int third_dimension, int fourth_dimension,
                      vector<vector<vector<vector<int>>>> &variable_master,
                      vector<vector<vector<vector<int>>>> variable_sub_problem, bool stochastic = false) {
    //indice auxilar para contabilizar o tempo
    int k;


    if (stochastic == true) {
        for (int i = 0; i < fourth_dimension; i++) {
            for (int j = 0; j < third_dimension; j++) {
                for (int l = 0; l < second_dimension; l++) {
                    k = 0;
                    for (int t = inicio_tempo; t < fim_tempo; ++t) {
                        variable_master[i][j][t][l] = variable_sub_problem[i][j][k][l];
                        k++;
                    }
                }
            }

        }
    } else {
        for (int i = 0; i < fourth_dimension; i++) {
            for (int j = 0; j < third_dimension; j++) {
                for (int l = 0; l < second_dimension; l++) {
                    k = 0;
                    for (int t = inicio_tempo; t < fim_tempo; ++t) {
                        variable_master[i][j][l][t] = variable_sub_problem[i][j][l][k];
                        k++;
                    }
                }
            }

        }
    }
}

void process_solution(int inicio_tempo, int fim_tempo, int second_dimension, int third_dimension, int fourth_dimension,
                      vector<vector<vector<vector<double>>>> &variable_master,
                      vector<vector<vector<vector<double>>>> variable_sub_problem, bool stochastic = false) {
    //indice auxilar para contabilizar o tempo
    int k;


    if (stochastic == true) {
        for (int i = 0; i < fourth_dimension; i++) {
            for (int j = 0; j < third_dimension; j++) {
                for (int l = 0; l < second_dimension; l++) {
                    k = 0;
                    for (int t = inicio_tempo; t < fim_tempo; ++t) {
                        variable_master[i][j][t][l] = variable_sub_problem[i][j][k][l];
                        k++;
                    }
                }
            }

        }
    } else {
        for (int i = 0; i < fourth_dimension; i++) {
            for (int j = 0; j < third_dimension; j++) {
                for (int l = 0; l < second_dimension; l++) {
                    k = 0;
                    for (int t = inicio_tempo; t < fim_tempo; ++t) {
                        variable_master[i][j][l][t] = variable_sub_problem[i][j][l][k];
                        k++;
                    }
                }
            }

        }
    }
}

//fim#######################################################################################################################################################################################################################################

//funcao para imprimir resultados de cada modelo
//imprimir solucao deterministico
void print_model_results_deterministic(char *model_name, int instances, char *results_string, char *solution_string,
                                       char *results_summed_string, int Time_Limit, float Gap_limit, int memory,
                                       int threads,
                                       double objective_function_value, double TotalTime_value, double GAP_value,
                                       vector<double> DefinedBudget_value, vector<double> AllowedDeviation_value,
                                       vector<double> UnderBudget_value, vector<double> OverBudget_value,
                                       double PreventiveMaintenanceCosts_value, double CorrectiveMaintenanceCosts_value,
                                       double ExpectedOverBudget_value, double ExpectedUnderBudget_value,
                                       vector<vector<int>> ReplaceAsset_value,
                                       vector<vector<vector<int>>> MaintenanceAction_value,
                                       vector<vector<int>> AssetFailed_value, vector<vector<int>> InitialHealth_value,
                                       int numAsset_value, int numLengthTW_value, int numAction_value,
                                       int subLengthTW_value, int numSubHorizons_value) {

//#####################//
//#####Print results   //
//#####################//

//verificar existencia do ficheiro dos resultados
    bool existe;
    existe = check_results_output(numAsset_value, subLengthTW_value * numSubHorizons_value);

    //para imprimir a solucao e resultados
    ofstream results, solution, results_summed;

    //esta linha de codigo permite fazer append aos resultados obtidos
    results.open(results_string, fstream::app);
    //esta linha de codigo permite fazer append as solucoes obtidas
    solution.open(solution_string, fstream::app);
    //esta linha de codigo permite fazer append aos resultados resumidos obtidas
    results_summed.open(results_summed_string, fstream::app);

    //passar solucao para um ficheiro csv
    results << "Instance\t" << model_name << "\t" << (instances) << endl;
    results << "Objective Function=\n" << objective_function_value << endl;
    results << "Time=\n" << TotalTime_value << endl;
    results << "GAP(%)=\n" << GAP_value * 100 << endl;

    //Instance cplex Parameters
    results << "Time Limit=\n" << Time_Limit << endl;
    results << "Memory Limit=\n" << memory << endl;
    results << "Threads Limit=\n" << threads << endl;
    results << "GAP Limit=\n" << Gap_limit << endl;

    //CVAR Parameters
    results << "Alpha (Nivel confianca)=\n" << "none" << endl;
    results << "Beta (Peso Risco)=\n" << "none" << endl;

    //imprimir matriz custos over budget e under budget
    results << "Under Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == 0)results << "";
        results << UnderBudget_value[t] << ";";
        if (t == numLengthTW_value - 1)results << endl;
    }

    results << "Over Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == 0)results << "";
        results << OverBudget_value[t] << ";";
        if (t == numLengthTW_value - 1)results << endl;
    }


    results << "Expected Preventive costs=\n" << PreventiveMaintenanceCosts_value << endl;
    results << "Expected Corrective costs=\n" << CorrectiveMaintenanceCosts_value << endl;

    //imprimir expected cost para o under budget e o over budget
    results << "Total Over Budget=\n" << ExpectedOverBudget_value << endl;
    results << "Total Under Budget=\n" << ExpectedUnderBudget_value << endl;

    //Imprimir os custos de manutencao por periodo
    results << "Expected cost=\n" << objective_function_value << endl;

    //imprimir o orcamento definido
    results << "Defined Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << DefinedBudget_value[t] << endl;
        } else {
            results << DefinedBudget_value[t] << "  ";
        }
    }


    //imprimir Budget Deviation
    results << "Allowed Budget Deviation=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << AllowedDeviation_value[t] << endl;
        } else {
            results << AllowedDeviation_value[t] << "  ";
        }
    }

    //imprimir o replacement plan
    results << "Replacement plan=" << endl;
    for (int i = 0; i < numAsset_value; i++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            results << ReplaceAsset_value[i][t] << ";";
            if (t == numLengthTW_value - 1 & i != numAsset_value - 1)results << endl;
        }
        if (i == numAsset_value - 1)results << endl;
    }

    //imprimir o maintenance action
    results << "Maintenance Action" << endl;
    for (int a = 0; a < numAction_value; a++) {
        results << "(A" << a + 1 << ")" << endl;
        for (int t = 0; t < numLengthTW_value; t++) {
            int contador = 0;
            for (int z = 0; z < numAsset_value; z++)contador += MaintenanceAction_value[a][z][t];
            if (contador != 0) {
                results << "(T" << t + 1 << ");";
                for (int i = 0; i < numAsset_value; i++) {
                    if (MaintenanceAction_value[a][i][t] == 1)results << i << ";";
                }
                results << endl;
            }
        }
    }

    //imprimir falha do ativo
    results << "Asset failed" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        int contador = 0;
        for (int z = 0; z < numAsset_value; z++)contador = contador + AssetFailed_value[z][t];
        if (contador != 0) {
            results << "(T" << t + 1 << ");";
            for (int i = 0; i < numAsset_value; i++) {
                if (AssetFailed_value[i][t] == 1)results << i << ";";
            }
            results << endl;
        }
    }

    //imprimir solucao para depois adicionar as instancias respetivas! atencao que tem se usar as instancias originais
    //só utilizar as variáveis de primeiro estágio
    solution << "[";
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            solution << DefinedBudget_value[t] << "]" << endl;
        } else {
            solution << DefinedBudget_value[t] << ",";
        }
    }

    //imprimir o plano de substituição
    for (int i = 0; i < numAsset_value; i++) {
        if (i == 0)solution << "[";
        for (int t = 0; t < numLengthTW_value; t++) {
            if (t == 0)solution << "[";
            if (t < numLengthTW_value & i < numAsset_value) solution << (int) round(ReplaceAsset_value[i][t]) << ",";
            if (t == numLengthTW_value - 1 & i != numAsset_value - 1)
                solution << (int) round(ReplaceAsset_value[i][t]) << "],";
            if (t == numLengthTW_value - 1 & i == numAsset_value - 1)
                solution << (int) round(ReplaceAsset_value[i][t]) << "]]" << endl;
        }
    }

    //imprimir evolucao da condicao
    results << "Asset condition initial health per period=" << endl;
    for (int i = 0; i < numAsset_value; i++) {
        for (int k = 1; k <= numSubHorizons_value; k++) {
            for (int t = (subLengthTW_value * k - subLengthTW_value); t < subLengthTW_value * k; t++) {
                if (t == subLengthTW_value * k - 1 &
                    subLengthTW_value * k != subLengthTW_value * numSubHorizons_value) {
                    results << InitialHealth_value[i][t] << "||";
                } else {
                    results << InitialHealth_value[i][t] << ";";
                }
                if (t == subLengthTW_value * numSubHorizons_value - 1 & i != numAsset_value - 1)results << endl;
            }
        }
        if (i == numAsset_value - 1)results << endl;
    }

    //imprimir resultados resumidos
    if (existe == true) {
        results_summed << instances << "\t" << model_name << "\t" << "none" << "\t" << numAsset_value << "\t"
                       << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << "none" << "\t" << "none" << "\t" << objective_function_value << "\t" << TotalTime_value
                       << "\t" << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << "none"
                       << "\t" << "none" << endl;
    } else {
        results_summed
                << "Instance number\tModel version\tScenarios number\t Assets number\tTime window Length\tThreads\tMemory\tTime limit\tGAP_limit\talpha\tbeta"
                << "\t" << "Objective Function" << "\t" << "Time" << "\t" << "GAP" << "\t"
                << "Expected Preventive costs " << "\t" << "Expected Corrective costs " << "\t" << "Under Budget"
                << "\t" << "Over Budget" "\t"
                << "VaR" << "\t" << "CVaR\n";
        results_summed << instances << "\t" << model_name << "\t" << "none" << "\t" << numAsset_value << "\t"
                       << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << "none" << "\t" << "none" << "\t" << objective_function_value << "\t" << TotalTime_value
                       << "\t" << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << "none"
                       << "\t" << "none" << endl;
    }

    solution.close();
    results.close();
    results_summed.close();

}

void print_model_results_budget_planning(char *model_name, int instances, char *results_string, char *solution_string,
                                         char *results_summed_string, int Time_Limit, float Gap_limit, int memory,
                                         int threads,
                                         double objective_function_value, double CVaR_value, double VaR_value,
                                         double ExpectedValue_value, double TotalTime_value, double GAP_value,
                                         vector<double> ScenarioProbability_value, vector<double> DefinedBudget_value,
                                         vector<double> AllowedDeviation_value,
                                         vector<double> TotalExpectedCosts_value,
                                         vector<vector<double>> UnderBudget_value,
                                         vector<vector<double>> OverBudget_value,
                                         double PreventiveMaintenanceCosts_value,
                                         double CorrectiveMaintenanceCosts_value, double ExpectedOverBudget_value,
                                         double ExpectedUnderBudget_value,
                                         vector<vector<vector<int>>> ReplaceAsset_value,
                                         vector<vector<vector<vector<int>>>> MaintenanceAction_value,
                                         vector<vector<vector<int>>> AssetFailed_value,
                                         vector<vector<double>> InitialHealth_value,
                                         float alpha_value, float beta_value, int numAsset_value, int numLengthTW_value,
                                         int numAction_value, int numScenarios_value, int subLengthTW_value,
                                         int numSubHorizons_value) {

    //#####################//
    //#####Print results   //
    //#####################//

    //verificar existencia do ficheiro dos resultados
    bool existe;
    existe = check_results_output(numAsset_value, numLengthTW_value);

    //para imprimir a solucao e resultados
    ofstream results, solution, results_summed;

    //esta linha de codigo permite fazer append aos resultados obtidos
    results.open(results_string, fstream::app);
    //esta linha de codigo permite fazer append as solucoes obtidas
    solution.open(solution_string, fstream::app);
    //esta linha de codigo permite fazer append aos resultados resumidos obtidas
    results_summed.open(results_summed_string, fstream::app);

    //passar solucao para um ficheiro csv
    results << "Instance\t" << model_name << "\t" << (instances) << endl;
    results << "Objective Function=\n" << objective_function_value << endl;
    results << "Time=\n" << TotalTime_value << endl;
    results << "GAP(%)=\n" << GAP_value * 100 << endl;

    //Instance cplex Parameters
    results << "Time Limit=\n" << Time_Limit << endl;
    results << "Memory Limit=\n" << memory << endl;
    results << "Threads Limit=\n" << threads << endl;
    results << "GAP Limit=\n" << Gap_limit << endl;

    //CVAR Parameters
    results << "Alpha (Nivel confianca)=\n" << alpha_value << endl;
    results << "Beta (Peso Risco)=\n" << beta_value << endl;

    //imprimir matriz custos over budget e under budget
    results << "Under Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        for (int s = 0; s < numScenarios_value; s++) {
            results << UnderBudget_value[t][s] << ";";
            if (s == numScenarios_value - 1 & t != numLengthTW_value - 1)results << endl;
        }
        if (t == numLengthTW_value - 1)results << endl;
    }

    results << "Over Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        for (int s = 0; s < numScenarios_value; s++) {
            results << OverBudget_value[t][s] << ";";
            if (s == numScenarios_value - 1 & t != numLengthTW_value - 1)results << endl;
        }
        if (t == numLengthTW_value - 1)results << endl;
    }


    results << "Expected Preventive costs=\n" << PreventiveMaintenanceCosts_value << endl;
    results << "Expected Corrective costs=\n" << CorrectiveMaintenanceCosts_value << endl;

    //imprimir expected cost para o under budget e o over budget
    results << "Total Over Budget=\n" << ExpectedOverBudget_value << endl;
    results << "Total Under Budget=\n" << ExpectedUnderBudget_value << endl;

    //imprimir CVAR, VAR e Expected Value
    results << "CVaR=\n" << CVaR_value << endl;
    results << "VaR=\n" << VaR_value << endl;
    results << "Expected Value=\n" << ExpectedValue_value << endl;

    //Imprimir probabilidades cenarios
    results << "Scenario probability =" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        results << ScenarioProbability_value[s] << ";";
        if (s == numScenarios_value - 1)results << endl;
    }

    //Imprimir os custos de manutencao por periodo
    results << "Expected cost per scenario=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        results << TotalExpectedCosts_value[s] << ";";
        if (s == numScenarios_value - 1)results << endl;
    }

    //imprimir o orcamento definido
    results << "Defined Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << DefinedBudget_value[t] << endl;
        } else {
            results << DefinedBudget_value[t] << "  ";
        }
    }


    //imprimir Budget Deviation
    results << "Allowed Budget Deviation=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << AllowedDeviation_value[t] << endl;
        } else {
            results << AllowedDeviation_value[t] << "  ";
        }
    }

    //imprimir o maintenance action
    results << "Maintenance Action=" << endl;
    for (int a = 0; a < numAction_value; a++) {
        results << "(A" << a << ")" << endl;
        for (int s = 0; s < numScenarios_value; s++) {
            for (int t = 0; t < numLengthTW_value; t++) {
                int contador = 0;
                for (int z = 0; z < numAsset_value; z++)
                    contador = contador + MaintenanceAction_value[a][z][t][s];
                if (contador != 0) {
                    results << "(S" << s << ";T" << t << ");";
                    for (int i = 0; i < numAsset_value; i++) {
                        if (MaintenanceAction_value[a][i][t][s] == 1)results << i << ";";
                    }
                    results << endl;
                }
            }
        }
    }

    //imprimir substituicao antecipada
    results << "Replacement plan scenario=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            int contador = 0;
            for (int z = 0; z < numAsset_value; z++)contador = contador + ReplaceAsset_value[z][t][s];
            if (contador != 0) {
                results << "(S" << s << ";T" << t << ");";
                for (int i = 0; i < numAsset_value; i++) {
                    if (ReplaceAsset_value[i][t][s] == 1)results << i << ";";
                }
                results << endl;
            }
        }
    }

    //imprimir falha do ativo
    results << "Asset failed=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            int contador = 0;
            for (int z = 0; z < numAsset_value; z++)contador = contador + AssetFailed_value[z][t][s];
            if (contador != 0) {
                results << "(S" << s << ";T" << t << ");";
                for (int i = 0; i < numAsset_value; i++) {
                    if (AssetFailed_value[i][t][s] == 1)results << i << ";";
                }
                results << endl;
            }
        }
    }

    //imprimir solucao para depois adicionar as instancias respetivas! atencao que tem se usar as instancias originais
    //só utilizar as variáveis de primeiro estágio
    solution << "[";
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            solution << DefinedBudget_value[t] << "]" << endl;
        } else {
            solution << DefinedBudget_value[t] << ",";
        }
    }

    //imprimir evolucao da condicao
    results << "Asset condition initial health per period=" << endl;
    for (int i = 0; i < numAsset_value; i++) {
        for (int k = 1; k <= numSubHorizons_value; k++) {
            for (int t = (subLengthTW_value * k - subLengthTW_value); t < subLengthTW_value * k; t++) {
                if (t == subLengthTW_value * k - 1 &
                    subLengthTW_value * k != subLengthTW_value * numSubHorizons_value) {
                    results << InitialHealth_value[i][t] << "||";
                } else {
                    results << InitialHealth_value[i][t] << ";";
                }
                if (t == subLengthTW_value * numSubHorizons_value - 1 & i != numAsset_value - 1)results << endl;
            }
        }
        if (i == numAsset_value - 1)results << endl;
    }


    //imprimir resultados resumidos
    if (existe == true) {
        results_summed << instances << "\t" << model_name << "\t" << numScenarios_value << "\t" << numAsset_value
                       << "\t" << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << alpha_value << "\t" << beta_value << "\t" << objective_function_value << "\t" << TotalTime_value << "\t"
                       << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << VaR_value << "\t"
                       << CVaR_value << endl;
    } else {
        results_summed
                << "Instance number\tModel version\tScenarios number\t Assets number\tTime window Length\tThreads\tMemory\tTime limit\tGAP_limit\talpha\tbeta"
                << "\t" << "Objective Function" << "\t" << "Time" << "\t" << "GAP" << "\t"
                << "Expected Preventive costs " << "\t" << "Expected Corrective costs " << "\t" << "Under Budget"
                << "\t" << "Over Budget" "\t"
                << "VaR" << "\t" << "CVaR\n";
        results_summed << instances << "\t" << model_name << "\t" << numScenarios_value << "\t" << numAsset_value
                       << "\t" << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << alpha_value << "\t" << beta_value << "\t" << objective_function_value << "\t"
                       << TotalTime_value << "\t" << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << VaR_value
                       << "\t" << CVaR_value << endl;
    }

    solution.close();
    results.close();
    results_summed.close();

}

void
print_model_results_replacement_planning(char *model_name, int instances, char *results_string, char *solution_string,
                                         char *results_summed_string, int Time_Limit, float Gap_limit, int memory,
                                         int threads,
                                         double objective_function_value, double CVaR_value, double VaR_value,
                                         double ExpectedValue_value, double TotalTime_value, double GAP_value,
                                         vector<double> ScenarioProbability_value, vector<double> DefinedBudget_value,
                                         vector<double> AllowedDeviation_value,
                                         vector<double> TotalExpectedCosts_value,
                                         vector<vector<double>> UnderBudget_value,
                                         vector<vector<double>> OverBudget_value,
                                         double PreventiveMaintenanceCosts_value,
                                         double CorrectiveMaintenanceCosts_value, double ExpectedOverBudget_value,
                                         double ExpectedUnderBudget_value, vector<vector<int>> ReplaceAsset_value,
                                         vector<vector<vector<int>>> AntecipateReplaceAsset_value,
                                         vector<vector<vector<vector<int>>>> MaintenanceAction_value,
                                         vector<vector<vector<int>>> AssetFailed_value,
                                         vector<vector<double>> InitialHealth_value,
                                         float alpha_value, float beta_value, int numAsset_value, int numLengthTW_value,
                                         int numAction_value, int numScenarios_value, int subLengthTW_value,
                                         int numSubHorizons_value) {

    //#####################//
    //#####Print results   //
    //#####################//

    //verificar existencia do ficheiro dos resultados
    bool existe;
    existe = check_results_output(numAsset_value, numLengthTW_value);

    //para imprimir a solucao e resultados
    ofstream results, solution, results_summed;

    //esta linha de codigo permite fazer append aos resultados obtidos
    results.open(results_string, fstream::app);
    //esta linha de codigo permite fazer append as solucoes obtidas
    solution.open(solution_string, fstream::app);
    //esta linha de codigo permite fazer append aos resultados resumidos obtidas
    results_summed.open(results_summed_string, fstream::app);

    //passar solucao para um ficheiro csv
    results << "Instance\t" << model_name << "\t" << (instances) << endl;
    results << "Objective Function=\n" << objective_function_value << endl;
    results << "Time=\n" << TotalTime_value << endl;
    results << "GAP(%)=\n" << GAP_value * 100 << endl;

    //Instance cplex Parameters
    results << "Time Limit=\n" << Time_Limit << endl;
    results << "Memory Limit=\n" << memory << endl;
    results << "Threads Limit=\n" << threads << endl;
    results << "GAP Limit=\n" << Gap_limit << endl;

    //CVAR Parameters
    results << "Alpha (Nivel confianca)=\n" << alpha_value << endl;
    results << "Beta (Peso Risco)=\n" << beta_value << endl;

    //imprimir matriz custos over budget e under budget
    results << "Under Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        for (int s = 0; s < numScenarios_value; s++) {
            results << UnderBudget_value[t][s] << ";";
            if (s == numScenarios_value - 1 & t != numLengthTW_value - 1)results << endl;
        }
        if (t == numLengthTW_value - 1)results << endl;
    }

    results << "Over Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        for (int s = 0; s < numScenarios_value; s++) {
            results << OverBudget_value[t][s] << ";";
            if (s == numScenarios_value - 1 & t != numLengthTW_value - 1)results << endl;
        }
        if (t == numLengthTW_value - 1)results << endl;
    }


    results << "Expected Preventive costs=\n" << PreventiveMaintenanceCosts_value << endl;
    results << "Expected Corrective costs=\n" << CorrectiveMaintenanceCosts_value << endl;

    //imprimir expected cost para o under budget e o over budget
    results << "Total Over Budget=\n" << ExpectedOverBudget_value << endl;
    results << "Total Under Budget=\n" << ExpectedUnderBudget_value << endl;

    //imprimir CVAR, VAR e Expected Value
    results << "CVaR=\n" << CVaR_value << endl;
    results << "VaR=\n" << VaR_value << endl;
    results << "Expected Value=\n" << ExpectedValue_value << endl;

    //Imprimir probabilidades cenarios
    results << "Scenario probability =" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        results << ScenarioProbability_value[s] << ";";
        if (s == numScenarios_value - 1)results << endl;
    }

    //Imprimir os custos de manutencao por periodo
    results << "Expected cost per scenario=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        results << TotalExpectedCosts_value[s] << ";";
        if (s == numScenarios_value - 1)results << endl;
    }

    //imprimir o orcamento definido
    results << "Defined Budget=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << DefinedBudget_value[t] << endl;
        } else {
            results << DefinedBudget_value[t] << "  ";
        }
    }


    //imprimir Budget Deviation
    results << "Allowed Budget Deviation=" << endl;
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            results << AllowedDeviation_value[t] << endl;
        } else {
            results << AllowedDeviation_value[t] << "  ";
        }
    }

    //imprimir o replacement plan
    results << "Replacement plan=" << endl;
    for (int i = 0; i < numAsset_value; i++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            results << ReplaceAsset_value[i][t] << ";";
            if (t == numLengthTW_value - 1 & i != numAsset_value - 1)results << endl;
        }
        if (i == numAsset_value - 1)results << endl;
    }

    //imprimir o maintenance action
    results << "Maintenance Action=" << endl;
    for (int a = 0; a < numAction_value; a++) {
        results << "(A" << a << ")" << endl;
        for (int s = 0; s < numScenarios_value; s++) {
            for (int t = 0; t < numLengthTW_value; t++) {
                int contador = 0;
                for (int z = 0; z < numAsset_value; z++)
                    contador = contador + MaintenanceAction_value[a][z][t][s];
                if (contador != 0) {
                    results << "(S" << s << ";T" << t << ");";
                    for (int i = 0; i < numAsset_value; i++) {
                        if (MaintenanceAction_value[a][i][t][s] == 1)results << i << ";";
                    }
                    results << endl;
                }
            }
        }
    }

    //imprimir substituicao antecipada
    results << "Antecipate Replacement plan scenario=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            int contador = 0;
            for (int z = 0; z < numAsset_value; z++)contador = contador + AntecipateReplaceAsset_value[z][t][s];
            if (contador != 0) {
                results << "(S" << s << ";T" << t << ");";
                for (int i = 0; i < numAsset_value; i++) {
                    if (AntecipateReplaceAsset_value[i][t][s] == 1)results << i << ";";
                }
                results << endl;
            }
        }
    }

    //imprimir falha do ativo
    results << "Asset failed=" << endl;
    for (int s = 0; s < numScenarios_value; s++) {
        for (int t = 0; t < numLengthTW_value; t++) {
            int contador = 0;
            for (int z = 0; z < numAsset_value; z++)contador = contador + AssetFailed_value[z][t][s];
            if (contador != 0) {
                results << "(S" << s << ";T" << t << ");";
                for (int i = 0; i < numAsset_value; i++) {
                    if (AssetFailed_value[i][t][s] == 1)results << i << ";";
                }
                results << endl;
            }
        }
    }

    //imprimir solucao para depois adicionar as instancias respetivas! atencao que tem se usar as instancias originais
    //só utilizar as variáveis de primeiro estágio
    solution << "[";
    for (int t = 0; t < numLengthTW_value; t++) {
        if (t == numLengthTW_value - 1) {
            solution << DefinedBudget_value[t] << "]" << endl;
        } else {
            solution << DefinedBudget_value[t] << ",";
        }
    }

    //imprimir o plano de substituição
    for (int i = 0; i < numAsset_value; i++) {
        if (i == 0)solution << "[";
        for (int t = 0; t < numLengthTW_value; t++) {
            if (t == 0)solution << "[";
            if (t < numLengthTW_value & i < numAsset_value) solution << (int) round(ReplaceAsset_value[i][t]) << ",";
            if (t == numLengthTW_value - 1 & i != numAsset_value - 1)
                solution << (int) round(ReplaceAsset_value[i][t]) << "],";
            if (t == numLengthTW_value - 1 & i == numAsset_value - 1)
                solution << (int) round(ReplaceAsset_value[i][t]) << "]]" << endl;
        }
    }

    //imprimir evolucao da condicao
    results << "Asset condition initial health per period=" << endl;
    for (int i = 0; i < numAsset_value; i++) {
        for (int k = 1; k <= numSubHorizons_value; k++) {
            for (int t = (subLengthTW_value * k - subLengthTW_value); t < subLengthTW_value * k; t++) {
                if (t == subLengthTW_value * k - 1 &
                    subLengthTW_value * k != subLengthTW_value * numSubHorizons_value) {
                    results << InitialHealth_value[i][t] << "||";
                } else {
                    results << InitialHealth_value[i][t] << ";";
                }
                if (t == subLengthTW_value * numSubHorizons_value - 1 & i != numAsset_value - 1)results << endl;
            }
        }
        if (i == numAsset_value - 1)results << endl;
    }


    //imprimir resultados resumidos
    if (existe == true) {
        results_summed << instances << "\t" << model_name << "\t" << numScenarios_value << "\t" << numAsset_value
                       << "\t" << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << alpha_value << "\t" << beta_value << "\t" << objective_function_value << "\t" << TotalTime_value << "\t"
                       << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << VaR_value << "\t"
                       << CVaR_value << endl;
    } else {
        results_summed
                << "Instance number\tModel version\tScenarios number\t Assets number\tTime window Length\tThreads\tMemory\tTime limit\tGAP_limit\talpha\tbeta"
                << "\t" << "Objective Function" << "\t" << "Time" << "\t" << "GAP" << "\t"
                << "Expected Preventive costs " << "\t" << "Expected Corrective costs " << "\t" << "Under Budget"
                << "\t" << "Over Budget" "\t"
                << "VaR" << "\t" << "CVaR\n";
        results_summed << instances << "\t" << model_name << "\t" << numScenarios_value << "\t" << numAsset_value
                       << "\t" << numLengthTW_value << "\t" << threads << "\t" << memory << "\t" << Time_Limit << "\t"
                       << Gap_limit << "\t"
                       << alpha_value << "\t" << beta_value << "\t" << objective_function_value << "\t"
                       << TotalTime_value << "\t" << GAP_value << "\t" << PreventiveMaintenanceCosts_value << "\t"
                       << CorrectiveMaintenanceCosts_value
                       << "\t" << ExpectedOverBudget_value << "\t" << ExpectedUnderBudget_value << "\t" << VaR_value
                       << "\t" << CVaR_value << endl;
    }

    solution.close();
    results.close();
    results_summed.close();

}


//MODELOS CPLEX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//modelo em que temos como variavel de primeiro estagio a definicao do budget mas avaliamos a solucao obtida com um modelo estocastico ou deterministico
void solve_CVAR_Budget_planning_solution(char *model_name, IloEnv env, const char *filename, int instances,
                                         char *results_string, char *solution_string, char *results_summed_string,
                                         int Time_Limit, float Gap, int memory, int threads, int subLengthTW,
                                         float alpha, float beta, vector<double> DefinedBudget_solution) {


    //verificar se o ficheiro existe da instancia
    ifstream file(filename);
    if (!file) {
        cerr << "No such file: " << filename << endl;
        throw (-1);
    }

    //MODELO CPLEX////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IloNum RiskFreeRatePlus, RiskFreeRateMinus;
    IloInt MaxHealth;
    IloNumArray CostFailure(env), CostReplacingAsset(env), InitialHealth(env), InterestMultiplier(
            env), ScenarioProbability(env);
    IloNumArray2 CostAction(env), BenefitAction(env);
    IloNumArray3 AssetDegradation(env);

    file >> RiskFreeRateMinus >> RiskFreeRatePlus >> MaxHealth >> CostFailure >> CostReplacingAsset
         >> CostAction
         >> BenefitAction >> InitialHealth >> InterestMultiplier >> AssetDegradation >> ScenarioProbability;

    int numAsset = CostFailure.getSize();
    int numLengthTW = InterestMultiplier.getSize();
    int numAction = CostAction[0].getSize();
    int numScenarios = ScenarioProbability.getSize();
    int picked_scenario = 0;

    int i, t, a, s, l;
    int numSubHorizons = 1;

    //copy solution into the variable model
    vector<double> DefinedBudget(InterestMultiplier.getSize(), 0);
    for(t=0; t<numLengthTW; t++){
        DefinedBudget[t]=DefinedBudget_solution[t];
    }

    if (numLengthTW != subLengthTW) {
        //calcular numero de subproblemas
        numLengthTW = subLengthTW;
        numSubHorizons = InterestMultiplier.getSize() / subLengthTW;
        //atualizar model name
        strcat(model_name, "_Rolling_Horizon");

        // atualizar calculo dos juros a pagar com base no comprimento escolhido e taxa de juro
        int tempo;
        for (int k = 1; k <= numSubHorizons; k++) {
            tempo = 0;
            if (subLengthTW > 1) {
                for (int i = (k - 1) * subLengthTW; i < k * subLengthTW - 1; i++) {
                    tempo++;
                    InterestMultiplier[i] = (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) * RiskFreeRatePlus /
                                             (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) - 1));
                }
                //atualizar o ultimo valor do calculo dos juros
                InterestMultiplier[k * subLengthTW - 1] = InterestMultiplier[k * subLengthTW - 2];
            } else {
                InterestMultiplier[k * subLengthTW - 1] = 1;
            }

        }
    }

    //Declarar variaveis output resultado
    double objective_function = 0;
    double TotalTime = 0;
    double GAP = 0;
    double PreventiveMaintenanceCosts_output = 0;
    double CorrectiveMaintenanceCosts_output = 0;
    double ExpectedOverBudget_output = 0;
    double ExpectedUnderBudget_output = 0;
    double CVaR_output = 0;
    double VaR_output = 0;
    double ExpectedValue_output = 0;

    //First dimension
    vector<double> TotalExpectedCosts_output(numScenarios, 0);
    //First dimension
    vector<double> DefinedBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> AllowedDeviation_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> ScenarioProbability_output(numScenarios, 0);

    //copiar valores que advem do modelo
    for (s = 0; s < numScenarios; s++)ScenarioProbability_output[s] = ScenarioProbability[s];

    //First dimension
    vector<vector<double>> UnderBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        UnderBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        OverBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> InitialHealth_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        InitialHealth_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<vector<int>>> ReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)ReplaceAsset_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)AssetFailed_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_output(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_output[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_output[a][i].resize(InterestMultiplier.getSize());
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < InterestMultiplier.getSize(); t++)
                MaintenanceAction_output[a][i][t].resize(numScenarios, 0);
        }
    }

    //criar variaveis auxiliares
    vector<double> DefinedBudget_aux(subLengthTW, 0);
    vector<double> AllowedDeviation_aux(subLengthTW, 0);
    vector<double> TotalExpectedCosts_aux(numScenarios, 0);

    //First dimension
    vector<vector<double>> UnderBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        UnderBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        OverBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> ReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)ReplaceAsset_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AssetFailed_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> Health_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        Health_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_aux(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_aux[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_aux[a][i].resize(subLengthTW);
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < subLengthTW; t++)MaintenanceAction_aux[a][i][t].resize(numScenarios, 0);
        }
    }

    //#####################//
    //#####Model definition//
    //#####################//
    for (int k = 1; k <= numSubHorizons; k++) {
        IloModel mod(env);

        //atualizar o budget definido na solucao de acordo com o modelo gerado(atencao que os primeiros valores do subperiodo sao sempre atualizados)
        if (k > 1) {
            for (t = 0; t < numLengthTW; t++) {
                DefinedBudget[t] = DefinedBudget[(k - 1) * subLengthTW + t];
            }
        }

        //DECISION VARIABLES
        IloNumVarArray AllowedDeviation(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);

        IloNumVarArray3 ReplaceAsset(env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            ReplaceAsset[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t)
                ReplaceAsset[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0, ILOBOOL);
        }

        IloNumVarArray4 MaintenanceAction(env, numAction);
        for (a = 0; a < numAction; ++a) {
            MaintenanceAction[a] = IloNumVarArray3(env, numAsset);
            for (i = 0; i < numAsset; ++i) {
                MaintenanceAction[a][i] = IloNumVarArray2(env, numLengthTW);
                for (t = 0; t < numLengthTW; t++)
                    MaintenanceAction[a][i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0, ILOBOOL);
            }
        }


        //AUXILIARY VARIABLES

        //calculos auxiliares
        IloNumVarArray TotalExpectedCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //calcula o custo esperado

        IloNumVarArray2 EffectiveBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            EffectiveBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                ILOFLOAT); //calcula os gastos efetivos em mautencao

        IloNumVarArray2 UnderBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            UnderBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                            ILOFLOAT); //calcula os gastos em mautencao que nao excederam o budget planeado

        IloNumVarArray2 OverBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            OverBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                           ILOFLOAT); //calcula os gastos em mautencao que excederam o budget planeado

        IloNumVarArray3 Health(env, numAsset), HealthFailure(env, numAsset), HealthReset(env, numAsset), AssetFailed(
                env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            Health[i] = IloNumVarArray2(env, numLengthTW);
            HealthFailure[i] = IloNumVarArray2(env, numLengthTW);
            HealthReset[i] = IloNumVarArray2(env, numLengthTW);
            AssetFailed[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                Health[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                              ILOFLOAT); //atualiza a saude do equipamento
                HealthFailure[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //ativa a falha
                HealthReset[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                   ILOFLOAT); //ativa o reset da condição
                AssetFailed[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                   ILOBOOL); // variavel utilzada para quando o equipamento falha
            }
        }

        IloNumVar CorrectiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das corretivas expectável
        IloNumVar PreventiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das preventivas expectável
        IloNumVar ExpectedOverBudget(env, 0.0, IloInfinity, ILOFLOAT); //custo expectavel de over budget
        IloNumVar ExpectedUnderBudget(env, 0.0, IloInfinity, ILOFLOAT);  //custo expectavel de under budget

        //variaveis auxiliares do modelo CVaR
        IloNumVar VaR(env, 0.0, IloInfinity, ILOFLOAT); //Value at Risk
        IloNumVar CVaR(env, 0.0, IloInfinity, ILOFLOAT); //Conditional Value at Risk
        IloNumVarArray ConditionalValue(env, numScenarios, 0.0, IloInfinity,
                                        ILOFLOAT); //toma valores diferentes de zero quando acima do VaR
        IloNumVar ExpectedValue(env, 0.0, IloInfinity, ILOFLOAT); //valor esperado da função objetivo
        IloNumVarArray ScenarioCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //custo de um dado cenário

        // OBJECTIVE FUNCTION

        //custo do overbudget por scenario
        for (s = 0; s < numScenarios; s++) {
            IloExpr OverBudgetCost(env);
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    OverBudgetCost += InterestMultiplier[l] * OverBudget[t][s] / pow(1 + RiskFreeRateMinus, l + 1);
            }
            mod.add(TotalExpectedCost[s] == OverBudgetCost);
            OverBudgetCost.end();
        }

        IloExpr TotalBudget(env);
        //valor esperado dos custos
        for (t = 0; t < numLengthTW; t++)TotalBudget += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
        for (s = 0; s < numScenarios; s++)TotalBudget += ScenarioProbability[s] * TotalExpectedCost[s];
        mod.add(ExpectedValue == TotalBudget);

        //cálculo do custo de cada cenário
        for (s = 0; s < numScenarios; s++) {
            IloExpr ScenarioCost_aux(env);
            for (t = 0; t < numLengthTW; t++)
                ScenarioCost_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1) + TotalExpectedCost[s];
            mod.add(ScenarioCost[s] == ScenarioCost_aux);
            ScenarioCost_aux.end();
        }

        //Calculo CVaR
        IloExpr CVaR_tail_cost(env);
        for (s = 0; s < numScenarios; s++)CVaR_tail_cost += ScenarioProbability[s] * ConditionalValue[s];
        mod.add(CVaR == VaR + 1 / (1 - alpha) * CVaR_tail_cost);
        CVaR_tail_cost.end();

        //adicionar a funcao objetivo
        mod.add(IloMinimize(env, (1 - beta) * TotalBudget + beta * CVaR));
        TotalBudget.end();

        //Calculos auxiliares

        //calculo dos custos corretiva
        IloExpr CorrectiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++)
                    CorrectiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostFailure[i] * AssetFailed[i][t][s] /
                                                                                pow(1 + RiskFreeRateMinus, t + 1));
            }
        }
        mod.add(CorrectiveMaintenanceCosts == CorrectiveMaintenanceCosts_aux);
        CorrectiveMaintenanceCosts_aux.end();

        //calculo dos custos preventiva
        IloExpr PreventiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++) {
                    PreventiveMaintenanceCosts_aux +=
                            ScenarioProbability[s] * (CostReplacingAsset[i] * ReplaceAsset[i][t][s]) /
                            pow(1 + RiskFreeRateMinus, t + 1);
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostAction[i][a] *
                                                                                    MaintenanceAction[a][i][t][s]) /
                                                          pow(1 + RiskFreeRateMinus, t + 1);
                }
            }
        }
        mod.add(PreventiveMaintenanceCosts == PreventiveMaintenanceCosts_aux);
        PreventiveMaintenanceCosts_aux.end();

        //calculo do custo expectavel de underbudget
        IloExpr ExpectedUnderBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++)
                ExpectedUnderBudget_aux += ScenarioProbability[s] * UnderBudget[t][s] /
                                           pow(1 + RiskFreeRateMinus, t + 1);
        }
        mod.add(ExpectedUnderBudget == ExpectedUnderBudget_aux);
        ExpectedUnderBudget_aux.end();

        //calculo do custo expectavel de overbudget
        IloExpr ExpectedOverBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    ExpectedOverBudget_aux += ScenarioProbability[s] * OverBudget[t][s] /
                                              pow(1 + RiskFreeRateMinus, l + 1);
            }
        }
        mod.add(ExpectedOverBudget == ExpectedOverBudget_aux);
        ExpectedOverBudget_aux.end();

        //PROBLEM CONSTRAINTS

        //verificar se está acima do VaR
        for (s = 0; s < numScenarios; s++) {
            IloExpr DefinedBudgetUpdated_aux(env);
            for (t = 0; t < numLengthTW; t++)
                DefinedBudgetUpdated_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
            mod.add(DefinedBudgetUpdated_aux + TotalExpectedCost[s] - VaR - ConditionalValue[s] <= 0);
            DefinedBudgetUpdated_aux.end();
        }


        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance(env);
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        for (a = 0; a < numAction; a++)
                            Maintenance += BenefitAction[i][a] * MaintenanceAction[a][i][t][s];
                        mod.add(Health[i][t][s] - HealthFailure[i][t][s] ==
                                Maintenance + (Health[i][t - 1][s] - AssetDegradation[s][t][i]) + HealthReset[i][t][s]);
                        Maintenance.end();
                    }
                }
            }
        }


        //faz o reset da condicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(HealthReset[i][t][s] == 0);
                    } else {
                        mod.add(HealthReset[i][t][s] <=
                                5 * MaxHealth * (AssetFailed[i][t - 1][s] + ReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        mod.add(Health[i][t][s] >= MaxHealth * (AssetFailed[i][t - 1][s] + ReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        //evita o overlap de manutencao com substituicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance_overlap(env);
                    for (a = 0; a < numAction; a++)Maintenance_overlap += MaintenanceAction[a][i][t][s];
                    mod.add(Maintenance_overlap <= 1 - (AssetFailed[i][t][s]));
                    Maintenance_overlap.end();
                }
            }
        }

        //falha do ativo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(HealthFailure[i][t][s] <= 200 * AssetFailed[i][t][s]);
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(Health[i][t][s] <= MaxHealth * (1 - AssetFailed[i][t][s]));
        }

        //o numero de subsituições esta limitada a uma
        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr SumReplaceAsset(env);
                for (t = 0; t < numLengthTW; t++)SumReplaceAsset += ReplaceAsset[i][t][s];
                mod.add(SumReplaceAsset <= 1);
            }
        }

        //sem overlap de substituicao corretiva com preventiva
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(ReplaceAsset[i][t][s] + AssetFailed[i][t][s] <= 1);
        }

        //calcula os gastos effetivos
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr CorrectiveMaintenanceCosts_aux(env), PreventiveMaintenanceCosts_aux(
                        env), PreventiveReplacementCosts_aux(env);
                for (i = 0; i < numAsset; i++) {
                    CorrectiveMaintenanceCosts_aux += CostFailure[i] * AssetFailed[i][t][s];
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += CostAction[i][a] * MaintenanceAction[a][i][t][s];
                    PreventiveReplacementCosts_aux += CostReplacingAsset[i] * ReplaceAsset[i][t][s];
                }
                mod.add(EffectiveBudget[t][s] == CorrectiveMaintenanceCosts_aux + PreventiveMaintenanceCosts_aux +
                                                 PreventiveReplacementCosts_aux);
                CorrectiveMaintenanceCosts_aux.end();
                PreventiveMaintenanceCosts_aux.end();
                PreventiveReplacementCosts_aux.end();
            }
        }

        //calculo do underbudget e overbudget
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)
                mod.add(DefinedBudget[t] - EffectiveBudget[t][s] == UnderBudget[t][s] - OverBudget[t][s]);

        //calculo dos gastos efetivos
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)mod.add(EffectiveBudget[t][s] <= DefinedBudget[t] + AllowedDeviation[t]);


        IloCplex cplex(mod);

        //Definir parametros do CPLEX
        cplex.setParam(IloCplex::TiLim, Time_Limit);
        cplex.setParam(IloCplex::EpGap, Gap);
        cplex.setParam(IloCplex::TreLim, memory);
        cplex.setParam(IloCplex::Threads, threads);

        //calcular o tempo de execucao
        time_t startTime = time(NULL);


        //resolver o modelo gerado
        cplex.solve();
        env.out() << "Solution status: " << cplex.getStatus() << endl;
        time_t finishTime = time(NULL);

        //atualizacao da condicao escolhendo um cenario

        random_device rd;     // only used once to initialise (seed) engine
        mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        uniform_int_distribution<int> uni(0, numScenarios - 1); // guaranteed unbiased

        picked_scenario = uni(rng);

        for (i = 0; i < numAsset; i++) {
            InitialHealth[i] = cplex.getValue(Health[i][numLengthTW - 1][picked_scenario]);
        }

        //#####################//
        //#####Update solution //
        //#####################//

        //atualizar valores unicos
        objective_function += cplex.getObjValue() / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        TotalTime += finishTime - startTime;
        GAP += cplex.getMIPRelativeGap();
        CorrectiveMaintenanceCosts_output +=
                cplex.getValue(CorrectiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        PreventiveMaintenanceCosts_output +=
                cplex.getValue(PreventiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedOverBudget_output +=
                cplex.getValue(ExpectedOverBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedUnderBudget_output +=
                cplex.getValue(ExpectedUnderBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedValue_output +=
                cplex.getValue(ExpectedValue) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        CVaR_output += cplex.getValue(CVaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        VaR_output += cplex.getValue(VaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);

        //atualizar matrizes de um dimensao
        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)DefinedBudget_aux[t] = DefinedBudget[t];

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, DefinedBudget_output, DefinedBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)AllowedDeviation_aux[t] = cplex.getValue(AllowedDeviation[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, AllowedDeviation_output, AllowedDeviation_aux);

        //copiar valores que advem do modelo
        for (s = 0; s < numScenarios; s++)TotalExpectedCosts_aux[s] = cplex.getValue(TotalExpectedCost[s]);

        //update solucao da variavel
        for (s = 0; s < numScenarios; s++)
            TotalExpectedCosts_output[s] += TotalExpectedCosts_aux[s] / pow(1 + RiskFreeRateMinus,
                                                                            subLengthTW * k - subLengthTW);

        //atualizar matrizes de duas dimensoes

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)UnderBudget_aux[t][s] = cplex.getValue(UnderBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, UnderBudget_output,
                         UnderBudget_aux, true);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)OverBudget_aux[t][s] = cplex.getValue(OverBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, OverBudget_output,
                         OverBudget_aux, true);

        //atualizar matrizes de tres dimensoes

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) ReplaceAsset_aux[i][t][s] = cplex.getValue(ReplaceAsset[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, ReplaceAsset_output,
                         ReplaceAsset_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) AssetFailed_aux[i][t][s] = cplex.getValue(AssetFailed[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, AssetFailed_output,
                         AssetFailed_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                Health_aux[i][t] = cplex.getValue(Health[i][t][picked_scenario]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, InitialHealth_output, Health_aux);

        //atualizar matrizes de quatro dimensoes
        //copiar valores que advem do modelo
        for (a = 0; a < numAction; a++) {
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++) {
                    for (s = 0; s < numScenarios; s++)MaintenanceAction_aux[a][i][t][s] = cplex.getValue(MaintenanceAction[a][i][t][s]);
                }
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, numAction,
                         MaintenanceAction_output, MaintenanceAction_aux, true);
    }

    //#####################//
    //#####Print results   //
    //#####################//

    print_model_results_budget_planning(model_name, instances, results_string, solution_string, results_summed_string,
                                        Time_Limit, Gap, memory, threads,
                                        objective_function, CVaR_output, VaR_output, ExpectedValue_output, TotalTime,
                                        GAP, ScenarioProbability_output, DefinedBudget_output, AllowedDeviation_output,
                                        TotalExpectedCosts_output, UnderBudget_output, OverBudget_output,
                                        PreventiveMaintenanceCosts_output, CorrectiveMaintenanceCosts_output,
                                        ExpectedOverBudget_output, ExpectedUnderBudget_output, ReplaceAsset_output,
                                        MaintenanceAction_output, AssetFailed_output, InitialHealth_output, alpha, beta,
                                        numAsset, numSubHorizons * subLengthTW, numAction, numScenarios, subLengthTW,
                                        numSubHorizons);


}

//modelo em que temos como variavel de primeiro estagio a definicao do budget e substituicao de ativos mas avaliamos a solucao obtida com um modelo estocastico ou deterministico
void solve_CVAR_Replacement_planning_solution(char *model_name, IloEnv env, const char *filename, int instances,
                                              char *results_string, char *solution_string, char *results_summed_string,
                                              int Time_Limit, float Gap, int memory, int threads, int subLengthTW,
                                              float alpha, float beta, vector<double> DefinedBudget_solution, vector<vector<int>> ReplaceAsset_solution) {


    //verificar se o ficheiro existe da instancia
    ifstream file(filename);
    if (!file) {
        cerr << "No such file: " << filename << endl;
        throw (-1);
    }

    //MODELO CPLEX////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IloNum RiskFreeRatePlus, RiskFreeRateMinus;
    IloInt MaxHealth;
    IloNumArray CostFailure(env), CostReplacingAsset(env), InitialHealth(env), InterestMultiplier(
            env), ScenarioProbability(env);
    IloNumArray2 CostAction(env), BenefitAction(env);
    IloNumArray3 AssetDegradation(env);
    file >> RiskFreeRateMinus >> RiskFreeRatePlus >> MaxHealth >> CostFailure >> CostReplacingAsset
         >> CostAction
         >> BenefitAction >> InitialHealth >> InterestMultiplier >> AssetDegradation >> ScenarioProbability;


    int numAsset = CostFailure.getSize();
    int numLengthTW = InterestMultiplier.getSize();
    int numAction = CostAction[0].getSize();
    int numScenarios = ScenarioProbability.getSize();
    int picked_scenario = 0;

    int i, t, a, s, l;
    int numSubHorizons = 1;

    //copy solution into the variable model
    vector<double> DefinedBudget(InterestMultiplier.getSize(), 0);
    for(t=0; t<numLengthTW; t++){
        DefinedBudget[t]=DefinedBudget_solution[t];
    }

    //First dimension
    vector<vector<int>> ReplaceAsset(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset[i].resize(InterestMultiplier.getSize(), 0);
    }

    for(i=0; i<numAsset; i++){
        for(t=0; t<numLengthTW; t++){
            ReplaceAsset[i][t]=ReplaceAsset_solution[i][t];
        }
    }

    if (numLengthTW != subLengthTW) {
        //calcular numero de subproblemas
        numLengthTW = subLengthTW;
        numSubHorizons = InterestMultiplier.getSize() / subLengthTW;
        //atualizar model name
        strcat(model_name, "_Rolling_Horizon");

        // atualizar calculo dos juros a pagar com base no comprimento escolhido e taxa de juro
        int tempo;
        for (int k = 1; k <= numSubHorizons; k++) {
            tempo = 0;
            if (subLengthTW > 1) {
                for (int i = (k - 1) * subLengthTW; i < k * subLengthTW - 1; i++) {
                    tempo++;
                    InterestMultiplier[i] = (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) * RiskFreeRatePlus /
                                             (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) - 1));
                }
                //atualizar o ultimo valor do calculo dos juros
                InterestMultiplier[k * subLengthTW - 1] = InterestMultiplier[k * subLengthTW - 2];
            } else {
                InterestMultiplier[k * subLengthTW - 1] = 1;
            }

        }
    }

    //Declarar variaveis output resultado
    double objective_function = 0;
    double TotalTime = 0;
    double GAP = 0;
    double PreventiveMaintenanceCosts_output = 0;
    double CorrectiveMaintenanceCosts_output = 0;
    double ExpectedOverBudget_output = 0;
    double ExpectedUnderBudget_output = 0;
    double CVaR_output = 0;
    double VaR_output = 0;
    double ExpectedValue_output = 0;

    //First dimension
    vector<double> TotalExpectedCosts_output(numScenarios, 0);
    //First dimension
    vector<double> DefinedBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> AllowedDeviation_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> ScenarioProbability_output(numScenarios, 0);

    //copiar valores que advem do modelo
    for (s = 0; s < numScenarios; s++)ScenarioProbability_output[s] = ScenarioProbability[s];

    //First dimension
    vector<vector<double>> UnderBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        UnderBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        OverBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> InitialHealth_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        InitialHealth_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<int>> ReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<vector<int>>> AntecipateReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AntecipateReplaceAsset_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)
            AntecipateReplaceAsset_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)AssetFailed_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_output(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_output[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_output[a][i].resize(InterestMultiplier.getSize());
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < InterestMultiplier.getSize(); t++)
                MaintenanceAction_output[a][i][t].resize(numScenarios, 0);
        }
    }

    //criar variaveis auxiliares
    vector<double> DefinedBudget_aux(subLengthTW, 0);
    vector<double> AllowedDeviation_aux(subLengthTW, 0);
    vector<double> TotalExpectedCosts_aux(numScenarios, 0);

    //First dimension
    vector<vector<double>> UnderBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        UnderBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        OverBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<int>> ReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AntecipateReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AntecipateReplaceAsset_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AntecipateReplaceAsset_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AssetFailed_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> Health_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        Health_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_aux(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_aux[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_aux[a][i].resize(subLengthTW);
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < subLengthTW; t++)MaintenanceAction_aux[a][i][t].resize(numScenarios, 0);
        }
    }

    //#####################//
    //#####Model definition//
    //#####################//
    for (int k = 1; k <= numSubHorizons; k++) {
        IloModel mod(env);

        //atualizar o budget e substituicao definido na solucao de acordo com o modelo gerado(atencao que os primeiros valores do subperiodo sao sempre atualizados)
        if (k > 1) {
            //budget
            for (t = 0; t < numLengthTW; t++) {
                DefinedBudget[t] = DefinedBudget[(k - 1) * subLengthTW + t];
            }

            //substituicao
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++) {
                    ReplaceAsset[i][t] = ReplaceAsset[i][(k - 1) * subLengthTW + t];
                }

            }
        }


        //DECISION VARIABLES
        IloNumVarArray AllowedDeviation(env, numLengthTW, 0.0, IloInfinity,
                                        ILOFLOAT); //desvio orçamental que é permitido em média para cada instante t

        IloNumVarArray4 MaintenanceAction(env, numAction);
        for (a = 0; a < numAction; ++a) {
            MaintenanceAction[a] = IloNumVarArray3(env, numAsset);
            for (i = 0; i < numAsset; ++i) {
                MaintenanceAction[a][i] = IloNumVarArray2(env, numLengthTW);
                for (t = 0; t < numLengthTW; t++)
                    MaintenanceAction[a][i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                                ILOBOOL); //plano de manutencao preventiva depende dos cenarios
            }
        }


        IloNumVarArray3 AntecipateReplaceAsset(env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            AntecipateReplaceAsset[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                AntecipateReplaceAsset[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                              ILOBOOL); //Plano de emergencia para substituir ativos que avariam antes do planeado. Depende do cenario
            }
        }

        //AUXILIARY VARIABLES

        //calculos auxiliares
        IloNumVarArray TotalExpectedCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //calcula o custo esperado

        IloNumVarArray2 EffectiveBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            EffectiveBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                ILOFLOAT); //calcula os gastos efetivos em mautencao

        IloNumVarArray2 UnderBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            UnderBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                            ILOFLOAT); //calcula os gastos em mautencao que nao excederam o budget planeado

        IloNumVarArray2 OverBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            OverBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                           ILOFLOAT); //calcula os gastos em mautencao que excederam o budget planeado

        IloNumVarArray3 Health(env, numAsset), HealthFailure(env, numAsset), HealthReset(env, numAsset), AssetFailed(
                env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            Health[i] = IloNumVarArray2(env, numLengthTW);
            HealthFailure[i] = IloNumVarArray2(env, numLengthTW);
            HealthReset[i] = IloNumVarArray2(env, numLengthTW);
            AssetFailed[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                Health[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                              ILOFLOAT); //atualiza a saude do equipamento
                HealthFailure[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //ativa a falha
                HealthReset[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                   ILOFLOAT); //ativa o reset da condição
                AssetFailed[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                   ILOBOOL); // variavel utilzada para quando o equipamento falha
            }
        }

        IloNumVar CorrectiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das corretivas expectável
        IloNumVar PreventiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das preventivas expectável
        IloNumVar ExpectedOverBudget(env, 0.0, IloInfinity, ILOFLOAT); //custo expectavel de over budget
        IloNumVar ExpectedUnderBudget(env, 0.0, IloInfinity, ILOFLOAT);  //custo expectavel de under budget

        //variaveis auxiliares do modelo CVaR
        IloNumVar VaR(env, 0.0, IloInfinity, ILOFLOAT); //Value at Risk
        IloNumVar CVaR(env, 0.0, IloInfinity, ILOFLOAT); //Conditional Value at Risk
        IloNumVarArray ConditionalValue(env, numScenarios, 0.0, IloInfinity,
                                        ILOFLOAT); //toma valores diferentes de zero quando acima do VaR
        IloNumVar ExpectedValue(env, 0.0, IloInfinity, ILOFLOAT); //valor esperado da função objetivo
        IloNumVarArray ScenarioCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //custo de um dado cenário

        // OBJECTIVE FUNCTION

        //custo do overbudget por scenario
        for (s = 0; s < numScenarios; s++) {
            IloExpr OverBudgetCost(env);
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    OverBudgetCost += InterestMultiplier[l] * OverBudget[t][s] / pow(1 + RiskFreeRateMinus, l + 1);
            }
            mod.add(TotalExpectedCost[s] == OverBudgetCost);
            OverBudgetCost.end();
        }

        IloExpr TotalBudget(env);
        //valor esperado dos custos
        for (t = 0; t < numLengthTW; t++)TotalBudget += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
        for (s = 0; s < numScenarios; s++)TotalBudget += ScenarioProbability[s] * TotalExpectedCost[s];
        mod.add(ExpectedValue == TotalBudget);

        //cálculo do custo de cada cenário
        for (s = 0; s < numScenarios; s++) {
            IloExpr ScenarioCost_aux(env);
            for (t = 0; t < numLengthTW; t++)
                ScenarioCost_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1) + TotalExpectedCost[s];
            mod.add(ScenarioCost[s] == ScenarioCost_aux);
            ScenarioCost_aux.end();
        }

        //Calculo CVaR
        IloExpr CVaR_tail_cost(env);
        for (s = 0; s < numScenarios; s++)CVaR_tail_cost += ScenarioProbability[s] * ConditionalValue[s];
        mod.add(CVaR == VaR + 1 / (1 - alpha) * CVaR_tail_cost);
        CVaR_tail_cost.end();

        //adicionar a funcao objetivo
        mod.add(IloMinimize(env, (1 - beta) * TotalBudget + beta * CVaR));
        TotalBudget.end();

        //Calculos auxiliares

        //calculo dos custos corretiva
        IloExpr CorrectiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++)
                    CorrectiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostFailure[i] * AssetFailed[i][t][s]) /
                                                      pow(1 + RiskFreeRateMinus, t + 1);
            }
        }
        mod.add(CorrectiveMaintenanceCosts == CorrectiveMaintenanceCosts_aux);
        CorrectiveMaintenanceCosts_aux.end();

        //calculo dos custos preventiva
        IloExpr PreventiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++) {
                    PreventiveMaintenanceCosts_aux +=
                            ScenarioProbability[s] * (CostReplacingAsset[i] * AntecipateReplaceAsset[i][t][s]) /
                            pow(1 + RiskFreeRateMinus, t + 1);
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostAction[i][a] *
                                                                                    MaintenanceAction[a][i][t][s]) /
                                                          pow(1 + RiskFreeRateMinus, t + 1);
                }
            }
        }
        mod.add(PreventiveMaintenanceCosts == PreventiveMaintenanceCosts_aux);
        PreventiveMaintenanceCosts_aux.end();

        //calculo do custo expectavel de underbudget
        IloExpr ExpectedUnderBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++)
                ExpectedUnderBudget_aux += ScenarioProbability[s] * UnderBudget[t][s] /
                                           pow(1 + RiskFreeRateMinus, t + 1);
        }
        mod.add(ExpectedUnderBudget == ExpectedUnderBudget_aux);
        ExpectedUnderBudget_aux.end();

        //calculo do custo expectavel de overbudget
        IloExpr ExpectedOverBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    ExpectedOverBudget_aux += ScenarioProbability[s] * OverBudget[t][s] /
                                              pow(1 + RiskFreeRateMinus, l + 1);
            }
        }
        mod.add(ExpectedOverBudget == ExpectedOverBudget_aux);
        ExpectedOverBudget_aux.end();

        //PROBLEM CONSTRAINTS

        //verificar se está acima do VaR
        for (s = 0; s < numScenarios; s++) {
            IloExpr DefinedBudgetUpdated_aux(env);
            for (t = 0; t < numLengthTW; t++)
                DefinedBudgetUpdated_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
            mod.add(DefinedBudgetUpdated_aux + TotalExpectedCost[s] - VaR - ConditionalValue[s] <= 0);
            DefinedBudgetUpdated_aux.end();
        }


        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance(env);
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        for (a = 0; a < numAction; a++)
                            Maintenance += BenefitAction[i][a] * MaintenanceAction[a][i][t][s];
                        mod.add(Health[i][t][s] - HealthFailure[i][t][s] ==
                                Maintenance + (Health[i][t - 1][s] - AssetDegradation[s][t][i]) + HealthReset[i][t][s]);
                        Maintenance.end();
                    }
                }
            }
        }


        //faz o reset da condicao (ter cuidado com o upper bound no reset da saude do ativo)
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(HealthReset[i][t][s] == 0);
                    } else {
                        mod.add(HealthReset[i][t][s] <=
                                5 * MaxHealth * (AssetFailed[i][t - 1][s] + AntecipateReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        mod.add(Health[i][t][s] >=
                                MaxHealth * (AssetFailed[i][t - 1][s] + AntecipateReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        //evita o overlap de manutencao com substituicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance_overlap(env);
                    for (a = 0; a < numAction; a++)Maintenance_overlap += MaintenanceAction[a][i][t][s];
                    mod.add(Maintenance_overlap <= 1 - (AssetFailed[i][t][s]));
                    Maintenance_overlap.end();
                }
            }
        }

        //falha do ativo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(HealthFailure[i][t][s] <= 5 * MaxHealth * AssetFailed[i][t][s]);
        }


        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(Health[i][t][s] <= MaxHealth * (1 - AssetFailed[i][t][s]));
        }

        //substituicao do ativo antes do inicialmente planeado
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(AntecipateReplaceAsset[i][t][s] >= ReplaceAsset[i][t]);
                    } else {
                        IloExpr FailuresCount(env);
                        for (l = 0; l <= t - 1; l++)FailuresCount += AssetFailed[i][l][s];
                        mod.add(AntecipateReplaceAsset[i][t][s] + FailuresCount >= ReplaceAsset[i][t]);
                        FailuresCount.end();
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr UnplannedFailuresCount(env), PlannedReplacement(env);
                for (t = 0; t < numLengthTW; t++) {
                    UnplannedFailuresCount += AntecipateReplaceAsset[i][t][s];
                    PlannedReplacement += ReplaceAsset[i][t];
                }
                mod.add(UnplannedFailuresCount <= PlannedReplacement);
                UnplannedFailuresCount.end();
                PlannedReplacement.end();
            }
        }

        //o numero de subsituições esta limitada a uma
        for (i = 0; i < numAsset; i++) {
            IloExpr SumReplaceAsset(env);
            for (t = 0; t < numLengthTW; t++)SumReplaceAsset += ReplaceAsset[i][t];
            mod.add(SumReplaceAsset <= 1);
        }

        //sem overlap de substituicao corretiva com preventiva
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(ReplaceAsset[i][t] + AssetFailed[i][t][s] <= 1);
        }

        //Para evitar que exista a substituição planeada quando temos uma falha
        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++){
                IloExpr checkPlannedReplacement(env);
                for (t = 0; t < numLengthTW; t++){
                    checkPlannedReplacement+=ReplaceAsset[i][t]+AntecipateReplaceAsset[i][t][s]+AssetFailed[i][t][s];
                }
                mod.add(checkPlannedReplacement <= 2);
                checkPlannedReplacement.end();
            }
        }

        //calcula os gastos effetivos
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr CorrectiveMaintenanceCosts_aux(env), PreventiveMaintenanceCosts_aux(
                        env), PreventiveReplacementCosts_aux(env);
                for (i = 0; i < numAsset; i++) {
                    CorrectiveMaintenanceCosts_aux += CostFailure[i] * AssetFailed[i][t][s];
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += CostAction[i][a] * MaintenanceAction[a][i][t][s];
                    PreventiveReplacementCosts_aux += CostReplacingAsset[i] * AntecipateReplaceAsset[i][t][s];
                }
                mod.add(EffectiveBudget[t][s] == CorrectiveMaintenanceCosts_aux + PreventiveMaintenanceCosts_aux +
                                                 PreventiveReplacementCosts_aux);
                CorrectiveMaintenanceCosts_aux.end();
                PreventiveMaintenanceCosts_aux.end();
                PreventiveReplacementCosts_aux.end();
            }
        }

        //calculo do underbudget e overbudget
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)
                mod.add(DefinedBudget[t] - EffectiveBudget[t][s] == UnderBudget[t][s] - OverBudget[t][s]);

        //calculo dos gastos efetivos
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)mod.add(EffectiveBudget[t][s] <= DefinedBudget[t] + AllowedDeviation[t]);


        IloCplex cplex(mod);

        //Definir parametros do CPLEX
        cplex.setParam(IloCplex::TiLim, Time_Limit);
        cplex.setParam(IloCplex::EpGap, Gap);
        cplex.setParam(IloCplex::TreLim, memory);
        cplex.setParam(IloCplex::Threads, threads);

        //calcular o tempo de execucao
        time_t startTime = time(NULL);


        //resolver o modelo gerado
        cplex.solve();
        env.out() << "Solution status: " << cplex.getStatus() << endl;
        time_t finishTime = time(NULL);

        //atualizacao da condicao escolhendo um cenario

        random_device rd;     // only used once to initialise (seed) engine
        mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        uniform_int_distribution<int> uni(0, numScenarios - 1); // guaranteed unbiased

        picked_scenario = uni(rng);

        for (i = 0; i < numAsset; i++) {
            InitialHealth[i] = cplex.getValue(Health[i][numLengthTW - 1][picked_scenario]);
        }

        //#####################//
        //#####Update solution //
        //#####################//

        //atualizar valores unicos
        objective_function += cplex.getObjValue() / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        TotalTime += finishTime - startTime;
        GAP += cplex.getMIPRelativeGap();
        CorrectiveMaintenanceCosts_output +=
                cplex.getValue(CorrectiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        PreventiveMaintenanceCosts_output +=
                cplex.getValue(PreventiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedOverBudget_output +=
                cplex.getValue(ExpectedOverBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedUnderBudget_output +=
                cplex.getValue(ExpectedUnderBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedValue_output +=
                cplex.getValue(ExpectedValue) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        CVaR_output += cplex.getValue(CVaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        VaR_output += cplex.getValue(VaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);

        //atualizar matrizes de um dimensao
        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)DefinedBudget_aux[t] = DefinedBudget[t];

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, DefinedBudget_output, DefinedBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)AllowedDeviation_aux[t] = cplex.getValue(AllowedDeviation[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, AllowedDeviation_output, AllowedDeviation_aux);

        //copiar valores que advem do modelo
        for (s = 0; s < numScenarios; s++)TotalExpectedCosts_aux[s] = cplex.getValue(TotalExpectedCost[s]);

        //update solucao da variavel
        for (s = 0; s < numScenarios; s++)
            TotalExpectedCosts_output[s] += TotalExpectedCosts_aux[s] / pow(1 + RiskFreeRateMinus,
                                                                            subLengthTW * k - subLengthTW);

        //atualizar matrizes de duas dimensoes

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)UnderBudget_aux[t][s] = cplex.getValue(UnderBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, UnderBudget_output,
                         UnderBudget_aux, true);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)OverBudget_aux[t][s] = cplex.getValue(OverBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, OverBudget_output,
                         OverBudget_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                ReplaceAsset_aux[i][t] = ReplaceAsset[i][t];
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, ReplaceAsset_output,
                         ReplaceAsset_aux);

        //atualizar matrizes de tres dimensoes

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++)
                    AntecipateReplaceAsset_aux[i][t][s] = cplex.getValue(AntecipateReplaceAsset[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset,
                         AntecipateReplaceAsset_output, AntecipateReplaceAsset_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) AssetFailed_aux[i][t][s] = cplex.getValue(AssetFailed[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, AssetFailed_output,
                         AssetFailed_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                Health_aux[i][t] = cplex.getValue(Health[i][t][picked_scenario]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, InitialHealth_output, Health_aux);

        //atualizar matrizes de quatro dimensoes
        //copiar valores que advem do modelo
        for (a = 0; a < numAction; a++) {
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++) {
                    for (s = 0; s < numScenarios; s++)MaintenanceAction_aux[a][i][t][s] = cplex.getValue(MaintenanceAction[a][i][t][s]);
                }
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, numAction,
                         MaintenanceAction_output, MaintenanceAction_aux, true);
    }

    //#####################//
    //#####Print results   //
    //#####################//

    print_model_results_replacement_planning(model_name, instances, results_string, solution_string,
                                             results_summed_string, Time_Limit, Gap, memory, threads,
                                             objective_function, CVaR_output, VaR_output, ExpectedValue_output,
                                             TotalTime, GAP, ScenarioProbability_output, DefinedBudget_output,
                                             AllowedDeviation_output,
                                             TotalExpectedCosts_output, UnderBudget_output, OverBudget_output,
                                             PreventiveMaintenanceCosts_output, CorrectiveMaintenanceCosts_output,
                                             ExpectedOverBudget_output, ExpectedUnderBudget_output, ReplaceAsset_output,
                                             AntecipateReplaceAsset_output, MaintenanceAction_output,
                                             AssetFailed_output, InitialHealth_output, alpha, beta, numAsset,
                                             numSubHorizons * subLengthTW, numAction, numScenarios, subLengthTW,
                                             numSubHorizons);


}


//a funcao e utiliza para selecionar o modelo escolhido por defeito para utilizar solucao previament gerada
void solve_selected_model_with_solution(int model_version, IloEnv env, const char *filename, int instances, char *results_string,
                                        char *solution_string, char *results_summed_string, int Time_Limit, float Gap, int memory,
                                        int threads, int subLengthTW, float alpha, float beta,
                                        vector<double> DefinedBudget_solution=DefinedBudget_default, vector<vector<int>> ReplaceAsset_solution=ReplaceAsset_default) {

    char *model_name = new char[100];

    switch (model_version) {
        case 2: //"CVAR_budget_planning_Stochastic_solution"
            strcpy(model_name, "CVAR_budget_planning_Stochastic_solution");
            solve_CVAR_Budget_planning_solution(model_name, env, filename, instances, results_string, solution_string,
                                                results_summed_string, Time_Limit, Gap, memory, threads, subLengthTW,
                                                alpha, beta,DefinedBudget_solution);
            break;
        case 3: //"CVAR_budget_planning_Deterministic_solution"
            strcpy(model_name, "CVAR_budget_planning_Deterministic_solution");
            solve_CVAR_Budget_planning_solution(model_name, env, filename, instances, results_string, solution_string,
                                                results_summed_string, Time_Limit, Gap, memory, threads, subLengthTW,
                                                alpha, beta,DefinedBudget_solution);
            break;
        case 5: //"CVAR_replacement_planning_Stochastic_solution"
            strcpy(model_name, "CVAR_replacement_planning_Stochastic_solution");
            solve_CVAR_Replacement_planning_solution(model_name, env, filename, instances, results_string,
                                                     solution_string, results_summed_string, Time_Limit, Gap, memory,
                                                     threads, subLengthTW, alpha, beta,DefinedBudget_solution,ReplaceAsset_solution);
            break;
        case 6: //"CVAR_replacement_planning_Deterministic_solution"
            strcpy(model_name, "CVAR_replacement_planning_Deterministic_solution");
            solve_CVAR_Replacement_planning_solution(model_name, env, filename, instances, results_string,
                                                     solution_string, results_summed_string, Time_Limit, Gap, memory,
                                                     threads, subLengthTW, alpha, beta,DefinedBudget_solution,ReplaceAsset_solution);
            break;
        default: //optional
            cout << "Failed to find a valid model";
            exit(EXIT_FAILURE);
    }
}

//modelo em que temos como variavel de primeiro estagio a definicao do budget
void solve_CVAR_Budget_planning(char *model_name, IloEnv env,const char *filename, int instances, char *results_string,
                                char *solution_string, char *results_summed_string, int Time_Limit, float Gap,
                                int memory, int threads, int subLengthTW, float alpha, float beta) {


    //verificar se o ficheiro existe da instancia
    ifstream file(filename);
    if (!file) {
        cerr << "No such file: " << filename << endl;
        throw (-1);
    }

    //MODELO CPLEX////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IloNum RiskFreeRatePlus, RiskFreeRateMinus;
    IloInt MaxHealth;
    IloNumArray CostFailure(env), CostReplacingAsset(env), InitialHealth(env), InterestMultiplier(
            env), ScenarioProbability(env);
    IloNumArray2 CostAction(env), BenefitAction(env);
    IloNumArray3 AssetDegradation(env);

    file >> RiskFreeRateMinus >> RiskFreeRatePlus >> MaxHealth >> CostFailure >> CostReplacingAsset
         >> CostAction
         >> BenefitAction >> InitialHealth >> InterestMultiplier >> AssetDegradation >> ScenarioProbability;


    int numAsset = CostFailure.getSize();
    int numLengthTW = InterestMultiplier.getSize();
    int numAction = CostAction[0].getSize();
    int numScenarios = ScenarioProbability.getSize();
    int picked_scenario = 0;

    int i, t, a, s, l;
    int numSubHorizons = 1;

    if (numLengthTW != subLengthTW) {
        //calcular numero de subproblemas
        numLengthTW = subLengthTW;
        numSubHorizons = InterestMultiplier.getSize() / subLengthTW;
        //atualizar model name
        strcat(model_name, "_Rolling_Horizon");

        // atualizar calculo dos juros a pagar com base no comprimento escolhido e taxa de juro
        int tempo;
        for (int k = 1; k <= numSubHorizons; k++) {
            tempo = 0;
            if (subLengthTW > 1) {
                for (int i = (k - 1) * subLengthTW; i < k * subLengthTW - 1; i++) {
                    tempo++;
                    InterestMultiplier[i] = (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) * RiskFreeRatePlus /
                                             (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) - 1));
                }
                //atualizar o ultimo valor do calculo dos juros
                InterestMultiplier[k * subLengthTW - 1] = InterestMultiplier[k * subLengthTW - 2];
            } else {
                InterestMultiplier[k * subLengthTW - 1] = 1;
            }

        }
    }

    //Declarar variaveis output resultado
    double objective_function = 0;
    double TotalTime = 0;
    double GAP = 0;
    double PreventiveMaintenanceCosts_output = 0;
    double CorrectiveMaintenanceCosts_output = 0;
    double ExpectedOverBudget_output = 0;
    double ExpectedUnderBudget_output = 0;
    double CVaR_output = 0;
    double VaR_output = 0;
    double ExpectedValue_output = 0;

    //First dimension
    vector<double> TotalExpectedCosts_output(numScenarios, 0);
    //First dimension
    vector<double> DefinedBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> AllowedDeviation_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> ScenarioProbability_output(numScenarios, 0);

    //copiar valores que advem do modelo
    for (s = 0; s < numScenarios; s++)ScenarioProbability_output[s] = ScenarioProbability[s];

    //First dimension
    vector<vector<double>> UnderBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        UnderBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        OverBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> InitialHealth_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        InitialHealth_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<vector<int>>> ReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)ReplaceAsset_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)AssetFailed_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_output(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_output[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_output[a][i].resize(InterestMultiplier.getSize());
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < InterestMultiplier.getSize(); t++)
                MaintenanceAction_output[a][i][t].resize(numScenarios, 0);
        }
    }

    //criar variaveis auxiliares
    vector<double> DefinedBudget_aux(subLengthTW, 0);
    vector<double> AllowedDeviation_aux(subLengthTW, 0);
    vector<double> TotalExpectedCosts_aux(numScenarios, 0);

    //First dimension
    vector<vector<double>> UnderBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        UnderBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        OverBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> ReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)ReplaceAsset_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AssetFailed_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> Health_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        Health_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_aux(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_aux[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_aux[a][i].resize(subLengthTW);
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < subLengthTW; t++)MaintenanceAction_aux[a][i][t].resize(numScenarios, 0);
        }
    }

    //#####################//
    //#####Model definition//
    //#####################//
    for (int k = 1; k <= numSubHorizons; k++) {
        IloModel mod(env);

        //DECISION VARIABLES
        IloNumVarArray DefinedBudget(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);
        IloNumVarArray AllowedDeviation(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);

        IloNumVarArray3 ReplaceAsset(env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            ReplaceAsset[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t)
                ReplaceAsset[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0, ILOBOOL);
        }

        IloNumVarArray4 MaintenanceAction(env, numAction);
        for (a = 0; a < numAction; ++a) {
            MaintenanceAction[a] = IloNumVarArray3(env, numAsset);
            for (i = 0; i < numAsset; ++i) {
                MaintenanceAction[a][i] = IloNumVarArray2(env, numLengthTW);
                for (t = 0; t < numLengthTW; t++)
                    MaintenanceAction[a][i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0, ILOBOOL);
            }
        }


        //AUXILIARY VARIABLES

        //calculos auxiliares
        IloNumVarArray TotalExpectedCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //calcula o custo esperado

        IloNumVarArray2 EffectiveBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            EffectiveBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                ILOFLOAT); //calcula os gastos efetivos em mautencao

        IloNumVarArray2 UnderBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            UnderBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                            ILOFLOAT); //calcula os gastos em mautencao que nao excederam o budget planeado

        IloNumVarArray2 OverBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            OverBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                           ILOFLOAT); //calcula os gastos em mautencao que excederam o budget planeado

        IloNumVarArray3 Health(env, numAsset), HealthFailure(env, numAsset), HealthReset(env, numAsset), AssetFailed(
                env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            Health[i] = IloNumVarArray2(env, numLengthTW);
            HealthFailure[i] = IloNumVarArray2(env, numLengthTW);
            HealthReset[i] = IloNumVarArray2(env, numLengthTW);
            AssetFailed[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                Health[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                              ILOFLOAT); //atualiza a saude do equipamento
                HealthFailure[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //ativa a falha
                HealthReset[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                   ILOFLOAT); //ativa o reset da condição
                AssetFailed[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                   ILOBOOL); // variavel utilzada para quando o equipamento falha
            }
        }

        IloNumVar CorrectiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das corretivas expectável
        IloNumVar PreventiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das preventivas expectável
        IloNumVar ExpectedOverBudget(env, 0.0, IloInfinity, ILOFLOAT); //custo expectavel de over budget
        IloNumVar ExpectedUnderBudget(env, 0.0, IloInfinity, ILOFLOAT);  //custo expectavel de under budget

        //variaveis auxiliares do modelo CVaR
        IloNumVar VaR(env, 0.0, IloInfinity, ILOFLOAT); //Value at Risk
        IloNumVar CVaR(env, 0.0, IloInfinity, ILOFLOAT); //Conditional Value at Risk
        IloNumVarArray ConditionalValue(env, numScenarios, 0.0, IloInfinity,
                                        ILOFLOAT); //toma valores diferentes de zero quando acima do VaR
        IloNumVar ExpectedValue(env, 0.0, IloInfinity, ILOFLOAT); //valor esperado da função objetivo
        IloNumVarArray ScenarioCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //custo de um dado cenário

        // OBJECTIVE FUNCTION

        //custo do overbudget por scenario
        for (s = 0; s < numScenarios; s++) {
            IloExpr OverBudgetCost(env);
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    OverBudgetCost += InterestMultiplier[l] * OverBudget[t][s] / pow(1 + RiskFreeRateMinus, l + 1);
            }
            mod.add(TotalExpectedCost[s] == OverBudgetCost);
            OverBudgetCost.end();
        }

        IloExpr TotalBudget(env);
        //valor esperado dos custos
        for (t = 0; t < numLengthTW; t++)TotalBudget += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
        for (s = 0; s < numScenarios; s++)TotalBudget += ScenarioProbability[s] * TotalExpectedCost[s];
        mod.add(ExpectedValue == TotalBudget);

        //cálculo do custo de cada cenário
        for (s = 0; s < numScenarios; s++) {
            IloExpr ScenarioCost_aux(env);
            for (t = 0; t < numLengthTW; t++)
                ScenarioCost_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1) + TotalExpectedCost[s];
            mod.add(ScenarioCost[s] == ScenarioCost_aux);
            ScenarioCost_aux.end();
        }

        //Calculo CVaR
        IloExpr CVaR_tail_cost(env);
        for (s = 0; s < numScenarios; s++)CVaR_tail_cost += ScenarioProbability[s] * ConditionalValue[s];
        mod.add(CVaR == VaR + 1 / (1 - alpha) * CVaR_tail_cost);
        CVaR_tail_cost.end();

        //adicionar a funcao objetivo
        mod.add(IloMinimize(env, (1 - beta) * TotalBudget + beta * CVaR));
        TotalBudget.end();

        //Calculos auxiliares

        //calculo dos custos corretiva
        IloExpr CorrectiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++)
                    CorrectiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostFailure[i] * AssetFailed[i][t][s] /
                                                                                pow(1 + RiskFreeRateMinus, t + 1));
            }
        }
        mod.add(CorrectiveMaintenanceCosts == CorrectiveMaintenanceCosts_aux);
        CorrectiveMaintenanceCosts_aux.end();

        //calculo dos custos preventiva
        IloExpr PreventiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++) {
                    PreventiveMaintenanceCosts_aux +=
                            ScenarioProbability[s] * (CostReplacingAsset[i] * ReplaceAsset[i][t][s]) /
                            pow(1 + RiskFreeRateMinus, t + 1);
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostAction[i][a] *
                                                                                    MaintenanceAction[a][i][t][s]) /
                                                          pow(1 + RiskFreeRateMinus, t + 1);
                }
            }
        }
        mod.add(PreventiveMaintenanceCosts == PreventiveMaintenanceCosts_aux);
        PreventiveMaintenanceCosts_aux.end();

        //calculo do custo expectavel de underbudget
        IloExpr ExpectedUnderBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++)
                ExpectedUnderBudget_aux += ScenarioProbability[s] * UnderBudget[t][s] /
                                           pow(1 + RiskFreeRateMinus, t + 1);
        }
        mod.add(ExpectedUnderBudget == ExpectedUnderBudget_aux);
        ExpectedUnderBudget_aux.end();

        //calculo do custo expectavel de overbudget
        IloExpr ExpectedOverBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    ExpectedOverBudget_aux += ScenarioProbability[s] * OverBudget[t][s] /
                                              pow(1 + RiskFreeRateMinus, l + 1);
            }
        }
        mod.add(ExpectedOverBudget == ExpectedOverBudget_aux);
        ExpectedOverBudget_aux.end();

        //PROBLEM CONSTRAINTS

        //verificar se está acima do VaR
        for (s = 0; s < numScenarios; s++) {
            IloExpr DefinedBudgetUpdated_aux(env);
            for (t = 0; t < numLengthTW; t++)
                DefinedBudgetUpdated_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
            mod.add(DefinedBudgetUpdated_aux + TotalExpectedCost[s] - VaR - ConditionalValue[s] <= 0);
            DefinedBudgetUpdated_aux.end();
        }


        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance(env);
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        for (a = 0; a < numAction; a++)
                            Maintenance += BenefitAction[i][a] * MaintenanceAction[a][i][t][s];
                        mod.add(Health[i][t][s] - HealthFailure[i][t][s] ==
                                Maintenance + (Health[i][t - 1][s] - AssetDegradation[s][t][i]) + HealthReset[i][t][s]);
                        Maintenance.end();
                    }
                }
            }
        }


        //faz o reset da condicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(HealthReset[i][t][s] == 0);
                    } else {
                        mod.add(HealthReset[i][t][s] <=
                                5 * MaxHealth * (AssetFailed[i][t - 1][s] + ReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        mod.add(Health[i][t][s] >= MaxHealth * (AssetFailed[i][t - 1][s] + ReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        //evita o overlap de manutencao com substituicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance_overlap(env);
                    for (a = 0; a < numAction; a++)Maintenance_overlap += MaintenanceAction[a][i][t][s];
                    mod.add(Maintenance_overlap <= 1 - (AssetFailed[i][t][s]));
                    Maintenance_overlap.end();
                }
            }
        }

        //falha do ativo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(HealthFailure[i][t][s] <= 200 * AssetFailed[i][t][s]);
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(Health[i][t][s] <= MaxHealth * (1 - AssetFailed[i][t][s]));
        }

        //o numero de subsituições esta limitada a uma
        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr SumReplaceAsset(env);
                for (t = 0; t < numLengthTW; t++)SumReplaceAsset += ReplaceAsset[i][t][s];
                mod.add(SumReplaceAsset <= 1);
            }
        }

        //sem overlap de substituicao corretiva com preventiva
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(ReplaceAsset[i][t][s] + AssetFailed[i][t][s] <= 1);
        }

        //calcula os gastos effetivos
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr CorrectiveMaintenanceCosts_aux(env), PreventiveMaintenanceCosts_aux(
                        env), PreventiveReplacementCosts_aux(env);
                for (i = 0; i < numAsset; i++) {
                    CorrectiveMaintenanceCosts_aux += CostFailure[i] * AssetFailed[i][t][s];
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += CostAction[i][a] * MaintenanceAction[a][i][t][s];
                    PreventiveReplacementCosts_aux += CostReplacingAsset[i] * ReplaceAsset[i][t][s];
                }
                mod.add(EffectiveBudget[t][s] == CorrectiveMaintenanceCosts_aux + PreventiveMaintenanceCosts_aux +
                                                 PreventiveReplacementCosts_aux);
                CorrectiveMaintenanceCosts_aux.end();
                PreventiveMaintenanceCosts_aux.end();
                PreventiveReplacementCosts_aux.end();
            }
        }

        //calculo do underbudget e overbudget
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)
                mod.add(DefinedBudget[t] - EffectiveBudget[t][s] == UnderBudget[t][s] - OverBudget[t][s]);

        //calculo dos gastos efetivos
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)mod.add(EffectiveBudget[t][s] <= DefinedBudget[t] + AllowedDeviation[t]);


        IloCplex cplex(mod);

        //Definir parametros do CPLEX
        cplex.setParam(IloCplex::TiLim, Time_Limit);
        cplex.setParam(IloCplex::EpGap, Gap);
        cplex.setParam(IloCplex::TreLim, memory);
        cplex.setParam(IloCplex::Threads, threads);

        //calcular o tempo de execucao
        time_t startTime = time(NULL);


        //resolver o modelo gerado
        cplex.solve();
        env.out() << "Solution status: " << cplex.getStatus() << endl;
        time_t finishTime = time(NULL);

        //atualizacao da condicao escolhendo um cenario

        random_device rd;     // only used once to initialise (seed) engine
        mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        uniform_int_distribution<int> uni(0, numScenarios - 1); // guaranteed unbiased

        picked_scenario = uni(rng);

        for (i = 0; i < numAsset; i++) {
            InitialHealth[i] = cplex.getValue(Health[i][numLengthTW - 1][picked_scenario]);
        }

        //#####################//
        //#####Update solution //
        //#####################//

        //atualizar valores unicos
        objective_function += cplex.getObjValue() / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        TotalTime += finishTime - startTime;
        GAP += cplex.getMIPRelativeGap();
        CorrectiveMaintenanceCosts_output +=
                cplex.getValue(CorrectiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        PreventiveMaintenanceCosts_output +=
                cplex.getValue(PreventiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedOverBudget_output +=
                cplex.getValue(ExpectedOverBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedUnderBudget_output +=
                cplex.getValue(ExpectedUnderBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedValue_output +=
                cplex.getValue(ExpectedValue) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        CVaR_output += cplex.getValue(CVaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        VaR_output += cplex.getValue(VaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);

        //atualizar matrizes de um dimensao
        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)DefinedBudget_aux[t] = cplex.getValue(DefinedBudget[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, DefinedBudget_output, DefinedBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)AllowedDeviation_aux[t] = cplex.getValue(AllowedDeviation[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, AllowedDeviation_output, AllowedDeviation_aux);

        //copiar valores que advem do modelo
        for (s = 0; s < numScenarios; s++)TotalExpectedCosts_aux[s] = cplex.getValue(TotalExpectedCost[s]);

        //update solucao da variavel
        for (s = 0; s < numScenarios; s++)
            TotalExpectedCosts_output[s] += TotalExpectedCosts_aux[s] / pow(1 + RiskFreeRateMinus,
                                                                            subLengthTW * k - subLengthTW);

        //atualizar matrizes de duas dimensoes

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)UnderBudget_aux[t][s] = cplex.getValue(UnderBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, UnderBudget_output,
                         UnderBudget_aux, true);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)OverBudget_aux[t][s] = cplex.getValue(OverBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, OverBudget_output,
                         OverBudget_aux, true);

        //atualizar matrizes de tres dimensoes

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) ReplaceAsset_aux[i][t][s] = cplex.getValue(ReplaceAsset[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, ReplaceAsset_output,
                         ReplaceAsset_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) AssetFailed_aux[i][t][s] = cplex.getValue(AssetFailed[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, AssetFailed_output,
                         AssetFailed_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                Health_aux[i][t] = cplex.getValue(Health[i][t][picked_scenario]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, InitialHealth_output, Health_aux);

        //atualizar matrizes de quatro dimensoes
        //copiar valores que advem do modelo
        for (a = 0; a < numAction; a++) {
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++) {
                    for (s = 0; s < numScenarios; s++)MaintenanceAction_aux[a][i][t][s] = cplex.getValue(MaintenanceAction[a][i][t][s]);
                }
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, numAction,
                         MaintenanceAction_output, MaintenanceAction_aux, true);
    }

    //#####################//
    //#####Print results   //
    //#####################//

    print_model_results_budget_planning(model_name, instances, results_string, solution_string, results_summed_string,
                                        Time_Limit, Gap, memory, threads,
                                        objective_function, CVaR_output, VaR_output, ExpectedValue_output, TotalTime,
                                        GAP, ScenarioProbability_output, DefinedBudget_output, AllowedDeviation_output,
                                        TotalExpectedCosts_output, UnderBudget_output, OverBudget_output,
                                        PreventiveMaintenanceCosts_output, CorrectiveMaintenanceCosts_output,
                                        ExpectedOverBudget_output, ExpectedUnderBudget_output, ReplaceAsset_output,
                                        MaintenanceAction_output, AssetFailed_output, InitialHealth_output, alpha, beta,
                                        numAsset, numSubHorizons * subLengthTW, numAction, numScenarios, subLengthTW,
                                        numSubHorizons);

    //################################################//
    //#####Evaluate solution on the master problem   //
    //##############################################//

    //CVAR_budget_planning model version
    int numscenarios_master=50;

    if(numscenarios_master>numScenarios){
        IloEnv new_env;
        char *new_results_string = new char[200];
        char *new_solution_string = new char[200];
        char *new_results_summed_string = new char[200];
        char *new_path = new char[200];

        const char *new_filename = build_instance_path(8,new_path,numscenarios_master,numAsset,numSubHorizons * subLengthTW,instances);

        //esta linha de codigo permite fazer append aos resultados obtidos
        build_results_path(new_results_string,numAsset,numSubHorizons * subLengthTW,3);

        //terminal print
        cout << "\nFile Path\n" << new_filename << endl;

        //esta linha de codigo permite fazer append as solucoes obtidas
        build_solution_path(new_solution_string,numAsset,numSubHorizons * subLengthTW,numscenarios_master,instances,3);
        cout << "PATH \t" << new_solution_string << endl;

        //esta linha de codigo permite fazer append aos resultados resumidos obtidas
        sprintf(new_results_summed_string,"resultados/todos_resumidos/N%dTW%d/%s",numAsset,numSubHorizons * subLengthTW,"results_summed.csv");

        solve_selected_model_with_solution(2, new_env, new_filename, instances, new_results_string,
                                           new_solution_string, new_results_summed_string, Time_Limit, Gap, memory,
                                           threads, numSubHorizons * subLengthTW, alpha, beta, DefinedBudget_output);
    }

}

//modelo em que temos como variavel de primeiro estagio a definicao do budget e substituicao de ativos
void solve_CVAR_Replacement_planning(char *model_name, IloEnv env,const char *filename, int instances, char *results_string,
                                     char *solution_string, char *results_summed_string, int Time_Limit, float Gap,
                                     int memory, int threads, int subLengthTW, float alpha, float beta) {


    //verificar se o ficheiro existe da instancia
    ifstream file(filename);
    if (!file) {
        cerr << "No such file: " << filename << endl;
        throw (-1);
    }

    //MODELO CPLEX////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IloNum RiskFreeRatePlus, RiskFreeRateMinus;
    IloInt MaxHealth;
    IloNumArray CostFailure(env), CostReplacingAsset(env), InitialHealth(env), InterestMultiplier(
            env), ScenarioProbability(env);
    IloNumArray2 CostAction(env), BenefitAction(env);
    IloNumArray3 AssetDegradation(env);

    file >> RiskFreeRateMinus >> RiskFreeRatePlus >> MaxHealth >> CostFailure >> CostReplacingAsset
         >> CostAction
         >> BenefitAction >> InitialHealth >> InterestMultiplier >> AssetDegradation >> ScenarioProbability;


    int numAsset = CostFailure.getSize();
    int numLengthTW = InterestMultiplier.getSize();
    int numAction = CostAction[0].getSize();
    int numScenarios = ScenarioProbability.getSize();
    int picked_scenario = 0;

    int i, t, a, s, l;
    int numSubHorizons = 1;

    if (numLengthTW != subLengthTW) {
        //calcular numero de subproblemas
        numLengthTW = subLengthTW;
        numSubHorizons = InterestMultiplier.getSize() / subLengthTW;
        //atualizar model name
        strcat(model_name, "_Rolling_Horizon");

        // atualizar calculo dos juros a pagar com base no comprimento escolhido e taxa de juro
        int tempo;
        for (int k = 1; k <= numSubHorizons; k++) {
            tempo = 0;
            if (subLengthTW > 1) {
                for (int i = (k - 1) * subLengthTW; i < k * subLengthTW - 1; i++) {
                    tempo++;
                    InterestMultiplier[i] = (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) * RiskFreeRatePlus /
                                             (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) - 1));
                }
                //atualizar o ultimo valor do calculo dos juros
                InterestMultiplier[k * subLengthTW - 1] = InterestMultiplier[k * subLengthTW - 2];
            } else {
                InterestMultiplier[k * subLengthTW - 1] = 1;
            }

        }
    }

    //Declarar variaveis output resultado
    double objective_function = 0;
    double TotalTime = 0;
    double GAP = 0;
    double PreventiveMaintenanceCosts_output = 0;
    double CorrectiveMaintenanceCosts_output = 0;
    double ExpectedOverBudget_output = 0;
    double ExpectedUnderBudget_output = 0;
    double CVaR_output = 0;
    double VaR_output = 0;
    double ExpectedValue_output = 0;

    //First dimension
    vector<double> TotalExpectedCosts_output(numScenarios, 0);
    //First dimension
    vector<double> DefinedBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> AllowedDeviation_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> ScenarioProbability_output(numScenarios, 0);

    //copiar valores que advem do modelo
    for (s = 0; s < numScenarios; s++)ScenarioProbability_output[s] = ScenarioProbability[s];

    //First dimension
    vector<vector<double>> UnderBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        UnderBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_output(InterestMultiplier.getSize());
    //Second dimension
    for (int t = 0; t < InterestMultiplier.getSize(); t++) {
        OverBudget_output[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> InitialHealth_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        InitialHealth_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<int>> ReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_output[i].resize(InterestMultiplier.getSize(), 0);
    }

    //First dimension
    vector<vector<vector<int>>> AntecipateReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AntecipateReplaceAsset_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)
            AntecipateReplaceAsset_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_output[i].resize(InterestMultiplier.getSize());
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < InterestMultiplier.getSize(); t++)AssetFailed_output[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_output(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_output[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_output[a][i].resize(InterestMultiplier.getSize());
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < InterestMultiplier.getSize(); t++)
                MaintenanceAction_output[a][i][t].resize(numScenarios, 0);
        }
    }

    //criar variaveis auxiliares
    vector<double> DefinedBudget_aux(subLengthTW, 0);
    vector<double> AllowedDeviation_aux(subLengthTW, 0);
    vector<double> TotalExpectedCosts_aux(numScenarios, 0);

    //First dimension
    vector<vector<double>> UnderBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        UnderBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> OverBudget_aux(subLengthTW);
    //Second dimension
    for (int t = 0; t < subLengthTW; t++) {
        OverBudget_aux[t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<int>> ReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AntecipateReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AntecipateReplaceAsset_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AntecipateReplaceAsset_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<vector<int>>> AssetFailed_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_aux[i].resize(subLengthTW);
    }
    //Third dimension
    for (int i = 0; i < numAsset; i++) {
        for (int t = 0; t < subLengthTW; t++)AssetFailed_aux[i][t].resize(numScenarios, 0);
    }

    //First dimension
    vector<vector<double>> Health_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        Health_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<vector<int>>>> MaintenanceAction_aux(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_aux[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_aux[a][i].resize(subLengthTW);
    }
    //Fourth dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++) {
            for (int t = 0; t < subLengthTW; t++)MaintenanceAction_aux[a][i][t].resize(numScenarios, 0);
        }
    }

    //#####################//
    //#####Model definition//
    //#####################//
    for (int k = 1; k <= numSubHorizons; k++) {
        IloModel mod(env);

        //DECISION VARIABLES
        IloNumVarArray DefinedBudget(env, numLengthTW, 0.0, IloInfinity,
                                     ILOFLOAT); //orcamento planeado pelo agente de decisao que nao depende dos cenarios
        IloNumVarArray AllowedDeviation(env, numLengthTW, 0.0, IloInfinity,
                                        ILOFLOAT); //desvio orçamental que é permitido em média para cada instante t

        IloNumVarArray2 ReplaceAsset(env, numAsset);
        for (i = 0; i < numAsset; ++i)
            ReplaceAsset[i] = IloNumVarArray(env, numLengthTW, 0.0, 1.0,
                                             ILOBOOL); //plano de substituicao nao dependente dos cenarios

        IloNumVarArray4 MaintenanceAction(env, numAction);
        for (a = 0; a < numAction; ++a) {
            MaintenanceAction[a] = IloNumVarArray3(env, numAsset);
            for (i = 0; i < numAsset; ++i) {
                MaintenanceAction[a][i] = IloNumVarArray2(env, numLengthTW);
                for (t = 0; t < numLengthTW; t++)
                    MaintenanceAction[a][i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                                ILOBOOL); //plano de manutencao preventiva depende dos cenarios
            }
        }


        IloNumVarArray3 AntecipateReplaceAsset(env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            AntecipateReplaceAsset[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                AntecipateReplaceAsset[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                              ILOBOOL); //Plano de emergencia para substituir ativos que avariam antes do planeado. Depende do cenario
            }
        }

        //AUXILIARY VARIABLES

        //calculos auxiliares
        IloNumVarArray TotalExpectedCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //calcula o custo esperado

        IloNumVarArray2 EffectiveBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            EffectiveBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                ILOFLOAT); //calcula os gastos efetivos em mautencao

        IloNumVarArray2 UnderBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            UnderBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                            ILOFLOAT); //calcula os gastos em mautencao que nao excederam o budget planeado

        IloNumVarArray2 OverBudget(env, numLengthTW);
        for (t = 0; t < numLengthTW; t++)
            OverBudget[t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                           ILOFLOAT); //calcula os gastos em mautencao que excederam o budget planeado

        IloNumVarArray3 Health(env, numAsset), HealthFailure(env, numAsset), HealthReset(env, numAsset), AssetFailed(
                env, numAsset);
        for (i = 0; i < numAsset; ++i) {
            Health[i] = IloNumVarArray2(env, numLengthTW);
            HealthFailure[i] = IloNumVarArray2(env, numLengthTW);
            HealthReset[i] = IloNumVarArray2(env, numLengthTW);
            AssetFailed[i] = IloNumVarArray2(env, numLengthTW);
            for (t = 0; t < numLengthTW; ++t) {
                Health[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                              ILOFLOAT); //atualiza a saude do equipamento
                HealthFailure[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //ativa a falha
                HealthReset[i][t] = IloNumVarArray(env, numScenarios, 0.0, IloInfinity,
                                                   ILOFLOAT); //ativa o reset da condição
                AssetFailed[i][t] = IloNumVarArray(env, numScenarios, 0.0, 1.0,
                                                   ILOBOOL); // variavel utilzada para quando o equipamento falha
            }
        }

        IloNumVar CorrectiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das corretivas expectável
        IloNumVar PreventiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT); //custo das preventivas expectável
        IloNumVar ExpectedOverBudget(env, 0.0, IloInfinity, ILOFLOAT); //custo expectavel de over budget
        IloNumVar ExpectedUnderBudget(env, 0.0, IloInfinity, ILOFLOAT);  //custo expectavel de under budget

        //variaveis auxiliares do modelo CVaR
        IloNumVar VaR(env, 0.0, IloInfinity, ILOFLOAT); //Value at Risk
        IloNumVar CVaR(env, 0.0, IloInfinity, ILOFLOAT); //Conditional Value at Risk
        IloNumVarArray ConditionalValue(env, numScenarios, 0.0, IloInfinity,
                                        ILOFLOAT); //toma valores diferentes de zero quando acima do VaR
        IloNumVar ExpectedValue(env, 0.0, IloInfinity, ILOFLOAT); //valor esperado da função objetivo
        IloNumVarArray ScenarioCost(env, numScenarios, 0.0, IloInfinity, ILOFLOAT); //custo de um dado cenário

        // OBJECTIVE FUNCTION

        //custo do overbudget por scenario
        for (s = 0; s < numScenarios; s++) {
            IloExpr OverBudgetCost(env);
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    OverBudgetCost += InterestMultiplier[l] * OverBudget[t][s] / pow(1 + RiskFreeRateMinus, l + 1);
            }
            mod.add(TotalExpectedCost[s] == OverBudgetCost);
            OverBudgetCost.end();
        }

        IloExpr TotalBudget(env);
        //valor esperado dos custos
        for (t = 0; t < numLengthTW; t++)TotalBudget += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
        for (s = 0; s < numScenarios; s++)TotalBudget += ScenarioProbability[s] * TotalExpectedCost[s];
        mod.add(ExpectedValue == TotalBudget);

        //cálculo do custo de cada cenário
        for (s = 0; s < numScenarios; s++) {
            IloExpr ScenarioCost_aux(env);
            for (t = 0; t < numLengthTW; t++)
                ScenarioCost_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1) + TotalExpectedCost[s];
            mod.add(ScenarioCost[s] == ScenarioCost_aux);
            ScenarioCost_aux.end();
        }

        //Calculo CVaR
        IloExpr CVaR_tail_cost(env);
        for (s = 0; s < numScenarios; s++)CVaR_tail_cost += ScenarioProbability[s] * ConditionalValue[s];
        mod.add(CVaR == VaR + 1 / (1 - alpha) * CVaR_tail_cost);
        CVaR_tail_cost.end();

        //adicionar a funcao objetivo
        mod.add(IloMinimize(env, (1 - beta) * TotalBudget + beta * CVaR));
        TotalBudget.end();

        //Calculos auxiliares

        //calculo dos custos corretiva
        IloExpr CorrectiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++)
                    CorrectiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostFailure[i] * AssetFailed[i][t][s]) /
                                                      pow(1 + RiskFreeRateMinus, t + 1);
            }
        }
        mod.add(CorrectiveMaintenanceCosts == CorrectiveMaintenanceCosts_aux);
        CorrectiveMaintenanceCosts_aux.end();

        //calculo dos custos preventiva
        IloExpr PreventiveMaintenanceCosts_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (i = 0; i < numAsset; i++) {
                    PreventiveMaintenanceCosts_aux +=
                            ScenarioProbability[s] * (CostReplacingAsset[i] * AntecipateReplaceAsset[i][t][s]) /
                            pow(1 + RiskFreeRateMinus, t + 1);
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += ScenarioProbability[s] * (CostAction[i][a] *
                                                                                    MaintenanceAction[a][i][t][s]) /
                                                          pow(1 + RiskFreeRateMinus, t + 1);
                }
            }
        }
        mod.add(PreventiveMaintenanceCosts == PreventiveMaintenanceCosts_aux);
        PreventiveMaintenanceCosts_aux.end();

        //calculo do custo expectavel de underbudget
        IloExpr ExpectedUnderBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++)
                ExpectedUnderBudget_aux += ScenarioProbability[s] * UnderBudget[t][s] /
                                           pow(1 + RiskFreeRateMinus, t + 1);
        }
        mod.add(ExpectedUnderBudget == ExpectedUnderBudget_aux);
        ExpectedUnderBudget_aux.end();

        //calculo do custo expectavel de overbudget
        IloExpr ExpectedOverBudget_aux(env);
        for (s = 0; s < numScenarios; s++) {
            for (t = 0; t < numLengthTW; t++) {
                for (l = t; l < numLengthTW; l++)
                    ExpectedOverBudget_aux += ScenarioProbability[s] * OverBudget[t][s] /
                                              pow(1 + RiskFreeRateMinus, l + 1);
            }
        }
        mod.add(ExpectedOverBudget == ExpectedOverBudget_aux);
        ExpectedOverBudget_aux.end();

        //PROBLEM CONSTRAINTS

        //verificar se está acima do VaR
        for (s = 0; s < numScenarios; s++) {
            IloExpr DefinedBudgetUpdated_aux(env);
            for (t = 0; t < numLengthTW; t++)
                DefinedBudgetUpdated_aux += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
            mod.add(DefinedBudgetUpdated_aux + TotalExpectedCost[s] - VaR - ConditionalValue[s] <= 0);
            DefinedBudgetUpdated_aux.end();
        }


        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance(env);
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        for (a = 0; a < numAction; a++)
                            Maintenance += BenefitAction[i][a] * MaintenanceAction[a][i][t][s];
                        mod.add(Health[i][t][s] - HealthFailure[i][t][s] ==
                                Maintenance + (Health[i][t - 1][s] - AssetDegradation[s][t][i]) + HealthReset[i][t][s]);
                        Maintenance.end();
                    }
                }
            }
        }


        //faz o reset da condicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(HealthReset[i][t][s] == 0);
                    } else {
                        mod.add(HealthReset[i][t][s] <=
                                5 * MaxHealth * (AssetFailed[i][t - 1][s] + AntecipateReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(Health[i][t][s] == InitialHealth[i]);
                    } else {
                        mod.add(Health[i][t][s] >=
                                MaxHealth * (AssetFailed[i][t - 1][s] + AntecipateReplaceAsset[i][t - 1][s]));
                    }
                }
            }
        }

        //evita o overlap de manutencao com substituicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    IloExpr Maintenance_overlap(env);
                    for (a = 0; a < numAction; a++)Maintenance_overlap += MaintenanceAction[a][i][t][s];
                    mod.add(Maintenance_overlap <= 1 - (AssetFailed[i][t][s]));
                    Maintenance_overlap.end();
                }
            }
        }

        //falha do ativo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(HealthFailure[i][t][s] <= 200 * AssetFailed[i][t][s]);
        }


        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(Health[i][t][s] <= MaxHealth * (1 - AssetFailed[i][t][s]));
        }

        //substituicao do ativo antes do inicialmente planeado
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) {
                    if (t == 0) {
                        mod.add(AntecipateReplaceAsset[i][t][s] >= ReplaceAsset[i][t]);
                    } else {
                        IloExpr FailuresCount(env);
                        for (l = 0; l < t - 1; l++)FailuresCount += AssetFailed[i][l][s];
                        mod.add(AntecipateReplaceAsset[i][t][s] + FailuresCount >= ReplaceAsset[i][t]);
                        FailuresCount.end();
                    }
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr UnplannedFailuresCount(env), PlannedReplacement(env);
                for (t = 0; t < numLengthTW; t++) {
                    UnplannedFailuresCount += AntecipateReplaceAsset[i][t][s];
                    PlannedReplacement += ReplaceAsset[i][t];
                }
                mod.add(UnplannedFailuresCount <= PlannedReplacement);
                UnplannedFailuresCount.end();
                PlannedReplacement.end();
            }
        }

        //o numero de subsituições esta limitada a uma
        for (i = 0; i < numAsset; i++) {
            IloExpr SumReplaceAsset(env);
            for (t = 0; t < numLengthTW; t++)SumReplaceAsset += ReplaceAsset[i][t];
            mod.add(SumReplaceAsset <= 1);
        }

        //sem overlap de substituicao corretiva com preventiva
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)
                for (s = 0; s < numScenarios; s++)
                    mod.add(ReplaceAsset[i][t] + AssetFailed[i][t][s] <= 1);
        }

        //Para evitar que exista a substituição planeada quando temos uma falha
        for (i = 0; i < numAsset; i++) {
            for (s = 0; s < numScenarios; s++){
                IloExpr checkPlannedReplacement(env);
                for (t = 0; t < numLengthTW; t++){
                    checkPlannedReplacement+=ReplaceAsset[i][t]+AntecipateReplaceAsset[i][t][s]+AssetFailed[i][t][s];
                }
                mod.add(checkPlannedReplacement <= 2);
                checkPlannedReplacement.end();
            }
        }

        //calcula os gastos effetivos
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++) {
                IloExpr CorrectiveMaintenanceCosts_aux(env), PreventiveMaintenanceCosts_aux(
                        env), PreventiveReplacementCosts_aux(env);
                for (i = 0; i < numAsset; i++) {
                    CorrectiveMaintenanceCosts_aux += CostFailure[i] * AssetFailed[i][t][s];
                    for (a = 0; a < numAction; a++)
                        PreventiveMaintenanceCosts_aux += CostAction[i][a] * MaintenanceAction[a][i][t][s];
                    PreventiveReplacementCosts_aux += CostReplacingAsset[i] * AntecipateReplaceAsset[i][t][s];
                }
                mod.add(EffectiveBudget[t][s] == CorrectiveMaintenanceCosts_aux + PreventiveMaintenanceCosts_aux +
                                                 PreventiveReplacementCosts_aux);
                CorrectiveMaintenanceCosts_aux.end();
                PreventiveMaintenanceCosts_aux.end();
                PreventiveReplacementCosts_aux.end();
            }
        }

        //calculo do underbudget e overbudget
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)
                mod.add(DefinedBudget[t] - EffectiveBudget[t][s] == UnderBudget[t][s] - OverBudget[t][s]);

        //calculo dos gastos efetivos
        for (t = 0; t < numLengthTW; t++)
            for (s = 0; s < numScenarios; s++)mod.add(EffectiveBudget[t][s] <= DefinedBudget[t] + AllowedDeviation[t]);


        IloCplex cplex(mod);

        //Definir parametros do CPLEX
        cplex.setParam(IloCplex::TiLim, Time_Limit);
        cplex.setParam(IloCplex::EpGap, Gap);
        cplex.setParam(IloCplex::TreLim, memory);
        cplex.setParam(IloCplex::Threads, threads);

        //calcular o tempo de execucao
        time_t startTime = time(NULL);


        //resolver o modelo gerado
        cplex.solve();
        env.out() << "Solution status: " << cplex.getStatus() << endl;
        time_t finishTime = time(NULL);

        //atualizacao da condicao escolhendo um cenario

        random_device rd;     // only used once to initialise (seed) engine
        mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        uniform_int_distribution<int> uni(0, numScenarios - 1); // guaranteed unbiased

        picked_scenario = uni(rng);

        for (i = 0; i < numAsset; i++) {
            InitialHealth[i] = cplex.getValue(Health[i][numLengthTW - 1][picked_scenario]);
        }

        //#####################//
        //#####Update solution //
        //#####################//

        //atualizar valores unicos
        objective_function += cplex.getObjValue() / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        TotalTime += finishTime - startTime;
        GAP += cplex.getMIPRelativeGap();
        CorrectiveMaintenanceCosts_output +=
                cplex.getValue(CorrectiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        PreventiveMaintenanceCosts_output +=
                cplex.getValue(PreventiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedOverBudget_output +=
                cplex.getValue(ExpectedOverBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedUnderBudget_output +=
                cplex.getValue(ExpectedUnderBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedValue_output +=
                cplex.getValue(ExpectedValue) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        CVaR_output += cplex.getValue(CVaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        VaR_output += cplex.getValue(VaR) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);

        //atualizar matrizes de um dimensao
        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)DefinedBudget_aux[t] = cplex.getValue(DefinedBudget[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, DefinedBudget_output, DefinedBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)AllowedDeviation_aux[t] = cplex.getValue(AllowedDeviation[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, AllowedDeviation_output, AllowedDeviation_aux);

        //copiar valores que advem do modelo
        for (s = 0; s < numScenarios; s++)TotalExpectedCosts_aux[s] = cplex.getValue(TotalExpectedCost[s]);

        //update solucao da variavel
        for (s = 0; s < numScenarios; s++)
            TotalExpectedCosts_output[s] += TotalExpectedCosts_aux[s] / pow(1 + RiskFreeRateMinus,
                                                                            subLengthTW * k - subLengthTW);

        //atualizar matrizes de duas dimensoes

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)UnderBudget_aux[t][s] = cplex.getValue(UnderBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, UnderBudget_output,
                         UnderBudget_aux, true);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++) {
            for (s = 0; s < numScenarios; s++)OverBudget_aux[t][s] = cplex.getValue(OverBudget[t][s]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, OverBudget_output,
                         OverBudget_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                ReplaceAsset_aux[i][t] = cplex.getValue(ReplaceAsset[i][t]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, ReplaceAsset_output,
                         ReplaceAsset_aux);

        //atualizar matrizes de tres dimensoes

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++)
                    AntecipateReplaceAsset_aux[i][t][s] = cplex.getValue(AntecipateReplaceAsset[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset,
                         AntecipateReplaceAsset_output, AntecipateReplaceAsset_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                for (s = 0; s < numScenarios; s++) AssetFailed_aux[i][t][s] = cplex.getValue(AssetFailed[i][t][s]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, AssetFailed_output,
                         AssetFailed_aux, true);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                Health_aux[i][t] = cplex.getValue(Health[i][t][picked_scenario]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, InitialHealth_output, Health_aux);

        //atualizar matrizes de quatro dimensoes
        //copiar valores que advem do modelo
        for (a = 0; a < numAction; a++) {
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++) {
                    for (s = 0; s < numScenarios; s++)MaintenanceAction_aux[a][i][t][s] = cplex.getValue(MaintenanceAction[a][i][t][s]);
                }
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numScenarios, numAsset, numAction,
                         MaintenanceAction_output, MaintenanceAction_aux, true);
    }

    //#####################//
    //#####Print results   //
    //#####################//

    print_model_results_replacement_planning(model_name, instances, results_string, solution_string,
                                             results_summed_string, Time_Limit, Gap, memory, threads,
                                             objective_function, CVaR_output, VaR_output, ExpectedValue_output,
                                             TotalTime, GAP, ScenarioProbability_output, DefinedBudget_output,
                                             AllowedDeviation_output,
                                             TotalExpectedCosts_output, UnderBudget_output, OverBudget_output,
                                             PreventiveMaintenanceCosts_output, CorrectiveMaintenanceCosts_output,
                                             ExpectedOverBudget_output, ExpectedUnderBudget_output, ReplaceAsset_output,
                                             AntecipateReplaceAsset_output, MaintenanceAction_output,
                                             AssetFailed_output, InitialHealth_output, alpha, beta, numAsset,
                                             numSubHorizons * subLengthTW, numAction, numScenarios, subLengthTW,
                                             numSubHorizons);

    //################################################//
    //#####Evaluate solution on the master problem   //
    //##############################################//

    //CVAR_replacement_planning model version
    int numscenarios_master=50;

    if(numscenarios_master>numScenarios){
        IloEnv new_env;
        char *new_results_string = new char[200];
        char *new_solution_string = new char[200];
        char *new_results_summed_string = new char[200];
        char *new_path = new char[200];

        const char *new_filename = build_instance_path(8,new_path,numscenarios_master,numAsset,numSubHorizons * subLengthTW,instances);

        //esta linha de codigo permite fazer append aos resultados obtidos
        build_results_path(new_results_string,numAsset,numSubHorizons * subLengthTW,3);

        //terminal print
        cout << "\nFile Path\n" << new_filename << endl;

        //esta linha de codigo permite fazer append as solucoes obtidas
        build_solution_path(new_solution_string,numAsset,numSubHorizons * subLengthTW,numscenarios_master,instances,3);
        cout << "PATH \t" << new_solution_string << endl;

        //esta linha de codigo permite fazer append aos resultados resumidos obtidas
        sprintf(new_results_summed_string,"resultados/todos_resumidos/N%dTW%d/%s",numAsset,numSubHorizons * subLengthTW,"results_summed.csv");

        solve_selected_model_with_solution(5, new_env, new_filename, instances, new_results_string,
                                           new_solution_string, new_results_summed_string, Time_Limit, Gap, memory,
                                           threads, numSubHorizons * subLengthTW, alpha, beta, DefinedBudget_output, ReplaceAsset_output);
    }

}

//modelo em que nao consideramos incerteza
void solve_Deterministic_model(char *model_name, IloEnv env, const char *filename, int instances, char *results_string,
                               char *solution_string, char *results_summed_string, int Time_Limit, float Gap,
                               int memory, int threads, int subLengthTW,float alpha, float beta) {

    //verificar se o ficheiro existe da instancia
    ifstream file(filename);
    if (!file) {
        cerr << "No such file: " << filename << endl;
        throw (-1);
    }

    //MODELO CPLEX////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IloNum RiskFreeRatePlus, RiskFreeRateMinus;
    IloInt MaxHealth;
    IloNumArray CostFailure(env), CostReplacingAsset(env), AssetDegradation(env), InitialHealth(
            env), InterestMultiplier(env);
    IloNumArray2 CostAction(env), BenefitAction(env);
    file >> RiskFreeRateMinus >> RiskFreeRatePlus >> MaxHealth >> CostFailure >> CostReplacingAsset
         >> CostAction
         >> BenefitAction >> InitialHealth >> InterestMultiplier >> AssetDegradation;

    int numAsset = CostFailure.getSize();
    int numLengthTW = InterestMultiplier.getSize();
    int numAction = CostAction[0].getSize();

    int i, t, a, l;

    //testar hipotese rolling horizon
    int numSubHorizons = 1;
    if (numLengthTW != subLengthTW) {
        //calcular numero de subproblemas
        numLengthTW = subLengthTW;
        numSubHorizons = InterestMultiplier.getSize() / subLengthTW;
        //atualizar model name
        strcat(model_name, "_Rolling_Horizon");

        // atualizar calculo dos juros a pagar com base no comprimento escolhido e taxa de juro
        int tempo;
        for (int k = 1; k <= numSubHorizons; k++) {
            tempo = 0;
            if (subLengthTW > 1) {
                for (int i = (k - 1) * subLengthTW; i < k * subLengthTW - 1; i++) {
                    tempo++;
                    InterestMultiplier[i] = (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) * RiskFreeRatePlus /
                                             (pow(1 + RiskFreeRatePlus, subLengthTW - tempo) - 1));
                }
                //atualizar o ultimo valor do calculo dos juros
                InterestMultiplier[k * subLengthTW - 1] = InterestMultiplier[k * subLengthTW - 2];
            } else {
                InterestMultiplier[k * subLengthTW - 1] = 1;
            }

        }
    }

    //Declarar variaveis output resultado
    double objective_function = 0;
    double TotalTime = 0;
    double GAP = 0;
    double PreventiveMaintenanceCosts_output = 0;
    double CorrectiveMaintenanceCosts_output = 0;
    double ExpectedOverBudget_output = 0;
    double ExpectedUnderBudget_output = 0;

    //First dimension
    vector<double> DefinedBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> AllowedDeviation_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> UnderBudget_output(InterestMultiplier.getSize(), 0);
    //First dimension
    vector<double> OverBudget_output(InterestMultiplier.getSize(), 0);

    //First dimension
    vector<vector<int>> ReplaceAsset_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_output[i].resize(numLengthTW * numSubHorizons, 0);
    }

    //First dimension
    vector<vector<int>> AssetFailed_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_output[i].resize(numLengthTW * numSubHorizons, 0);
    }

    //First dimension
    vector<vector<int>> InitialHealth_output(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        InitialHealth_output[i].resize(numLengthTW * numSubHorizons, 0);
    }

    //First dimension
    vector<vector<vector<int>>> MaintenanceAction_output(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_output[i].resize(numAsset);
    }
    //Third dimension
    for (int a = 0; a < numAction; a++) {
        for (int i = 0; i < numAsset; i++)MaintenanceAction_output[a][i].resize(InterestMultiplier.getSize(), 0);
    }

    //criar variaveis auxiliares
    vector<double> DefinedBudget_aux(subLengthTW, 0);
    vector<double> AllowedDeviation_aux(subLengthTW, 0);
    vector<double> UnderBudget_aux(subLengthTW, 0);
    vector<double> OverBudget_aux(subLengthTW, 0);

    //First dimension
    vector<vector<int>> ReplaceAsset_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        ReplaceAsset_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<int>> AssetFailed_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        AssetFailed_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<int>> Health_aux(numAsset);
    //Second dimension
    for (int i = 0; i < numAsset; i++) {
        Health_aux[i].resize(subLengthTW, 0);
    }

    //First dimension
    vector<vector<vector<int>>> MaintenanceAction_aux(numAction);
    //Second dimension
    for (int i = 0; i < numAction; i++) {
        MaintenanceAction_aux[i].resize(numAsset);
    }

    //Third dimension
    for (int i = 0; i < numAction; i++) {
        for (int j = 0; j < numAsset; j++)MaintenanceAction_aux[i][j].resize(subLengthTW, 0);
    }

    //#####################//
    //#####Model definition//
    //#####################//
    for (int k = 1; k <= numSubHorizons; k++) {

        IloModel mod(env);

        //DECISION VARIABLES

        IloNumVarArray DefinedBudget(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);
        IloNumVarArray AllowedDeviation(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);

        IloNumVarArray2 ReplaceAsset(env);
        for (i = 0; i < numAsset; i++) {
            ReplaceAsset.add(IloNumVarArray(env, numLengthTW, 0.0, 1.0, ILOBOOL));
        }

        IloNumVarArray3 MaintenanceAction(env, numAction);
        for (int a = 0; a < numAction; ++a) {
            MaintenanceAction[a] = IloNumVarArray2(env, numAsset);
            for (int i = 0; i < numAsset; ++i)
                MaintenanceAction[a][i] = IloNumVarArray(env, numLengthTW, 0.0, 1.0, ILOBOOL);
        }

        //AUXILIARY VARIABLES
        IloNumVarArray UnderBudget(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);
        IloNumVarArray OverBudget(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);
        IloNumVarArray EffectiveBudget(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT);
        IloNumVarArray2 Health(env), HealthFailure(env), HealthReset(env), AssetFailed(env);
        IloNumVar CorrectiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT);
        IloNumVar PreventiveMaintenanceCosts(env, 0.0, IloInfinity, ILOFLOAT);
        IloNumVar ExpectedOverBudget(env, 0.0, IloInfinity, ILOFLOAT);
        IloNumVar ExpectedUnderBudget(env, 0.0, IloInfinity, ILOFLOAT);

        for (i = 0; i < numAsset; i++) {
            Health.add(IloNumVarArray(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT));
            HealthFailure.add(IloNumVarArray(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT));
            HealthReset.add(IloNumVarArray(env, numLengthTW, 0.0, IloInfinity, ILOFLOAT));
            AssetFailed.add(IloNumVarArray(env, numLengthTW, 0.0, 1.0, ILOBOOL));
        }

        // OBJECTIVE FUNCTION
        IloExpr TotalBudget(env), OverBudgetCost(env);

        for (t = 0; t < numLengthTW; t++) TotalBudget += DefinedBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);

        for (t = 0; t < numLengthTW; t++) {
            for (l = t; l < numLengthTW; l++)
                OverBudgetCost += InterestMultiplier[l] * OverBudget[t] / pow(1 + RiskFreeRateMinus, l + 1);
        }

        mod.add(IloMinimize(env, TotalBudget + OverBudgetCost));
        TotalBudget.end();
        OverBudgetCost.end();

        //calculo dos custos corretiva
        IloExpr CorrectiveMaintenanceCosts_aux(env);
        for (t = 0; t < numLengthTW; t++) {
            for (i = 0; i < numAsset; i++)
                CorrectiveMaintenanceCosts_aux += CostFailure[i] * AssetFailed[i][t] /
                                                  pow(1 + RiskFreeRateMinus, t + 1);
        }
        mod.add(CorrectiveMaintenanceCosts == CorrectiveMaintenanceCosts_aux);
        CorrectiveMaintenanceCosts_aux.end();

        //calculo dos custos preventiva
        IloExpr PreventiveMaintenanceCosts_aux(env);
        for (t = 0; t < numLengthTW; t++) {
            for (i = 0; i < numAsset; i++) {
                PreventiveMaintenanceCosts_aux +=
                        CostReplacingAsset[i] * (ReplaceAsset[i][t]) / pow(1 + RiskFreeRateMinus, t + 1);
                for (a = 0; a < numAction; a++)
                    PreventiveMaintenanceCosts_aux += CostAction[i][a] * MaintenanceAction[a][i][t] /
                                                      pow(1 + RiskFreeRateMinus, t + 1);
            }
        }
        mod.add(PreventiveMaintenanceCosts == PreventiveMaintenanceCosts_aux);
        PreventiveMaintenanceCosts_aux.end();

        //calculo do custo expectavel de underbudget
        IloExpr ExpectedUnderBudget_aux(env);
        for (t = 0; t < numLengthTW; t++)ExpectedUnderBudget_aux += UnderBudget[t] / pow(1 + RiskFreeRateMinus, t + 1);
        mod.add(ExpectedUnderBudget == ExpectedUnderBudget_aux);
        ExpectedUnderBudget_aux.end();

        //calculo do custo expectavel de overbudget
        IloExpr ExpectedOverBudget_aux(env);
        for (t = 0; t < numLengthTW; t++) {
            for (l = t; l < numLengthTW; l++)
                ExpectedOverBudget_aux += InterestMultiplier[l] * OverBudget[t] / pow(1 + RiskFreeRateMinus, l + 1);
        }
        mod.add(ExpectedOverBudget == ExpectedOverBudget_aux);
        ExpectedOverBudget_aux.end();

        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                IloExpr Maintenance(env);
                if (t == 0) {
                    mod.add(Health[i][0] == InitialHealth[i]);
                } else {
                    for (a = 0; a < numAction; a++)Maintenance += BenefitAction[i][a] * MaintenanceAction[a][i][t];
                    mod.add(Health[i][t] - HealthFailure[i][t] ==
                            Maintenance + (Health[i][t - 1] - AssetDegradation[i]) + HealthReset[i][t]);
                    Maintenance.end();
                }
            }
        }


        //faz o reset da condicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                if (t == 0) {
                    mod.add(HealthReset[i][t] == 0);
                } else {
                    mod.add(HealthReset[i][t] <= 5 * MaxHealth * (AssetFailed[i][t - 1] + ReplaceAsset[i][t - 1]));
                }
            }
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                if (t == 0) {
                    mod.add(Health[i][t] == InitialHealth[i]);
                } else {
                    mod.add(Health[i][t] >= MaxHealth * (AssetFailed[i][t - 1] + ReplaceAsset[i][t - 1]));
                }
            }
        }

        //evita o overlap de manutencao com substituicao
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++) {
                IloExpr Maintenance_overlap(env);
                for (a = 0; a < numAction; a++)Maintenance_overlap += MaintenanceAction[a][i][t];
                mod.add(Maintenance_overlap <= 1 - (AssetFailed[i][t]));
                Maintenance_overlap.end();
            }
        }

        //falha do ativo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)mod.add(HealthFailure[i][t] <= 200 * AssetFailed[i][t]);
        }

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)mod.add(Health[i][t] <= MaxHealth * (1 - AssetFailed[i][t]));
        }

        //o numero de subsituições esta limitada a uma
        for (i = 0; i < numAsset; i++)mod.add(IloSum(ReplaceAsset[i]) <= 1);

        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)mod.add(ReplaceAsset[i][t] + AssetFailed[i][t] <= 1);
        }

        //calcula os gastos effetivos
        for (t = 0; t < numLengthTW; t++) {
            IloExpr CorrectiveMaintenanceCosts(env), PreventiveMaintenanceCosts(env), PreventiveReplacementCosts(env);
            for (i = 0; i < numAsset; i++) {
                CorrectiveMaintenanceCosts += CostFailure[i] * AssetFailed[i][t];
                for (a = 0; a < numAction; a++)
                    PreventiveMaintenanceCosts += CostAction[i][a] * MaintenanceAction[a][i][t];
                PreventiveReplacementCosts += CostReplacingAsset[i] * ReplaceAsset[i][t];
            }
            mod.add(EffectiveBudget[t] ==
                    CorrectiveMaintenanceCosts + PreventiveMaintenanceCosts + PreventiveReplacementCosts);
            CorrectiveMaintenanceCosts.end();
            PreventiveMaintenanceCosts.end();
            PreventiveReplacementCosts.end();
        }

        //calculo do underbudget e overbudget
        for (t = 0; t < numLengthTW; t++)
            mod.add(DefinedBudget[t] - EffectiveBudget[t] == UnderBudget[t] - OverBudget[t]);

        //calculo dos gastos efetivos
        for (t = 0; t < numLengthTW; t++)mod.add(EffectiveBudget[t] <= DefinedBudget[t] + AllowedDeviation[t]);

        IloCplex cplex(mod);

        //Definir parametros do CPLEX
        cplex.setParam(IloCplex::TiLim, Time_Limit);
        cplex.setParam(IloCplex::EpGap, Gap);
        cplex.setParam(IloCplex::TreLim, memory);
        cplex.setParam(IloCplex::Threads, threads);

        //calcular o tempo de execucao
        time_t startTime = time(NULL);


        //resolver o modelo gerado
        cplex.solve();
        env.out() << "Solution status: " << cplex.getStatus() << endl;

        //atualizacao da condicao_
        for (i = 0; i < numAsset; i++) {
            InitialHealth[i] = cplex.getValue(Health[i][numLengthTW - 1]);
        }

        time_t finishTime = time(NULL);

        //#####################//
        //#####Update solution //
        //#####################//

        //atualizar valores unicos
        objective_function += cplex.getObjValue() / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        TotalTime += finishTime - startTime;
        GAP += cplex.getMIPRelativeGap();
        CorrectiveMaintenanceCosts_output +=
                cplex.getValue(CorrectiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        PreventiveMaintenanceCosts_output +=
                cplex.getValue(PreventiveMaintenanceCosts) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedOverBudget_output +=
                cplex.getValue(ExpectedOverBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);
        ExpectedUnderBudget_output +=
                cplex.getValue(ExpectedUnderBudget) / pow(1 + RiskFreeRateMinus, subLengthTW * k - subLengthTW);

        //atualizar matrizes de um dimensao
        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)DefinedBudget_aux[t] = cplex.getValue(DefinedBudget[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, DefinedBudget_output, DefinedBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)AllowedDeviation_aux[t] = cplex.getValue(AllowedDeviation[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, AllowedDeviation_output, AllowedDeviation_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)UnderBudget_aux[t] = cplex.getValue(UnderBudget[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, UnderBudget_output, UnderBudget_aux);

        //copiar valores que advem do modelo
        for (t = 0; t < numLengthTW; t++)OverBudget_aux[t] = cplex.getValue(OverBudget[t]);

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, OverBudget_output, OverBudget_aux);

        //atualizar matrizes de duas dimensoes

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)ReplaceAsset_aux[i][t] = cplex.getValue(ReplaceAsset[i][t]);
        }


        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, ReplaceAsset_output,
                         ReplaceAsset_aux);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)AssetFailed_aux[i][t] = cplex.getValue(AssetFailed[i][t]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, AssetFailed_output, AssetFailed_aux);

        //copiar valores que advem do modelo
        for (i = 0; i < numAsset; i++) {
            for (t = 0; t < numLengthTW; t++)Health_aux[i][t] = cplex.getValue(Health[i][t]);
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, InitialHealth_output, Health_aux);

        //atualizar matrizes de tres dimensoes
        //copiar valores que advem do modelo
        for (a = 0; a < numAction; a++) {
            for (i = 0; i < numAsset; i++) {
                for (t = 0; t < numLengthTW; t++)MaintenanceAction_aux[a][i][t] = cplex.getValue(MaintenanceAction[a][i][t]);
            }
        }

        //update solucao da variavel
        process_solution(subLengthTW * k - subLengthTW, subLengthTW * k, numAsset, numAction, MaintenanceAction_output,
                         MaintenanceAction_aux);
    }

    //#####################//
    //#####Print results   //
    //#####################//
    print_model_results_deterministic(model_name, instances, results_string, solution_string, results_summed_string,
                                      Time_Limit, Gap, memory, threads,
                                      objective_function, TotalTime, GAP, DefinedBudget_output, AllowedDeviation_output,
                                      UnderBudget_output, OverBudget_output,
                                      PreventiveMaintenanceCosts_output, CorrectiveMaintenanceCosts_output,
                                      ExpectedOverBudget_output, ExpectedUnderBudget_output, ReplaceAsset_output,
                                      MaintenanceAction_output,
                                      AssetFailed_output, InitialHealth_output, numAsset, numSubHorizons * subLengthTW,
                                      numAction, subLengthTW, numSubHorizons);

    //################################################//
    //#####Evaluate solution on the master problem   //
    //##############################################//

    //CVAR_budget_planning model version
    int numscenarios_master=50;
    IloEnv new_env;
    char *new_results_string = new char[200];
    char *new_solution_string = new char[200];
    char *new_results_summed_string = new char[200];
    char *new_path = new char[200];

    const char *new_filename = build_instance_path(8,new_path,numscenarios_master,numAsset,numSubHorizons * subLengthTW,instances);

    //esta linha de codigo permite fazer append aos resultados obtidos
    build_results_path(new_results_string,numAsset,numSubHorizons * subLengthTW,3);

    //terminal print
    cout << "\nFile Path\n" << new_filename << endl;

    //esta linha de codigo permite fazer append as solucoes obtidas
    build_solution_path(new_solution_string,numAsset,numSubHorizons * subLengthTW,numscenarios_master,instances,3);
    cout << "PATH \t" << new_solution_string << endl;

    //esta linha de codigo permite fazer append aos resultados resumidos obtidas
    sprintf(new_results_summed_string,"resultados/todos_resumidos/N%dTW%d/%s",numAsset,numSubHorizons * subLengthTW,"results_summed.csv");

    solve_selected_model_with_solution(3, new_env, new_filename, instances, new_results_string,
                                       new_solution_string, new_results_summed_string, Time_Limit, Gap, memory,
                                       threads, numSubHorizons * subLengthTW, alpha, beta, DefinedBudget_output, ReplaceAsset_output);

    //CVAR_replacement_planning model version
    IloEnv new_env_2;
    char *new_results_string_2 = new char[200];
    char *new_solution_string_2 = new char[200];
    char *new_results_summed_string_2 = new char[200];

    //esta linha de codigo permite fazer append aos resultados obtidos
    build_results_path(new_results_string_2,numAsset,numSubHorizons * subLengthTW,3);

    //terminal print
    cout << "\nFile Path\n" << new_filename << endl;

    //esta linha de codigo permite fazer append as solucoes obtidas
    build_solution_path(new_solution_string_2,numAsset,numSubHorizons * subLengthTW,numscenarios_master,instances,3);
    cout << "PATH \t" << new_solution_string_2 << endl;

    //esta linha de codigo permite fazer append aos resultados resumidos obtidas
    sprintf(new_results_summed_string_2,"resultados/todos_resumidos/N%dTW%d/%s",numAsset,numSubHorizons * subLengthTW,"results_summed.csv");

    solve_selected_model_with_solution(6, new_env_2, new_filename, instances, new_results_string_2,
                                       new_solution_string_2, new_results_summed_string_2, Time_Limit, Gap, memory,
                                       threads, numSubHorizons * subLengthTW, alpha, beta, DefinedBudget_output, ReplaceAsset_output);

}


//a funcao e utiliza para selecionar o modelo escolhido por defeito para gerar solucao de raiz
void solve_selected_model(int model_version, IloEnv env, const char *filename, int instances, char *results_string,
                          char *solution_string, char *results_summed_string, int Time_Limit, float Gap, int memory,
                          int threads, int subLengthTW, float alpha, float beta) {

    char *model_name = new char[100];

    switch (model_version) {
        case 1: //"CVAR_budget_planning"
            strcpy(model_name, "CVAR_budget_planning");
            solve_CVAR_Budget_planning(model_name, env,filename, instances, results_string, solution_string,
                                       results_summed_string, Time_Limit, Gap, memory, threads, subLengthTW, alpha,
                                       beta);
            break;
        case 4: //"CVAR_replacement_planning"
            strcpy(model_name, "CVAR_replacement_planning");
            solve_CVAR_Replacement_planning(model_name, env,filename, instances, results_string, solution_string,
                                            results_summed_string, Time_Limit, Gap, memory, threads, subLengthTW, alpha,
                                            beta);
            break;
        case 7: //"Deterministic_model"
            strcpy(model_name, "Deterministic_model");
            solve_Deterministic_model(model_name, env,filename, instances, results_string, solution_string,
                                      results_summed_string, Time_Limit, Gap, memory, threads, subLengthTW,alpha,
                                      beta);
            break;
        default: //optional
            cout << "Failed to find a valid model";
            exit(EXIT_FAILURE);
    }
}


#endif