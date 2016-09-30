/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "rpag_topologies.h"
namespace rpag {
int insert_into_predecessor_pair_set(set<pair<vec_t,vec_t> > *predecessor_pair_set, vec_pair_t in)
{
     predecessor_pair_set->insert(in);
     return 0;
}


int insert_into_predecessor_pair_set(set<pair<int_t,int_t> > *predecessor_pair_set, vec_pair_t in)
{
    predecessor_pair_set->insert(pair<int_t,int_t>(in.first[0], in.second[0]));
    return 0;
}

int insert_into_predecessor_triplet_set(set<triplet<vec_t,vec_t,vec_t> > *predecessor_triplet_set, vec_triplet_t in)
{
     predecessor_triplet_set->insert(in);
     return 0;
}


int insert_into_predecessor_triplet_set(set<triplet<int_t,int_t,int_t> > *predecessor_triplet_set, vec_triplet_t in)
{
    predecessor_triplet_set->insert(triplet<int_t,int_t,int_t>(in.first[0], in.second[0], in.third[0]));
    return 0;
}

}
