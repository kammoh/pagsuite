/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include <iostream>
#include <cstring>
#include <strstream>
#include "linear_solver/linear_solver.h"
#include "opag.h"
#include "adder_depth.h"
#include "fundamental.h"

using namespace std;
using namespace operations_research;

void print_short_help()
{
    cout << "usage: opag [OPTIONS] coefficients" << endl;
    cout << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "General Options:" << endl;
    cout << "Option                                         Meaning" << endl;
    cout << "--help                                         Prints this help" << endl;
    cout << "--verbose=0...8                                Verbosity level (0: no information, 8: all information during optimization), default:1" << endl;
    cout << "--input_wordsize=[int]                         Word size of the input of the MCM block (for low level cost model, see below)" << endl;
    cout << "--cost_model=[cost model]                      Selects one of the following cost models (default: high level):" << endl;
    cout << "                                               high level: high level cost model, the cost of adders (cost_add) and registers (cost_reg) are counted" << endl;
    cout << "                                               low level: low level cost model, the cost of full adders (cost_fa) and flip flops (cost_ff) are counted" << endl;
    cout << "--cost_add=[int]                               Cost of a single adder, used in high level cost model (default:1)" << endl;
    cout << "--cost_reg=[int]                               Cost of a single register, used in high level cost model (default:1)" << endl;
    cout << "--cost_fa=[int]                                Cost of a single full adder, used in low level cost model (default:1)" << endl;
    cout << "--cost_ff=[int]                                Cost of a single flip flop, used in low level cost model (default:1)" << endl;
    cout << endl;
    cout << "--problem=[string]                             Sets the problem to one of the following (default: pmcm):" << endl;
    cout << "                                               pmcm: pipelined MCM problem" << endl;
    cout << "                                               mcmmad: MCM problem with minimal AD" << endl;
    cout << "                                               mcmrgpc: MCM problem with minimal GPC but reduced search space (only nodes with minimal GPC are considered, fast and good quality!)" << endl;
    cout << "                                               mcmmgpc: MCM problem with minimal GPC (very slow!)" << endl;
    cout << "--ilp_solver=[string]                          Sets the ILP solver to one of the following (default: scip):" << endl;
    cout << "                                               scip: the SCIP solver" << endl;
    cout << "                                               cplex: the CPLEX solver (available only if compiled with USE_CPLEX)" << endl;
    cout << "                                               gurobi: the Gurobi solver (available only if compiled with USE_GUROBI)" << endl;
    cout << "--ilp_formulation=[string]                     Selects the ILP formulation to one of the following (default: ilp3):" << endl;
    cout << "                                               ilp1: ILP formulation 1" << endl;
    cout << "                                               ilp1bin: ILP formulation 1 with only binary variables" << endl;
    cout << "                                               ilp2: ILP formulation 2" << endl;
    cout << "                                               ilp2bin: ILP formulation 2 with only binary variables" << endl;
    cout << "                                               ilp3: ILP formulation 3" << endl;
    cout << "                                               A detailed description of the variables can be found in:" << endl;
    cout << "                                               \"Martin Kumm, Multiple Constant Multiplication Optimizations for Field Programmable Gate Arrays, PhD thesis University of Kassel, 2015\"" << endl;
    cout << "--no_of_extra_stages=[int]                     Per default, the pipeline depth is chosen to the minimum possible, more pipeline stages can be selected by choosing no_of_extra_stages>0 (default:0)" << endl;
    cout << "--timeout=[int]                                An optimization timeout in seconds, after the timout the best solution found so far (if any) is returned (default:-1, no timeout)" << endl;
    cout << "--lp_relaxation                                If provided, ALL variables are treated as continuous to get an upper bound of the cost" << endl;
    cout << "--optimize_single_stage                        If provided, only the preceding stage of the target set is optimized (for PMCM problem, only supported in ILP formulation 3)" << endl;
    cout << endl;

}

int main(int argc, char *argv[])
{
    global_verbose = 0;

    int_set_t target_set;

    opag *opagp = new opag();

    if(argc == 1)
    {
        print_short_help();
        exit(0);
    }
    for(int i = 1; i < argc; i++)
    {
      if((argv[i][0] != '-') || (argv[i][1] != '-'))
      {
        int_t f = fundamental(atoi(argv[i]));
        if(f !=0) target_set.insert(f);
      }
      else
      {
        if(strstr(argv[i], "--verbose="))
        {
          global_verbose = atol(argv[i]+10);
        }
        else if(strstr(argv[i], "--input_wordsize="))
        {
          opagp->input_wordsize = atoi(argv[i]+17);
        }
        else if(strstr(argv[i], "--cost_model="))
        {
            if(!strcmp(argv[i]+13,"high_level"))
            {
                opagp->costModel = opag::HIGH_LEVEL;
            }
            else if(!strcmp(argv[i]+13,"low_level"))
            {
                opagp->costModel = opag::LOW_LEVEL;
            }
            else
            {
                cerr << "Error: cost_model " << argv[i]+13 << " unknown" << endl;
                exit(-1);
            }
        }
        else if(strstr(argv[i], "--problem="))
        {
            if(!strcmp(argv[i]+10,"pmcm"))
            {
                opagp->problemType = opag::PMCM;
            }
            else if(!strcmp(argv[i]+10,"mcmmad"))
            {
                opagp->problemType = opag::MCM_MAD;
            }
            else if(!strcmp(argv[i]+10,"mcmrgpc"))
            {
                opagp->problemType = opag::MCM_RGPC;
            }
            else if(!strcmp(argv[i]+10,"mcmmgpc"))
            {
                opagp->problemType = opag::MCM_MGPC;
            }
            else
            {
                cerr << "Error: cost_model " << argv[i]+13 << " unknown" << endl;
                exit(-1);
            }
        }
        else if(strstr(argv[i], "--ilp_solver="))
        {
#ifdef USE_SCIP
            if(!strcmp(argv[i]+13,"scip"))
            {
                opagp->solverType = opag::SCIP;
            }
#endif
#ifdef USE_CPLEX
            else if(!strcmp(argv[i]+13,"cplex"))
            {
                opagp->solverType = opag::CPLEX;
            }
#endif
#ifdef USE_GUROBI
            else if(!strcmp(argv[i]+13,"gurobi"))
            {
                opagp->solverType = opag::GUROBI;
            }
#endif
#ifdef USE_GLPK
            else if(!strcmp(argv[i]+13,"glpk"))
            {
                opagp->solverType = opag::GLPK;
            }
#endif
#ifdef USE_CBC
            else if(!strcmp(argv[i]+13,"cbc"))
            {
                opagp->solverType = opag::CBC;
            }
#endif
            else
            {
                cerr << "Error: ilp_solver " << argv[i]+13 << " unknown" << endl;
                exit(-1);
            }
        }
        else if(strstr(argv[i], "--ilp_formulation="))
        {
            if(!strcmp(argv[i]+18,"pmcm1") || !strcmp(argv[i]+18,"ilp1"))
            {
                opagp->ilp_formulation = opag::ILP1;
            }
            else if(!strcmp(argv[i]+18,"pmcm1bin") || !strcmp(argv[i]+18,"ilp1bin"))
            {
                opagp->ilp_formulation = opag::ILP1BIN;
            }
            else if(!strcmp(argv[i]+18,"pmcm2") || !strcmp(argv[i]+18,"ilp2"))
            {
                opagp->ilp_formulation = opag::ILP2;
            }
            else if(!strcmp(argv[i]+18,"pmcm2bin") || !strcmp(argv[i]+18,"ilp2bin"))
            {
                opagp->ilp_formulation = opag::ILP2BIN;
            }
            else if(!strcmp(argv[i]+18,"pmcm3") || !strcmp(argv[i]+18,"ilp3"))
            {
                opagp->ilp_formulation = opag::ILP3;
            }
            else
            {
                cerr << "Error: ilp_model " << argv[i]+12 << " unknown" << endl;
                exit(-1);
            }
        }
        else if(strstr(argv[i], "--no_of_extra_stages="))
        {
            opagp->no_of_extra_stages = atoi(argv[i]+21);
        }
        else if(strstr(argv[i], "--timeout="))
        {
            opagp->timeout = atoi(argv[i]+10)*1000;
        }
        else if(strstr(argv[i], "--matlab_out_address"))
        {
            opagp->matlab_output= true;
            opagp->matlab_out_adress_string = argv[i]+21;
        }
        else if(strstr(argv[i], "--cost_add="))
        {
            opagp->cost_add = atoi(argv[i]+11);
        }
        else if(strstr(argv[i], "--cost_reg="))
        {
            opagp->cost_reg = atoi(argv[i]+11);
        }
        else if(strstr(argv[i], "--cost_fa="))
        {
            opagp->cost_fa = atoi(argv[i]+10);
        }
        else if(strstr(argv[i], "--cost_ff="))
        {
            opagp->cost_ff = atoi(argv[i]+10);
        }
        else if(strstr(argv[i], "--lp_relaxation"))
        {
            opagp->lp_relaxation = true;
        }
        else if(strstr(argv[i], "--optimize_single_stage"))
        {
            opagp->optimize_single_stage = true;
        }
        else if(strstr(argv[i], "--help"))
        {
          print_short_help();
          exit(0);
        }
        else
        {
          cout << "Error: Illegal Option: " << argv[i] << endl;
          print_short_help();
          exit(-1);
        }
      }
    }

    IF_VERBOSE(1) cout << "target_set=" << target_set << endl;

    opagp->prepare_problem(&target_set);
    IF_VERBOSE(1) opagp->print_problem_info();
    opagp->optimize();

    delete opagp;
    return 0;
}
