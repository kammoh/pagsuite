/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef FUNDAMENTAL_H
#define FUNDAMENTAL_H

#include "type_class.h"
#include "debug.h"

//returns the fundamental of x
inline int_t fundamental(int_t x)//inline and template doesn't work together
{
  if(x == 0)
    return 0;
  else
  {
    while((x & 0x01) == 0) x >>= 1;
//    while((x % 2) == 0) x /= 2;
    return x;
  }
}

//returns the fundamental of x
inline int_t fundamental(int_t x, int *e)
{
  int e_tmp=0;
  if(x == 0)
    return x;
  else
  {
    while((x & 0x01) == 0)
    {
      x >>= 1;
      e_tmp++;
    }
    *e = e_tmp;
    return x;
  }
}

inline vec_t fundamental(vec_t x)//inline and template doesn't work together
{
  if(x == 0)
    return x;// if x is 0 we can return x as 0. (for correct dimensions)
  else
  {
    while((x % 2) == 0) {x /= 2;}
    return x;
  }
}

template <class T>
int fundamental_count(T x)
{
  if(x == 0)
    return 0;
  else
  {
    int c=0;
    while((x % 2) == 0){ x /= 2; ++c;}
    return c;
  }
}

template <class T>
T fundamental_count(T x, int &c)
{
  c = 0;
  if(x == 0)
    return x; // if x is 0 we can return x as 0. (for correct dimensions in vector case...)
  else
  {
    while((x % 2) == 0){ x /= 2; ++c;}
    return x;
  }
}


inline int_t fundamental_count(int_t x, int &c)
{
  c = 0;
  if(x == 0)
    return x; // if x is 0 we can return x as 0. (for correct dimensions in vector case...)
  else
  {
    while((x % 2) == 0){ x /= 2; ++c;}
    return x;
  }
}

//returns the fundamental of x without checking x!=0 !
inline int_t fundamental_unsave(int_t x)
{
  while((x & 0x01) == 0) x >>= 1;
  return x;

  /*
   alternative implementations (turned out to be slower!):

//  while((x % 2) == 0) x /= 2;
//  while((x % 2) == 0) x = x >> 1;

  if((x & 0x1) != 0) return x;
  if((x & 0x2) != 0) return x >> 1;
  if((x & 0x4) != 0) return x >> 2;
  if((x & 0x8) != 0) return x >> 3;
  if((x & 0x10) != 0) return x >> 4;
  if((x & 0x20) != 0) return x >> 5;
  if((x & 0x40) != 0) return x >> 6;
  if((x & 0x80) != 0) return x >> 7;

  if((x & 0x100) != 0) return x >> 8;
  if((x & 0x200) != 0) return x >> 9;
  if((x & 0x400) != 0) return x >> 10;
  if((x & 0x800) != 0) return x >> 11;
  if((x & 0x1000) != 0) return x >> 12;
  if((x & 0x2000) != 0) return x >> 13;
  if((x & 0x4000) != 0) return x >> 14;
  if((x & 0x8000) != 0) return x >> 15;


  if((x & 0xFFFF) == 0) return x >> 16;
  if((x & 0x7FFF) == 0) return x >> 15;
  if((x & 0x3FFF) == 0) return x >> 14;
  if((x & 0x1FFF) == 0) return x >> 13;

  if((x & 0xFFF) == 0) return x >> 12;
  if((x & 0x7FF) == 0) return x >> 11;
  if((x & 0x3FF) == 0) return x >> 10;
  if((x & 0x1FF) == 0) return x >> 9;

  if((x & 0xFF) == 0) return x >> 8;
  if((x & 0x7F) == 0) return x >> 7;
  if((x & 0x3F) == 0) return x >> 6;
  if((x & 0x1F) == 0) return x >> 5;

  if((x & 0x0F) == 0) return x >> 4;
  if((x & 0x07) == 0) return x >> 3;
  if((x & 0x03) == 0) return x >> 2;
  if((x & 0x01) == 0) return x >> 1;
*/
}

void fundamental(int_set_t *set);
void fundamental(vec_set_t *set);


#endif //FUNDAMENTAL_H
