/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "rpag_functions.h"

#include "fundamental.h"
#include "log2_64.h"
#include "debug.h"
#include "csd.h"
#include "compute_successor_set.h"
#include "norm.h"
#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <string> // for rpag_atoll

int_t max_elem(const vec_t &rhs)
{
  return abs(rhs).max();
}

int_t max_elem(const int_t &rhs)
{
return rhs;
}

vec_t my_atoll(vec_t to, const char* input)
{
UNUSED(to);
char const* cut = "+-,";

int pos=0;
vec_t output(0);
std::string in = input;

while(pos != -1)
{
  if(*(in.begin()) == ',')
  {
    in = in.substr(1,in.size()-1);// delete the first element
  }
  pos = in.find_first_of(cut,1);
  output.push_back(atoll(in.substr(0,pos).c_str()));
  in.erase(0,pos);
}

vec_t::default_elem_count=output.size();
return norm(output);
}

int_t my_atoll(int_t to, const char* input)
{
UNUSED(to);
return atoll(input);
}


void test_target_size(const char* target, int &size)
{
vec_t dummi;
if(size == -1)// first run
{
  dummi = my_atoll(dummi, target);
  size = dummi.size();
}
else
{
  dummi = my_atoll(dummi, target);
  if(dummi.size() != (unsigned int)size)//now size has to be a positiv value.
  {
    std::cout << "ERROR: \t This is not a valid Paramet set! " << std::endl;
    std::cout << "\t All elements must have the same size!" << std::endl;
    std::cout << "\t Problem with:" << target << std::endl;
    std::cout << "\t For Example:{ 5+0 -7+3 0-3 } or { 5 7 99 } or { 5,1,2 7,-9,-2 }" << std::endl;
    exit(-1);
  }
}
}

string rpag_info(vector<string> &start_arguments, bool new_line)
{
  stringstream output_stream;
  string out;

  output_stream << "  last full compile \t\t = " << __DATE__ << " " << __TIME__;              if(new_line==true){ output_stream << std::endl << std::endl;}
  unsigned long long MAX_INT32_var  = MAX_INT32;
  unsigned long long MAX_UINT32_var = MAX_UINT32;

  output_stream << "  #" << "MAX_INT32 \t\t\t = "   << MAX_INT32_var;                          if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "MAX_UINT32 \t\t\t = "  << MAX_UINT32_var;                         if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "USE_NEW_NZ_COMPARE_FUNTION \t = " << USE_NEW_NZ_COMPARE_FUNTION; if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "USE_TOPOLOGY_E \t\t = " << USE_TOPOLOGY_E;                       if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "COST_FF_DEFAULT_ASIC \t = " << COST_FF_DEFAULT_ASIC;             if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "COST_FA_DEFAULT_ASIC \t = " << COST_FA_DEFAULT_ASIC;             if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "COST_FF_DEFAULT_FPGA \t = " << COST_FF_DEFAULT_FPGA;             if(new_line==true){ output_stream << std::endl;}
  output_stream << "  #" << "COST_FA_DEFAULT_FPGA \t = " << COST_FA_DEFAULT_FPGA;             if(new_line==true){ output_stream << std::endl;}

  output_stream << "  started with arguments\t = ";
  for(unsigned int i = 0; i < start_arguments.size(); ++i)
  {
    output_stream << start_arguments[i] << " ";
  }
  if(new_line==true){ output_stream << std::endl;}

  if(new_line==true)
  {
      out = output_stream.str();
  }
  else
  {
      out = output_stream.str();
      out.erase(std::remove(out.begin(), out.end(), '\t'),out.end());

      std::string::iterator space = out.begin();
      while(1)//remove all duble spaces
      {
        space = std::find(space, out.end(), ' ');
        if(space != out.end())
        {
          if((*(space+1)) == ' ')
          {
            out.erase(space,space+1);
          }
          else
          {
            space++;
          }
        }
        else
        {
          break;
        }
      }
  }


  return out;
}


int compute_k_max(int_t &r1, int_t &r2, int_t &c_max)
{
  return log2c_64((int_t) ceil(((double)c_max+r1)/((double)r2)));
}
int compute_k_max(int_t &r1, int_t &r2, int_t &r3, int_t &c_max)
{
  int_t r1_ = fundamental(r1);
  int_t r2_ = fundamental(r2);
  int_t r3_ = fundamental(r3);
  //set r1 to the maximum value
  { //to make r1 > r2 > r3
    int_t tmp;
    if(r3_ > r2_)
    {
        tmp = r2_;
        r2_ = r3_;
        r3_ = tmp;
    }
    if(r2_ > r1_)
    {
        tmp = r1_;
        r1_ = r2_;
        r2_ = tmp;
    }
    if(r3_ > r2_)
    {
        tmp = r2_;
        r2_ = r3_;
        r3_ = tmp;
    }
  }
  if(r1_ > c_max) return 0;

  int k_max;
  if(r3_ == 0)
    k_max = compute_k_max(r1_,r2_,c_max);
  else
    k_max = compute_k_max(r1_,r3_,c_max);

  if(k_max < 0)
  {
    std::cout << "ERROR: k_max < 0 " << std::endl;
    exit(-1);
    return 0;
  }
  else
  {
    return k_max;
  }
}

int compute_k_max(vec_t &r1, vec_t &r2, int_t &c_max)
{
  return log2c_64((int_t) max(ceil(((double)c_max+abs(r2).max())/((double)abs(r1).min(true))),ceil(((double)c_max+abs(r1).max())/((double)abs(r2).min(true)))));;
}
int compute_k_max(vec_t &r1, vec_t &r2, vec_t r3, int_t &c_max)
{
  int k_max;
  rpag_vec<double> r1_double;
  rpag_vec<double> r2_double;
  rpag_vec<double> r3_double;
  rpag_vec<double> c_max_double;

  r1_double     = abs(r1);
  r2_double     = abs(r2);
  r3_double     = abs(r3);
  c_max_double  = c_max;

  int kmax_12 = ceil((c_max+abs(r1_double).max())/(abs(r2_double).min(true)));
  int kmax_13 = ceil((c_max+abs(r1_double).max())/(abs(r3_double).min(true)));
  kmax_12 = max(kmax_12,kmax_13);

  int kmax_21 = ceil((c_max+abs(r2_double).max())/(abs(r1_double).min(true)));
  int kmax_23 = ceil((c_max+abs(r2_double).max())/(abs(r3_double).min(true)));
  kmax_21 = max(kmax_21,kmax_23);

  int kmax_31 = ceil((c_max+abs(r3_double).max())/(abs(r1_double).min(true)));
  int kmax_32 = ceil((c_max+abs(r3_double).max())/(abs(r2_double).min(true)));
  kmax_31 = max(kmax_31,kmax_32);

  k_max = log2c_64((int_t) max( max(kmax_12,kmax_21), kmax_31) );
  if(k_max < 0)
  {
    std::cout << "ERROR: k_max < 0 " << std::endl;
    exit(-1);
    return 0;
  }
  else
  {
    return k_max;
  }
}

void create_unit_element(vec_t &var, unsigned int i)
{
var[i] = 1;
}
void create_unit_element(int_t &var, unsigned int i)
{
UNUSED(i);
var = 1;
}


void create_null_element(vec_t &var)
{
  for(unsigned i=0; i < var.size(); i++)
    var[i] = 0;
}
void create_null_element(int_t &var)
{
  var = 0;
}

bool just_one_negative_sign(bool a, bool b, bool c)
{
  int counter=0;
  if(is_it_negative(a)){++counter;}
  if(is_it_negative(b)){++counter;}
  if(is_it_negative(c)){++counter;}

  if(counter > 1)
    return false;
  else
    return true;
}

bool just_one_negative_sign(int_t &a, int_t &b, int_t &c)
{
  int counter=0;
  if(is_it_negative(a)){++counter;}
  if(is_it_negative(b)){++counter;}
  if(is_it_negative(c)){++counter;}

  if(counter > 1)
    return false;
  else
    return true;
}

bool just_one_negative_sign(int &a, int &b, int &c)
{
  int counter=0;
  if(is_it_negative(a)){++counter;}
  if(is_it_negative(b)){++counter;}
  if(is_it_negative(c)){++counter;}

  if(counter > 1)
    return false;
  else
    return true;
}

bool just_one_negative_sign(vec_t &a, vec_t &b, vec_t &c)
{
  int counter=0;
  if(is_it_negative(a)){++counter;}
  if(is_it_negative(b)){++counter;}
  if(is_it_negative(c)){++counter;}

  if(counter > 1)
    return false;
  else
    return true;
}


bool skip_predecessor(const vec_t &w,const vec_t &p,const bool &dont_skip)
{
    //                                     _
    // for example:  (  not(o) is equal to o  )
    //w={0, 1, 0,13, 0, 0}
    //   _     _     _  _
    //=> o  o  o  o  o  o
    //
    //skip if at leest one not(o) is not zero or if all o are zero (in p)
    if(dont_skip){return false;}

    vec_t::const_iterator p_it=p.begin();
    for(vec_t::const_iterator w_it = w.begin(); w_it!=w.end();++w_it)//go thrue all elemnts of w and the corresponding eleemnts of p
    {
        if(*w_it) // if the element of w is not zero (o)
        {
            if(*p_it){return 0;}
        }
        else // if the elemt of w is zero => not(o)
        {
            if((*p_it)){return 1;}
        }

        ++p_it;//w will be increased also ...
    }
    return 1;
}

//do the sciping just in case of vectors
bool skip_predecessor(const int_t &w,const  int_t &p,const bool &dont_skip)
{
    UNUSED(w);
    UNUSED(p);
    UNUSED(dont_skip);
    return 0;
}



