/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "rpag_pointer.h"

int rpag_pointer::set_cost_FF(float FF)
{
  if(cost_FF == -1)
  {
    cost_FF = FF;
    return 1;
  }
  else
  {
    return -1;
  }
}

int rpag_pointer::set_cost_FA(float FA)
{
  if(cost_FA == -1)
  {
    cost_FA = FA;
    return 1;
  }
  else
  {
    return -1;
  }
}

rpag_pointer* rpag_pointer::copy_parameters (rpag_pointer* rhs)
{
    this->target_vec                  = rhs->target_vec;

    this->exhaustive                  = rhs->exhaustive;
    this->rand_variance               = rhs->rand_variance;
    this->rand_seed                   = rhs->rand_seed;
    this->no_of_runs                  = rhs->no_of_runs;
    this->input_wordsize              = rhs->input_wordsize;
    this->mult_wordsize               = rhs->mult_wordsize;
    this->max_no_of_mult              = rhs->max_no_of_mult;
    this->stages_embedded_mult        = rhs->stages_embedded_mult;
    this->no_of_extra_stages          = rhs->no_of_extra_stages;
    this->force_minimal_depth         = rhs->force_minimal_depth;
    this->search_limit                = rhs->search_limit;
    this->benchmark                   = rhs->benchmark;
    this->show_adder_graph            = rhs->show_adder_graph;
    this->ternary_adders              = rhs->ternary_adders;
    this->msd_digit_permutation_limit    = rhs->msd_digit_permutation_limit;
    this->msd_representation_limit           = rhs->msd_representation_limit;

    this->matlab_output               = rhs->matlab_output;
    this->matlab_out_address_string     = rhs->matlab_out_address_string;

    this->adder_depth_constraints     = rhs->adder_depth_constraints;
    this->wordsize_constraints        = rhs->wordsize_constraints;

    this->cost_FA                     = rhs->cost_FA;
    this->cost_FA                     = rhs->cost_FA;

    this->file_output                 = rhs->file_output;
    this->filename                    = rhs->filename;

    this->start_arguments             = rhs->start_arguments;

    this->fix_decision_cnt            = rhs->fix_decision_cnt;

    this->ternary_sign_filter         = rhs->ternary_sign_filter;

    this->cost_model                  = rhs->cost_model;

    this->enable_addergraph_computation_and_final_optimisation = rhs->enable_addergraph_computation_and_final_optimisation;

    return this;
}
