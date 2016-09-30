/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "fundamental.h"
namespace rpag {

void fundamental(int_set_t *set)
{
  int_set_t::iterator seti;
  int_t fun;

  for(seti = set->begin(); seti != set->end(); ++seti)
  {
    fun = fundamental(*seti);
    if(fun != *seti)
    {
      set->erase(*seti);
      set->insert(fun);
    }
  }
}

void fundamental(vec_set_t *set)
{
  vec_set_t::iterator seti;
  vec_t fun;

  for(seti = set->begin(); seti != set->end(); ++seti)
  {
    fun = fundamental(*seti);
    if(fun != *seti)
    {
      set->erase(*seti);
      set->insert(fun);
    }
  }
}
}
