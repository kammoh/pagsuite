/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef CONFIGURABLE_H
#define CONFIGURABLE_H
#include "rpag_vec.h"

namespace PAGSuite
{

  template<typename T, typename Alloc = std::allocator<T> >
  class conf_predecessor : public rpag_vec<T, Alloc>
  {
  public:
    conf_predecessor()
    {};

    virtual ~conf_predecessor()
    {};
    vector<int> schift;
    vector<bool> dontcare;
    set<int> conf;
  };

}

#endif //CONFIGURABLE_H
