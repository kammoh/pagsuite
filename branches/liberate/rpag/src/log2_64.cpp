/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "log2_64.h"
#include <math.h>

//64 bit implementation of floor(log2(int x))
int log2f_64(uint64_t n)
{
  if(n == 0) return -1;

  int log = 0;
  if (n & 0xffffffff00000000LL) { n >>= 32; log |= 32; } //check if upper 32 bits contain a one, if true, shift them to LSB and add 32 to the logarithm
  if (n & 0xffff0000)           { n >>= 16; log |= 16; }
  if (n & 0xff00)               { n >>= 8;  log |= 8; }
  if (n & 0xf0)                 { n >>= 4;  log |= 4; }
  if (n & 0xc)                  { n >>= 2;  log |= 2; }
  if (n & 0x2)                  { n >>= 1;  log |= 1; }

  return log;
}

//64 bit implementation of ceil(log2(int x))
int log2c_64(uint64_t n)
{
  if(n == 0) return -1;

  int log = 0;
  uint64_t zero_check=0;
  if (n & 0xffffffff00000000LL) { log |= 32; zero_check |= (n & 0x00000000ffffffffLL); n >>= 32; } //check if upper 32 bits contain a one, if true, shift them to LSB and add 32 to the logarithm
  if (n & 0xffff0000)           { log |= 16; zero_check |= (n & 0x0000ffff);           n >>= 16; }
  if (n & 0xff00)               { log |= 8;  zero_check |= (n & 0x00ff);               n >>= 8;  }
  if (n & 0xf0)                 { log |= 4;  zero_check |= (n & 0x0f);                 n >>= 4;  }
  if (n & 0xc)                  { log |= 2;  zero_check |= (n & 0x3);                  n >>= 2;  }
  if (n & 0x2)                  { log |= 1;  zero_check |= (n & 0x1);                          }

  return zero_check == 0? log : log+1; //if zero_check != 1 means that n != 2^i and it must be round up
}

//64 bit implementation of floor(log3(int x))
int log3f_64(uint64_t n)
{
    //floor(log3(n))
    if(n < 3)     {return  0;}
    if(n < 9)     {return  1;}
    if(n < 27)    {return  2;}
    if(n < 81)    {return  3;}
    if(n < 243)   {return  4;}
    if(n < 729)   {return  5;}
    if(n < 2187)  {return  6;}
    if(n < 6561)  {return  7;}
    if(n < 19683) {return  8;}
    if(n < 59049) {return  9;}
    if(n < 177147){return 10;}
    if(n < 531441){return 11;}
    return (int) floor(logf((float) n)/logf(3));
}

//64 bit implementation of ceil(log3(int x))
int log3c_64(uint64_t n)
{
    //ceil(log3(n))
    if(n <= 3)     {return  1;}
    if(n <= 9)     {return  2;}
    if(n <= 27)    {return  3;}
    if(n <= 81)    {return  4;}
    if(n <= 243)   {return  5;}
    if(n <= 729)   {return  6;}
    if(n <= 2187)  {return  7;}
    if(n <= 6561)  {return  8;}
    if(n <= 19683) {return  9;}
    if(n <= 59049) {return 10;}
    if(n <= 177147){return 11;}
    if(n <= 531441){return 12;}
    return (int) ceil(logf((float) n)/logf(3));
}
