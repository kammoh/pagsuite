/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAG_TOPOLOGIES_H
#define RPAG_TOPOLOGIES_H

#include "type_class.h"
#include "rpag_base.h"
#include "debug.h"
#include "csd.h"
#include "cost_model.h"
#include "rpag_functions.h"

int insert_into_predecessor_pair_set(set<pair<vec_t,vec_t> > *predecessor_pair_set, vec_pair_t in);
int insert_into_predecessor_pair_set(set<pair<int_t,int_t> > *predecessor_pair_set, vec_pair_t in);
int insert_into_predecessor_triplet_set(set<triplet<vec_t,vec_t,vec_t> > *predecessor_triplet_set, vec_triplet_t in);
int insert_into_predecessor_triplet_set(set<triplet<int_t,int_t,int_t> > *predecessor_triplet_set, vec_triplet_t in);


template<typename T>// input wirdsize have to be included in the parameter set...
void rpag_base<T>::compute_topology_a_predecessors_2_add(const set<T> *working_set, map<T,double> *single_p_gain_map, int s_max, int ws_max, set<T> *p_set_a, map<pair<T,T>,double> *p_w_gain_map)
{
  IF_VERBOSE(4) cout << "compute 2-input predecessors for topology a (register)" << endl;
  typename set<T>::iterator set_iter;
  for(set_iter = working_set->begin(); set_iter != working_set->end(); ++set_iter)
  {
    T w = *set_iter;
    if(((s_max < 0) || (adder_depth(w) <= s_max)) && ((ws_max < 0) || (compute_word_size(w,input_wordsize) <= ws_max)))
    {
      IF_VERBOSE(4) cout << "**case 2a met for c=" << *set_iter << " (p'=" << *set_iter << ")" << endl;
      if(p_set_a != NULL)
        p_set_a->insert(w);

      if(single_p_gain_map != NULL)
        (*single_p_gain_map)[w] = (*single_p_gain_map)[w] + 1/cost->cost_reg(w);

      if(p_w_gain_map != NULL)
        (*p_w_gain_map)[pair<T,T>(w,w)] = max((*p_w_gain_map)[pair<T,T>(w,w)],1/cost->cost_reg(w));
    }
  }
}

template<typename T>
void rpag_base<T>::compute_topology_b_predecessors_2_add(const set<T> *working_set, map<T,double> *single_p_gain_map, int s_max, int ws_max,  set<T> *p_set_b, map<pair<T,T>,double> *p_w_gain_map)
{
  IF_VERBOSE(4) cout << "compute 2-input predecessors for topology b" << endl;
  flush(cout);
  typename set<T>::iterator set_iter;
  for(set_iter = working_set->begin(); set_iter != working_set->end(); ++set_iter)
  {
    int k=2;
    int_t div=3;
    T quot;
    T w=*set_iter;
    while(abs(w) >= div)
    {
      div=(1LL << k)-1; //=2^k-1
      if((w % div) == 0)
      {
        quot=w/div;
        if(((s_max < 0) || (adder_depth(quot) <= s_max)) && ((ws_max < 0) || (compute_word_size(quot,0) <= ws_max)))
        {
          IF_VERBOSE(4) cout << "**case 2b met for w=" << w << " (p'=" << quot << ")" << endl;

          if(p_set_b != NULL)
            p_set_b->insert(quot);

          if(single_p_gain_map != NULL)
            (*single_p_gain_map)[quot] = (*single_p_gain_map)[quot] + 1/cost->cost_add(w,quot,quot,1);

          if(p_w_gain_map != NULL)
            (*p_w_gain_map)[pair<T,T>(quot,w)] = max((*p_w_gain_map)[pair<T,T>(quot,w)],1/cost->cost_add(w,quot,quot,1));
        }
      }
      div += 2; //=2^k+1
      if((w % div) == 0)
      {
        quot=w/div;
        if(((s_max < 0) || (adder_depth(quot) <= s_max)) && ((ws_max < 0) || (compute_word_size(quot,0) <= ws_max)))
        {
          IF_VERBOSE(4) cout << "**case 2b met for w=" << w << " (p'=" << quot << ")" << endl;

          if(p_set_b != NULL)
            p_set_b->insert(quot);

          if(single_p_gain_map != NULL)
            (*single_p_gain_map)[quot] = (*single_p_gain_map)[quot] + 1/cost->cost_add(w,quot,quot,1);

          if(p_w_gain_map != NULL)
            (*p_w_gain_map)[pair<T,T>(quot,w)] = max((*p_w_gain_map)[pair<T,T>(quot,w)],1/cost->cost_add(w,quot,quot,1));
        }
      }
      k++;
    }
  }
}

template<typename T>
void rpag_base<T>::compute_topology_c_predecessors_2_add(const set<T> *working_set, const set<T> *predecessor_set, map<T,double> *single_p_gain_map, int s_max, int ws_max, set<T> *p_set_c, map<pair<T,T>,double> *p_w_gain_map)
{
  IF_VERBOSE(4) cout << "compute 2-input predecessors for topology c" << endl;
  if(!predecessor_set->empty())
  {
    typename set<T>::iterator working_set_iter;
    typename set<T>::iterator potential_set_iter;
    typename set<T>::iterator predecessor_set_iter;

    set<T> predecessor_ignore_set; //set for predecessors that have to be ignored to avoid double counting predecessors of the same element in working set

    set<T> potential_predecessors;
    for(working_set_iter = working_set->begin(); working_set_iter != working_set->end(); ++working_set_iter)
    {
      T w = *working_set_iter;
      predecessor_ignore_set.clear();

      for(predecessor_set_iter = predecessor_set->begin(); predecessor_set_iter != predecessor_set->end(); ++predecessor_set_iter)
      {
        T p_realized = *predecessor_set_iter;
        compute_successor_set(w, p_realized, c_max, &potential_predecessors);

        for(potential_set_iter = potential_predecessors.begin(); potential_set_iter != potential_predecessors.end(); ++potential_set_iter)
        {
          T p = *potential_set_iter;
          if(((s_max < 0) || (adder_depth(p) <= s_max)) && ((ws_max < 0) || (compute_word_size(w,input_wordsize) <= ws_max)))
          {
            IF_VERBOSE(4) cout << "**case 2c met for w=" << w << " (p=" << p << ", p'=" << p_realized << ")" << endl;

            if(p_set_c != NULL)
              p_set_c->insert(norm(p));

            if(single_p_gain_map != NULL)
            {
              if(predecessor_ignore_set.find(p) == predecessor_ignore_set.end())
              {
                (*single_p_gain_map)[p] = (*single_p_gain_map)[p] + 1/cost->cost_add(w,p,p_realized,1,c_max);
                predecessor_ignore_set.insert(norm(p));
              }
              else
              {
                IF_VERBOSE(5) cout << "-> ignored for gain" << endl;
              }
            }

            if(p_w_gain_map != NULL)
              (*p_w_gain_map)[pair<T,T>(p,w)] = max((*p_w_gain_map)[pair<T,T>(p,w)],1/cost->cost_add(w,p,p_realized,1,c_max));

          }
        }
        potential_predecessors.clear();
      }
    }

  }
}

template<typename T>
void rpag_base<T>::compute_topology_d_predecessors_2_add(T x, int nz_max, int ws_max, set<pair<T,T> > *predecessor_pair_set)
{
  int_t iteration_counter=0;
  IF_VERBOSE(4) cout << "compute 2-input predecessors for topology d (pairs)" << endl;

  sd_vec_set_t msd_vec_set;

  int ws = compute_word_size(x,0);
  // old version int ws = log2c_64(abs(x).max());

  //if max. nonzeros are unconstrained, use wordsize-1
  if(nz_max < 0)
  {
    nz_max=ws-1;
  }

  IF_VERBOSE(4) cout << "max. nonzeros:" << nz_max << endl;
  IF_VERBOSE(4) cout << "max. wordsize:" << ws_max << endl;
  IF_VERBOSE(4) cout << "ws(" << x << ") = " << ws << endl;

  vec_t dummi; // this have to be two lines because initalisation of a vector with not a vector will be interpreted as the number of dimentions. vec_t dummi = 5; => (0;0;0;0;0) vec_t dummi; dummi=5; => (5)
  dummi = x; // if x is an int_t then the default size of vec_t was switched to 1 in the beginning of the optimize() funktion. So dummi is a vector with just x as element.

  vec2msd_vec(dummi, &msd_vec_set, msd_representation_limit);
  //IF_VERBOSE(6) cout << "msd_set(" << x << "):" << endl << msd_vec_set << endl;

  sd_vec_set_t::iterator msd_vec_set_iter;

  for(msd_vec_set_iter = msd_vec_set.begin(); msd_vec_set_iter != msd_vec_set.end(); ++msd_vec_set_iter)
  {
    sd_vec_t msd_vec = *msd_vec_set_iter;
    int nz = nonzeros(msd_vec); //!!!!

    //IF_VERBOSE(6) cout << "msd(" << x << ")=" << msd << ", nz=" << nz << endl;

    vector<int> nonzero_values(nz);         //vector that stores signs of nonzero values
    vector<int_t> nonzero_shift_factor(nz); //vector that stores the shift weight (2^"shift factor")
    vector<int> nonzero_element(nz);        //vector that stores the element number that it belongs to

    //fill nonzero_values and nonzero_shift_factor with data:
    vector<int>::iterator int_vector_iter;
    int i=0;
    int_t shift_factor=0;
    for(unsigned j = 0; j < msd_vec.size(); ++j)
    {
      shift_factor=0;//for each element of the vector it starts witch a new number
      for (int_vector_iter=msd_vec[j].begin(); int_vector_iter != msd_vec[j].end(); ++int_vector_iter)
      {
        if(*int_vector_iter != 0)
        {
          nonzero_values[i]=*int_vector_iter;
          nonzero_shift_factor[i]=shift_factor;
          nonzero_element[i]=j;
          i++;
        }
        shift_factor++;
      }
    }
    IF_VERBOSE(5) cout << "nonzero_values=" << nonzero_values << endl;
    IF_VERBOSE(5) cout << "nonzero_shift_factor=" << nonzero_shift_factor << endl;
    IF_VERBOSE(5) cout << "nonzero_element=" << nonzero_element << endl;

    //iterate for different nonzero count for the left side predecessor p1 (the right side predecessor p2 has size nz-nz_p1):
    int nz_p2;

    for(int nz_p1=nz_max; nz_p1 > 0; nz_p1--)
    {
      if(nz_p1 >= nz)
        continue;

      nz_p2=nz-nz_p1;
      if(nz_p2 > nz_p1) //!!!
        break;

      IF_VERBOSE(5) cout << "nz_p1=" << nz_p1 << ", nz_p2=" << nz_p2 << endl;
      vector<int> left_nonzero_indices(nz_p1); //vector left_nonzeros is used to address the nonzeros which are used for one part of the pair

      //init left_nonzero_indices:
      for(int i=0; i < nz_p1; i++)
      {
        left_nonzero_indices[i] = i;
      }

      //permutate left_nonzero_indices:
      vec_t p1,p2;
      int ws_p1=-1,ws_p2=-1;  //word size of p1 and p2
      int index;
      while((msd_digit_permutation_limit == -1) || (iteration_counter < msd_digit_permutation_limit)) //do permutation: (if(index < 0) break;)
      {
        index=nz_p1-1;
        p1=0;
        //evaluate nonzero_values:
        for(int i=0; i < nz_p1; i++)
        {
          p1[nonzero_element[left_nonzero_indices[i]]] += ((int_t) nonzero_values[left_nonzero_indices[i]]) << nonzero_shift_factor[left_nonzero_indices[i]];
        }

        int min_index_p2,max_index_p2;

        if(ws_max > 0)
        {
          std::cout << "Error wordsize constraint is not soportet for vectors"<< std::endl;
          std::cout << "rpagcm: topologie d: line 462";
          exit(-1);
          //############################################FIX ME#######################################
          // die nonzeros werden ohne abhaengigkeit zum element ueberprueft
          //#########################################################################################
          //detect wordsize of p1 and p2:

          IF_VERBOSE(6) cout << "left_nonzero_indices=" << left_nonzero_indices << endl;

          int min_index_p1,max_index_p1;
          min_index_p1=left_nonzero_indices[0];
          max_index_p1=left_nonzero_indices[nz_p1-1];

          //wordsize of p1 can be directly determined:
          ws_p1 = nonzero_shift_factor[max_index_p1]-nonzero_shift_factor[min_index_p1]+1;
          IF_VERBOSE(6) cout << "min. index p1=" << left_nonzero_indices[0] << " max. index p1=" << left_nonzero_indices[nz_p1-1] << " wordsize p1=" << ws_p1 << endl;

          // if wordsize constraint of p1 is met, check p2:
          if(ws_p1 <= ws_max)
          {
            int i;
            //to find the first index of p2, search for the first occurrence of an index step by 2:
            for(i=0; i < nz_p1; i++)
            {
              if(i != left_nonzero_indices[i])
                break;
            }
            min_index_p2 = i; //min index is step position

            //to find the last index of p2, search backwards for the first occurrence of an index step by -2:
            int j=0;
            for(i=nz_p1-1; i >= 0; i--, j++)
            {
              if(left_nonzero_indices[i] != nz-1-j)
                break;
            }
            max_index_p2 = nz-1-j;
            ws_p2 = nonzero_shift_factor[max_index_p2]-nonzero_shift_factor[min_index_p2]+1;
            IF_VERBOSE(6) cout << "min. index p2=" << min_index_p2 << " max. index p2=" << max_index_p2 << " wordsize p2=" << ws_p2 << endl;
          }
        }

        //for constrained wordsize, check wordsize for p1 and p2 before computing them, otherwise continue with permutation:
        if((ws_max < 0) || ((ws_p1 <= ws_max) && (ws_p2 <= ws_max)))
        {
          p1 = p1;
          p2 = x - p1;
          IF_VERBOSE(7) cout << "left_nonzero_indices=" << left_nonzero_indices << ": (" << x << "=" << p1 << "+" << p2 << ")";

          p1 = fundamental(norm(p1));
          p2 = fundamental(norm(p2));

          IF_VERBOSE(5) cout  << " fun pair: (" << p1 << "," << p2 << ")" << endl;

          if(p2 < p1) //swap elements such that p1 <= p2
          {
            swap(p1,p2);
          }
          if((abs(p1) <= c_max) && (abs(p2) <= c_max))// to filter the results witch are bigger then c_max
          {
            IF_VERBOSE(5) cout  << " fun pair: (" << p1 << "," << p2 << ")" << endl;

            //predecessor_pair_set->insert(vec_pair_t(p1,p2));
            insert_into_predecessor_pair_set(predecessor_pair_set, vec_pair_t(p1,p2));// to be able to insert the Vectors in a int_t based predecessor_pair_set.(in case of MCM)
          }
        }

        ++iteration_counter;
        //do permutation:
        while(left_nonzero_indices[index] >= nz-1-(nz_p1-1-index))
        {
          index--;
          if(index < 0)
            break;
        }
        if(index < 0)
          break;

        if(left_nonzero_indices[index] < nz-1-(nz_p1-1-index))
          left_nonzero_indices[index]++;

        for(;index < nz_p1-1;index++)
        {
          if(left_nonzero_indices[index] < nz-1)
            left_nonzero_indices[index+1] = left_nonzero_indices[index]+1;
        }
      }
    }
  }
  if(iteration_counter == msd_digit_permutation_limit)
  {
    IF_VERBOSE(3) cout << "Attention: msd_cutter_iteration_max has stoped topology d" << endl;
  }
  IF_VERBOSE(4) cout << "end by iteration=" << iteration_counter << endl;
  IF_VERBOSE(4) cout << "predecessor_pair_set=" << *predecessor_pair_set << endl;
}


template<typename T>
void rpag_base<T>::explore_cse(vector<vec_t> &working_vec, vec_t currentCSE, int baseIndex, int endIndex, int frequency, int cseSizeMin, int cseSizeMax, string indexCombinations, map<T,double> *single_p_gain_map)
{
  cout << "explore_cse with currentCSE=" << currentCSE << ", baseIndex=" << baseIndex << ", frequency=" << frequency << endl;
  frequency++;
  vec_t nextCSE(currentCSE.size());
  for(int currentIndex=baseIndex+1; currentIndex <= endIndex; currentIndex++)
  {
    vec_t &w = working_vec[currentIndex];
    int w_ns = nonzeros(w);
    cseSizeMin=max(w_ns-cseSizeMax,cseSizeMin);

    for(int sign=-1; sign <= 1; sign+=2)
    {
      bool allNegative=true; //flag that indicated if all the elements in a CSE vector are negative. This can not be realized and has to be filtered out
      for(int i=0; i < currentCSE.size(); i++)
      {
        if((w[i] != 0) && (w[i] == sign * currentCSE[i]))
        {
          nextCSE[i] = w[i];
          if(w[i] > 0) allNegative=false;
        }
        else
        {
          nextCSE[i] = 0;
        }
      }
      nextCSE = fundamental(norm(nextCSE));
      int cseSize = nonzeros(nextCSE);
      if((cseSize >= cseSizeMin) && (cseSize <= cseSizeMax) && !allNegative) //!!!
      {
        IF_VERBOSE(4) cout << "found CSE pattern for working set elements " << indexCombinations << "," << (sign > 0 ? '+' : '-') << to_string(currentIndex) << " of size " << cseSize << " (" << cseSizeMin << "<=" << cseSize << "<=" << cseSizeMax << ") with frequency " << frequency << ": " << nextCSE << endl;

        if(single_p_gain_map != NULL)
        {
          double gain = ((double) frequency) / (((double) frequency)+1.0); //we can realize f working set elements by implementing the CSE + f additional predecessors
//          gain = gain*((double) cseSize)/((double) cseSizeMin); //the larger the pattern size the better, this changes the gain by a factor of 1 (when cseSize==cseMin) to 2 (when cseSize==2*cseMin)
          (*single_p_gain_map).at()
          (*single_p_gain_map)[nextCSE] = gain;
        }

        if(currentIndex < endIndex)
        {
          //start next recursion:
          explore_cse(working_vec, nextCSE, currentIndex, endIndex, frequency,cseSizeMin,cseSizeMax,indexCombinations + ","  + (sign > 0 ? "+" : "-") + to_string(currentIndex),single_p_gain_map);
        }
      }
    }
  }
}

template<typename T>
void rpag_base<T>::compute_cse_predecessors_2_add(const vec_set_t *working_set, map<T,double> *single_p_gain_map, int max_adder_depth)
{
  int cseSizeMax=1<<max_adder_depth;

  IF_VERBOSE(3) cout << "searching for common subexpressions in working set " << *working_set << endl;
  IF_VERBOSE(4) cout << "maximum CSE size is " << cseSizeMax << endl;

  int no_of_working_set_elements = working_set->size();
  vector<vec_t> working_vec(no_of_working_set_elements);

  int i=0;
  for(vec_t w : *working_set)
  {
    int w_ns = nonzeros(w);
    IF_VERBOSE(4) cout << "working set element " << i << " is " << w << " and has " << w_ns << " non-zeros" << endl;
    working_vec[i++] = w;
  }

  for(int i=0; i < no_of_working_set_elements; i++)
  {
    vec_t cse = working_vec[i]; //take each target coefficient as CSE seed with a frequency of 1
    int cse_ns = nonzeros(cse);
    int cseSizeMin=max(cse_ns-cseSizeMax,1);
    explore_cse(working_vec, cse, i, no_of_working_set_elements-1, 1, cseSizeMin, cseSizeMax, "+" + to_string(i),single_p_gain_map);
  }

  IF_VERBOSE(3) cout << "gain_map for CSE=" << *single_p_gain_map << endl;

//  exit(-1); //!!!
}


template<typename T>
void rpag_base<T>::compute_topology_b_predecessors_3_add(const set<T> *working_set, map<pair<T,T>, double> *p_w_gain_map, int s_max, int ws_max, set<T> *p_set_b)
{
  IF_VERBOSE(4) cout << "compute 3-input vector predecessors for topology a" << endl;
  set<int_t>::iterator set_int_t_iter;
  typename set<T>::iterator set_iter;

  set<int_t> possible_divider_set;

  for(set_iter = working_set->begin(); set_iter != working_set->end(); ++set_iter)
  {
    T w = *set_iter; //element from working_set
    int_t div = 3; // ,quot;
    int k = 3; //shift factor 1
    int l = 2; //shift factor 2

    while(w*2 >= div)// k loop
    {
      while((w*2 >= div) && (l < k))// l loop // l=k are elements whitch are olready found by tpologie_b_2add with 2^(l+k)+1 2^(l+k)-1
      {
        //2^k-2^l-1
        if(!ternary_sign_filter)
        {
          div = (1ll << k) - (1ll << l) - 1;
          IF_VERBOSE(5) cout << "div=2^" << k << " - 2^" << l << " - 1 = " << div << endl;
          if(div != 0) if((w % div) == 0) if(div != 1) possible_divider_set.insert(div);
        }
        //2^k+2^l-1
        div = (1ll << k) + (1ll << l) - 1;
        IF_VERBOSE(5) cout << "div=2^" << k << " + 2^" << l << " - 1 = " << div << endl;
        if(div != 0) if((w % div) == 0) if(div != 1) possible_divider_set.insert(div);

        //2^k-2^l+1
        div = (1ll << k) - (1ll << l) + 1;
        IF_VERBOSE(5) cout << "div=2^" << k << " - 2^" << l << " + 1 = " << div << endl;
        if(div != 0) if((w % div) == 0) if(div != 1) possible_divider_set.insert(div);

        //2^k+2^l+1
        div = (1ll << k) + (1ll << l) + 1;
        IF_VERBOSE(5) cout << "div=2^" << k << " + 2^" << l << " + 1 = " << div << endl;
        if(div != 0) if((w % div) == 0) if(div != 1) possible_divider_set.insert(div);

        l++;
      }
      l=2;
      k++;
    }
  }

  IF_VERBOSE(5) cout << "possible_divider_set=" << possible_divider_set << endl;

  //to avoid double conting of identical predecessors, evaluate the gain map in a secon step:
  for(set_iter = working_set->begin(); set_iter != working_set->end(); ++set_iter)
  {
    T w = *set_iter;//element from working_set
    int_t div;
    T quot;

    for(set_int_t_iter = possible_divider_set.begin(); set_int_t_iter != possible_divider_set.end(); ++set_int_t_iter)
    {
      div = *set_int_t_iter;
      if((w % div) == 0)
      {
        quot = w / div;
        if(((s_max < 0) || (adder_depth(quot) <= s_max)) && ((ws_max < 0) || (compute_word_size(quot,0) <= ws_max)))
        {
          IF_VERBOSE(4) cout << "**case 3a: w=" << w << "=" << quot << "*" << div << " (p'=" << quot << ")" << endl;

          if(p_set_b != NULL) p_set_b->insert(quot);

          (*p_w_gain_map)[pair<T,T>(quot,w)] = max((*p_w_gain_map)[pair<T,T>(quot,w)],1/cost->cost_ternary_add(w,quot,quot,quot));
        }
      }
    }
  }
}

template<typename T>
void rpag_base<T>::compute_topology_c_predecessors_3_add(const set<T> *working_set, const set<T> *predecessor_set, map<pair<T,T>, double> *p_w_gain_map, int s_max, int ws_max, set<T> *p_set_c)
{
  IF_VERBOSE(4) cout << "compute 3-input predecessors for topology c" << endl;
  if(!predecessor_set->empty())
  {
    typename set<T>::iterator working_set_iter;
    typename set<T>::iterator potential_set_iter;
    typename set<T>::iterator predecessor_1_set_iter;
    typename set<T>::iterator predecessor_2_set_iter;


    set<T> potential_predecessors;

    for(working_set_iter = working_set->begin(); working_set_iter != working_set->end(); ++working_set_iter)
    {
      T w = *working_set_iter;

      for(predecessor_1_set_iter = predecessor_set->begin(); predecessor_1_set_iter != predecessor_set->end(); ++predecessor_1_set_iter)
      {
        T p_realized1 = *predecessor_1_set_iter;
        for(predecessor_2_set_iter = predecessor_set->begin(); predecessor_2_set_iter != predecessor_set->end(); ++predecessor_2_set_iter)
        {
          T p_realized2 = *predecessor_2_set_iter;
          //-p_realized1 and -p_realized1 are negative because they will switch the side of the equation. just important in case of ternary_sign_filter = true;
          //w = p_realized1 + p_realized2 + potential_predecessors => w -p_realized1 -p_realized2 = potential_predecessors
          compute_successor_set(w, -p_realized1, -p_realized2, c_max, &potential_predecessors, true, ternary_sign_filter);

          for(potential_set_iter = potential_predecessors.begin(); potential_set_iter != potential_predecessors.end(); ++potential_set_iter)
          {
            T p = *potential_set_iter;
            if(((s_max < 0) || (adder_depth(p) <= s_max)) && ((ws_max < 0) || (compute_word_size(p,0) <= ws_max)))
            {
              IF_VERBOSE(4) cout << "**case 3c met for w=" << w << " (p=" << p << ", p'=" << p_realized1 << " p''=" << p_realized2 << ")" << endl;

              if(p_set_c != NULL) p_set_c->insert(p);

              (*p_w_gain_map)[pair<T,T>(p,w)] = max((*p_w_gain_map)[pair<T,T>(p,w)],1/cost->cost_ternary_add(w,p,p_realized1,p_realized2));
            }
          }
          potential_predecessors.clear();
        }
      }
    }
  }
}

template<typename T>
void rpag_base<T>::compute_topology_d_predecessors_3_add(T x, int nz_max, int ws_max, set<triplet<T,T,T> > *predecessor_triplet_set)
{
    int iteration_counter=0;
    IF_VERBOSE(4) cout << "compute 2-input predecessors for topology d (pairs)" << endl;

    sd_vec_set_t msd_vec_set;

    int ws = compute_word_size(x,0);
    // old version int ws = log2c_64(abs(x).max());

    //if max. nonzeros are unconstrained, use wordsize-1
    if(nz_max < 0)
    {
      nz_max=ws-1;
    }

    IF_VERBOSE(4) cout << "max. nonzeros:" << nz_max << endl;
    IF_VERBOSE(4) cout << "max. wordsize:" << ws_max << endl;
    IF_VERBOSE(4) cout << "ws(" << x << ") = " << ws << endl;

    vec_t dummy; // this have to be two lines because initalisation of a vector with not a vector will be interpreted as the number of dimentions. vec_t dummi = 5; => (0;0;0;0;0) vec_t dummi; dummi=5; => (5)
    dummy = x; // if x is an int_t then the default size of vec_t was switched to 1 in the beginning of the optimize() funktion. So dummi is a vector with just x as element.

    vec2msd_vec(dummy, &msd_vec_set, msd_representation_limit);
    //IF_VERBOSE(6) cout << "msd_set(" << x << "):" << endl << msd_vec_set << endl;

    sd_vec_set_t::iterator msd_vec_set_iter;

    for(msd_vec_set_iter = msd_vec_set.begin(); msd_vec_set_iter != msd_vec_set.end(); ++msd_vec_set_iter)
    {
      sd_vec_t msd_vec = *msd_vec_set_iter;
      int nz = nonzeros(msd_vec); //!!!!

      //IF_VERBOSE(6) cout << "msd(" << x << ")=" << msd << ", nz=" << nz << endl;

      vector<int_t> nonzero_values(nz);         //vector that stores signs of nonzero values
      vector<int> nonzero_shift_factor(nz); //vector that stores the shift weight (2^"shift factor")
      vector<int> nonzero_element(nz);        //vector that stores the element number that it belongs to

      //fill nonzero_values, nonzero_element and nonzero_shift_factor with data:
      vector<int>::iterator int_vector_iter;
      int i=0;
      int shift_factor=0;
      for(unsigned j = 0; j < msd_vec.size(); ++j)
      {
        shift_factor=0;//for each element of the vector it starts witch a new number
        for (int_vector_iter=msd_vec[j].begin(); int_vector_iter != msd_vec[j].end(); ++int_vector_iter)
        {
          if(*int_vector_iter != 0)
          {
            nonzero_values[i]=*int_vector_iter;
            nonzero_shift_factor[i]=shift_factor;
            nonzero_element[i]=j;
            i++;
          }
          shift_factor++;
        }
      }
      IF_VERBOSE(5) cout << "nonzero_values=" << nonzero_values << endl;
      IF_VERBOSE(5) cout << "nonzero_shift_factor=" << nonzero_shift_factor << endl;
      IF_VERBOSE(5) cout << "nonzero_element=" << nonzero_element << endl;

      vec_t null_element; null_element = 0; // this have to be seperate comands! (because of vector interpretation during initialisation)

      permutation_for_predeccessor_pairs(nonzero_values, nonzero_shift_factor, nonzero_element, predecessor_triplet_set, ws_max, nz_max, true, null_element, iteration_counter);
  }
}

template<typename T>
void rpag_base<T>::permutation_for_predeccessor_pairs(vector<int_t> &nonzero_values, vector<int> &nonzero_shift_factor, vector<int> &nonzero_element, set<triplet<T,T,T> > *predecessor_triplet_set, int &ws_max, int nz_max, bool rekursion, vec_t p3, int &iteration_counter)
{
  //iterate for different nonzero count for the left side predecessor p1 (the right side predecessor p2 has size nz-nz_p1):
  int nz_p2;
  int nz = nonzero_values.size();

  //compute the input value x (defind by nonzero_indices,nonzero_values and nonzero_shift_factor)
  vec_t x; x = 0;
  for(int i=0; i < nz; i++)
  {
    x[nonzero_element[i]] += ((int_t) nonzero_values[i]) << nonzero_shift_factor[i];
  }
  IF_VERBOSE(5) cout << "x=" << x << std::endl;

  int nz_max_left;
  int ws_max_left;
  if(rekursion)
  {
    nz_max_left = nz_max*2;
    ws_max_left = ws_max*2;
  }
  else
  {
    nz_max_left = nz_max;
    ws_max_left = ws_max;
  }

  for(int nz_p1=nz_max_left; nz_p1 > 0; nz_p1--)
  {
    if(nz_p1 >= nz)
      continue;

    nz_p2=nz-nz_p1;
    if(nz_p2 > nz_max)
      break;

    IF_VERBOSE(4) cout << "nz_p1=" << nz_p1 << ", nz_p2=" << nz_p2 << endl;
    vector<int> left_nonzero_indices(nz_p1); //vector left_nonzeros is used to address the nonzeros which are used for one part of the pair

    //init left_nonzero_indices:
    for(int i=0; i < nz_p1; i++)
    {
      left_nonzero_indices[i]=i;
    }

    //permutate left_nonzero_indices:
    vec_t p1, p2;
    p1 = -1,
    p2 = -1;
    int ws_p1=-1,ws_p2=-1;
    int index;
    bool valid_predecessor=true; // necesserry for sign filter system

    while((msd_digit_permutation_limit == -1) || (iteration_counter < msd_digit_permutation_limit)) //do permutation: (if(index < 0) break;)
    {
      index=nz_p1-1;
      p1=0;
      //evaluate nonzero_values:
      for(int i=0; i < nz_p1; i++)
      {
          p1[nonzero_element[left_nonzero_indices[i]]] += ((int_t) nonzero_values[left_nonzero_indices[i]]) << nonzero_shift_factor[left_nonzero_indices[i]];
      }

      int min_index_p2=-1, max_index_p2=-1; //!!!

      if(ws_max > 0)
      {
          std::cout << "Error wordsize constraint is not soportet for vectors"<< std::endl;
          std::cout << "rpag: topologie d 3 input adder;" << std::endl;
          exit(-1);
        //detect wordsize of p1 and p2:

        IF_VERBOSE(5) cout << "left_nonzero_indices=" << left_nonzero_indices << endl;

        int min_index_p1,max_index_p1;
        min_index_p1=left_nonzero_indices[0];
        max_index_p1=left_nonzero_indices[nz_p1-1];

        //wordsize of p1 can be directly determined:
        ws_p1 = nonzero_shift_factor[max_index_p1]-nonzero_shift_factor[min_index_p1]+1;

        IF_VERBOSE(5) cout << "min. index p1=" << left_nonzero_indices[0] << " max. index p1=" << left_nonzero_indices[nz_p1-1] << " wordsize p1=" << ws_p1 << endl;

        // if wordsize constraint of p1 is met, check p2:
        if(ws_p1 <= ws_max_left)
        {
          int i;
          //to find the first index of p2, search for the first occurrence of an index step by 2:
          for(i=0; i < nz_p1; i++)
          {
            if(i != left_nonzero_indices[i])
              break;
          }
          min_index_p2 = i; //min index is step position

          //to find the last index of p2, search backwards for the first occurrence of an index step by -2:
          int j=0;
          for(i=nz_p1-1; i >= 0; i--, j++)
          {
            if(left_nonzero_indices[i] != nz-1-j)
              break;
          }
          max_index_p2 = nz-1-j;
          ws_p2 = nonzero_shift_factor[max_index_p2]-nonzero_shift_factor[min_index_p2]+1;
          IF_VERBOSE(5) cout << "min. index p2=" << min_index_p2 << " max. index p2=" << max_index_p2 << " wordsize p2=" << ws_p2 << endl;
        }
      }

      //for constrained wordsize, check wordsize for p1 and p2 before computing them, otherwise continue with permutation:
      if((ws_max < 0) || ((ws_p1 <= ws_max_left) && (ws_p2 <= ws_max)))
      {
        //p1 = p1;
        p2 = x - p1;
        IF_VERBOSE(5) cout << "left_nonzero_indices=" << left_nonzero_indices << ": (" << (p1+p2+p3) << "=" << p1 << "+" << p2 << "+" << p3 << ")";

        if(ternary_sign_filter)
        {
           valid_predecessor = just_one_negative_sign(p1,p2,p3);
        }

        p1 = fundamental( norm(p1));
        p2 = fundamental( norm(p2));
        p3 = fundamental( norm(p3));

        if(ternary_sign_filter && ((nonzeros_for_sign_filter(p1) > nz_max) || (nonzeros_for_sign_filter(p2) > nz_max) || (nonzeros_for_sign_filter(p3) > nz_max)))
        {
          //cout  << " invalid fun triplet: (" << p1 << "," << p2 << "," << p3 << ")" << endl;
          valid_predecessor = false;
        }
        IF_VERBOSE(7) cout  << " fun triplet: (" << p1 << "," << p2 << "," << p3 << ")" << endl;

        //to make p1 < p2 < p3 // just for the unicness in the predecessorset
        vec_t tmp, insert_p1, insert_p2, insert_p3;
        insert_p1 = p1;
        insert_p2 = p2;
        insert_p3 = p3;

        if(insert_p3 < insert_p2)
        {
            tmp = insert_p2;
            insert_p2 = insert_p3;
            insert_p3 = tmp;
        }
        if(insert_p2 < insert_p1)
        {
            tmp = insert_p1;
            insert_p1 = insert_p2;
            insert_p2 = tmp;
        }
        if(insert_p3 < insert_p2)
        {
            tmp = insert_p2;
            insert_p2 = insert_p3;
            insert_p3 = tmp;
        }

        if((nz_p1 <= nz_max) && (nz_p2 <= nz_max) && (ws_p1 <= ws_max) && valid_predecessor)
        {
          IF_VERBOSE(4)cout  << " predecessor triplet: (" << insert_p1 << "," << insert_p2 << "," << insert_p3 << ")" << endl;
          insert_into_predecessor_triplet_set(predecessor_triplet_set, vec_triplet_t(insert_p1,insert_p2,insert_p3));
        }
        else
        {
          IF_VERBOSE(7) cout  << "!! invalid predecessor triplet: (" << insert_p1 << "," << insert_p2 << "," << insert_p3 << ")" << endl;
        }


      }

      if(rekursion)//for 3 input adders
      {
        vector<int_t> left_nonzero_values(nz_p1);
        vector<int>   left_nonzero_shift_factor(nz_p1);
        vector<int>   left_nonzero_element(nz_p1);

        for(int i = 0; i < nz_p1; i++)
        {
          left_nonzero_values[i]        = nonzero_values[left_nonzero_indices[i]];
          left_nonzero_shift_factor[i]  = nonzero_shift_factor[left_nonzero_indices[i]];
          left_nonzero_element[i]       = nonzero_element[left_nonzero_indices[i]];
        }

        IF_VERBOSE(7) cout  << "rekursion begin" << endl;
        permutation_for_predeccessor_pairs(left_nonzero_values, left_nonzero_shift_factor, left_nonzero_element,  predecessor_triplet_set, ws_max, nz_max, false, p2, iteration_counter);
        IF_VERBOSE(7) cout  << "rekursion end" << endl;
      }

      ++iteration_counter;
      //do permutation:
      while(left_nonzero_indices[index] >= nz-1-(nz_p1-1-index))
      {
        index--;
        if(index < 0)
          break;
      }
      if(index < 0)
        break;

      if(left_nonzero_indices[index] < nz-1-(nz_p1-1-index))
        left_nonzero_indices[index]++;

      for(;index < nz_p1-1;index++)
      {
        if(left_nonzero_indices[index] < nz-1)
          left_nonzero_indices[index+1] = left_nonzero_indices[index]+1;
      }

    }
  }
}

template <typename T>
void rpag_base<T>::compute_topology_e_predecessors_2_add(T w1, T w2, int l_max, int nz_max, int ws_max, set<pair<T,T> > *predecessor_pair_set)
{
  int l11, l12, l21, l22, r1, r2, s12, s22;


  UNUSED(ws_max); //ToDo: Implement & test filter for word size constraint

  IF_VERBOSE(4) cout << "l_max=" << l_max << endl;

  //case i:
  IF_VERBOSE(5) cout << "evaluating topology (e) case i" << endl;
  l21=0; l22=0; r1=0; r2=0;
  for(s12=1; s12 >= -1; s12-=2)
  {
    for(s22=1; s22 >= -1; s22-=2)
    {
      for(l11=1; l11 < l_max; l11++)
      {
        for(l12=1; l12 < l_max; l12++)
        {
          evaluate_usefull_predecessor_pair(w1,w2,predecessor_pair_set,nz_max,l11,l12,l21,l22,r1,r2,s12,s22);
        }
      }
    }
  }

  //case ii:
  IF_VERBOSE(5) cout << "evaluating topology (e) case ii" << endl;
  l21=0; l12=0; r1=0; r2=0;
  for(s12=1; s12 >= -1; s12-=2)
  {
    for(s22=1; s22 >= -1; s22-=2)
    {
      for(l11=1; l11 < l_max; l11++)
      {
        for(l22=1; l22 < l_max; l22++)
        {
          evaluate_usefull_predecessor_pair(w1,w2,predecessor_pair_set,nz_max,l11,l12,l21,l22,r1,r2,s12,s22);
        }
      }
    }
  }

  //case iii:
  IF_VERBOSE(5) cout << "evaluating topology (e) case iii" << endl;
  l21=0; l12=0; l22=0; r1=0;
  for(s12=1; s12 >= -1; s12-=2)
  {
    for(s22=1; s22 >= -1; s22-=2)
    {
      for(l11=1; l11 < l_max; l11++)
      {
        for(r2=1; r2 < l_max; r2++)
        {
          evaluate_usefull_predecessor_pair(w1,w2,predecessor_pair_set,nz_max,l11,l12,l21,l22,r1,r2,s12,s22);
        }
      }
    }
  }

  //case iv:
  IF_VERBOSE(5) cout << "evaluating topology (e) case iv" << endl;
  l11=0; l21=0; l12=0; l22=0;
  for(s12=1; s12 >= -1; s12-=2)
  {
    for(s22=1; s22 >= -1; s22-=2)
    {
      for(r1=1; r1 < l_max; r1++)
      {
        for(r2=1; r2 < l_max; r2++)
        {
          evaluate_usefull_predecessor_pair(w1,w2,predecessor_pair_set,nz_max,l11,l12,l21,l22,r1,r2,s12,s22);
        }
      }
    }
  }
}


#endif // RPAG_TOPOLOGIES_H
