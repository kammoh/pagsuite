/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef _DEBUG_H
#define _DEBUG_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <typeinfo>

#include "types.h"

#include <iostream>
#include <sstream>

#define DEBUG

using namespace std;

namespace rpag {

extern int global_verbose;

#define IF_VERBOSE(verbose) if(global_verbose >= verbose) for(int i=0; i < verbose; i++) cout << "  "; if(global_verbose >= verbose)
//#define IF_VERBOSE(verbose) cout << verb(0); if(global_verbose >= verbose)
//#define IF_VERBOSE(verbose) if(global_verbose >= verbose)

ostream& operator<<(ostream &s, sd_t& sd);
ostream& operator<<(ostream &s, sd_set_t& msd_set);

//ostream& operator<<(ostream &s, int_pair_set_t& int_pair_set);

template <typename T>
ostream& operator<<(ostream &s, vector<T>& vec)
{
  typename vector<T>::iterator iter;
  for(iter = vec.begin(); iter != vec.end(); )
  {
    s << *iter;
    ++iter;
    if(iter != vec.end())
      s  << ",";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, const set<T>& st)
{
  typename set<T>::iterator iter;
  for(iter = st.begin(); iter != st.end(); )
  {
    if(typeid(T)!=typeid(int_t)) s << "(";
    s << (*iter);
    if(typeid(T)!=typeid(int_t)) s << ")";
    ++iter;
    if(iter != st.end())
      s  << " ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, list<T>& st)
{
  typename list<T>::iterator iter;

  for(iter = st.begin(); iter != st.end(); )
  {
    if(typeid(T)!=typeid(int_t)) s << "(";
    s << (*iter);
    if(typeid(T)!=typeid(int_t)) s << ")";
    ++iter;
    if(iter != st.end())
      s  << " ";
  }
  return s;
}


template <typename T>
ostream& operator<<(ostream &s, vector<vector<set<T> > >& ps)
{
  typename vector<vector<set<T> > >::iterator iter;
  s << "{";
  if(ps.size() > 0)
  {
    iter = ps.begin();
    s << "[" << *iter << "]";
    for(++iter; iter != ps.end(); ++iter)
    {
      s << ",[" << *iter << "]";
    }
  }
  s << "}";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, vector<set<T> >& ps)
{
  typename vector<set<T> >::iterator iter;
  s << "{";
  if(ps.size() > 0)
  {
    iter = ps.begin();
    s << "[" << *iter << "]";
    for(++iter; iter != ps.end(); ++iter)
    {
      s << ",[" << *iter << "]";
    }
  }
  s << "}";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, map<T, double>& int_double_map)
{
  typename map<T, double>::iterator iter;
  for (iter=int_double_map.begin(); iter != int_double_map.end(); ++iter)
  {
      s << (*iter).first << ":" << (*iter).second << " ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, pair<T, T>& int_pair)
{
  s << "(" << int_pair.first << "," << int_pair.second << ")";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, map<pair<T, T>, double>& int_pair_double_map)
{
  typename map<pair<T, T>, double>::iterator iter;
  pair<T, T> int_pair;
  for (iter=int_pair_double_map.begin(); iter != int_pair_double_map.end(); ++iter)
  {
    int_pair = (*iter).first;
    s << int_pair << ":" << (*iter).second << " ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, set<pair<T, T> >& int_pair_set)
{
  typename set<pair<T, T> >::iterator iter;
  pair<T, T> predecessor_pair;

  s << "{"; //output first element (without comma)
  //output remaining elements
  for(iter = int_pair_set.begin(); iter != int_pair_set.end(); ++iter)
  {
    predecessor_pair = *iter;
    if(iter != int_pair_set.begin())
      s << ",";
    s << predecessor_pair;
  }
  s << "}";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, triplet<T, T, T>& int_triplet)
{
  s << "<" << int_triplet.first << "," << int_triplet.second << "," << int_triplet.third << ">";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, map<triplet<T, T, T>, double>& int_triplet_double_map)
{
  typename map<triplet<T, T, T>, double>::iterator iter;
  for (iter=int_triplet_double_map.begin(); iter != int_triplet_double_map.end(); ++iter)
  {
    triplet<T, T, T> t = (*iter).first;
    s << t << ":" << (*iter).second << " ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, set<triplet<T, T, T> >& int_triplet_set)
{
  typename set<triplet<T, T, T> >::iterator iter;
  triplet<T, T, T> predecessor_triplet;

  s << "{"; //output first element (without comma)
  //output remaining elements
  for(iter = int_triplet_set.begin(); iter != int_triplet_set.end(); ++iter)
  {
    predecessor_triplet = *iter;
    if(iter != int_triplet_set.begin())
      s << ",";
    s << predecessor_triplet;
  }
  s << "}";
  return s;
}

template<class T>
std::string toString(const T& t)
{
     std::ostringstream stream;
     stream << t;
     return stream.str();
}

}

#endif //_DEBUG_H


