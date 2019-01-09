/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef TRIPLET_H
#define TRIPLET_H

namespace PAGSuite
{

  template<typename T1, typename T2, typename T3>
  class triplet
  {
  public:

    T1 first;
    T2 second;
    T3 third;

    triplet()
    {}

    triplet(T1 f, T2 s, T3 t)
    {
      first = f;
      second = s;
      third = t;
    }

    bool operator==(triplet input)
    {
      if ((first == input.first) && (second == input.second) && (third == input.third))
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    bool operator!=(triplet input)
    {
      if ((*this) == input)
      {
        return false;
      }
      else
      {
        return true;
      }
    }

    void operator=(triplet input)
    {
      first = input.first;
      second = input.second;
      third = input.third;
    }

    bool operator<(const triplet &input) const
    {
      if (third == input.third)
      {
        if (second == input.second)
        {
          return first < input.first;
        }
        else
        {
          return second < input.second;
        }
      }
      else
      {
        return third < input.third;
      }
    }

  };

}

#endif //TRIPLET_H
