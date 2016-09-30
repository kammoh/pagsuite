/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "rpag.h"
#include "fundamental.h"
#include "log2_64.h"
#include "debug.h"
#include "csd.h"
#include "compute_successor_set.h"
#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include"rpag_topologies.h"

namespace rpag {

rpag::rpag()
{}

rpag::~rpag()
{}

rpag_type rpag::get_rpag_type()
{
    return typ_rpag;
}

int_t rpag::get_best_single_predecessor(const int_set_t &working_set, const int_set_t &predecessor_set, int s)
{
  int_set_t::iterator set_iter;
  int_set_t p_set_a,p_set_b,p_set_c,p_set_d;
  int_double_map_t single_p_gain_map;

  IF_VERBOSE(3) cout << "searching for best single predecessor ..." << endl;
  IF_VERBOSE(5)
  {
    compute_topology_a_predecessors_2_add(&working_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1], &p_set_a,NULL);
    cout << "p_set_a=" << p_set_a << endl;

    compute_topology_b_predecessors_2_add(&working_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1],&p_set_b,NULL);
    cout << "p_set_b=" << p_set_b << endl;

    compute_topology_c_predecessors_2_add(&working_set, &predecessor_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1],&p_set_c,NULL);
    IF_VERBOSE(5) cout << "p_set_c=" << p_set_c << endl;

  }
  else
  {     
      compute_topology_a_predecessors_2_add(&working_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1],NULL,NULL);
      compute_topology_b_predecessors_2_add(&working_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1],NULL,NULL);
      compute_topology_c_predecessors_2_add(&working_set, &predecessor_set, &single_p_gain_map, adder_depth_constraints[s-1], wordsize_constraints[s-1],NULL,NULL);
  }

  //remove already chosen predecessors from gain map:
  for(set_iter = predecessor_set.begin(); set_iter != predecessor_set.end(); ++set_iter)
  {
    single_p_gain_map.erase(*set_iter);
  }
  IF_VERBOSE(3) cout << "gain_map=" << single_p_gain_map << endl;

  if(single_p_gain_map.empty())
    return -1;

  int_t best_single_predecessor;
  int_double_map_t::iterator single_p_gain_map_iter;

  int decision = get_local_decision(single_p_gain_map.size()-1);
  IF_VERBOSE(3) cout << "rand variance decision= " << decision << endl;

  //remove the best N (which is equal to 'decision') elements from set:
  for(int i=0; i < decision; i++)
  {
    single_p_gain_map_iter = max_element(single_p_gain_map.begin(), single_p_gain_map.end(), T_double_map_nz_cmp());
    single_p_gain_map.erase((*single_p_gain_map_iter).first);
  }
  single_p_gain_map_iter = max_element(single_p_gain_map.begin(), single_p_gain_map.end(), T_double_map_nz_cmp());
  best_single_predecessor = (*single_p_gain_map_iter).first;

  IF_VERBOSE(5) cout << "best single gain map: " << (*single_p_gain_map_iter).first << "," << (*single_p_gain_map_iter).second << endl;

  IF_VERBOSE(3) cout << "choosing best predecessor to " << best_single_predecessor << endl;
  return best_single_predecessor;
}

void  rpag::get_best_multi_predecessor(const int_set_t &working_set, int_set_t *best_multi_predecessor_set, int s)
{
  IF_VERBOSE(3) cout << "searching for best single predecessor pairs ..." << endl;

  int nz_max;

  if(adder_depth_constraints[s-1] >= 0)
    nz_max = 1 << adder_depth_constraints[s-1];
  else
    nz_max = -1;

  IF_VERBOSE(3) cout << "optimizing for wordsize constraint=" << wordsize_constraints[s-1] << ", max. nonzeros constraint=" << nz_max << ", adder depth constraint=" << adder_depth_constraints[s-1] << endl;


  int_pair_set_t predecessor_pair_set;
//  set<pair<int_t,int_t> > predecessor_pair_set;
  int_pair_double_map_t pair_gain_map;

#if USE_TOPOLOGY_E
  //2:2 topology (e)
  if(working_set.size() > 1)
  {
    IF_VERBOSE(3) cout << "searching for best predecessor pair from from topology e using max. nonzeros=" << nz_max << " and max. wordsize " << wordsize_constraints[s-1] << endl;
    int_set_t::iterator w_set_iter1,w_set_iter2;
    for(w_set_iter1 = working_set.begin(); w_set_iter1 != working_set.end(); ++w_set_iter1)
    {
      int_t w1 = *w_set_iter1;

      w_set_iter2 = w_set_iter1;
      ++w_set_iter2;
      while(w_set_iter2 != working_set.end())
      {
        int_t w2 = *w_set_iter2;

        IF_VERBOSE(4) cout << "*** evaluating working set elements w1=" << w1 << " and w2=" << w2 << endl;
        int l_max=log2c_64(max(w1,w2))+1;

        compute_topology_e_predecessors_2_add(w1,w2,l_max,nz_max,wordsize_constraints[s-1], &predecessor_pair_set);
        compute_topology_e_predecessors_2_add(w2,w1,l_max,nz_max,wordsize_constraints[s-1], &predecessor_pair_set);

        IF_VERBOSE(3) cout << "predecessor_pair_set=" << predecessor_pair_set << endl;

        assert(msd_successor_evaluation);//update_pair_gain_map is not implemented without msd_successor_evaluation as w is unknown
        update_pair_gain_map(predecessor_pair_set, 1, working_set, pair_gain_map);
        predecessor_pair_set.clear();

        ++w_set_iter2;
      }
    }
  }
#endif

  if(pair_gain_map.empty())
  {
    int_set_t::iterator w_set_iter;
    IF_VERBOSE(3) cout << "searching for best predecessor pair from MSD using max. nonzeros=" << nz_max << " and max. wordsize " << wordsize_constraints[s-1] << endl;
    for(w_set_iter = working_set.begin(); w_set_iter != working_set.end(); ++w_set_iter)
    {
      int_t w=*w_set_iter;

      compute_topology_d_predecessors_2_add(w, nz_max, wordsize_constraints[s-1], &predecessor_pair_set);
      update_pair_gain_map(predecessor_pair_set, w, working_set, pair_gain_map);

      predecessor_pair_set.clear();
    }
  }
  else
  {
    IF_VERBOSE(3) cout << "!!! predecessor pair from from topology e found !!!" << endl;
  }
  IF_VERBOSE(3) cout << "predecessor pair gain map=" << pair_gain_map << endl;

  //select best predecessor pair:
  double gain_max=0;
  int_pair_t predecessor_pair_best;
  int decision = get_local_decision(pair_gain_map.size()-1);
  IF_VERBOSE(4) cout << "rand variance decision= " << decision << endl;

  int_pair_double_map_t::iterator pair_gain_map_iter;

  //remove the best N (which is equal to 'decision') elements from set:
  for(int i=0; i < decision; i++)
  {
    pair_gain_map_iter = max_element(pair_gain_map.begin(), pair_gain_map.end(), T_pair_or_tripl_double_map_nz_cmp());
    pair_gain_map.erase((*pair_gain_map_iter).first);
  }
  pair_gain_map_iter = max_element(pair_gain_map.begin(), pair_gain_map.end(), T_pair_or_tripl_double_map_nz_cmp());
  predecessor_pair_best = (*pair_gain_map_iter).first;
  gain_max = (*pair_gain_map_iter).second;

  IF_VERBOSE(3) cout << "best predecessor pair: " << predecessor_pair_best << " with gain " << gain_max << endl;

  if((predecessor_pair_best.first == 0) && (predecessor_pair_best.second == 0))
  {
    cerr << "Error: no predecessors were found (maybe optimization plan is too tough?)" << endl;
    flush(cerr);
    exit(-1);
  }
  best_multi_predecessor_set->insert(predecessor_pair_best.first);
  best_multi_predecessor_set->insert(predecessor_pair_best.second);

}


void rpag::update_pair_gain_map(set<pair<int_t,int_t> > &predecessor_pair_set, int_t w, const int_set_t &working_set, int_pair_double_map_t &pair_gain_map)
{
  int_pair_set_t::iterator p_set_iter;
  int_set_t successor_set;
  int_set_t realized_target_set;
  double gain;
  int_pair_t predecessor_pair;
  int_set_t::iterator set_iter;

  for(p_set_iter=predecessor_pair_set.begin(); p_set_iter != predecessor_pair_set.end(); ++p_set_iter)
  {
    predecessor_pair = *p_set_iter;

    if(msd_successor_evaluation)
    {
      if(pair_gain_map.find(predecessor_pair) == pair_gain_map.end())
      {
        successor_set.clear();
        compute_successor_set(predecessor_pair.first, predecessor_pair.second, c_max, &successor_set, false);

        realized_target_set.clear();
        set_intersection(successor_set.begin(), successor_set.end(), working_set.begin(), working_set.end(), inserter(realized_target_set, realized_target_set.begin()));

        for(set_iter = realized_target_set.begin(); set_iter != realized_target_set.end(); ++set_iter)
        {
          gain = pair_gain_map[predecessor_pair] + 1.0 / (cost->cost_add(*set_iter,predecessor_pair.first,predecessor_pair.second,2,c_max));
          pair_gain_map[predecessor_pair] = gain;
        }
      }
    }
    else
    {
      gain = pair_gain_map[predecessor_pair] + 1.0 / (cost->cost_add(w,predecessor_pair.first,predecessor_pair.second,2,c_max));
      pair_gain_map[predecessor_pair] = gain;
    }
  }
}

int rpag::adder_depth(int_t x)
{
  assert(is_this_a_two_input_system());
  return log2c_64(nonzeros(x));
}

}
