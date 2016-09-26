/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "adder_depth.h"


int adder_depth(int_set_t &x_set)
{
    int admax=0;
    for(int_set_t::iterator iter=x_set.begin(); iter != x_set.end(); ++iter)
    {
        int ad=log2c_64(nonzeros(*iter));
        if(ad > admax) admax = ad;
    }
    return admax;
}
