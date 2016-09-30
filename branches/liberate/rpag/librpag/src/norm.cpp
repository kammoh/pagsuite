/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "norm.h"
#include "rpag_functions.h"
#include <algorithm>

namespace rpag {

int_t norm(int_t in, bool *switched)
{
	if(switched != NULL)
	{
	  if(in < 0)
	  {
	    in = -in;
	    *switched = true;
	  }
	  else
	  {
	    *switched = false;
	  }
	  return in;
	}
	else
	{
      return abs(in);
	}
}

vec_t norm(vec_t in, bool *switched) // the first element witch is different  to zero have to be positiv!
{
	vec_t out = in;
	vec_t::iterator it;
	for(it = in.begin(); it != in.end(); ++it)
	{
		if(*it != 0)
		{
			if(*it < 0)
			{
				out = in* (-1);
				if(switched != NULL){*switched = true;}
			}
			break;
		}
	}
	if(switched != NULL){*switched = false;}
	return out;
}

}
