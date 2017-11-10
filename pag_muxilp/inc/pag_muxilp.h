#pragma once

#include "adder_graph.h"
#include "helper.h"

#include <string>
#include <iostream>
#include <list>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <set>

#include <ILP/Solver.h>
#include <ILP/Exception.h>    // ILP::Exception
#include <ILP/SolverGurobi.h> // ILP::newSolverGurobi

// Values taken from Original DAG Fusion Algo
#define ADDSIZE    67.5625
#define SUBSIZE    75.25
#define ADDSUBSIZE 97.5
#define MUXSIZE    28.65625
#define MUXPLUS    14.34375
#define INPUTBITWITH 16
#define COST_MODEL 2

using namespace std;

/*class topo_output_node_t : public node_t
{
public:
    ~topo_output_node_t(){}
    std::vector<int64_t> output_shifts;
    std::vector<int64_t> input_shifts;
    node_t* original_node;
};*/

class pag_muxilp
{
private:
    string instance;
    unsigned int no_conf;
    set<string> variables;
    ILP::VariableMap var;
    set<string> mux_binaries;
    ILP::VariableMap mux_bin;
    set<string> shift_binaries;
    ILP::VariableMap shift_bin;
    string constra;
    ILP::Result res;
    ILP::Solver s = ILP::Solver(ILP::newSolverGurobi());
    set<ILP::Constraint> con;
    adder_graph_base_node_t* configuration_output;
    map<adder_graph_base_node_t*,set<adder_graph_base_node_t*>> successors_map;

public:
    adder_graph_t* source_graph;
    pag_muxilp(string complete_graph, string instance_name, bool quiet);
    ~pag_muxilp();
    void generate_input_file(string input_string, string instance_name);
    int getMuxIn();
    void generateOptimalMUXDistribution();
    void travelPath(int config, adder_graph_base_node_t* pred, string temp_string, int path_value, ILP::Term path_term);
    void setMUXPath(int config, adder_graph_base_node_t* pred);
    int getCost(int const_model);
    void fill_successors_map();
    void addNonMux();
    int updateNodeValues(adder_graph_base_node_t *node, int config);
    void updateStagesASAP(adder_graph_base_node_t *node, int stage);
    void updateStagesALAP(adder_graph_base_node_t *node);
    void removeNonMux();
    void applySolution();
};


