/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

//#define KEEP_C_MAX_CONST_ONE

#ifndef RPAG_BASE_H
#define RPAG_BASE_H

#include "type_class.h"

#include "rpag_pointer.h"
#include "fundamental.h"
#include "log2_64.h"
#include "debug.h"
#include "csd.h"
#include "compute_successor_set.h"
#include "norm.h"
#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <typeinfo> // for rpag optimize target set
#include "cost_model.h"
#include "rpag_functions.h"
#include <string>
#include <ios>
#include <fstream>
#include "tic_toc.h"
#include "abstraction_time.h"

template <class T>
class rpag_base : public rpag_pointer
{
public:
  rpag_base();
  virtual ~rpag_base();
  virtual bool is_this_a_two_input_system(void)=0;
  int_t get_c_max();

  int optimize();
protected:
  int optimize_single_run(const set<T> *target_fun_set, vector< set<T> > *pipeline_set, bool first_decision_break, int *first_decision_predecessor_stage, set<T> *first_decison_predecessor);
  int create_rpag_output(vector< set<T> > &pipeline_set_best, double pag_cost_best, set<T> *target_set);


  //internal parameters:
#ifdef KEEP_C_MAX_CONST_ONE
  const int_t c_max = 1;
#else
  int_t c_max;
#endif

  int stages_input_pag;
  int stages_output_pag;

  //function pointer to selected cost model:
  cost_model_base<T> *cost;

  int get_local_decision(int min_decision);

public:
    T _target;
  template<typename T2>
  void load_target_set(T2 target_set)
  {
    _target = target_set;
  }

  virtual int adder_depth(T x);

  int_vec_t decision_vec;
  int_vec_t decision_max_vec;
  unsigned decision_cnt;

protected:

  inline void evaluate_usefull_predecessor_pair(T w1, T w2, set<pair<T,T> > *predecessor_pair_set, int nz_max, int_t l11, int_t l12, int_t l21, int_t l22, int_t r1, int_t r2, int_t s12, int_t s22)
  {
    T p1,p2;
    //den p1: 2^{l_{11}+l_{22}}\, - (-1)^{s_{22}} 2^{l_{12}+l_{21}
    int_t den = (1LL<<(l11+l22))-(s22<<(l12+l21));

    if(den != 0)
    {
      //num p1: w_1 2^{r_1+l_{22}}\, - (-1)^{s_{12}} (-1)^{s_{22}} w_2 2^{r_2+l_{21}}
      T num = (w1<<(r1+l22))-((s12*s22*w2)<<(r2+l21));

      if(num % den == 0)
      {
        p1 = fundamental(norm(num / den));
        if(nonzeros(p1) <= nz_max)
        {
          //den p2: 2^{l_{21}+l_{12}} - (-1)^{s_{22}}\, 2^{l_{22}+l_{11}}
          den = (1LL<<(l21+l12)) - (s22<<(l22+l11));
          if(den != 0)
          {
            //num p2: w_1 \, 2^{r_1+l_{12}} - (-1)^{s_{12}} \, w_2 \, 2^{r_2+l_{11}}
            num = (w1<<(r1+l12)) - ((s12*w2)<<(r2+l11));
            if(num % den == 0)
            {
              p2 = fundamental(norm(num / den));
              if(nonzeros(p2) <= nz_max)
              {
                if(p1 > p2)
                {
                  //swap p1 and p2:
                  T tmp=p1; p1=p2; p2=tmp;
                }
                if((abs(p1) < c_max) && (abs(p2) < c_max))
                {
                  IF_VERBOSE(4) cout << "(p1,p2)=[" << p1 << "," << p2 << "]" << ", with s12=" << s12 << ", s22=" << s22 << ", l11=" << l11 << ", l12=" << l12 << ", l21=" << l21 << ", l22=" << l22 << ", r1=" << r1 << ", r2=" << r2 << endl;
                  predecessor_pair_set->insert(pair<T,T >(p1,p2));
                }
              }
            }
          }
        }
      }
    }
  }

private:
  //###################################################################

  virtual T get_best_single_predecessor(const set<T> &working_set, const set<T> &predecessor_set, int s)
  {
		UNUSED(working_set);
		UNUSED(predecessor_set);
		UNUSED(s);
		std::cout << "Error: Called from base class! This should never happen!\n";
    exit(-1);
  }

  virtual void get_best_multi_predecessor(const set<T> &working_set, set<T> *best_multi_predecessor_set, int s)
  {
		UNUSED(working_set);
		UNUSED(best_multi_predecessor_set);
		UNUSED(s);
		std::cout << "Error: Called from base class! This should never happen!\n";
		exit(-1);
  }

  int_t compute_c_max(int_t target_fun_max);

  int initialize(const set<T>* target_set, set<T>* target_fun_set);
  int create_constraints(int adder_depth_max, int target_wordsize_max);
  void check_constraints(int no_of_pipeline_stages);

  time_measure timer;

public:
  void compute_topology_a_predecessors_2_add(const set<T> *working_set, map<T,double> *single_p_gain_map, int s_max, int ws_max, set<T> *p_set_a, map<pair<T,T>,double> *p_w_gain_map);
  void compute_topology_b_predecessors_2_add(const set<T> *working_set, map<T,double> *single_p_gain_map, int s_max, int ws_max,  set<T> *p_set_b, map<pair<T,T>,double> *p_w_gain_map);
  void compute_topology_c_predecessors_2_add(const set<T> *working_set, const set<T> *predecessor_set, map<T,double> *single_p_gain_map, int s_max, int ws_max, set<T> *p_set_c, map<pair<T,T>,double> *p_w_gain_map);
  void compute_topology_d_predecessors_2_add(T x, int nz_max, int ws_max, set<pair<T,T> > *predecessor_pair_set);
  void compute_topology_e_predecessors_2_add(T w1, T w2, int l_max, int nz_max, int ws_max, set<pair<T,T> > *predecessor_pair_set);
  void compute_cse_predecessors_2_add(const vec_set_t *working_set, map<T, double> *single_p_gain_map, int max_adder_depth);


  void compute_topology_b_predecessors_3_add(const set<T> *working_set, map<pair<T,T>, double> *p_w_gain_map, int s_max, int ws_max, set<T> *p_set_b);
  void compute_topology_c_predecessors_3_add(const set<T> *working_set, const set<T> *predecessor_set, map<pair<T,T>, double> *p_w_gain_map, int s_max, int ws_max, set<T> *p_set_c);
  void compute_topology_d_predecessors_3_add(T x, int nz_max, int ws_max, set<triplet<T,T,T> > *predecessor_triplet_set);
  void permutation_for_predeccessor_pairs(vector<int_t> &nonzero_values, vector<int> &nonzero_shift_factor, vector<int> &nonzero_element, set<triplet<T,T,T> > *predecessor_triplet_set, int &ws_max,int nz_max, bool rekursion, vec_t p3, int &iteration_counter);

protected:
  void explore_cse(vector<vec_t> &working_vec, vec_t currentCSE, int baseIndex, int endIndex, int frequency, int cseSizeMin, int cseSizeMax, string indexCombinations, map<T, double> *single_p_gain_map);

};

template <class T>
std::string generate_matlab_line(T r, int stage_r, T r1, int stage_r1, int k_r1, bool sg_r1, T r2, int stage_r2, int k_r2, bool sg_r2)
{
  if(sg_r1){r1 *= -1;}
  if(sg_r2){r2 *= -1;}
  std::string out;
  out = "{[" + toString(r) + "]," + toString(stage_r) + ",[" + toString(r1) + "]," + toString(stage_r1) + "," + toString(k_r1) + ",[" + toString(r2) + "]," + toString(stage_r2) + "," + toString(k_r2) + "}";
  return out;
}


template <class T>
rpag_base<T>::rpag_base()
{
  //default values:
  exhaustive=false;
  max_no_of_mult=-1; //no limit of multipliers
  mult_wordsize=-1;  //no multipliers used
  stages_embedded_mult=-1; //no of pipeline stages used for multiplication
  msd_successor_evaluation=true;
//  extra_pipeline_stages=0;
  stages_input_pag=-1;
  stages_output_pag=-1;
  force_minimal_depth=false;
  input_wordsize=-1;
  no_of_extra_stages=0;
  benchmark =false;

  rand_variance=0.0;
  rand_seed=-1;
  no_of_runs=1;
  search_limit=10;
  target_vec.clear();
  show_adder_graph= true;
  ternary_adders = false;
  use_cse = false;

  msd_digit_permutation_limit = -1;
  msd_representation_limit = -1;

  matlab_output = false;
  matlab_out_address_string = "";

  file_output = false;
  filename = "adder_graph.txt";
  ternary_sign_filter = false;

  //the cost model will set correct default values
  cost_FF = -1; // the -1 is a marker that this is not set yet;
  cost_FA = -1; // the -1 is a marker that this is not set yet;
  cost = NULL;

}

template <class T>
rpag_base<T>::~rpag_base()
{
  if(cost != NULL)
  {
    delete cost;
  }
}

template <class T>
int_t rpag_base<T>::compute_c_max(int_t target_fun_max)
{
  return 1LL << (log2c_64(target_fun_max)+1);
}


template <class T>
int rpag_base<T>::adder_depth(T x)
{
  assert(is_this_a_two_input_system());
  return log2c_64(nonzeros(x));
}


template <class T>
int rpag_base<T>::get_local_decision(int decision_max)
{
  int decision;

  if((search_limit <= 0) && (rand_variance > 0) && (decision_max > 0))
  {
    //random selection heuristic is selected:
    double rnd=(double) rand()/RAND_MAX;
    decision = ((int) round(rnd*min(rand_variance,((double) decision_max))));
  }
  else
  {
    decision=0;
  }

  if(decision_cnt >= decision_vec.size())
  {
    //decision vector length is less than actual decision -> enlarge vector
    decision_vec.push_back(decision);
    decision_max_vec.push_back(decision_max);
  }
  else
  {
    //decision vector length is large enough, read from or write result (for debug) to this vector
    if(search_limit > 0)
    {
      decision = decision_vec[decision_cnt];
      decision_max_vec[decision_cnt] = decision_max;
    }
    else
    {
      decision_vec[decision_cnt] = decision;
    }
  }
  decision_cnt++;

  if(decision > decision_max) decision = decision_max; //it may happen that the decision max value changes and is then less than the predicted value in the decision_vec -> limit to the max value

  return decision;
}

template <class T>
int rpag_base<T>::initialize(const set<T>* target_set, set<T>* target_fun_set)
{
  typename set<T>::iterator set_iter;
  int ad,ad_max=0,no_of_pipeline_stages;

  srand(rand_seed); //set random seed

  if(adder_depth_constraints.size() != wordsize_constraints.size())
  {
    cerr << "Error: adder depth and word size constraints must be of equal length!" << endl;
    exit(1);
  }

  if((max_no_of_mult > 0) && (mult_wordsize < 0))
  {
    cerr << "Error: No multiplier word size is given.";
    exit(-1);

  }

  if(max_no_of_mult == 0)
    mult_wordsize=-1;

  //compute fundamentals and adder depths:
  T fun;
  for(set_iter = target_set->begin(); set_iter != target_set->end(); ++set_iter)
  {
    fun = fundamental(norm(*set_iter));
    if(fun != 0)
      target_fun_set->insert(fun);
  }
  IF_VERBOSE(2) cout << "target fundamentals=" << *target_fun_set << endl;

  int_t target_fun_max=0;
  for(set_iter = target_fun_set->begin(); set_iter != target_fun_set->end(); ++set_iter)
  {
    ad = adder_depth(*set_iter);
    IF_VERBOSE(5) cout << "adder_depth(" << *set_iter << ")=" << ad << endl;
    if(ad > ad_max)
      ad_max = ad;

    if(max_elem(*set_iter) > target_fun_max)
      target_fun_max = max_elem(*set_iter);
  }

#ifndef KEEP_C_MAX_CONST_ONE
  c_max = this->compute_c_max(target_fun_max);
#endif

  IF_VERBOSE(2) cout << "c_max=" << c_max << endl;

  int target_wordsize_max = log2c_64(target_fun_max);

  IF_VERBOSE(2) cout << "max target value is " << target_fun_max << ", max target word size is " << target_wordsize_max << " bit" << endl;

  if((adder_depth_constraints.size() == 0) && (wordsize_constraints.size() == 0))
  {
    IF_VERBOSE(3) cout << "Determining adder depth and word size constraints" << endl;
    create_constraints(ad_max + no_of_extra_stages,target_wordsize_max);
    no_of_pipeline_stages=stages_output_pag+stages_input_pag;
  }
  else
  {
    if(mult_wordsize > 0)
    {
      stages_input_pag = log2c_64(mult_wordsize+1)-1;
      no_of_pipeline_stages = adder_depth_constraints.size();
      stages_output_pag = no_of_pipeline_stages - stages_input_pag + no_of_extra_stages;
    }
    else
    {
      stages_input_pag = adder_depth_constraints.size() + no_of_extra_stages;
      no_of_pipeline_stages = stages_input_pag;
      stages_output_pag = 0;
    }
  }


  IF_VERBOSE(2) cout << "optimization plan:" << endl;
  for(int s=0; s < no_of_pipeline_stages; s++)
  {
    IF_VERBOSE(2) cout << "stage " << s+1 << ", wordsize constraint=" << wordsize_constraints[s] << ", adder depth constraint=" << adder_depth_constraints[s] << endl;
  }
  return no_of_pipeline_stages;
}

template <class T>
int rpag_base<T>::optimize()
{
  timer.start();
  fix_decision_cnt = 0;
  set<T> *target_set= new set<T>;
  {
    int target_size = -1; // for testing all targets to have the same size!
    vector<char*>::iterator it;
    T to=0;

    for(it = target_vec.begin(); it != target_vec.end();++it)
    {
      // to seperate the different parts that should be able to reconfigure
      std::string target = (*it);

      if(target.find(";") != std::string::npos)
      {
        size_t break_position = target.find_first_of(";");
        while(break_position != std::string::npos)
        {
          IF_VERBOSE(5) std::cout << "insert target to target set:" << target.substr(0,break_position).c_str() << std::endl;
          test_target_size(target.substr(0,break_position).c_str(),target_size);//to test the size of the target
          target_set->insert( my_atoll(to,target.substr(0,break_position).c_str()));
          target.erase(0,break_position+1);
          break_position = target.find_first_of(";");
        }
      }
      IF_VERBOSE(5) std::cout << "insert target to target set:" << target << std::endl;

      test_target_size(target.c_str(),target_size);//to test the size of the target
      target_set->insert( my_atoll(to,target.c_str()) );
    }
  }

  // to get access to all functions of the cost_model
  cost = ( cost_model_base<T>* )this->cost_pointer;
  cost_pointer = NULL;

  set<T> target_fun_set;
  int no_of_pipeline_stages = initialize(target_set,&target_fun_set);

    if(no_of_pipeline_stages == 0)
    {
        // the create_rpag_output function needs a vector but the target_fun_set is just a set.
        vector<set<T> > pipeline_set_best;
        pipeline_set_best.push_back(target_fun_set);

        create_rpag_output(pipeline_set_best, 0, target_set);
        exit(0);
    }
  unsigned act_decision_no=0,act_decision_value=0;
  unsigned act_decision_value_best=0;

  check_constraints(no_of_pipeline_stages);

  //  validate_parameters();
  IF_VERBOSE(2) cout << "No of pipeline stages: " << no_of_pipeline_stages << endl;

  double rand_variance = this->rand_variance;

  vector< set<T> > pipeline_begin_set(no_of_pipeline_stages);//this is used for meta greedy... to remember fix decisions for predecessors in a stage.
  vector< set<T> > pipeline_set(no_of_pipeline_stages);
  vector< set<T> > pipeline_set_best(no_of_pipeline_stages);
  double pag_cost,pag_cost_best=10E10;
  unsigned int decision_cnt_copy=0;
  int_vec_t decision_vec_copy;
  int_vec_t decision_max_vec_copy;

  int first_decision_predecessor_stage = -1; int first_decision_predecessor_stage_best= -1;
  set<T> first_decison_predecessor;          set<T> first_decison_predecessor_best;


  for(int i=0; (i < no_of_runs) || (search_limit > 0); i++)
  {
    //if several runs are done, the first one should be without random variance
    if((no_of_runs > 1) && (i==0))
    {
      this->rand_variance = 0.0;
    }
    else
    {
      this->rand_variance = rand_variance;
    }

    first_decison_predecessor.clear();
    first_decision_predecessor_stage= -1;

    //the pipelineset has to be cleard from a preview optimazation
    for(int j=0; j<no_of_pipeline_stages; ++j)
    {
      pipeline_set[j].clear();
    }
    pipeline_set = pipeline_begin_set;// to start the optimization at the next greedy decision. (don't do things twice)

    if((act_decision_value == 0)&&(act_decision_no != 0)) // this case was alredy computed, but we want to know the next predecessor
    {
        //Break up the optimization after first decison, if the act_decision_value = 0 and it is not the first run
        optimize_single_run(&target_fun_set,&pipeline_set,true,&first_decision_predecessor_stage,&first_decison_predecessor);

        pag_cost = pag_cost_best;
        pipeline_set = pipeline_set_best;
        act_decision_value_best = act_decision_value;
        decision_cnt = decision_cnt_copy-1;// there is one decision less, because of we are in the first step of a new decision.

       decision_vec = decision_vec_copy;
       decision_max_vec = decision_max_vec_copy;

        //Remember best predecessors of 1st decision and its stage
        first_decision_predecessor_stage_best = first_decision_predecessor_stage;
        first_decison_predecessor_best = first_decison_predecessor;
        //i;// this isn't a real iteration. It is just a way to get the next predecesseor in a alreaedy computed case.
        IF_VERBOSE(1) cout << "iteration " << i << ": decision_vec=(" << decision_vec << "), pag_cost=" << pag_cost << ", pipeline_set=" << pipeline_set << endl;
    }
    else // the normal optimization case.
    {
        optimize_single_run(&target_fun_set,&pipeline_set,false,&first_decision_predecessor_stage,&first_decison_predecessor);
        //check total cost of result:
        //TODO remove this ... std::cout << "decision_cnt=" << decision_cnt << std::endl;
        pag_cost = cost->cost_pag(&pipeline_set);
        if((pag_cost < pag_cost_best)||(act_decision_value == 0))
        {
          pag_cost_best = pag_cost;
          pipeline_set_best = pipeline_set;
          act_decision_value_best = act_decision_value;

          // this is needed to repair the counter in already computed case
          decision_cnt_copy = decision_cnt;
          decision_vec_copy = decision_vec;
          decision_max_vec_copy = decision_max_vec;

          //Remember best predecessors of 1st decision and its stage
          first_decision_predecessor_stage_best = first_decision_predecessor_stage;
          first_decison_predecessor_best = first_decison_predecessor;
        }
        IF_VERBOSE(1) cout << "iteration " << i << ": decision_vec=(" << decision_vec << "), pag_cost=" << pag_cost << ", pipeline_set=" << pipeline_set << endl;
    }

    //adjust decision vector:
    if(search_limit > 0)
    {
      if(decision_vec.size() == 0) break; //trivial case, exit optimization loop
      if(decision_cnt < decision_vec.size())
      {
        decision_vec.resize(decision_cnt);
        decision_max_vec.resize(decision_cnt);
        if(act_decision_no >= decision_cnt) break; //decision vector was resized in one of the last iteration -> exit optimization loop
      }
      if((((int) act_decision_value) < search_limit) && (act_decision_value < decision_max_vec[act_decision_no]))
      {
        act_decision_value++;
      }
      else
      {
        if(act_decision_no < decision_cnt-1)
        {
          decision_vec[act_decision_no] = act_decision_value_best;
          act_decision_value_best=0;

          //move best predecessors of 1st decision into pipeline_set corresponding stage
          pipeline_begin_set[first_decision_predecessor_stage_best].insert(first_decison_predecessor_best.begin(),first_decison_predecessor_best.end());
          //TODO remove this line
          //std::cout << "pipeline_set nach insert:" << pipeline_begin_set << std::endl;

          //TODO fix this... that there is no work for no decision...
          //do
          //{
          //  act_decision_no++; //select next decision element, if this has a max decision value of zero, increment again
          //  //++fix_decision_cnt;
          //} while((act_decision_no < decision_cnt-1) && (decision_max_vec[act_decision_no]==0));
          ++act_decision_no;
          act_decision_value=0;
          ++fix_decision_cnt;
        }
        else
        {
          break; //exit optimization loop
        }
      }
      decision_vec[act_decision_no] = act_decision_value;
    }
  }
  timer.stop();
  create_rpag_output(pipeline_set_best, pag_cost_best, target_set);
  delete target_set;
  return 1;
}

template <class T>
int rpag_base<T>::optimize_single_run(const set<T> *target_fun_set, vector< set<T> > *pipeline_set, bool first_decision_break, int *first_decision_predecessor_stage, set<T> *first_decision_predecessor)
{
  //Variables, sets and maps needed during optimization
  set<T> embedded_mult_set;
  set<T> working_set;
  set<T> predecessor_set;
  set<T> best_multi_predecessor_set;
  embedded_mult_set.clear();
  working_set.clear();
  predecessor_set.clear();

  T best_single_predecessor;
  set<T> successor_set;
  set<T> realized_target_set;

  int no_of_stages_skipped=0;
  bool first_decision_done = false;

  typename set<T>::iterator set_iter;
  int s=0;

  decision_cnt = fix_decision_cnt; //reset decision counter

  int no_of_pipeline_stages = pipeline_set->size();

#ifdef EMBEDDED_MULT_FIX_ME
  if((stages_embedded_mult < 0) && (mult_wordsize > 0))
  {
    stages_embedded_mult = stages_input_pag;
    IF_VERBOSE(2) cout << "Embedded multiplier pipeline stages: " << stages_embedded_mult << endl;
  }

  if(no_of_pipeline_stages == 0)
  {
    //all elements can be realized using embedded multipliers
    embedded_mult_set = *target_fun_set;
    goto finito;
  }
#endif

  //for non-pipelined cost models, add input node(s) at every stage:
  if((cost_model == HL_MIN_AD) || (cost_model == MIN_GPC))
  {
    int min_adder_depth=0;
    for(set_iter = target_fun_set->begin(); set_iter != target_fun_set->end(); ++set_iter)
    {
      min_adder_depth = adder_depth(*set_iter);
      if(min_adder_depth > 0)
      {
        (*pipeline_set)[min_adder_depth+no_of_extra_stages-1].insert(*set_iter);
      }
    }

    //input node 1 is present in every stage:
    for(int s=0; s < no_of_pipeline_stages; s++)
    {
        for(unsigned int i=0; i < vec_t::default_elem_count; ++i)
        {
            T base;//in CMM case, this is a Vector of Dimension 1 filled with zeros
            base = 0;// it is not possible to set the elements before they are created.
            create_unit_element(base,i);
            (*pipeline_set)[s].insert(base);
        }
    }

  }
  else
  {
    (*pipeline_set)[no_of_pipeline_stages-1] = *target_fun_set;
  }

  IF_VERBOSE(2) cout << "initial pipeline_set=" << *pipeline_set << endl;

  //############################################################################################################//
  //######################################      main optimization loop   #######################################//
  //############################################################################################################//
  for(s=no_of_pipeline_stages-1; s > 0; s--)
  {
    IF_VERBOSE(2) cout << "****** realizing elements in pipeline stage " << s << " ******" << endl;

    // set (or update) the current pipeline stage for the cost model
    cost->set_pipeline_state(s);

    predecessor_set = (*pipeline_set)[s-1];
    working_set = (*pipeline_set)[s];

    IF_VERBOSE(4) cout << "predecessor_set=" << predecessor_set << endl;
    IF_VERBOSE(4) cout << "working_set=" << working_set << endl;

		//if there are target coefficients which should be realized in a lower pipeline stage than the output stage (which currently can not be configured),
    //remove corresponding elements from working set
    if(!predecessor_set.empty())
    {
      successor_set.clear();
      realized_target_set.clear();

      compute_successor_set(&predecessor_set, c_max, &successor_set, false, is_this_a_two_input_system(), ternary_sign_filter);
      set_intersection(successor_set.begin(), successor_set.end(), working_set.begin(), working_set.end(), inserter(realized_target_set, realized_target_set.begin()));

      if(!realized_target_set.empty())
      {
        IF_VERBOSE(3) cout << "**coefficient(s) " << realized_target_set << " can be already realized using p=" << predecessor_set << endl;

        for(set_iter = realized_target_set.begin(); set_iter != realized_target_set.end(); ++set_iter)
          working_set.erase(*set_iter);
      }
    }


#ifdef EMBEDDED_MULT_FIX_ME
    if(s == stages_embedded_mult-1)
    {
      //remove elements from working set that are realized by embedded multipliers
      for(set_iter = embedded_mult_set.begin(); set_iter != embedded_mult_set.end(); ++set_iter)
        working_set.erase(*set_iter);
    }
    IF_VERBOSE(3) cout << "working_set=" << working_set << endl;
    IF_VERBOSE(3) cout << "predecessor_set=" << predecessor_set << endl;

    //############################################################################################################//
    //#######################     include embedded multipliers for hybrid PMCM   #################################//
    //############################################################################################################//
    if(s == stages_embedded_mult-1)
    {
      IF_VERBOSE(3) cout << "realizing coefficients with embedded multipliers..." << endl;

      //coefficients can be realized using embedded multipliers in this stage:
      if((max_no_of_mult < 0) || (working_set.size() <= (unsigned) max_no_of_mult))
      {
        IF_VERBOSE(2) cout << "all coefficients at this stage can be realized using multipliers -> terminate" << endl;
        embedded_mult_set = working_set;
        no_of_stages_skipped = s;
        break;
      }
      else
      {
        //chose coefficients that are realized using embedded multipliers:
        int_double_map_t working_set_depth_map;
        for(set_iter = working_set.begin(); set_iter != working_set.end(); ++set_iter)
        {
          working_set_depth_map[*set_iter] = log(nonzeros(*set_iter))/log(2);
        }
        IF_VERBOSE(3) cout << "working_set_depth_map=" << working_set_depth_map << endl;

        int_double_map_t::iterator max_ad_gain_map_iter;
        while(embedded_mult_set.size() < (unsigned) max_no_of_mult)
        {
          max_ad_gain_map_iter = max_element(working_set_depth_map.begin(), working_set_depth_map.end(), int_double_map_cmp_both());
//          max_ad_gain_map_iter = max_element(working_set_depth_map.begin(), working_set_depth_map.end(), int_double_map_cmp());

          IF_VERBOSE(4) cout << "max. adder depth: " << (*max_ad_gain_map_iter).first << "," << (*max_ad_gain_map_iter).second << endl;

          embedded_mult_set.insert((*max_ad_gain_map_iter).first);
          working_set.erase((*max_ad_gain_map_iter).first);
          //set depth to -1 that embedded multiplier elements are ignored for remaining adder depth:
          working_set_depth_map[(*max_ad_gain_map_iter).first] = -1;
        }

        if(force_minimal_depth)
        {
          max_ad_gain_map_iter = max_element(working_set_depth_map.begin(), working_set_depth_map.end(), int_double_map_cmp());
          IF_VERBOSE(3) cout << "remaining adder depth for element " << (*max_ad_gain_map_iter).first << ": " << ceil((*max_ad_gain_map_iter).second) << endl;
          no_of_stages_skipped = adder_depth_constraints[s]-ceil((*max_ad_gain_map_iter).second);
          IF_VERBOSE(3) cout << "skipping " << no_of_stages_skipped << " stage(s)" << endl;

          if(no_of_stages_skipped > 0)
          {
            for(int i=0; i < no_of_pipeline_stages; i++)
            {
              if(i+no_of_stages_skipped < no_of_pipeline_stages)
              {
                (*pipeline_set)[i] = (*pipeline_set)[i+no_of_stages_skipped];
              }
              else
              {
                pipeline_set->pop_back();
              }
            }
            IF_VERBOSE(3) cout << "reduced pipeline_set=" << (*pipeline_set) << endl;
            s -= no_of_stages_skipped;
            stages_embedded_mult -= no_of_stages_skipped;
            IF_VERBOSE(3) cout << "****** pipeline stages reduced to " << s << " ******" << endl;
            if(s == 0)
            {
              IF_VERBOSE(3) cout << "remaining elements can be realized from input '1' -> terminating" << endl;
              no_of_stages_skipped = no_of_pipeline_stages;
              goto finito;
            }
          }
        }
      }
      IF_VERBOSE(3) cout << "coefficients realized with embedded multipliers: " << embedded_mult_set << endl;

      if(s+no_of_stages_skipped == no_of_pipeline_stages)
      {
        IF_VERBOSE(3) cout << "embedded multipliers at last stage realized -> continue with next lower stage" << endl;
        continue; //embedded multipliers at last stage realized -> continue with next previous stage
      }

      IF_VERBOSE(3) cout << "working_set=" << working_set << endl;
      IF_VERBOSE(3) cout << "predecessor_set=" << predecessor_set << endl;
    }
#endif

    //############################################################################################################//
    //######################################     single stage optimization loop    ###############################//
    //############################################################################################################//
    while(!working_set.empty())
    {
      //searching for best predecessors
      best_single_predecessor = get_best_single_predecessor(working_set, predecessor_set, s);

      if(exhaustive)
      {
        cerr << "error: exhaustive mode currently unsupported!" << endl;
        exit(-1);
      }
      else
      {
        if((best_single_predecessor != -1)&&(best_single_predecessor != 0))
        {
          predecessor_set.insert(best_single_predecessor);
          if(first_decision_done==false)
          {
             first_decision_done=true;
             first_decision_predecessor->insert(best_single_predecessor);
             *first_decision_predecessor_stage = (s-1);

             if(first_decision_break){return 1;} //just to get the next predecessor in a already computet case
             //TODO REmove this line
             //std::cout << "hallo false:" << s << "|" << (*first_decision_predecessor_stage) << "|" << (*first_decision_predecessor);
          }
        }
        else
        {
          IF_VERBOSE(3) cout << "no single predecessor found, searching for multiple predecessors" << endl;

          best_multi_predecessor_set.clear();
          get_best_multi_predecessor(working_set, &best_multi_predecessor_set, s);
          predecessor_set.insert(best_multi_predecessor_set.begin(),best_multi_predecessor_set.end()); //insert the multi_predecessor into the predecessor set.
          if(first_decision_done==false)
          {
             first_decision_done=true;
             first_decision_predecessor->insert(best_multi_predecessor_set.begin(),best_multi_predecessor_set.end());
             *first_decision_predecessor_stage = (s-1);

             if(first_decision_break){return 1;} //just to get the next predecessor in a already computet case
          }

        }

        successor_set.clear();
        compute_successor_set(&predecessor_set, c_max, &successor_set, false,is_this_a_two_input_system(), ternary_sign_filter);
        IF_VERBOSE(8)
        {
          std::cout << "predecessor_set = " << predecessor_set << std::endl;
          std::cout << "successor_set = " << successor_set << std::endl;
          std::cout << "working_set = " << working_set << std::endl;
        }
        realized_target_set.clear();
        set_intersection(successor_set.begin(), successor_set.end(), working_set.begin(), working_set.end(), inserter(realized_target_set, realized_target_set.begin()));

        IF_VERBOSE(3) cout << "coefficient(s) " << realized_target_set << " can be realized using p=" << predecessor_set << endl;

        for(set_iter = realized_target_set.begin(); set_iter != realized_target_set.end(); ++set_iter)
          working_set.erase(*set_iter);

        IF_VERBOSE(3) cout << "working_set=" << working_set << endl;
        IF_VERBOSE(3) cout << "predecessor_set=" << predecessor_set << endl;

        IF_VERBOSE(2)
        {
          cout << "working set size:" << working_set.size() << ", predecessor set size:" << predecessor_set.size();
          if(best_single_predecessor != -1)
          {
            cout << ", added predecessor: " << best_single_predecessor << endl;
          }
          else
          {
            cout << ", added predecessors: " << best_multi_predecessor_set << endl;
          }
        }
      }
    }
    (*pipeline_set)[s-1].insert(predecessor_set.begin(),predecessor_set.end());
    predecessor_set.clear();
    best_multi_predecessor_set.clear();

  }

#ifdef EMBEDDED_MULT_FIX_ME
	finito:
#endif //EMBEDDED_MULT_FIX_ME

#ifdef EMBEDDED_MULT_FIX_ME
  //elliminate multiplier nodes from realization:
  if(stages_embedded_mult > 0)
  {
    IF_VERBOSE(2) cout << "pipeline_set=" << (*pipeline_set) << endl;
    for(set_iter = embedded_mult_set.begin(); set_iter != embedded_mult_set.end(); ++set_iter)
    {
      (*pipeline_set)[stages_embedded_mult-1].erase(*set_iter);
    }
    IF_VERBOSE(2) cout << "reduced pipeline_set=" << (*pipeline_set) << endl;
  }

  if(!embedded_mult_set.empty())
  {
    IF_VERBOSE(2) cout << "coefficients realized by embedded multiplers: [" << embedded_mult_set << "]" << endl;
  }
#endif //EMBEDDED_MULT_FIX_ME

  //count no of adders and registers:
  int no_of_adders=0,no_of_registers=0,no_of_registered_ops=0;
  for(unsigned int s=0; s < pipeline_set->size(); s++)
  {
    for(set_iter = (*pipeline_set)[s].begin(); set_iter != (*pipeline_set)[s].end(); ++set_iter)
    {
      if(s == 0)
      {
        if(nonzeros(*set_iter)==1)
          no_of_registers++;
        else
          no_of_adders++;
      }
      else
      {
        bool element_is_mult=false;
        //is the element realized by a multiplier?
        if(s == (unsigned) stages_embedded_mult-1)
        {
          if(embedded_mult_set.find(*set_iter) != embedded_mult_set.end())
          {
            element_is_mult=true;
          }
        }
        //ignore multiplies for counting adders and registers
        if(!element_is_mult)
        {
          //seach element in previous stage
          if((*pipeline_set)[s-1].find(*set_iter) != (*pipeline_set)[s-1].end())
          {
            //element found -> count as register
            no_of_registers++;
          }
          else
          {
            no_of_adders++;
          }
        }
      }

    }
    no_of_registered_ops += (*pipeline_set)[s].size();
  }

  IF_VERBOSE(2) cout << "no of adders=" << no_of_adders << endl;
  IF_VERBOSE(2) cout << "no of registers=" << no_of_registers << endl;
  IF_VERBOSE(2) cout << "no of registered operations=" << no_of_registers + no_of_adders << endl;
  IF_VERBOSE(2) cout << "no of embedded multipliers= " << embedded_mult_set.size() << endl;
  IF_VERBOSE(2) cout << "effective adder depth / min. pipeline depth of multipliers=" << no_of_pipeline_stages-no_of_stages_skipped << endl;
  IF_VERBOSE(2) cout << "pipeline_set=" << (*pipeline_set) << endl;

  return 1;
}

template <class T>
int rpag_base<T>::create_constraints(int adder_depth_max, int target_wordsize_max)
{
  if(stages_input_pag < 0)
  {
    IF_VERBOSE(3) cout << "No stages for input PAG are given, determining the minimal necessary number of stages" << endl;

    if(mult_wordsize > 0)
    {
      //multipliers are used, the input PAG stages are determined using half the multiplier wordsize
//      stages_input_pag = log2c_64(ceil(mult_wordsize/2.0));
      stages_input_pag = log2c_64(mult_wordsize+1)-1;
    }
    else
    {
      //no multipliers are used, the input PAG stages are determined from the maximum adder depth of the target coefficients:
      stages_input_pag = adder_depth_max;
    }
  }
  IF_VERBOSE(3) cout << "Input PAG stages: " << stages_input_pag << endl;

  if(stages_output_pag < 0)
  {
    IF_VERBOSE(3) cout << "No stages for output PAG are given, determining the minimal necessary number of stages" << endl;

    if((mult_wordsize > 0) && (target_wordsize_max > mult_wordsize))
    {
      //if wordsize is halfed in each stage, ceil(log2(target_wordsize_max/mult_wordsize))+1 stages are necessary:
      stages_output_pag = ((int) ceil(log2(((double) target_wordsize_max)/mult_wordsize)));
    }
    else
    {
      //no mutliplier is used, so no output PAG is necessary:
      stages_output_pag = 0;
    }
  }
  IF_VERBOSE(3) cout << "Output PAG stages: " << stages_output_pag << endl;

  //determine the optimization plan
  if(mult_wordsize == -1)
  {
    //no multipliers are used, simply constrain the adder depth
    for(int s=0; s < stages_input_pag; s++)
    {
      wordsize_constraints.push_back(-1);
      adder_depth_constraints.push_back(s+1);
    }
  }
  else
  {
    //multipliers are used, constrain the adder depth of the input PAG and the word length of the output PAG:
    for(int s=0; s < stages_input_pag-1; s++)
    {
      wordsize_constraints.push_back(-1);
      adder_depth_constraints.push_back(s+1);
    }
    wordsize_constraints.push_back(mult_wordsize);
    adder_depth_constraints.push_back(-1);
    for(int s=0; s < stages_output_pag-1; s++)
    {
      adder_depth_constraints.push_back(-1);
      wordsize_constraints.push_back((s+2)*mult_wordsize); //!!!!
    }
    wordsize_constraints.push_back(target_wordsize_max);
    adder_depth_constraints.push_back(-1);
  }



  return 1;
}

template <class T>
int_t rpag_base<T>::get_c_max()
{
  return c_max;
}

template <class T>
int rpag_base<T>::create_rpag_output(vector< set<T> > &pipeline_set_best, double pag_cost_best, set<T> *target_set)
{
  {
    vec_t dummi;// it is nessesary to have an object to switch the seperate symbol
    dummi.set_stream_seperate_symbol(',');// the output has a "special" or different syntax
  }
  stringstream output_stream; // to be able to stream the output in file and display

  //compute adder graph:
  stringstream pag;
  list< realization_row<T> > pipelined_adder_graph;
  pipeline_set_to_adder_graph(pipeline_set_best, pipelined_adder_graph,is_this_a_two_input_system(),c_max, ternary_sign_filter);
  append_targets_to_adder_graph(pipeline_set_best, pipelined_adder_graph, *target_set);


  //for non-pipelined cost models, remove unused input nodes:
  if((cost_model == HL_MIN_AD) || (cost_model == MIN_GPC))
  {
      remove_redundant_inputs_from_adder_graph(pipelined_adder_graph, pipeline_set_best);
  }

  if(benchmark || matlab_output)
  {
    int no_of_adders=0,no_of_registers=0,no_of_registered_ops=0;
    typename set<T>::iterator set_iter;
    for(unsigned int s = 0; s < pipeline_set_best.size(); ++s)
    {
      for(set_iter = pipeline_set_best[s].begin(); set_iter != pipeline_set_best[s].end(); ++set_iter)
      {
        if(s == 0)
        {
          if(nonzeros(*set_iter) == 1)
            no_of_registers++;
          else
            no_of_adders++;
        }
        else
        {
          //seach element in previous stage
          if(pipeline_set_best[s-1].find(*set_iter) != pipeline_set_best[s-1].end())
          {
            //element found -> count as register
            no_of_registers++;
          }
          else
          {
            no_of_adders++;
          }
        }
      }
      no_of_registered_ops += pipeline_set_best[s].size();
    }

    if(benchmark)
    {
      output_stream << "no_of_pipeline_stages=\t" << pipeline_set_best.size()-1 << std::endl;
      output_stream << "no_of_registered_ops=\t" << no_of_registered_ops << std::endl;
      output_stream << "no_of_registers=\t" << no_of_registers << std::endl;
      output_stream << "no_of_adders=\t\t" << no_of_adders << std::endl;
      output_stream << "pag_cost=\t\t" << pag_cost_best << std::endl;
    }
    if(matlab_output)
    {
      output_stream << "no_of_adders{" << matlab_out_address_string << "}=" << no_of_adders <<"; ";
      output_stream << "no_of_registers{" << matlab_out_address_string << "}=" << no_of_registers <<"; ";
//      output_stream << "already_computed(" << matlab_out_address_string << ")=1; ";
      output_stream << "pipeline_set_best{" << matlab_out_address_string << "}=" << pipeline_set_best <<"; " ;
      if(show_adder_graph)
      {
          output_stream << "pipelined_adder_graph{" << matlab_out_address_string << "}=" << output_adder_graph(pipelined_adder_graph,true) << ";" << endl;
      }
      output_stream << "needed_time_in_s(" << matlab_out_address_string << ")=" << std::fixed << std::setprecision(3) << (timer.time_elapsed * 1E-6) <<"; " ;
//      output_stream << "rpag_info{" << matlab_out_address_string << "}='" << rpag_info(start_arguments,false) <<"';" ;
      output_stream << std::endl;
    }
  }

  if(!matlab_output)
  {
    int no_of_adders=0,no_of_registers=0,no_of_registered_ops=0;
    typename set<T>::iterator set_iter;

    for(unsigned int s = 1; s < pipeline_set_best.size(); ++s)
    {
//      cout << "!!!! pipeline_set_best[" << s << "]=" << pipeline_set_best[s] << endl;

      for(set_iter = pipeline_set_best[s].begin(); set_iter != pipeline_set_best[s].end(); ++set_iter)
      {
        if(s == 0)
        {
          if(nonzeros(*set_iter) == 1)
            no_of_registers++;
          else
            no_of_adders++;
        }
        else
        {
          //seach element in previous stage
          if(pipeline_set_best[s-1].find(*set_iter) != pipeline_set_best[s-1].end())
          {
            //element found -> count as register
            no_of_registers++;
          }
          else
          {
            no_of_adders++;
          }
        }
      }
      no_of_registered_ops += pipeline_set_best[s].size();
    }

    output_stream << "pipeline_set_best=" << pipeline_set_best << endl;
    if(show_adder_graph)
    {
        pag << "pipelined_adder_graph=" << output_adder_graph(pipelined_adder_graph,true) << endl;
        output_stream << pag.str();// the endl is inluded in the pag string
    }
      output_stream << "pag_cost=" << pag_cost_best << endl;
      output_stream << "no_of_pipeline_stages=" << pipeline_set_best.size()-1 << std::endl;
      output_stream << "no_of_registered_ops=" << no_of_registered_ops << std::endl;
      output_stream << "no_of_registers=" << no_of_registers << std::endl;
      output_stream << "no_of_adders=" << no_of_adders << std::endl;
      output_stream << "pag_cost=" << pag_cost_best << std::endl;

    }

//      IF_VERBOSE(0) output_stream << "best result: pag_cost=" << pag_cost_best << ", pipeline_set_best=" << pipeline_set_best << endl;

    IF_VERBOSE(0) cout << output_stream.str();

    if(file_output)
    {
      // output has to be in a file to
      std::fstream file;
      file.open(filename.c_str(),ios::out);
      if(file.is_open())
      {
        file.clear();
        file << output_stream.str();
        file.close();
      }
      else
      {
        std::cout << "ERROR: \t Not able to open output file." << std::endl;
        std::cout << "\t No output file was created." << std::endl;
        std::cout << "\t From funktion create_rpag_output" << std::endl;
      }
    }

    return 0;
}


template <class T>
void rpag_base<T>::check_constraints(int no_of_pipeline_stages)
{
  for(int s=0; s < no_of_pipeline_stages-1; s++)
  {
    IF_VERBOSE(4) cout << "checking constraints in stage " << s << endl;
    if((adder_depth_constraints[s] > 0) && (adder_depth_constraints[s+1] > 0))
    {
      IF_VERBOSE(4) cout << "checking " << adder_depth_constraints[s+1] << "<=" << adder_depth_constraints[s]+1 << endl;
      if(adder_depth_constraints[s+1] > adder_depth_constraints[s]+1)
      {
        cout << "Warning: constraints inconsistent, adder depth difference is greater than one from stage " << s << " to stage " << s+1 << endl;
      }
    }
    if((wordsize_constraints[s] > 0) && (wordsize_constraints[s+1] > 0))
    {
      IF_VERBOSE(4) cout << "checking " << wordsize_constraints[s+1] << "<=" << (wordsize_constraints[s]<<1) << endl;

      if(is_this_a_two_input_system())
      {
        if(wordsize_constraints[s+1] > (wordsize_constraints[s]<<1))
        {
          cout << "Warning: constraints inconsistent, word size is more than doubled from stage " << s << " to stage " << s+1 << endl;
        }
      }
      else
      {
        if(wordsize_constraints[s+1] > 3*(wordsize_constraints[s]))
        {
          cout << "Warning: constraints inconsistent, word size is more than a factor of three from stage " << s << " to stage " << s+1 << endl;
        }
      }
    }
    if((adder_depth_constraints[s] > 0) && (wordsize_constraints[s+1] > 0))
    {
      IF_VERBOSE(4) cout << "checking " << (log2c_64(wordsize_constraints[s+1]+1)-1) << "<=" << adder_depth_constraints[s]+1 << endl;
      if((log2c_64(wordsize_constraints[s+1]+1)-1) > adder_depth_constraints[s]+1)
      {
        cout << "Warning: there is no guarantee that a word size of " << wordsize_constraints[s+1] << " in stage " << s+1 << " can be reached with an adder depth of " << adder_depth_constraints[s] << " in stage " << s << endl;
      }
    }
  }
}

#endif // RPAG_BASE_H

