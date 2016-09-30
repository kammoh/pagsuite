/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "compute_successor_set.h"

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#include "main.h"
#include "csd.h"
#include "rpag_base.h"
#include "rpag.h"   // two input adder
#include "rpagt.h"  // three input adder
#include "rpagvm.h" // vector matrix
#include "rpagtvm.h" // vector matrix with three input adder
#include "cost_model.h"
#include "tic_toc.h"
#include "configurable.h"

#include "fundamental.h"
#include "cost_model.h"

using namespace rpag;

int print_short_help()
{
  cout << "usage: rpag [OPTIONS] coefficients" << endl;
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "General Options:" << endl;
  cout << "Option                                         Meaning" << endl;
  cout << "--help                                         Prints this help" << endl;
  cout << "--verbose=0...8                                Verbosity level (0: no information, 8: all information during optimization), default:1" << endl;                      
  cout << "--input_wordsize=[int]                         Word size of the input of the MCM block (for low level cost model, see below)" << endl;
  cout << "--cost_model=[cost model]                      Selects one of the following cost models (default: hl_fpga):" << endl;
  cout << "                                               hl_fpga: high level cost model for FPGAs, the cost of each adder and register are equal to one" << endl;
  cout << "                                               ll_fpga: low level cost model for FPGAs, the cost of each adder and register are equal to its word size (input word size + coeff. word size)" << endl;
  cout << "                                               hl_asic: high level cost model for ASICs,the cost of adder is twice than of a register (c_a=2, c_r=1)" << endl;
  cout << "                                               min_ad_fpga: each target is scheduled according to its minimal adder depth; the cost of each adder is one, the cost of a register is zero" << endl;
  cout << "                                               min_gpc: " << endl;
  cout << "                                               Example: --cost_model=hl_asic" << endl;
  //features not implemented:
  //  cout << "                                               ll_asic: currently not supported" << endl;
  //  cout << "--exhaustive=true|false                        Enables the exhaustive mode, where allways successor pairs are evaluated, default:false" << endl;
  cout << "--no_of_extra_stages=int                       Per default, the pipeline depth is chosen to the minimum possible, more pipeline stages can be selected by choosing no_of_extra_stages>0, default:0" << endl;
  cout << "--without_adder_graph                          Disables the computation of the adder graph solution including shift values" << endl;
  cout << "--benchmark                                    Print a list of counted operation of the solution" << endl;
//  cout << "--matlab_out_address=[string]                  Print a matlab compatible line with information about the solution" << endl;
  cout << "--file_output                                  The RPAG output will be printed in a file (output_adder_graph.txt) too" << endl;
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "Options for meta optimizations (where several RPAG runs are used to further optimize the result):" << endl;
  cout << "  There are two meta-heuristics implemented:" << endl;
  cout << "   1) Meta greedy heuristic: The n'th best decisions (set by the 'search_limit' parameter') are evaluated and the best one is taken for evaluating the next decision." << endl;
  cout << "      The algorithm automatically terminates, the 'no_of_runs' parameter is ignored." << endl;
  cout << "   2) Random selection: Not the locally best decision is taken but the n'th best is chosen randomly (set by 'rand_variance' parameter)." << endl;
  cout << "      The best result out of 'no_of_runs' is taken, the first run is always pure greedy." << endl;
  cout << "      The meta greedy has to be disabled by setting search_limit=-1 for random selection." << endl;
  cout << "  Note that better results in less iterations are typically achieved by using the meta greedy heuristic." << endl;
  cout << endl;
  cout << "Option                                         Meaning" << endl;
  cout << "--search_limit=int                             Sets the search width limit which determines how many different decisions exept the globally best one are evaluated" << endl;
  cout << "                                               (larlge search_limit => long runtime & better results), default:10" << endl;
  cout << "--rand_variance=real                           Sets the random variance in selecting not the locally best solution, default:0" << endl;
  cout << "--no_of_runs=int                               Sets the number of optimization runs that are performed for a single instance, default:1" << endl;
  cout << "--rand_seed=int                                For manually setting the seed used for random variance generation, default:current time" << endl;
  cout << "--msd_representation_limit=int                 Limit the number of different MSD representations, evaluated in topology (d) (-1: no limit), default: -1" << endl;
  cout << "--msd_digit_permutation_limit=int              Limit the number of digit permutations in topology (d) (-1: no limit), default: -1" << endl;
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "Options for ternary (3-input) adder support:" << endl;
  cout << "Option                                         Meaning" << endl;
  cout << "--ternary_adders                               Uses ternary adders where possible to reduce resources and pipeline depth" << endl;
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "Options for the constan matrix multiplication (CMM) problem:" << endl;
  cout << "Option                                         Meaning" << endl;
  cout << "--cmm                                          constant matrix multiplication: Treats every coefficient as vector of input weights separated by commas (see examples below)" << endl;

#ifdef EMBEDDED_MULT_FIX_ME
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "Options for hybrid optimizations (which include embedded multipliers):" << endl;
  cout << "Option                                         Meaning" << endl;
  cout << "--mult_wordsize=int                            Word size of embedded multiplier that should be incorporated into the optimization" << endl;
  cout << "--max_no_of_mult=int                           Maximal number of embedded multiplier that should be used" << endl;
  cout << "--wordsize_constraints=[int int ...]           A vector of word size constraints for each stage" << endl;
  cout << "--adder_depth_constraints=[int int ...]        A vector of adder depth constraints for each stage" << endl;
  cout << endl;
#endif
  cout << endl;
  cout << "-----------------------------------------------" << endl;
  cout << "Examples:" << endl;
  cout << "rpag 123 321                                   Finds the pipelined adder graph nodes for output coefficients 123 and 321" << endl;
  cout << endl;
  cout << "rpag --cost_model=min_ad_hl_fpga 123 321       Finds the adder graph nodes with minimal adder depth for output coefficients 123 and 321" << endl;
  cout << endl;
  cout << "rpag --ternary_adders 11280171 13342037        As above, but by using ternary adders" << endl;
  cout << endl;
  cout << "rpag --cmm 123,321 345,-543                    Computes a constant matrix multiplication of instance 123*x1+321*x2 345*x1-543*x2" << endl;
  cout << endl;
  cout << "rpag --cmm 123+321 345-543                     The syntax from older versions is still supported 123*x1+321*x2 345*x1-543*x2" << endl;
  cout << endl;
  cout << "rpag --cmm \"123,321\" \"345,-543\"                Attention! If a unix system is used, the CMM-coefficients have to be set in \"\" " << endl;


#ifdef EMBEDDED_MULT_FIX_ME
  cout << "rpag --mult_wordsize=17 11280171 13342037      As above, but includes 17 bit multipliers (as many as necessary)" << endl;
  cout << endl;
  cout << "rpag --mult_wordsize=17 \\" << endl;
  cout << "     --max_no_of_mult=1 11280171 13342037      As above, but uses only one embedded multiplier" << endl;
  cout << endl;
  cout << "rpag --mult_wordsize=17 \\" << endl;
  cout << "     --wordsize_constraints=[-1 -1 17 24] \\" << endl;
  cout << "     --adder_depth_constraints=[1 2 3 -1] \\" << endl;
  cout << "     --max_no_of_mult=1 11280171 13342037      As above, but explicitly set the constraints to result in a lower graph depth" << endl;
  cout << endl;
#endif

  exit(0);
}

int print_exhaustive_help()
{
  cout << "exhaustive help is not implemented" << endl;
  print_short_help();
  exit(0);
}
template<class T>
void set_cost_model(cost_model_t cost_model, rpag_pointer *rpagp)
{
  switch(cost_model)
  {
  case HL_FPGA:
    IF_VERBOSE(2) cout << "using hl_fpga cost model" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_FPGA);
    rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
    rpagp->cost_pointer = new cost_model_HL<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    break;

  case LL_FPGA:
    IF_VERBOSE(2) cout << "using ll_fpga cost model" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_FPGA);
    rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
    rpagp->cost_pointer= new cost_model_LL<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    if(rpagp->input_wordsize < 0)
    {
      cerr << "Error: input word size necessary for ll_fpga cost model (parameter: --input_wordsize)" << endl;
      exit(-1);
    }
    break;

  case LL_ASIC:
    cerr << "Error: LL_ASIC cost model currently not supported!" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_ASIC);
    rpagp->set_cost_FA(COST_FA_DEFAULT_ASIC);
    rpagp->cost_pointer= new cost_model_LL<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    exit(-1);
    break;

  case HL_ASIC:
    IF_VERBOSE(2) cout << "using hl_asic cost model" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_ASIC);
    rpagp->set_cost_FA(COST_FA_DEFAULT_ASIC);
    rpagp->cost_pointer= new cost_model_HL<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    break;

  case MIN_AD_FPGA:
    IF_VERBOSE(2) cout << "using cost model for minimal adder depth" << endl;
    rpagp->set_cost_FF(0.001); //!!??!!
    rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
    rpagp->cost_pointer= new cost_model_min_ad<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    break;

  case HL_FPGA_OLD:
    IF_VERBOSE(2) cout << "using OLD hl FPGA cost model-- just for debuging" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_FPGA);
    rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
    rpagp->cost_pointer= new cost_model_HL_old<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    break;

  case LL_FPGA_OLD:
    IF_VERBOSE(2) cout << "using OLD hl FPGA cost model-- just for debuging" << endl;
    rpagp->set_cost_FF(COST_FF_DEFAULT_FPGA);
    rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
    rpagp->cost_pointer= new cost_model_LL_old<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
    break;

  case MIN_GPC:
      IF_VERBOSE(2) cout << "using cost model for minimal glitch path count (GPC)" << endl;
      rpagp->set_cost_FF(0.001); //!!??!!
      rpagp->set_cost_FA(COST_FA_DEFAULT_FPGA);
      rpagp->cost_pointer= new cost_model_min_gpc<T, rpag_base<T> >( (rpag_base<T>*)rpagp );
      break;

  }
}

int main(int argc, char *argv[])
{
  global_verbose=0;

  rpag_pointer *rpagp = NULL;
  rpagp = new rpag::rpag(); //default is RPAG with 2 input adders
  cost_model_t cost_model = HL_FPGA;// with default value
  bool ternary_adders = false;
  bool vector_input = false;
  int_set_t target_set;
  vec_set_t vec_target_set;

  bool display_rpag_info = false;

  //set_default_elem_count(1);// to be shure that all vectors have minimal one element

  //enum to identify if a sequence of numbers corresponds to coefficients, adder depth constraints or word size constraints:
  enum number_sequence_mode_t{COEFF, AD, WS} number_sequence_mode=COEFF;

  for(int i = 1; i < argc; i++)
  {
    string argument = argv[i];
    rpagp->start_arguments.push_back(argument);

    if((argv[i][0] != '-') || (argv[i][1] != '-'))
      switch(number_sequence_mode)
      {
      case COEFF:
        rpagp->target_vec.push_back(argv[i]);
        break;
      case AD:
        rpagp->adder_depth_constraints.push_back(atoi(argv[i]));
        break;
      case WS:
        rpagp->wordsize_constraints.push_back(atoi(argv[i]));
        break;
      }
    else
    {
      number_sequence_mode = COEFF; //default mode
      if(strstr(argv[i], "--verbose="))
      {
        global_verbose = atol(argv[i]+10);
      }
      else if(strstr(argv[i], "--cost_model="))
      {
        if(!strcmp(argv[i]+13,"hl_fpga"))
        {
          cost_model = HL_FPGA;
        }
        else if(!strcmp(argv[i]+13,"hl_fpga_old"))
        {
          cost_model = HL_FPGA_OLD;
        }
        else if(!strcmp(argv[i]+13,"ll_fpga"))
        {
          cost_model = LL_FPGA;
        }
        else if(!strcmp(argv[i]+13,"ll_fpga_old"))
        {
          cost_model = LL_FPGA_OLD;
        }
        else if(!strcmp(argv[i]+13,"hl_asic"))
        {
          cost_model = HL_ASIC;
        }
        else if(!strcmp(argv[i]+13,"ll_asic"))
        {
          cost_model = LL_ASIC;
        }
        else if(!strcmp(argv[i]+13,"min_ad_fpga"))
        {
          cost_model = MIN_AD_FPGA;
        }
        else if(!strcmp(argv[i]+13,"min_gpc"))
        {
          cost_model = MIN_GPC;
        }
        else
        {
          cerr << "Error: cost model " << argv[i]+13 << " unknown" << endl;
          exit(-1);
        }
      }
      else if(strstr(argv[i], "--input_wordsize="))
      {
        rpagp->input_wordsize = atoi(argv[i]+17);
      }
      else if(strstr(argv[i], "--exhaustive="))
      {
        if(strstr(argv[i]+13, "true"))
        {
          rpagp->exhaustive=true;
        }
        else if(strstr(argv[i]+13, "false"))
        {
          rpagp->exhaustive=false;
        }
        else
        {
          cout << "Error: exhaustive must be true or false" << endl;
          return -1;
        }
      }
      else if(strstr(argv[i], "--rand_variance="))
      {
        rpagp->rand_variance = atof(argv[i]+16);
      }
      else if(strstr(argv[i], "--rand_seed="))
      {
        rpagp->rand_seed = atoi(argv[i]+12);
      }
      else if(strstr(argv[i], "--no_of_runs="))
      {
        rpagp->no_of_runs = atoi(argv[i]+13);
      }
      else if(strstr(argv[i], "--mult_wordsize="))
      {
        rpagp->mult_wordsize = atoi(argv[i]+16);
      }
      else if(strstr(argv[i], "--max_no_of_mult="))
      {
        rpagp->max_no_of_mult = atoi(argv[i]+17);
      }
      else if(strstr(argv[i], "--help"))
      {
        print_exhaustive_help();
      }
      else if(strstr(argv[i], "--stages_embedded_mult="))
      {
        rpagp->stages_embedded_mult = atoi(argv[i]+23);

      }
      else if(strstr(argv[i], "--wordsize_constraints=["))
      {
        rpagp->wordsize_constraints.push_back(atoi(argv[i]+24));
        number_sequence_mode = WS; //swith to wordsize constraints
      }
      else if(strstr(argv[i], "--adder_depth_constraints=["))
      {
        rpagp->adder_depth_constraints.push_back(atoi(argv[i]+27));
        number_sequence_mode = AD; //swith to wordsize constraints
      }
      else if(strstr(argv[i], "--force_minimal_depth"))
      {
        rpagp->force_minimal_depth = true;
      }
      else if(strstr(argv[i], "--no_of_extra_stages="))
      {
        rpagp->no_of_extra_stages = atoi(argv[i]+21);
      }
      else if(strstr(argv[i], "--meta_greedy_search_depth=")) //deprecated parameter (now search_limit)
      {
        rpagp->search_limit = atoi(argv[i]+27);
      }
      else if(strstr(argv[i], "--search_limit="))
      {
        rpagp->search_limit = atoi(argv[i]+15);
      }
      else if(strstr(argv[i], "--benchmark"))
      {
        rpagp->benchmark = true;
      }
      else if(strstr(argv[i], "--without_adder_graph"))
      {
        rpagp->show_adder_graph = false;
      }
      else if(strstr(argv[i], "--show_adder_graph")) //osolete flag which is maintained for compatibility with old versions!
      {
        rpagp->show_adder_graph = true;
      }
      else if(strstr(argv[i], "--msd_digit_permutation_limit="))
      {
        rpagp->msd_digit_permutation_limit = atoi(argv[i]+30);
      }
      else if(strstr(argv[i], "--msd_representation_limit="))
      {
        rpagp->msd_representation_limit = atoi(argv[i]+27);
      }
      else if(strstr(argv[i], "--matlab_out_address="))
      {
        rpagp->matlab_output= true;
        rpagp->matlab_out_address_string = argv[i]+21;
      }
      else if(strstr(argv[i], "--file_output"))
      {
          rpagp->file_output=true;
      }
      else if(strstr(argv[i], "--info"))
      {
          display_rpag_info = true;
      }
      else if(strstr(argv[i], "--ternary_adders"))
      {
        ternary_adders = true;
        rpag_pointer *rpag2p = rpagp;

        if( rpagp->get_rpag_type() == typ_rpagvm)
        {
            rpagp = new rpagtvm();// use the functions for 3 input adder and vector matrix multiplication.
        }
        else
        {
            rpagp = new rpagt();// use the functions for 3 input adder.
        }
        rpagp->set_parameter_equal_to(rpag2p);
        rpagp->ternary_adders = true;
        delete rpag2p;
      }
      else if(strstr(argv[i], "--cmm"))
      {
        vector_input = true;
        rpag_pointer *rpag2p = rpagp;
        if(rpagp->get_rpag_type() == typ_rpagt)
        {
            rpagp = new rpagtvm();// use the functions for 3 input adder and vector matrix multiplication.
        }
        else
        {
            rpagp = new rpagvm();// use the functions for vector matrix multiplikation.
        }
        rpagp->set_parameter_equal_to(rpag2p);

        delete rpag2p;
      }
      else if(strstr(argv[i], "--sign_correction"))
      {
        rpagp->ternary_sign_filter=true;
      }
      else
      {
        cout << "Error: Illegal Option: " << argv[i] << endl;
        print_short_help();
      }
    }
  }
  if(rpagp->rand_seed < 0)
  {
    rpagp->rand_seed = time(0); // use current time as seed for random generator
  }

  if(display_rpag_info)
  {
      std::cout << rpag_info(rpagp->start_arguments);
  }

  IF_VERBOSE(2)
  {
    cout << "    verbosity level=" << global_verbose << endl;
    cout << "    cost model=" << cost_model << endl;
    cout << "    input word size=" << rpagp->input_wordsize<< endl;

    cout << "    exhaustive=" << rpagp->exhaustive << endl;
    cout << "    random variance=" << rpagp->rand_variance << endl;
    cout << "    random seed=" << rpagp->rand_seed << endl;
    cout << "    number of runs=" << rpagp->no_of_runs << endl;
    cout << "    search width limit=" << rpagp->search_limit << endl;

    cout << "    msd cutter iteration max. =" << rpagp->msd_digit_permutation_limit << endl;
    cout << "    max. no of mult=" << rpagp->max_no_of_mult << endl;
    cout << "    multiplier word size=" << rpagp->mult_wordsize << endl;
    cout << "    wordsize constraints=" << rpagp->wordsize_constraints << endl;
    cout << "    adder depth constraints=" << rpagp->adder_depth_constraints << endl;
    cout << "    no. of extra stages=" << rpagp->no_of_extra_stages << endl;
    cout << "    optimize for ternary adders=" << ternary_adders << endl;

    cout << "    matlab output=" << rpagp->matlab_output << endl;
    cout << "    matlab out adress sting=" << rpagp->matlab_out_address_string << endl;
  }

  //select cost model:
  if (vector_input==true)
  {
    set_cost_model<vec_t>(cost_model, rpagp);
  }
  else
  {
    set_cost_model<int_t>(cost_model, rpagp);
  }

  rpagp->cost_model = cost_model;//this is necesseary because the min_ad cost_model change something whith where the target's are implementet in the pipeline set

  if(rpagp->target_vec.empty())
  {
    print_short_help();
  }
  else
  {
#ifdef USE_TIME
    tic();
#endif
      rpagp->optimize();
#ifdef USE_TIME
    IF_VERBOSE(1) toc();
#endif
  }

  return 1;
}

