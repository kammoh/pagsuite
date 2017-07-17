/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef COST_MODEL_H
#define COST_MODEL_H

#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "type_class.h"
#include "fundamental.h"
#include "log2_64.h"
#include "debug.h"
#include "csd.h"
#include "compute_successor_set.h"
#include "norm.h"
#include "rpag_functions.h"

typedef enum {LL_FPGA,HL_FPGA,LL_ASIC,HL_ASIC,HL_MIN_AD,LL_MIN_AD,HL_FPGA_OLD,LL_FPGA_OLD,MIN_GPC} cost_model_t;

class cost_model_pointer // the class is neccesary to implement a tmeplate indipendent pointer in the rpag_pointer class.
{
public:
  cost_model_pointer(){}
};

//######################################################################
//##### cost_model_base
template<class T_ELEM>
class cost_model_base : public cost_model_pointer // this class is used in the algorithm as interface vor the different cost models. It is not used in rpag_pointer class.
{
public:
  cost_model_base();
  virtual ~cost_model_base();
  virtual double cost_reg(T_ELEM w, int c_max = 1) = 0;
  virtual double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1) = 0;
  virtual double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3) = 0;
  virtual double cost_pag(vector<set<T_ELEM> > *pipeline_set) = 0;
  void set_pipeline_state(int s);

  int p_state;
  float cost_FF; // cost for a single flipflop in a register (copyed from the rpag variable by constructor)
  float cost_FA; // cost for a single full adder in a adder  (copyed from the rpag variable by constructor)
  void debug_print_var();
};

template<class T_ELEM>
cost_model_base<T_ELEM>::cost_model_base()
{
  this->p_state = -1;
}
template<class T_ELEM>
cost_model_base<T_ELEM>::~cost_model_base()
{

}
template<class T_ELEM>
void cost_model_base<T_ELEM>::set_pipeline_state(int s)
{
  this->p_state = s;
}
template<class T_ELEM>
void cost_model_base<T_ELEM>::debug_print_var()
{
  std::cout << std::endl << "cost_model p_state=" << this->p_state << std::endl;
}
//##### cost_model_base
//######################################################################

//######################################################################
//##### cost_model_High_Level
template<class T_ELEM, class T_CLASS>
class cost_model_HL : public cost_model_base<T_ELEM>
{
  public:
  cost_model_HL (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_HL<T_ELEM, T_CLASS>::cost_model_HL (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  UNUSED(w);
  UNUSED(c_max);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(c_max);
  return no_of_predecessors_used;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  double pag_cost=0;
  for(unsigned s=0; s < pipeline_set->size(); s++)
  {
    pag_cost += ((double) (*pipeline_set)[s].size());
  }
  return pag_cost;
}
//##### cost_model_High_Level
//######################################################################

//######################################################################
//##### cost_model_Low_Level
template<class T_ELEM, class T_CLASS>
class cost_model_LL : public cost_model_base<T_ELEM>
{
  public:
  cost_model_LL (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_LL<T_ELEM, T_CLASS>::cost_model_LL (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  return this->cost_add(w,w,0,0,c_max);
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(c_max);

  //register or adder with single predecessor:
  int predecessor_word_size = compute_word_size(p1 , rpag_p->input_wordsize);
  float cost_predecessor;

  //determine predecessor cost:
  if(rpag_p->adder_depth(p1) < this->p_state-1)
  {
    //predecessor u can be potentially realized as register
    cost_predecessor = predecessor_word_size * this->cost_FF;
  }
  else
  {
    //predecessor u must be realized as adder
    cost_predecessor = predecessor_word_size * this->cost_FA;
  }

  if(no_of_predecessors_used == 2)
  {
    //adder with two predecessors:
    //determine cost of 2nd predecessor:
    predecessor_word_size = compute_word_size(p2 , rpag_p->input_wordsize);
    if(rpag_p->adder_depth(p2) < this->p_state-1)
    {
      //predecessor u can be potentially realized as register
      cost_predecessor += predecessor_word_size * this->cost_FF;
    }
    else
    {
      //predecessor u must be realized as adder
      cost_predecessor += predecessor_word_size * this->cost_FA;
    }
  }
  //determine working set element cost:
  float cost_working_set_element;
  int working_set_element_word_size = compute_word_size(w , rpag_p->input_wordsize);

  if(no_of_predecessors_used == 0)
  {
    //register:
    cost_working_set_element = working_set_element_word_size*this->cost_FF;
  }
  else
  {
    //adder:
    cost_working_set_element = working_set_element_word_size*this->cost_FA;
  }

  return cost_predecessor/cost_working_set_element;
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return compute_word_size(w, rpag_p->input_wordsize);
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  typename set< T_ELEM >::iterator set_iter;

  double cost = 0.0;

  for(unsigned s = 0; s < pipeline_set->size(); ++s)
  {
    for(set_iter = (*pipeline_set)[s].begin(); set_iter != (*pipeline_set)[s].end(); ++set_iter)
    {
      cost += (double) compute_word_size(*set_iter, rpag_p->input_wordsize);
    }
  }
  return cost;
}
//##### cost_model_Low_Level
//######################################################################

//######################################################################
//##### cost_model_ll_min_add
template<class T_ELEM, class T_CLASS>
class cost_model_hl_min_ad : public cost_model_base<T_ELEM>
{
  public:
  cost_model_hl_min_ad (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_hl_min_ad<T_ELEM, T_CLASS>::cost_model_hl_min_ad (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_hl_min_ad<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  UNUSED(w);
  UNUSED(c_max);
  return 0;
}

template<class T_ELEM, class T_CLASS>
double cost_model_hl_min_ad<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(c_max);
  return no_of_predecessors_used;
}

template<class T_ELEM, class T_CLASS>
double cost_model_hl_min_ad<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_hl_min_ad<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  int no_of_adders = 0;
  typename set<T_ELEM>::iterator set_iter;

  for(unsigned int s = 0; s < pipeline_set->size(); ++s)
  {
    for(set_iter = (*pipeline_set)[s].begin(); set_iter != (*pipeline_set)[s].end(); ++set_iter)
    {
      if(s == 0)
      {
        if(nonzeros(*set_iter) != 1)
          no_of_adders++;
      }
      else
      {
        //seach element in previous stage
        if((*pipeline_set)[s-1].find(*set_iter) == (*pipeline_set)[s-1].end())
        {
          //element not found -> count as adder
          no_of_adders++;
        }
      }
    }
  }
  return no_of_adders;
}
//##### cost_model_hl_min_add
//######################################################################

//######################################################################
//##### cost_model_ll_min_add
template<class T_ELEM, class T_CLASS>
class cost_model_ll_min_ad : public cost_model_base<T_ELEM>
{
  public:
  cost_model_ll_min_ad (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_ll_min_ad<T_ELEM, T_CLASS>::cost_model_ll_min_ad (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_ll_min_ad<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  UNUSED(w);
  UNUSED(c_max);
  return 0;
}

template<class T_ELEM, class T_CLASS>
double cost_model_ll_min_ad<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
    UNUSED(c_max);

    //register or adder with single predecessor:
    int predecessor_word_size = compute_word_size(p1 , rpag_p->input_wordsize);
    float cost_predecessor;

    //determine predecessor cost:
    if(rpag_p->adder_depth(p1) < this->p_state-1)
    {
      //predecessor u can be potentially realized as register
      cost_predecessor = predecessor_word_size * this->cost_FF;
    }
    else
    {
      //predecessor u must be realized as adder
      cost_predecessor = predecessor_word_size * this->cost_FA;
    }

    if(no_of_predecessors_used == 2)
    {
      //adder with two predecessors:
      //determine cost of 2nd predecessor:
      predecessor_word_size = compute_word_size(p2 , rpag_p->input_wordsize);
      if(rpag_p->adder_depth(p2) < this->p_state-1)
      {
        //predecessor u can be potentially realized as register
        cost_predecessor += predecessor_word_size * this->cost_FF;
      }
      else
      {
        //predecessor u must be realized as adder
        cost_predecessor += predecessor_word_size * this->cost_FA;
      }
    }
    //determine working set element cost:
    float cost_working_set_element;
    int working_set_element_word_size = compute_word_size(w , rpag_p->input_wordsize);

    if(no_of_predecessors_used == 0)
    {
      //register:
      cost_working_set_element = working_set_element_word_size*this->cost_FF;
    }
    else
    {
      //adder:
      cost_working_set_element = working_set_element_word_size*this->cost_FA;
    }

    return cost_predecessor/cost_working_set_element;
}

template<class T_ELEM, class T_CLASS>
double cost_model_ll_min_ad<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return compute_word_size(w, rpag_p->input_wordsize);
}

template<class T_ELEM, class T_CLASS>
double cost_model_ll_min_ad<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  int no_of_adders = 0;
  typename set<T_ELEM>::iterator set_iter;


  double cost = 0.0;

  for(unsigned int s = 0; s < pipeline_set->size(); ++s)
  {
    for(set_iter = (*pipeline_set)[s].begin(); set_iter != (*pipeline_set)[s].end(); ++set_iter)
    {
      if(s == 0)
      {
        if(nonzeros(*set_iter) != 1)
          cost += (double) compute_word_size(*set_iter, rpag_p->input_wordsize);
      }
      else
      {
        //seach element in previous stage
        if((*pipeline_set)[s-1].find(*set_iter) == (*pipeline_set)[s-1].end())
        {
          //element not found -> count as adder
          cost += (double) compute_word_size(*set_iter, rpag_p->input_wordsize);
        }
      }
    }
  }
  return cost;
;
}
//##### cost_model_ll_min_add
//######################################################################

//######################################################################
//##### cost_model_min_gpc
template<class T_ELEM, class T_CLASS>
class cost_model_min_gpc : public cost_model_base<T_ELEM>
{
  public:
  cost_model_min_gpc (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_min_gpc<T_ELEM, T_CLASS>::cost_model_min_gpc (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_min_gpc<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  UNUSED(w);
  UNUSED(c_max);
  return 0;
}

template<class T_ELEM, class T_CLASS>
double cost_model_min_gpc<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(c_max);
  return no_of_predecessors_used;
}

template<class T_ELEM, class T_CLASS>
double cost_model_min_gpc<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_min_gpc<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  double gpcTotal=0;

  typename std::list<realization_row<T_ELEM> > pipelined_adder_graph;
  pipeline_set_to_adder_graph(*pipeline_set, pipelined_adder_graph,rpag_p->is_this_a_two_input_system(),rpag_p->get_c_max(),rpag_p->ternary_sign_filter);;

  map<T_ELEM, int> gpcMap;
  for(unsigned int i=0; i < vec_t::default_elem_count; ++i)
  {
      T_ELEM base;//in CMM Case this is a Vector of Dimension 1 fild with zeros
      base = 0;// it is not possible to set the elemts bevor they are created.
      make_one_by(base,i);
      gpcMap[base] = 0;;
  }

  typename std::list<realization_row<T_ELEM> >::iterator iter;
  for(iter = pipelined_adder_graph.begin(); iter != pipelined_adder_graph.end(); ++iter)
  {
    if((*iter).B != 0) //ignore registers/wires
    {
      gpcMap[(*iter).W] = gpcMap[(*iter).A] + gpcMap[abs((*iter).B)] + 1;
      gpcTotal += gpcMap[(*iter).W];
      IF_VERBOSE(3) cout << "GPC of node " << (*iter).W << " : " << gpcMap[(*iter).W] << endl;
    }
    /*
    for(unsigned int i=0; i < (*iter).size(); i++)
    {
      cout << (*iter)[i] << " ";
    }
    cout << endl;
    */
  }
  IF_VERBOSE(3) cout << "GPC total: " << gpcTotal << endl;

  return gpcTotal;
}
//##### cost_model_min_gpc
//######################################################################


//######################################################################
//##### cost_model_High_Level_old
template<class T_ELEM, class T_CLASS>
class cost_model_HL_old : public cost_model_base<T_ELEM>
{
  public:
  cost_model_HL_old (T_CLASS *rpag_p_in);

  double cost_reg(int_t w, int c_max = 1);
  double cost_reg(vec_t w, int c_max = 1);


  double cost_add(vec_t w, vec_t p1, vec_t p2, int no_of_predecessors_used, int c_max);
  double cost_add(int_t w, int_t p1, int_t p2, int no_of_predecessors_used, int c_max = 1);

  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_HL_old<T_ELEM, T_CLASS>::cost_model_HL_old (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_reg(int_t w, int c_max)
{
  UNUSED(w);
  UNUSED(c_max);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_reg(vec_t w, int c_max)
{
  return 1 + ((double) abs(w).max())/((double) c_max);
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_add(int_t w, int_t p1, int_t p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(c_max);
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  return no_of_predecessors_used;

}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_add(vec_t w, vec_t p1, vec_t p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(w);
  if(no_of_predecessors_used > 1)
  {
    return no_of_predecessors_used + ((double) max(abs(p1).max(),abs(p2).max()))/((double) c_max);
  }
  else
  {
    return no_of_predecessors_used + ((double) abs(p1).max())/((double) c_max);
  }
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(w);
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return 1;
}

template<class T_ELEM, class T_CLASS>
double cost_model_HL_old<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  double pag_cost=0;
  for(unsigned s=0; s < pipeline_set->size(); s++)
  {
    pag_cost += ((double) (*pipeline_set)[s].size());
  }
  return pag_cost;

}
//##### cost_model_High_Level_old
//######################################################################

//######################################################################
//##### cost_model_Low_Level_old
template<class T_ELEM, class T_CLASS>
class cost_model_LL_old : public cost_model_base<T_ELEM>
{
  public:
  cost_model_LL_old (T_CLASS *rpag_p_in);
  double cost_reg(T_ELEM w, int c_max = 1);
  double cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max = 1);
  double cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3);
  double cost_pag(vector<set<T_ELEM> > *pipeline_set);
  T_CLASS *rpag_p;
  int current_stage;
};

template<class T_ELEM, class T_CLASS>
cost_model_LL_old<T_ELEM, T_CLASS>::cost_model_LL_old (T_CLASS *rpag_p_in)
{
  rpag_p = rpag_p_in;
  this->cost_FA = rpag_p->cost_FA;
  this->cost_FF = rpag_p->cost_FF;
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL_old<T_ELEM, T_CLASS>::cost_reg(T_ELEM w, int c_max)
{
  UNUSED(c_max);
  return this->cost_add(w,0,0,0);
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL_old<T_ELEM, T_CLASS>::cost_add(T_ELEM w, T_ELEM p1, T_ELEM p2, int no_of_predecessors_used, int c_max)
{
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(no_of_predecessors_used);
  UNUSED(c_max);
  return compute_word_size(w,rpag_p->input_wordsize);
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL_old<T_ELEM, T_CLASS>::cost_ternary_add(T_ELEM w, T_ELEM p1, T_ELEM p2, T_ELEM p3)
{
  UNUSED(p1);
  UNUSED(p2);
  UNUSED(p3);
  return compute_word_size(w,rpag_p->input_wordsize);
}

template<class T_ELEM, class T_CLASS>
double cost_model_LL_old<T_ELEM, T_CLASS>::cost_pag(vector<set<T_ELEM> > *pipeline_set)
{
  typename set< T_ELEM >::iterator set_iter;
  double cost = 0.0;
  for(unsigned s = 0; s < pipeline_set->size(); ++s)
  {
    for(set_iter = (*pipeline_set)[s].begin(); set_iter != (*pipeline_set)[s].end(); ++set_iter)
    {
      cost += (double) compute_word_size(*set_iter, rpag_p->input_wordsize);
    }
  }
  return cost;
}
//##### cost_model_Low_Level_old
//######################################################################

#endif //COST_MODEL_H
