/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef LOG2_64_H
#define LOG2_64_H

#include <inttypes.h>
//#include <sys/types.h>

namespace PAGSuite
{

  int log2f_64(uint64_t n);

  int log2c_64(uint64_t n);

  int log3f_64(uint64_t n);

  int log3c_64(uint64_t n);

}

#endif // LOG2_64_H
