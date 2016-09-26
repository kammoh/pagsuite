/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef TYPE_CLASS_H
#define TYPE_CLASS_H

#include "types.h"
#include "csd.h"
using namespace std;

class int_double_map_cmp
{
  public:
  bool operator()(const std::pair<int_t,double> &a, const std::pair<int_t,double> &b)
  {
    return a.second < b.second;
  }
};
class vec_double_map_cmp
{
  public:
  bool operator()(const std::pair<vec_t,double> &a, const std::pair<vec_t,double> &b)
  {
    return a.second < b.second;
  }
};

class int_double_map_cmp_both
{
  public:
  bool operator()(const std::pair<int_t,double> &a, const std::pair<int_t,double> &b)
  {
    if(a.second == b.second)
    {
      return a.first < b.first;
    }
    else
    {
      return a.second < b.second;
    }
  }
};


class int_pair_double_map_cmp
{
  public:
  template <typename T>
  bool operator()(const std::pair<T,double> &a, const std::pair<T,double> &b)
  {
    return a.second < b.second;
  }
};


class int_triplet_double_map_cmp
{
  public:
  bool operator()(const std::pair<int_triplet_t,double> &a, const std::pair<int_triplet_t,double> &b)
  {
    return a.second < b.second;
  }
};

class T_double_map_nz_cmp
{
  public:
  template<typename T>
  bool operator()(const std::pair<T,double> &a, const std::pair<T,double> &b)
  {
   #if USE_NEW_NZ_COMPARE_FUNTION
    if(a.second == b.second)
    {

      return (nonzeros(a.first) > nonzeros( b.first));
    }
    else
   #endif //USE_NEW_NZ_COMPARE_FUNTION
    {
      return a.second < b.second;
    }
  }
};


class T_pair_or_tripl_double_map_nz_cmp
{
  public:
    template<typename T>
    bool operator()(std::pair< const std::pair<T,T> , double> &a, std::pair< const std::pair<T,T> , double> &b)
    {
      #if USE_NEW_NZ_COMPARE_FUNTION
        if(a.second == b.second)
        {
          //compare the total nonzeros of a pair
          return ( (nonzeros(a.first.first) + nonzeros(a.first.second)) > (nonzeros(b.first.first) + nonzeros(b.first.second)) );
        }
        else
      #endif // USE_NEW_NZ_COMPARE_FUNTION
        {
          return a.second < b.second;
        }
    }

    template<typename T>
    bool operator()(std::pair< const triplet<T,T,T> , double> &a, std::pair< const triplet<T,T,T> , double> &b)
    {
      #if USE_NEW_NZ_COMPARE_FUNTION
        if(a.second == b.second)
        {
          //compare the total nonzeros of a pair
          return ( (nonzeros(a.first.first) + nonzeros(a.first.second)+ nonzeros(a.first.third)) > (nonzeros(b.first.first) + nonzeros(b.first.second) + nonzeros(b.first.third)) );
        }
        else
      #endif // USE_NEW_NZ_COMPARE_FUNTION
        {
          return a.second < b.second;
        }
    }
 };

#endif //TYPE_CLASS_H
