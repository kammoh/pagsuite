/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAG_POINTER_H
#define RPAG_POINTER_H
#include <string>
#include <vector>
#include "cost_model.h"

namespace rpag {

enum rpag_type{typ_rpag, typ_rpagvm, typ_rpagt, typ_rpagtvm};

class rpag_pointer
{
public:
  rpag_pointer(){}
  virtual ~rpag_pointer(){}
  virtual int optimize()=0;
  virtual rpag_type get_rpag_type()=0;
  virtual int_t get_c_max()=0;

  std::vector<char*> target_vec;

  vector<string> start_arguments;

  //user specified parameters:  
  bool exhaustive;
  double rand_variance;
  int rand_seed;
  cost_model_t cost_model;
  int no_of_runs; //number of runs (with random variance)
  static int input_wordsize;
  int mult_wordsize;
  int max_no_of_mult;
  int stages_embedded_mult;
  int no_of_extra_stages;
  bool force_minimal_depth;
  int search_limit;
  bool benchmark;
  bool show_adder_graph;
  bool ternary_adders;
  long msd_digit_permutation_limit;
  long msd_representation_limit;

  bool ternary_sign_filter;

  bool file_output;

  bool matlab_output;
  std::string matlab_out_address_string;

  std::vector<int> wordsize_constraints;
  std::vector<int> adder_depth_constraints;

  bool msd_successor_evaluation; //determines, if predecessors are computed from all MSDs for gain calculation (more precise but more time consuming)

  unsigned int fix_decision_cnt;

  int set_cost_FF(float FF);
  int set_cost_FA(float FA);
  // just change or set this two cost-variable's with the set functions.(the only exeption is the constructor)
  float cost_FF; // cost for a single flipflop in a register
  float cost_FA; // cost for a single full adder in a adder

  //function pointer to selected cost model (it is not possible to use the a cost_model_base pointer bcause of the Template parameter)
  cost_model_pointer *cost_pointer;

  rpag_pointer* set_parameter_equal_to (rpag_pointer* rhs);
  void print_parameter(){}

};

}

#endif // RPAG_POINTER_H
