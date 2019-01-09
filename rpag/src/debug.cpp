/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include <iostream>
#include "pagsuite/types.h"
#include "pagsuite/debug.h"

using namespace std;

namespace PAGSuite
{


  int global_verbose = 0;

  ostream &operator<<(ostream &s, sd_t &sd)
  {
    sd_t::iterator iter;
    for (iter = sd.begin(); iter != sd.end();)
    {
      if (*iter >= 0)
        s << " " << *iter;
      else
        s << *iter;

      ++iter;
      if (iter != sd.end())
        s << " ";
    }
    return s;
  }

  ostream &operator<<(ostream &s, sd_set_t &msd_set)
  {
    sd_set_t::iterator iter;
    for (iter = msd_set.begin(); iter != msd_set.end(); ++iter)
    {
      sd_t sd = *iter;
      s << "(" << sd << ")" << endl;
    }
    return s;
  }

}