/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef OPAG_H
#define OPAG_H

#include <iostream>
#include <sstream>

#include "types.h"
#include "debug.h"
#include "linear_solver/linear_solver.h"
#include "rpag_functions.h"

using namespace operations_research;
using namespace std;

typedef map<int_pair_t, int_set_t> int_pair_int_set_map_t;
typedef vector<int_pair_int_set_map_t> int_pair_int_set_map_vec_t;

typedef map<int_t,int_pair_set_t> int_int_pair_set_map_t;          //maps w to set of pairs (u,v) such that w=A(u,v)
typedef vector<int_int_pair_set_map_t> int_int_pair_set_map_vec_t; //stores the maps for each stage

typedef vector<map<int_t,MPVariable*> > int_variable_map_vec_t;
typedef vector<map<int_pair_t ,MPVariable*> > int_pair_variable_map_vec_t;
typedef vector<map<int_triplet_t ,MPVariable*> > int_triplet_variable_map_vec_t;

class opag
{
public:
    opag();
    ~opag();

    enum SolverType {
        SCIP,
        CPLEX,
        GUROBI,
        GLPK,
        CBC
    };

    enum ILPFormulation {
        ILP1,   //PMCM ILP formulation 1
        ILP2,   //PMCM ILP formulation 2
        ILP3,   //PMCM ILP formulation 3
        ILP1BIN,//PMCM ILP formulation 1 with only binary variables
        ILP2BIN //PMCM ILP formulation 2 with only binary variables
    };

    enum CostModel {
        HIGH_LEVEL,
        LOW_LEVEL
    };

    enum ProblemType {
        PMCM,  //Solve the Pipelined MCM (PMCM) problem
        MCM_MAD, //Solve the MCM with minimum adder depth (MCM_MAD) problem
        MCM_RGPC, //Solve the MCM with reduced GPC problem
        MCM_MGPC //Solve the MCM with minimum GPC problem
    };


    void prepare_problem(int_set_t *target_set);
    void print_problem_info();
    void optimize();

    int no_of_extra_stages;
    int input_wordsize;

    CostModel costModel;
    ProblemType problemType;
    SolverType solverType;
    ILPFormulation ilp_formulation;

    double cost_add; //cost of an adder in HIGH_LEVEL cost model
    double cost_reg; //cost of a register in HIGH_LEVEL cost model
    double cost_fa;  //cost of a full adder in LOW_LEVEL cost model
    double cost_ff;  //cost of a flip flop in LOW_LEVEL cost model

    int64 timeout;

    bool lp_relaxation; //if true, all integer/binary variables are relaxated to real variables
    bool matlab_output;
    std::string matlab_out_adress_string;

    bool optimize_single_stage; //if true, only a single stage is optimized (a minimal predecessor set is found)
private:
    MPSolver::OptimizationProblemType optimization_problem_type;

    int no_of_pipeline_stages; //max. pipeline depth
    int c_max; //max. fundamental value

    int_set_t *target_set;

    pipeline_set_t single_set_vec;        //vector(0...s_max) of sets of single elements,
    int_int_pair_set_map_vec_t pair_map;  //vector(0...s_max) of maps that relate each element w to set of pairs (u,v) such that w=A(u,v)
    map<int_t, int> gpc_min_map; //map that stores the minimal GPC for each possible element w

    MPSolver *solver;

    int_variable_map_vec_t adder_variable_map_vec;
    int_variable_map_vec_t register_variable_map_vec; //only used in PMCM formulation 1
    int_pair_variable_map_vec_t uv_pair_variable_map_vec; //only used in PMCM formulation 1
    int_triplet_variable_map_vec_t uvw_triplet_variable_map_vec; //only used in PMCM formulation 2 & 3
    int_variable_map_vec_t gpc_variable_map_vec;

    void compute_complete_pipeline_set(int_set_t &target_set);

    void compute_pair_map(bool filter_pipeline_set=true);
    void compute_min_gpc_from_pair_map();
    void filter_pair_map_min_gpc(); //remove all pairs that would not lead to a minimal GPC
    void print_pair_map(); //print pair map to stdout
    int gpc_fom_pipeline_set(vector<set<int_t> > &pipeline_set);

    void insert_pmcm_ilp_formulation1(bool use_simplified_model=true);
    void insert_pmcm_ilp_formulation2(bool use_simplified_model=true);
    void insert_pmcm_ilp_formulation3();

    void create_pipeline_set_from_solution(vector< set<int_t> > *pipeline_set);
    void create_pipelined_adder_graph_from_solution(list<realization_row<int_t> > *pipelined_adder_graph);
};

#endif // OPAG_H
