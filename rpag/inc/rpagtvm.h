/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAGTVM_H
#define RPAGTVM_H

#include "rpagvm.h"

class rpagtvm : public rpagvm
{
public:
  rpagtvm();
  rpagtvm(rpag_pointer *rpagp); //copy constructor

  ~rpagtvm();
  rpag_type get_rpag_type();

  bool is_this_a_two_input_system(void){return false;}

  int adder_depth(vec_t x);

  vec_t get_best_single_predecessor(const vec_set_t &working_set, const vec_set_t &predecessor_set, int s);
  void get_best_multi_predecessor(const vec_set_t &working_set, vec_set_t *best_multi_predecessor_set, int s);
};

#endif // RPAGTVM_H
