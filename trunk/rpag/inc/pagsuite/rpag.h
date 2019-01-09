/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAG_H
#define RPAG_H

#include "type_class.h"
#include "rpag_base.h"
#include "rpag_topologies.h"

namespace PAGSuite
{

  class rpag : public rpag_base<int_t>
  {
  public:
    rpag();

    rpag(rpag_pointer *rpagp); //copy constructor

    virtual ~rpag();

    virtual rpag_type get_rpag_type();

  protected:
    virtual int adder_depth(int_t x);

  private:
    //###################################################################
    virtual bool is_this_a_two_input_system(void)
    { return true; }

    virtual int_t get_best_single_predecessor(const int_set_t &working_set, const int_set_t &predecessor_set, int s);

    virtual void get_best_multi_predecessor(const int_set_t &working_set, int_set_t *best_multi_predecessor_set, int s);

    void update_pair_gain_map(set<pair<int_t, int_t> > &predecessor_pair_set, int_t w, const int_set_t &working_set, int_pair_double_map_t &pair_gain_map);

  };

}

#endif // RPAG_H
