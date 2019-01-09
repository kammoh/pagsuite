/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "pagsuite/rpagtvm.h"
#include "pagsuite/fundamental.h"
#include "pagsuite/log2_64.h"
#include "pagsuite/debug.h"
#include "pagsuite/csd.h"
#include "pagsuite/compute_successor_set.h"
#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "pagsuite/log2_64.h" // adder_depth
#include "pagsuite/type_class.h"

using namespace::std;

namespace PAGSuite
{

  rpagtvm::rpagtvm()
  {}

  rpagtvm::rpagtvm(rpag_pointer *rpagp)
  {
    copy_parameters(rpagp);
  }

  rpagtvm::~rpagtvm()
  {}

  rpag_type rpagtvm::get_rpag_type()
  {
    return typ_rpagtvm;
  }

  int rpagtvm::adder_depth(vec_t x)
  {
    if (ternary_sign_filter)
    {
      return log3c_64(nonzeros_for_sign_filter(x));
    }
    else
    {
      return log3c_64(nonzeros(abs(x)));
    }
  }

  vec_t rpagtvm::get_best_single_predecessor(const vec_set_t &working_set, const vec_set_t &predecessor_set, int s)
  {
    vec_set_t::iterator set_iter;

    vec_double_map_t single_p_gain_map;
    vec_double_map_t::iterator single_p_gain_map_iter;

    vec_pair_double_map_t p_w_gain_map;
    vec_pair_double_map_t::iterator p_w_gain_map_iter;


    IF_VERBOSE(3) cout << "searching for best single predecessor ..." << endl;


    //compute 2-input adder topologies:
    compute_topology_a_predecessors_2_add(&working_set, NULL, adder_depth_constraints[s - 1], wordsize_constraints[s - 1], NULL, &p_w_gain_map);
    compute_topology_b_predecessors_2_add(&working_set, NULL, adder_depth_constraints[s - 1], wordsize_constraints[s - 1], NULL, &p_w_gain_map);
    compute_topology_c_predecessors_2_add(&working_set, &predecessor_set, NULL, adder_depth_constraints[s - 1], wordsize_constraints[s - 1], NULL, &p_w_gain_map);

    //compute 3-input adder topologies:
    compute_topology_b_predecessors_3_add(&working_set, &p_w_gain_map, adder_depth_constraints[s - 1], wordsize_constraints[s - 1], NULL);
    compute_topology_c_predecessors_3_add(&working_set, &predecessor_set, &p_w_gain_map, adder_depth_constraints[s - 1], wordsize_constraints[s - 1], NULL);

    if (use_cse)
    {
      compute_cse_predecessors_2_add(&working_set, &single_p_gain_map, adder_depth_constraints[s - 1]);
    }


    IF_VERBOSE(4) cout << "p_w_gain_map=" << p_w_gain_map << endl;


    //convert p_w_gain_map to single_p_gain_map:
    vec_pair_t key;
    vec_t p, w, p_last;
    p_last = -1;

    double gain;
    for (p_w_gain_map_iter = p_w_gain_map.begin(); p_w_gain_map_iter != p_w_gain_map.end(); ++p_w_gain_map_iter)
    {
      key = (*p_w_gain_map_iter).first;
      p = key.first;
      w = key.second;
      gain = (*p_w_gain_map_iter).second;

      if (p == p_last)
      {
        single_p_gain_map[p] = single_p_gain_map[p] + gain;
      }
      else
      {
        single_p_gain_map[p] = gain;
      }
      p_last = p;
    }

    IF_VERBOSE(4) cout << "single_p_gain_map=" << single_p_gain_map << endl;

    //remove already chosen predecessors from gain map:
    for (set_iter = predecessor_set.begin(); set_iter != predecessor_set.end(); ++set_iter)
    {
      single_p_gain_map.erase(*set_iter);
    }
    IF_VERBOSE(3) cout << "3-input gain_map=" << single_p_gain_map << endl;

    if (single_p_gain_map.empty())
    {
      vec_t dummi;
      dummi = -1;
      return dummi;
    }

    //make a random decision of the best predecessors
    int decision = get_local_decision(single_p_gain_map.size() - 1);
    IF_VERBOSE(3) cout << "rand variance decision= " << decision << endl;

    vec_t best_single_predecessor;

    //remove the best N (which is equal to 'decision') elements from set:
    for (int i = 0; i < decision; i++)
    {
      single_p_gain_map_iter = max_element(single_p_gain_map.begin(), single_p_gain_map.end(), T_double_map_nz_cmp());
      single_p_gain_map.erase((*single_p_gain_map_iter).first);
    }

    single_p_gain_map_iter = max_element(single_p_gain_map.begin(), single_p_gain_map.end(), T_double_map_nz_cmp());
    best_single_predecessor = (*single_p_gain_map_iter).first;

    IF_VERBOSE(3) cout << "max. gain 3-input single: " << best_single_predecessor << ":" << (*single_p_gain_map_iter).second << endl;
    return best_single_predecessor;
  }

  void rpagtvm::get_best_multi_predecessor(const vec_set_t &working_set, vec_set_t *best_multi_predecessor_set, int s)
  {
    IF_VERBOSE(2) cout << "searching for best single predecessor pairs & triplets ..." << endl;

    vec_set_t::iterator set_iter;

    int nz_max;

    if (adder_depth_constraints[s - 1] >= 0)
      nz_max = (int) pow(3.0, adder_depth_constraints[s - 1]);
    else
      nz_max = -1;

    IF_VERBOSE(3) cout << "optimizing for wordsize constraint=" << wordsize_constraints[s - 1] << ", max. nonzeros constraint=" << nz_max << ", adder depth constraint=" << adder_depth_constraints[s - 1] << endl;

    IF_VERBOSE(3) cout << "searching for best predecessor pairs & triplets from MSD using max. nonzeros=" << nz_max << " and max. wordsize " << wordsize_constraints[s - 1] << endl;

    vec_triplet_set_t predecessor_triplet_set;
    vec_triplet_set_t::iterator p_set_iter;
    vec_triplet_double_map_t triplet_gain_map;
    vec_set_t successor_set;
    vec_set_t realized_target_set;


    double gain, gain_max = 0;
    vec_triplet_t predecessor_triplet, predecessor_triplet_best;

    predecessor_triplet_best.first = -1;
    predecessor_triplet_best.second = -1;
    predecessor_triplet_best.third = -1;

    vec_set_t::iterator working_set_iter;

    //serch for each element from the working_set
    for (working_set_iter = working_set.begin(); working_set_iter != working_set.end(); ++working_set_iter)
    {
      vec_t w;
      w = *working_set_iter;


      //compute predecessor pairs and triplets witch can be used to create the current w
      compute_topology_d_predecessors_3_add(w, nz_max, wordsize_constraints[s - 1], &predecessor_triplet_set);

      //compute the gain for all predecessor triplet's
      for (p_set_iter = predecessor_triplet_set.begin(); p_set_iter != predecessor_triplet_set.end(); ++p_set_iter)
      {
        predecessor_triplet = (*p_set_iter);

        if (msd_successor_evaluation)
        {
          if (triplet_gain_map.find(predecessor_triplet) == triplet_gain_map.end())
          {
            successor_set.clear();
            compute_successor_set(predecessor_triplet.first, predecessor_triplet.second, predecessor_triplet.third, c_max, &successor_set, false, ternary_sign_filter);

            realized_target_set.clear();
            set_intersection(successor_set.begin(), successor_set.end(), working_set.begin(), working_set.end(), inserter(realized_target_set, realized_target_set.begin()));

            for (set_iter = realized_target_set.begin(); set_iter != realized_target_set.end(); ++set_iter)
            {
              //if 2 are equal the gain have to be different
              if ((predecessor_triplet.first == 0) ||
                  (predecessor_triplet.first == predecessor_triplet.second) ||
                  (predecessor_triplet.second == predecessor_triplet.third) ||
                  (predecessor_triplet.third == predecessor_triplet.first))
              {
                gain = triplet_gain_map[predecessor_triplet] + 1.0 / (2 * cost->cost_ternary_add(*set_iter, predecessor_triplet.first, predecessor_triplet.second, predecessor_triplet.third));
              }
              else
              {
                gain = triplet_gain_map[predecessor_triplet] + 1.0 / (3 * cost->cost_ternary_add(*set_iter, predecessor_triplet.first, predecessor_triplet.second, predecessor_triplet.third));
              }
              triplet_gain_map[predecessor_triplet] = gain;
            }
          }
        }
        else
        {
          //if 2 are equal the gain have to be different
          if ((predecessor_triplet.first == 0) ||
              (predecessor_triplet.first == predecessor_triplet.second) ||
              (predecessor_triplet.second == predecessor_triplet.third) ||
              (predecessor_triplet.third == predecessor_triplet.first))
          {
            gain = triplet_gain_map[predecessor_triplet] + 1.0 / (2 * cost->cost_ternary_add(*set_iter, predecessor_triplet.first, predecessor_triplet.second, predecessor_triplet.third));
          }
          else
          {
            gain = triplet_gain_map[predecessor_triplet] + 1.0 / (3 * cost->cost_ternary_add(*set_iter, predecessor_triplet.first, predecessor_triplet.second, predecessor_triplet.third));
          }
          triplet_gain_map[predecessor_triplet] = gain;
        }
      }
      predecessor_triplet_set.clear();
    }

    IF_VERBOSE(3) cout << "triplet gain map=" << triplet_gain_map << endl;


    //select best predecessor triplet:
    int decision = get_local_decision(triplet_gain_map.size() - 1);
    IF_VERBOSE(2) cout << "rand variance decision= " << decision << endl;

    vec_triplet_double_map_t::iterator triplet_gain_map_iter;

    //remove the best N (which is equal to 'decision') elements from set:
    for (int i = 0; i < decision; i++)
    {
      triplet_gain_map_iter = max_element(triplet_gain_map.begin(), triplet_gain_map.end(), T_pair_or_tripl_double_map_nz_cmp());
      triplet_gain_map.erase((*triplet_gain_map_iter).first);
    }
    triplet_gain_map_iter = max_element(triplet_gain_map.begin(), triplet_gain_map.end(), T_pair_or_tripl_double_map_nz_cmp());
    predecessor_triplet_best = (*triplet_gain_map_iter).first;
    gain_max = (*triplet_gain_map_iter).second;

    IF_VERBOSE(2) cout << "best predecessor triplet: " << predecessor_triplet_best << " with gain " << gain_max << endl;

    if ((predecessor_triplet_best.first == 0) && (predecessor_triplet_best.second == 0) && (predecessor_triplet_best.third == 0))
    {
      cerr << "Error: no predecessors were found (maybe optimization plan is too tough?)" << endl;
      flush(cerr);
      exit(-1);
    }

    //if the triplet is a pair the 0 must not be inserted!!
    if (predecessor_triplet_best.first != 0) best_multi_predecessor_set->insert(predecessor_triplet_best.first);
    best_multi_predecessor_set->insert(predecessor_triplet_best.second);
    best_multi_predecessor_set->insert(predecessor_triplet_best.third);

  }

}