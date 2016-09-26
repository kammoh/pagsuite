/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAG_VEC_H
#define RPAG_VEC_H
#include <vector>

#include <ostream>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>

//macros

#define GET_OPERATION_WITH_T(x) \
        bool out = true;\
        typename rpag_vec<T>::const_iterator iter = this->begin();\
        while(iter != this->end()){\
          if(!(*iter x rhs)){\
            out = false;}\
          ++iter;}\
        return out;

#define GET_OPERATION_WITH_RPAG_VEC(x) \
        if(this->size() == rhs.size()){\
          bool out = true;\
          typename rpag_vec<T>::const_iterator it_r = rhs.begin(), it_l = this->begin();\
          while(it_l != this->end()){\
            if(!(*it_l x *it_r)){\
            out = false;}\
            ++it_l;\
            ++it_r;}\
            return out;}\
        else{\
          return false;}

#define SET_OPERATION_WITH_T(x) \
        typename rpag_vec<T>::const_iterator it = this->begin(); \
        rpag_vec<T> out(0); \
        while(it != this->end()){ \
          out.push_back((*it x rhs)); \
          ++it;} \
        return out;

#define SET_OPERATION_RETURN_SELF_WITH_T(x) \
        typename rpag_vec<T>::iterator iter = this->begin();\
        while(iter != this->end()){\
          *iter x rhs;\
          ++iter;}\
        return *this;

#define SET_OPERATION_WITH_RPAG_VEC(x) \
        rpag_vec<T> out(0); \
        if(this->size() == rhs.size()){\
          typename rpag_vec<T>::const_iterator it_l = this->begin();\
          typename rpag_vec<T>::const_iterator it_r = rhs.begin();\
          while(it_l != this->end()){\
            out.push_back(*it_l x *it_r);\
            ++it_l;\
            ++it_r;}}else{std::cout << "\n!rpag_vector error: size difference!\n"; exit(-1);}\
         return out;

#define SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(x) \
        if(this->size() == rhs.size()){\
          typename rpag_vec<T>::iterator it_l = this->begin();\
          typename rpag_vec<T>::const_iterator it_r = rhs.begin();\
          while(it_l != this->end()){\
            *it_l x *it_r;\
            ++it_l;\
            ++it_r;}}else{std::cout << "\n!rpag_vector error: return self; size difference!\n"; exit(-1);}\
         return *this;

template<typename T, typename Alloc = std::allocator<T> >
class rpag_vec : public std::vector<T, Alloc>
{

public:

    static unsigned int default_elem_count;
    static char stream_seperate_symbol;
    friend void set_default_elem_count(unsigned int input);
    friend void set_stream_seperate_symbol(char input);
    rpag_vec<T, Alloc>(void)
    {
      for(unsigned int elements=default_elem_count; elements > 0; --elements)
      {
        this->push_back(0);
      }
    }

    rpag_vec<T, Alloc>(unsigned int elements)
    {
      for(; elements > 0; --elements)
      {
        this->push_back(0);
      }
    }

    void set_default_elem_count(unsigned int input)
    {
      this->default_elem_count = input;
    }

    void set_stream_seperate_symbol(char input)
    {
      this->stream_seperate_symbol = input;
    }



    // single element operators
    rpag_vec<T, Alloc> operator=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(=)
    }
    rpag_vec<T, Alloc> operator+=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(+=)
    }
    rpag_vec<T, Alloc> operator-=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(-=)
    }
    rpag_vec<T, Alloc> operator*=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(*=)
    }
    rpag_vec<T, Alloc> operator/=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(/=)
    }
    rpag_vec<T, Alloc> operator%=(const T rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_T(%=)
    }


    const rpag_vec<T, Alloc> operator+(const T rhs) const
    {
      SET_OPERATION_WITH_T(+)
    }
    const rpag_vec<T, Alloc> operator-(const T rhs) const
    {
      SET_OPERATION_WITH_T(-)
    }
    const rpag_vec<T, Alloc> operator*(const T rhs) const
    {
      SET_OPERATION_WITH_T(*)
    }
    const rpag_vec<T, Alloc> operator/(const T rhs) const
    {
      SET_OPERATION_WITH_T(/)
    }
    const rpag_vec<T, Alloc> operator%(const T rhs) const
    {
      SET_OPERATION_WITH_T(%)
    }
    const rpag_vec<T, Alloc> operator<< (const T rhs) const
    {
      SET_OPERATION_WITH_T(<<)
    }
    const rpag_vec<T, Alloc> operator>> (const T rhs) const
    {
      SET_OPERATION_WITH_T(>>)
    }


    // vector operators
    template<typename R, typename Alloc_R>
    rpag_vec<T, Alloc> operator=(const rpag_vec<R, Alloc_R> &rhs)
    {
      typename rpag_vec<R>::const_iterator it_r = rhs.begin();
      if(rhs.size() != this->size())
      {
        this->clear();
        for(; it_r != rhs.end(); ++it_r)
        {
          this->push_back(*it_r);
        }
        return *this;
      }
      else
      {
          typename rpag_vec<T>::iterator it_l = this->begin();
          while(it_l != this->end())
          {
            *it_l = *it_r;
            ++it_l;
            ++it_r;
          }
         return *this;
      }
    }


    rpag_vec<T, Alloc> operator+=(const rpag_vec<T, Alloc> rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(+=)
    }
    rpag_vec<T, Alloc> operator-=(const rpag_vec<T, Alloc> &rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(-=)
    }
    rpag_vec<T, Alloc> operator*=(const rpag_vec<T, Alloc> &rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(*=)
    }
    rpag_vec<T, Alloc> operator/=(const rpag_vec<T, Alloc> &rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(/=)
    }
    rpag_vec<T, Alloc> operator%=(const rpag_vec<T, Alloc> &rhs)
    {
      SET_OPERATION_RETURN_SELF_WITH_RPAG_VEC(%=)
    }


    const rpag_vec<T, Alloc> operator+(const rpag_vec<T, Alloc> &rhs) const
    {
      SET_OPERATION_WITH_RPAG_VEC(+)
    }
    const rpag_vec<T, Alloc> operator-(const rpag_vec<T, Alloc> &rhs) const
    {
      SET_OPERATION_WITH_RPAG_VEC(-)
    }
    const rpag_vec<T, Alloc> operator*(const rpag_vec<T, Alloc> &rhs) const
    {
      SET_OPERATION_WITH_RPAG_VEC(*)
    }
    const rpag_vec<T, Alloc> operator/(const rpag_vec<T, Alloc> &rhs) const
    {
      SET_OPERATION_WITH_RPAG_VEC(/)
    }
    const rpag_vec<T, Alloc> operator%(const rpag_vec<T, Alloc> &rhs) const
    {
      SET_OPERATION_WITH_RPAG_VEC(%)
    }

    // operators without argument
    rpag_vec<T, Alloc> operator++()
    {
      typename rpag_vec<T>::iterator it_l = this->begin();
      while(it_l != this->end())
      {
        ++(*it_l);
        ++it_l;
      }
      return *this;
    }

    rpag_vec<T, Alloc> operator--()
    {
      typename rpag_vec<T>::iterator it_l = this->begin();
      while(it_l != this->end())
      {
        --(*it_l);
        ++it_l;
      }
      return *this;
    }


    //comparisons

    bool operator==(const T &rhs) const
    {
      GET_OPERATION_WITH_T(==)
    }
    bool operator!=(const T &rhs) const
    {
      return (!(*this == rhs));
    }
    bool operator<(T &rhs) const
    {
      GET_OPERATION_WITH_T(<)
    }
    bool operator> (const T &rhs) const
    {
      GET_OPERATION_WITH_T(>)
    }
    bool operator<= (const T &rhs) const
    {
      GET_OPERATION_WITH_T(<=)
    }
    bool operator>= (const T &rhs) const
    {
      GET_OPERATION_WITH_T(>=)
    }


    //comparisons with vectors
    bool operator==(const rpag_vec<T, Alloc>& rhs) const
    {
      GET_OPERATION_WITH_RPAG_VEC(==)
    }
    bool operator!=(const rpag_vec<T, Alloc>& rhs) const
    {
      return (!(*this == rhs));
    }

    bool less (const rpag_vec<T, Alloc>& rhs) const
    {
      GET_OPERATION_WITH_RPAG_VEC(<)
    }
    bool greater(const rpag_vec<T, Alloc>& rhs) const
    {
      GET_OPERATION_WITH_RPAG_VEC(>)
    }
    bool less_or_equal(const rpag_vec<T, Alloc>& rhs) const
    {
      GET_OPERATION_WITH_RPAG_VEC(<=)
    }
    bool greater_or_equal(const rpag_vec<T, Alloc>& rhs) const
    {
      GET_OPERATION_WITH_RPAG_VEC(>=)
    }

    //unary operator
      rpag_vec<T, Alloc> operator-() const
      {
        return ((*this) * (T)(-1));
      }


    //functions with vectors
    const T max() const
    {
      typename rpag_vec<T, Alloc>::const_iterator iter = this->begin();
      if(this->size() == 0)
      {
        std::cerr << "ERROR: rpag_vec::max() No Elements in the Vector to test!!!";
        std::exit(-1);
      }
      else
      {
        T value = *iter;
        for(; iter != this->end(); ++iter)
        {
          if(*iter > value)
            value = *iter;
        }
        return value;
      }
    }

    const T min(bool without_zero = false) const
    {
      typename rpag_vec<T, Alloc>::const_iterator iter = this->begin();
      if(this->size() == 0)
      {
        std::cerr << "ERROR: rpag_vec::min() No Elements in the Vector to test!!!";
        std::exit(-1);
      }
      else
      {
       T value = *iter; // set value to the first element
        for(; iter != this->end(); ++iter)
        {
          if(without_zero)// if zero is not a valid answer.
          {
            if(*iter != 0) //just change something when value not become zero.
            {
              if(*iter < value)// the normal lower search
              {
                value = *iter;
              }
              else if(value == 0)// if the first element was zero(=> value = 0) then 'T value' have to chanche to the first element which is different to zero.
              {
                value = *iter;
              }
            }
          }
          else// if zero is a valid answer.
          {
            if(*iter < value)
            {
              value = *iter;
            }
          }
        }
        return value;
      }
    }
};

template<typename T, typename Alloc> unsigned int rpag_vec<T, Alloc>::default_elem_count = 1;
template<typename T, typename Alloc> char rpag_vec<T, Alloc>::stream_seperate_symbol = ';';

template<typename T, typename Alloc>
  rpag_vec<T, Alloc> operator+ (const T _lhs, const rpag_vec<T, Alloc> &_rhs)
  {
    return (_rhs + _lhs);
  }

template<typename T, typename Alloc>
  rpag_vec<T, Alloc> operator- (const T _lhs, const rpag_vec<T, Alloc> &_rhs)
  {
    return ((_rhs * ((T)-1)) + _lhs);
  }

template<typename T, typename Alloc>
  rpag_vec<T, Alloc> operator* (const int _lhs, const rpag_vec<T, Alloc> &_rhs)
  {
    return (_rhs * _lhs);
  }

template<typename T, typename Alloc>
  rpag_vec<T, Alloc> operator* (const T _lhs, const rpag_vec<T, Alloc> &_rhs)
  {
    return (_rhs * _lhs);
  }

template<typename T, typename Alloc>
  std::ostream & operator<< (std::ostream &s, const rpag_vec<T, Alloc>& vec)
  {
    typename rpag_vec<T, Alloc>::const_iterator iter;
    for(iter = vec.begin(); iter != vec.end();)
    {
      s << (*iter);
      ++iter;
      if(iter != vec.end())
      {s  << vec.stream_seperate_symbol;}
    }
    return s;
  }

template<typename T, typename Alloc>
  rpag_vec<T, Alloc> abs(const rpag_vec<T, Alloc> &rhs)
  {
    rpag_vec<T, Alloc> out(0);
    typename rpag_vec<T>::const_iterator it_r;
    {
      T dummi;
      for(it_r = rhs.begin(); it_r != rhs.end(); ++it_r)
      {
        dummi = *it_r;
        if(dummi < 0){dummi *= (-1);}
        out.push_back(dummi);

      }
      return out;
    }
  }


#endif // RPAG_VEC_H
