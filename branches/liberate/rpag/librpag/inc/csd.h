/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef CSD_H
#define CSD_H

#include "types.h"
#include "debug.h"

namespace rpag {

inline int csd_nonzeros2(int_t x)
{
  int N = 8*sizeof(int_t);
  int one_cnt=0;

  for(int i=0; i < N-1; i++)
  {
    if((x >> i) & 1)
    {
      if(i < 2)
        one_cnt++;
      else if((i == 2) || (i == N-1))
      {
        if(!(((x >> (i-2)) & 0x07) == 0x07))
          one_cnt++;
      }
      else // 2 < i < N-1
      {
        if(!(((x >> (i-2)) & 0x07) == 0x07) && !(((x >> (i-3)) & 0x1F) == 0x1B))
          one_cnt++;
      }
    }
  }
  IF_VERBOSE(7) cout << "csd_nonzeros(" << x << ")=" << one_cnt-1 << endl;
  return one_cnt;
}


/*
  In CSD a 11..11 is mapped to 10..00(-1)
*/
inline int csd_nonzeros(int_t x)
{
  IF_VERBOSE(7) cout << "csd_nonzeros(" << x << ")" << endl;
  int N = 8*sizeof(int_t);
  int one_cnt=0;

  int fourlsb;

  cout << "x=";
  for(int i=0; i < N-1; i++)
  {
    cout << ((x >> i) & 1);
  }
  cout << endl;

  cout << "   ";
  for(int i=0; i < N-1; i++)
  {
    x = x >> 1;
    fourlsb = (x & 15);
    if((fourlsb == 2) || (fourlsb == 3) || (fourlsb == 6) || (fourlsb == 10) || (fourlsb == 14))
    {
      cout << "*";
      one_cnt++;
    }
    else
    {
      cout << " ";
    }
  }
  cout << endl;
  return one_cnt;
}

int dec2csd(int_t x, sd_t* csd);
int nonzeros(sd_t *csd);
int nonzeros(int_t x);
int nonzeros(vec_t x);

int nonzeros(sd_vec_t& x);

void count_digits(sd_t *sd, int &p, int &n);
int nonzeros_for_sign_filter(int_t x);
int nonzeros_for_sign_filter(vec_t x);

void csd2msd_w_mnd(sd_t *msd);//csd to msd with minimum negative digits
void dec2msd(int_t x, sd_set_t* msd_set);
void vec2msd_vec(vec_t x, sd_vec_set_t* msd_vec_set, long iteration_max = -1);
void msd_vec2vec(sd_vec_t& msd_vec, vec_t& vec);

int compute_word_size(vec_t vec, int input_word_size);
int compute_word_size(int_t x, int input_word_size);

}

#endif // CSD_H
