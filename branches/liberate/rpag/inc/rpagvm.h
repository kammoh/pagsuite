/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAGVM_H
#define RPAGVM_H
#include "type_class.h"
#include "rpag_base.h"
#include "rpag_topologies.h"

class rpagvm : public rpag_base<vec_t>
{
public:
  rpagvm();
  virtual ~rpagvm();
  virtual rpag_type get_rpag_type();

protected:

  int adder_depth(vec_t x);

private:
  virtual bool is_this_a_two_input_system(void){return true;}

  vec_t get_best_single_predecessor(const vec_set_t &working_set, const vec_set_t &predecessor_set, int s);
  void get_best_multi_predecessor(const vec_set_t &working_set, vec_set_t *best_multi_predecessor_set, int s);

  void update_pair_gain_map(set<pair<vec_t,vec_t> > &predecessor_pair_set, vec_t w, const vec_set_t &working_set, vec_pair_double_map_t &pair_gain_map);
};

#endif // RPAGVM_H
