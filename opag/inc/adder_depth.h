/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef ADDER_DEPTH_H
#define ADDER_DEPTH_H
#include "csd.h"
#include "log2_64.h"

inline int adder_depth(int_t x)
{
  return log2c_64(nonzeros(x));
}


int adder_depth(int_set_t &x_set);

#endif // ADDER_DEPTH_H
