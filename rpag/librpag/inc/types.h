/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <set>
#include <map>
#include <list>
#include <ostream>
#include "rpag_vec.h"
#include "triplet.h"

#define UNUSED(x) (void)(x)
#define MAX_INT32  2147483647LL;
#define MAX_UINT32 4294967295LL;

#define USE_NEW_NZ_COMPARE_FUNTION 0 // 1= use it; 0 = use the old version

#define USE_TOPOLOGY_E 1 //if 0, the pair-pair topology e is not checked

// the default values which are used by the cost models to set the FF and Fa cost's
#define COST_FF_DEFAULT_ASIC 0.5
#define COST_FA_DEFAULT_ASIC 1
#define COST_FF_DEFAULT_FPGA 1
#define COST_FA_DEFAULT_FPGA 1

using namespace std;

//typedef int int_t; //type of integer numbers
typedef long long int int_t; //type of integer numbers
typedef vector<int_t> int_vec_t;
typedef set<int_t> int_set_t;

typedef vector<int_set_t> pipeline_set_t;

typedef vector<int> sd_t;
typedef set<sd_t> sd_set_t;
typedef vector<sd_t> sd_vec_t;
typedef set<sd_vec_t> sd_vec_set_t;

typedef map<int_t, double> int_double_map_t;

typedef pair<int_t,int_t> int_pair_t;
typedef set<int_pair_t> int_pair_set_t;
typedef map<int_pair_t, double> int_pair_double_map_t;

typedef rpag_vec<int_t> vec_t;
typedef map<vec_t, double> vec_double_map_t;
typedef pair<vec_t,vec_t> vec_pair_t;
typedef set<vec_t> vec_set_t;
typedef set<vec_pair_t> vec_pair_set_t;
typedef vector<vec_set_t> pipeline_vec_set_t;
typedef map<vec_pair_t, double> vec_pair_double_map_t;

typedef triplet<int_t, int_t, int_t> int_triplet_t;
typedef set<int_triplet_t> int_triplet_set_t;
typedef map<int_triplet_t, double> int_triplet_double_map_t;
typedef triplet<vec_t, vec_t, vec_t> vec_triplet_t;
typedef set<vec_triplet_t> vec_triplet_set_t;
typedef map<vec_triplet_t, double> vec_triplet_double_map_t;

#endif //TYPES_H
