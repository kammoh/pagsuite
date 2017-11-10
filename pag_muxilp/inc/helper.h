#ifndef HELPER_H
#define HELPER_H
#include "adder_graph.h"
#include "triplet.h"
#include <set>


template <typename T>
ostream& operator<<(ostream &s, set<T> vec)
{
  typename set<T>::iterator iter;
  for(iter = vec.begin(); iter != vec.end(); )
  {
    s << *iter;
    ++iter;
    if(iter != vec.end())
      s  << " ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, list<T> vec)
{
  typename list<T>::iterator iter;
  for(iter = vec.begin(); iter != vec.end(); )
  {
    s << *iter;
    ++iter;
    if(iter != vec.end())
      s  << " and ";
  }
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, triplet<T,T,T> p)
{
    s <<"{" << p.first <<  "," << p.second << "," << p.third<< "}";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, pair<T,T> p)
{
    s <<"{" << p.first <<  "," << p.second << "}";
  return s;
}

template <typename T>
ostream& operator<<(ostream &s, vector<T> vec)
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
ostream& operator<<(ostream &s, vector<vector<T> > vec)
{
  typename vector<vector<T> >::iterator iter;
  if(vec.size() > 0)
  {
    iter = vec.begin();
    s << *iter ;
    for(++iter; iter != vec.end(); ++iter)
    {
      s << "," << *iter << "";
    }
  }
  return s;
}

inline ostream& operator<<(ostream &s, adder_graph_base_node_t* node)
{
    s << node->output_factor;
  return s;
}


#endif // HELPER_H
