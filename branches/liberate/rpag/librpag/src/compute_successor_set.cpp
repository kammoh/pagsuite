/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "compute_successor_set.h"
#include "fundamental.h"
#include "rpag_functions.h"
#include "log2_64.h"
#include "debug.h"
#include <cstdlib>
#include <cmath>
#include "norm.h"

namespace rpag {

//###################################################################################################
//#######################################    2 input adders   #######################################
//###################################################################################################

void compute_successor_set(int_t r1, int_t r2, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    int_t s;

    if((r1 > c_max)||(r2 > c_max)) return;

    //for k=0, the fundamental() method hast to be called
    s=fundamental(r1 + r2);
#ifdef DEBUG
    IF_VERBOSE(7) cout << s << "=" << r1 << "+" << r2 << endl;
#endif
    if(s <= c_max) (*successor_set).insert(s);
#ifdef DEBUG
    if(s <= c_max) IF_VERBOSE(8) cout << s << "=|" << r1 << "+" << r2 << "|" << endl;
#endif
    if(s <= c_max) (*successor_set).insert(fundamental(abs(r1 - r2)));

    if(r1 < r2)
    {
      int_t temp = r1;
      r1 = r2;
      r2 = temp;
    }
    int k_max = compute_k_max(r1,r2,c_max);
    if(k_max < 0) return;

#ifdef DEBUG
    IF_VERBOSE(7) cout << "r1=" << r1 << ", r2=" << r2 << ", k_max=" << k_max << ", c_max=" << c_max << endl;
#endif

    for(int k=1; k <= k_max; k++)
    {
        s=(r1 << k) + r2;
#ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << k << "*" << r1 << "+" << r2 << endl;
#endif
        if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
        s=r1 + (r2 << k);
#ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << r1 << "+" << "2^" << k << "*" << r2 << endl;
#endif
        if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
        s=abs((r1 << k) - r2);
#ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=|" << "2^" << k << "*" << r1 << "-" << r2 << "|" << endl;
#endif
        if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
        s=abs(r1 - (r2 << k));
#ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=|" << r1 << "-" << "2^" << k << "*" << r2 << "|" << endl;
#endif
        if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
    }
    successor_set->erase(0);
    if(erase_predecessor)
    {
      successor_set->erase(r1);
      successor_set->erase(r2);
    }
  }
  else
  {
    compute_successor_set(r1,r2,c_max,successor_set,erase_predecessor);
    compute_successor_set(r1,r2,r2,c_max,successor_set,erase_predecessor, ternary_sign_filter);
    compute_successor_set(r1,r1,r2,c_max,successor_set,erase_predecessor, ternary_sign_filter);
  }
}

void compute_successor_set(const int_set_t *r1_set, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    int_t r1,r2,s;
    int k_max;

    int_set_t::iterator r1_p;
    int_set_t::iterator r2_p;
    
    for (r1_p=r1_set->begin(); r1_p != r1_set->end(); ++r1_p)
    {
        for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p)
        {    
            r1 = *r1_p;
            r2 = *r2_p;
            if(r1 < r2)
            {
              int_t temp = r1;
              r1 = r2;
              r2 = temp;
            }
            k_max = compute_k_max(r1,r2,c_max);

            IF_VERBOSE(7) cout << "computing successor set of " << r1 << " and " << r2 << " k_max=" << k_max << endl;

            //for k=0, the fundamental() method hast to be called
            (*successor_set).insert(fundamental(r1 + r2));
            (*successor_set).insert(fundamental(abs(r1 - r2)));
            
            for(int k=1; k < k_max; k++)
            {
                s=(r1 << k) + r2;
                if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
                s=r1 + (r2 << k);
                if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
                s=abs((r1 << k) - r2);
                if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
                s=abs(r1 - (r2 << k));
                if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
            }
            if(erase_predecessor)
            {
              successor_set->erase(r1);
              successor_set->erase(r2);
            }
        }
    }
    successor_set->erase(0);
//    successor_set->erase(1);
  }
  else
  {
    // 3 input adders
    int_t r1,r2;

    int_set_t::iterator r1_p;
    int_set_t::iterator r2_p;

    for (r1_p=r1_set->begin(); r1_p != r1_set->end(); ++r1_p)
    {
      for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p)
      {
        r1 = *r1_p;
        r2 = *r2_p;
        compute_successor_set(r1,r1,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
        compute_successor_set(r1,r2,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
      }
    }
  }
}

void compute_successor_set(int_t r1, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    int_t r1_int,r2_int,s;
    int k_max;

    int_set_t::iterator r2_p;

    for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p) //!!!
    {
        r1_int = r1;
        r2_int = *r2_p;
        if(r1_int < r2_int)
        {
          int_t temp = r1_int;
          r1_int = r2_int;
          r2_int = temp;
        }
        k_max = compute_k_max(r1_int,r2_int,c_max);

        IF_VERBOSE(7) cout << "computing successor set of " << r1_int << " and " << r2_int << " k_max=" << k_max << endl;

        //for k=0, the fundamental() method hast to be called
        (*successor_set).insert(fundamental(r1_int + r2_int));
        (*successor_set).insert(fundamental(abs(r1_int - r2_int)));

        for(int k=1; k < k_max; k++)
        {
            s=(r1_int << k) + r2_int;
            if((s > 0) && (s <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
            s=r1_int + (r2_int << k);
            if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
            s=abs((r1_int << k) - r2_int);
            if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
            s=abs(r1_int - (r2_int << k));
            if((s > 0) && (s <= c_max)) (*successor_set).insert(s);
        }
        if(erase_predecessor)
        {
          successor_set->erase(r1_int);
          successor_set->erase(r2_int);
        }
    }
    successor_set->erase(0);
//    successor_set->erase(1);
  }
  else
  {
    // 3 input adders
    int_t r1_int,r2_int;

    int_set_t::iterator r2_p;

    r1_int = r1;
    for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p) //!!!
    {
      r2_int = *r2_p;
      compute_successor_set(r1_int,r1_int,r2_int,c_max,successor_set,erase_predecessor,ternary_sign_filter);
      compute_successor_set(r1_int,r2_int,r2_int,c_max,successor_set,erase_predecessor,ternary_sign_filter);
    }
  }
}

//This method can be enhanced in speed
void compute_successor_set(const int_set_t *realized_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    compute_successor_set(realized_set, realized_set, c_max, successor_set, erase_predecessor, two_input_adder);
  }
  else
  {
    // 3 input adders
    int_set_t::iterator r1_p;
    int_set_t::iterator r2_p;
    int_set_t::iterator r3_p;

    //try any combiination of three elemts from the realized_set
    for (r1_p=realized_set->begin(); r1_p != realized_set->end(); ++r1_p) //this is a element from the same realized_set set.
    {
      for (r2_p=realized_set->begin(); r2_p != realized_set->end(); ++r2_p) //this is also a element from the same realized_set set.
      {
        for (r3_p=realized_set->begin(); r3_p != realized_set->end(); ++r3_p) //this is also a element from the same realized_set set.
        {
          compute_successor_set((*r1_p),(*r2_p),(*r3_p),c_max,successor_set,erase_predecessor,ternary_sign_filter);
        }
      }
    }
  }
}

/* This function is currently not used:
void compute_successor_set(int_t r1, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool two_input_adder)
{
    int_set_t::iterator r2_p;

    for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p) //!!!
    {
      compute_successor_set(r1, *r2_p, c_max, successor_set, erase_predecessor, two_input_adder);
    }
}
*/

void compute_successor_set(vec_t r1, vec_t r2, int_t c_max, vec_set_t *successor_set, bool erase_predecessor, bool two_input_adder,bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    vec_t s;
    int k_max = compute_k_max(r1,r2,c_max);

      //int_t k_max=log2c_64((int_t) round(c_max-(abs(r1).max()))/abs(r2).max());

      IF_VERBOSE(7) std::cout <<"k_max = " << k_max << std::endl;

      IF_VERBOSE(7) cout << "computing successor set of " << r1 << " and " << r2 << " k_max=" << k_max << endl;

      //for k=0, it isn't nessassery to check all
      s = fundamental(norm(r1 + r2));
      if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
      s = fundamental(norm(r1 - r2));
      if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
      s = fundamental(norm(r2 - r1));
      if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);

      for(int k=1; k < k_max; k++)
      {
          s=fundamental(norm((r1 << k) + r2));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
          s=fundamental(norm(r1 + (r2 << k)));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);

          s=fundamental(norm((r1 << k) - r2));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
          s=fundamental(norm(r1 - (r2 << k)));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
      }
      if(erase_predecessor)
      {
        successor_set->erase(r1);
        successor_set->erase(r2);
      }
  }
  else
  {
    compute_successor_set(r1,r2,c_max,successor_set,erase_predecessor);
    compute_successor_set(r1,r2,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
    compute_successor_set(r1,r1,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
  }
}

void compute_successor_set(const vec_set_t *r1_set, const vec_set_t *r2_set, int_t c_max, vec_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{

  if(two_input_adder)
  {
    vec_t r1,r2,s;
    int k_max;

    vec_set_t::iterator r1_p;
    vec_set_t::iterator r2_p;

    for (r1_p=r1_set->begin(); r1_p != r1_set->end(); ++r1_p)
    {
        for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p)
        {

          r1 = *r1_p;
          r2 = *r2_p;

          k_max = compute_k_max(r1,r2,c_max);
          IF_VERBOSE(7) std::cout << std::endl << "r1 max=" << abs(r1).max() << "  r2 max=" << abs(r2).max() << "  k_max=" << k_max << std::endl;


          IF_VERBOSE(7) cout << "computing successor set of " << r1 << " and " << r2 << " k_max=" << k_max << endl;

          //for k=0, it isn't nessassery to check all
          s = fundamental(norm(r1 + r2));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
          s = fundamental(norm(r1 - r2));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
          s = fundamental(norm(r2 - r1));
          if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);

          for(int k=1; k < k_max; k++)
          {
              s=fundamental(norm((r1 << k) + r2));
              if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s); //as k_max can be too large such that s overflows, detect such an overflow
              s=fundamental(norm(r1 + (r2 << k)));
              if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);

              s=fundamental(norm((r1 << k) - r2));
              if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
              s=fundamental(norm(r1 - (r2 << k)));
              if((s != 0) && (abs(s) <= c_max)) (*successor_set).insert(s);
          }
          if(erase_predecessor)
          {
            successor_set->erase(r1);
            successor_set->erase(r2);
          }
      }
    }
  }
  else
  {
      // 3 input adders
      vec_t r1,r2;

      vec_set_t::iterator r1_p;
      vec_set_t::iterator r2_p;

      for (r1_p=r1_set->begin(); r1_p != r1_set->end(); ++r1_p)
      {
        for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p)
        {
          r1 = *r1_p;
          r2 = *r2_p;
          compute_successor_set(r1,r1,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
          compute_successor_set(r1,r2,r2,c_max,successor_set,erase_predecessor,ternary_sign_filter);
        }
      }
  }
}
void compute_successor_set(const vec_set_t *realized_set, int_t c_max, vec_set_t *successor_set, bool erase_predecessor, bool two_input_adder, bool ternary_sign_filter)
{
  if(two_input_adder)
  {
    compute_successor_set(realized_set, realized_set, c_max, successor_set, erase_predecessor, two_input_adder);
  }
  else
  {
    // 3 input adders
    vec_set_t::iterator r1_p;
    vec_set_t::iterator r2_p;
    vec_set_t::iterator r3_p;

    //try any combiination of three elemts from the realized_set
    for (r1_p=realized_set->begin(); r1_p != realized_set->end(); ++r1_p) //this is a element from the realized_set set.
    {
      for (r2_p=realized_set->begin(); r2_p != realized_set->end(); ++r2_p) //this is a element from the same realized_set set.
      {
        for (r3_p=realized_set->begin(); r3_p != realized_set->end(); ++r3_p) //this is a element from the same realized_set set.
        {
          compute_successor_set((*r1_p),(*r2_p),(*r3_p),c_max,successor_set,erase_predecessor,ternary_sign_filter);
        }
      }
    }
  }
}

//###################################################################################################
//#######################################    3 input adders   #######################################
//###################################################################################################

void permut(int &a, int &b, int &c, int &k, int &l, int index)
{
  switch(index)
  {
  case 0:
    a = k;
    b = 0;
    c = l;
  break;
  case 1:
    a = l;
    b = k;
    c = 0;
  break;
  case 2:
    a = 0;
    b = l;
    c = k;
  break;
  case 3:
    a = k;
    b = l;
    c = 0;
  break;
  case 4:
    a = l;
    b = 0;
    c = k;
  break;
  case 5:
    a = 0;
    b = k;
    c = l;
  break;
  default:
    std::cout << "unknown index in: void permut(int &a, int &b, int &c, int &k, int &l, int index)" << std::endl;
    exit(-1);
  break;
  }

}

void compute_successor_set(int_t r1, int_t r2, int_t r3, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool ternary_sign_filter)
{
  int a = 0;
  int b = 0;
  int c = 0;
  int permut_border = 6;

  //for sign filter option to remember the original sign. it hase to be reconstruct in case of topologie C for 3 input adders
  //this method calculate the sign by xor all boolian parts of a value.
  bool r1_sign = is_it_negative(r1); //false = positive and true = negative
  bool r2_sign = is_it_negative(r2); //false = positive and true = negative
  bool r3_sign = is_it_negative(r3); //false = positive and true = negative

  r1 = norm(r1);
  r2 = norm(r2);
  r3 = norm(r3);
  int k_max = compute_k_max(r1,r2,r3,c_max);
  /* // k_max without funktion...
  //set r1 to the maximum value
  { //to make r1 > r2 > r3
    int_t tmp;
    if(r3 > r2)
    {
        tmp = r2;
        r2 = r3;
        r3 = tmp;
    }
    if(r2 > r1)
    {
        tmp = r1;
        r1 = r2;
        r2 = tmp;
    }
    if(r3 > r2)
    {
        tmp = r2;
        r2 = r3;
        r3 = tmp;
    }
  }


  if(r1 > c_max) return;

  int k_max = compute_k_max(r1,r3,c_max);
  if(k_max < 0) return;
  */


#ifdef DEBUG
  IF_VERBOSE(7) cout << "r1=" << r1 << ", r2=" << r2 << ", r3=" << r3 << ", k_max=" << k_max << ", c_max=" << c_max << endl;
#endif

  bool sign_switched = false;
  int_t s;

  for(int k = 0; k <= k_max; k++)
  {
    for(int l = 0; l <= k; l++)
    {
      if((k == l)||(k==0)||(l==0)){permut_border = 3;} else {permut_border = 6;}//if k = l it's not necessary to switch k and l

      for(int permut_counter = 0; permut_counter < permut_border; permut_counter++)
      {
        permut(a,b,c,k,l,permut_counter);

        s = norm((r1<<a) + (r2<<b) + (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s > 0) && (s <= c_max) && ((!ternary_sign_filter) || just_one_negative_sign((false^r1_sign^sign_switched),(false^r2_sign^sign_switched),(false^r3_sign^sign_switched)))) (*successor_set).insert(fundamental(s));

        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " + " << "2^" << b << " * " << r2 << " + " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) + (r2<<b) - (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s > 0) && (s <= c_max) && ((!ternary_sign_filter) || (just_one_negative_sign((false^r1_sign^sign_switched),(false^r2_sign^sign_switched),(true^r3_sign^sign_switched))))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " + " << "2^" << b << " * " << r2 << " - " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) - (r2<<b) + (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s > 0) && (s <= c_max) && ((!ternary_sign_filter) || (just_one_negative_sign((false^r1_sign^sign_switched),(true^r2_sign^sign_switched),(false^r3_sign^sign_switched))))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " - " << "2^" << b << " * " << r2 << " + " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) - (r2<<b) - (r3<<c),&sign_switched);
        if((s > 0) && (s <= c_max) && ((!ternary_sign_filter) || (just_one_negative_sign((false^r1_sign^sign_switched),(true^r2_sign^sign_switched),(true^r3_sign^sign_switched))))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " - " << "2^" << b << " * " << r2 << " - " << "2^" << c << " * " << r3 << std::endl << std::endl;
        #endif
      }
    }
  }
  successor_set->erase(0);
  if(erase_predecessor)
  {
    successor_set->erase(r1);
    successor_set->erase(r2);
    successor_set->erase(r3);
  }
}

void compute_successor_set(int_t r1, int_t r2, const int_set_t *r3_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor, bool ternary_sign_filter)
{
  for (int_set_t::iterator r3_p=r3_set->begin(); r3_p != r3_set->end(); ++r3_p)
  {
    compute_successor_set(r1, r2, *r3_p, c_max, successor_set, erase_predecessor, ternary_sign_filter);
  }
}

void compute_successor_set(vec_t r1, vec_t r2, vec_t r3, int_t c_max, vec_set_t *successor_set, bool erase_predecessor, bool ternary_sign_filter)
{
  int a = 0;
  int b = 0;
  int c = 0;
  int permut_border = 6;

  //for sign filter option to remember the original sign. it hase to be reconstruct in case of topologie C for 3 input adders
  //this method calculate the sign by xor all boolian parts of a value.
  bool r1_sign = is_it_negative(r1); //false = positive and true = negative
  bool r2_sign = is_it_negative(r2); //false = positive and true = negative
  bool r3_sign = is_it_negative(r3); //false = positive and true = negative

  r1 = norm(r1);
  r2 = norm(r2);
  r3 = norm(r3);

  //set r1 to the maximum value
  { //to make r1 > r2 > r3
    vec_t tmp;
    if(r3 > r2)
    {
        tmp = r2;
        r2 = r3;
        r3 = tmp;
    }
    if(r2 > r1)
    {
        tmp = r1;
        r1 = r2;
        r2 = tmp;
    }
    if(r3 > r2)
    {
        tmp = r2;
        r2 = r3;
        r3 = tmp;
    }
  }

  bool sign_switched = false;
  vec_t s;

  if(r1 > c_max) return;

  int k_max = compute_k_max(r1,r2,r3,c_max);

#ifdef DEBUG
  IF_VERBOSE(7) cout << "r1=" << r1 << ", r2=" << r2 << ", r3=" << r3 << ", k_max=" << k_max << ", c_max=" << c_max << endl;
#endif


  for(int k = 0; k <= k_max; k++)
  {
    for(int l = 0; l <= k; l++)
    {
      if((k == l)||(k==0)||(l==0)){permut_border = 3;} else {permut_border = 6;}//if k = l it's not necessary to switch k and l

      for(int permut_counter = 0; permut_counter < permut_border; permut_counter++)
      {
        permut(a,b,c,k,l,permut_counter);

        s = norm((r1<<a) + (r2<<b) + (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s != 0) && (abs(s) <= c_max) && ((!ternary_sign_filter) || just_one_negative_sign((false^r1_sign^sign_switched),(false^r2_sign^sign_switched),(false^r3_sign^sign_switched)))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " + " << "2^" << b << " * " << r2 << " + " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) + (r2<<b) - (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s != 0) && (abs(s) <= c_max) && ((!ternary_sign_filter) || just_one_negative_sign((false^r1_sign^sign_switched),(false^r2_sign^sign_switched),(true^r3_sign^sign_switched)))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " + " << "2^" << b << " * " << r2 << " - " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) - (r2<<b) + (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s != 0) && (abs(s) <= c_max) && ((!ternary_sign_filter) || just_one_negative_sign((false^r1_sign^sign_switched),(true^r2_sign^sign_switched),(false^r3_sign^sign_switched)))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " - " << "2^" << b << " * " << r2 << " + " << "2^" << c << " * " << r3 << std::endl;
        #endif

        s = norm((r1<<a) - (r2<<b) - (r3<<c),&sign_switched);
        //((!ternary_sign_filter) || (!sign_switched)) is to filter sign cominations with more then 1 negative sign.
        if((s != 0) && (abs(s) <= c_max) && ((!ternary_sign_filter) || just_one_negative_sign((false^r1_sign^sign_switched),(true^r2_sign^sign_switched),(true^r3_sign^sign_switched)))) (*successor_set).insert(fundamental(s));
        #ifdef DEBUG
        IF_VERBOSE(8) cout << s << "=" << "2^" << a << " * " << r1 << " - " << "2^" << b << " * " << r2 << " - " << "2^" << c << " * " << r3 << std::endl << std::endl;
        #endif
      }
    }
  }
  successor_set->erase(0);
  if(erase_predecessor)
  {
    successor_set->erase(r1);
    successor_set->erase(r2);
    successor_set->erase(r3);
  }
}
}
