/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef NORM_H
#define NORM_H

#include "type_class.h"
#include "debug.h"

int_t norm(int_t in, bool *switched = NULL);
vec_t norm(vec_t in, bool *switched = NULL);




#endif // NORM_H
