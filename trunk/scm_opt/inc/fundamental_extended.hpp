#ifndef FUNDAMENTAL_HPP
#define FUNDAMENTAL_HPP

#include <cstdio>

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
    if(e) *e = e_tmp;
    return x;
  }
}

//template <class T>
//T fundamental_count(T x, int &c)
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

/*
inline int fundamental_count(int x, int &c)
{
  c = 0;
  if(x == 0)
    return 0;
  else
  {
    while((x & 0x01) == 0) { x >>= 1; ++c;}
    return x;
  }
}
*/


#endif // FUNDAMENTAL_HPP
