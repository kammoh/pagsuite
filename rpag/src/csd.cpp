/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "csd.h"
#include "debug.h"
#include "log2_64.h"

#include <algorithm>

void dec2bin(int_t x, sd_t* csd)
{
  IF_VERBOSE(7) std::cout << "\n" << x << " in csd\n";
  //assert(x >= 0);
  if(x < 0){x *= -1;}// this is necessary because of the vector matrix multiplication, which have to work with negative numbers in a specific way.
  if(x!=0)
  {
    while(x != 0)
    {
      csd->push_back(x & 1);
      x = x >> 1;
    }
  }
  else
  {
    csd->push_back(0);
  }
}

int csd_replace(sd_t* csd, bool negativ)
{
  bool replace=false;
  sd_t::iterator iter;
  sd_t::iterator end_iter=csd->end();
  for (iter=csd->begin(); iter != end_iter; ++iter)
  {
    if(*iter == 1)
    {
      if(replace)
      {
        *iter = 0;
      }
      else if(iter+1 != end_iter)
      {
        if(*(iter+1) == 1)
        {
          *iter = -1;
          replace = true;
        }
      }
    }
    else if(replace && (*iter == 0))
    {
      *iter = 1;
      return 0;
    }
  }
  if(replace)
  {
    csd->push_back(1);
  }

  if(negativ)// for negativ numbers in vector matrix multiplication
  {
    end_iter=csd->end();
    for (iter=csd->begin(); iter != end_iter; ++iter)
    {
      if(*iter == 1){*iter = -1;}
      else if(*iter == -1){*iter = 1;}
    }
  }
  return 1;
}

int dec2csd(int_t x, sd_t* csd)
{
  bool negativ;
  if(x<0){negativ = true;}else{negativ = false;}

  dec2bin(x, csd);
  IF_VERBOSE(7) cout << x << ": bin=" << *csd << endl;

  while(!csd_replace(csd,negativ)) ;

  IF_VERBOSE(7) cout << x << ": csd=" << *csd << endl;
  return 1;
}

int nonzeros(sd_t *sd)
{
  sd_t::iterator iter;
  int nz=0;
  for (iter=sd->begin(); iter != sd->end(); ++iter)
  {
    if(*iter != 0)
      nz++;
  }
  return nz;
}

int nonzeros(int_t x)
{
  sd_t csd;
  dec2csd(x,&csd);
  return nonzeros(&csd);
}

int nonzeros(vec_t x)
{
  vec_t::iterator it;
  int count=0;
  for(it=x.begin(); it != x.end(); ++it)
  {
    count += nonzeros(*it);
  }
  return count;
}

int nonzeros(sd_vec_t& x)
{
  int nz=0;
  for(unsigned i=0;i<x.size();++i)
  {
    nz += nonzeros(&(x[i]));
  }
  return nz;
}

void count_digits(sd_t *sd, int &p, int &n) // count the positive and negative digits of the csd representation
{
  sd_t::iterator iter;
  for (iter=sd->begin(); iter != sd->end(); ++iter)
  {
    if(*iter < 0) ++n;
    if(*iter > 0) ++p;
  }
}

int nonzeros_for_sign_filter(int_t x)// to compute the correct adder depth in case of 3 input adder with reconfiguration
{
  int p = 0;
  int n = 0;
  sd_t msd;
  dec2csd(x,&msd);
  csd2msd_w_mnd(&msd);

  count_digits(&msd,p,n);

  if( p > (2*n)) {return (p+n);}
  else     {return (3*n) ;}

}
int nonzeros_for_sign_filter(vec_t x)// to compute the correct adder depth in case of 3 input adder with reconfiguration
{
  int p = 0;
  int n = 0;
  int p_ = 0;//to switch the sign of negative elements
  int n_ = 0;//to switch the sign of negative elements

  for(unsigned i = 0; i < x.size(); ++i)
  {
      sd_t msd;
      if(x[i] >= 0)
      {
        dec2csd(x[i],&msd);
        csd2msd_w_mnd(&msd);// to minimize the negative digits whithout incresing the whole count of digits.

        count_digits(&msd,p,n);
      }
      else
      {
        p_ = 0;
        n_ = 0;
        dec2csd(-x[i],&msd);
        //csd2msd_w_mnd(&msd); The sign will be switched agan. So every negative one is positive agan. So use as much negative signs as posible.

        count_digits(&msd,p_,n_);
        p += n_;
        n += p_;
      }
  }

  if(p > (2*n)) {return (p+n);}
  else     {return (3*n) ;}

}
void csd2msd_w_mnd(sd_t *msd)
{
  if(msd->size() >= 2)
  {
    for (unsigned int i=0; i < msd->size()-2; ++i)
    {
        if(((*msd)[i] == -1) && ((*msd)[i+1] == 0) && ((*msd)[i+2] == 1))
        {

          (*msd)[i]=1; (*msd)[i+1]=1; (*msd)[i+2]=0;
        }
    }
    if((*msd)[msd->size()-1] == 0)//if the higest digitis is zero now it has to be erased.
    {
        msd->pop_back();
    }
  }
}

void dec2msd(int_t x, sd_set_t* msd_set)
{
  sd_t msd,msd_next;
  sd_set_t msd_next_set;

  dec2csd(x,&msd);

  IF_VERBOSE(6) cout << "csd representation of " << x << ": " << msd << endl;

  msd_set->insert(msd);
  msd_next_set = *msd_set;

  do
  {
    msd = (*msd_next_set.begin());
    msd_next_set.erase(msd);

    for (unsigned int i=0; i < msd.size()-2; i++)
    {
      if((msd[i]==-1) && (msd[i+1]==0) && (msd[i+2]==1))
      {
        msd_next = msd;
        msd_next[i]=1;
        msd_next[i+1]=1;
        msd_next[i+2]=0;
        IF_VERBOSE(6) cout << "new msd representation of " << x << " found: " << msd_next << endl;
        msd_next_set.insert(msd_next);
        msd_set->insert(msd_next);
      }
      else if((msd[i]==1) && (msd[i+1]==0) && (msd[i+2]==-1))
      {
        msd_next = msd;
        msd_next[i]=-1;
        msd_next[i+1]=-1;
        msd_next[i+2]=0;
        IF_VERBOSE(6) cout << "new msd representation of " << x << " found: " << msd_next << endl;
        msd_next_set.insert(msd_next);
        msd_set->insert(msd_next);
      }
    }
  } while(!msd_next_set.empty());

  IF_VERBOSE(5) cout << "msd_set=" << *msd_set << endl;
}

void vec2msd_vec(vec_t x, sd_vec_set_t* msd_vec_set, long iteration_max)
{
  sd_t msd;
  sd_vec_t msd_vec,msd_vec_next;
  sd_vec_set_t msd_next_set;
  long long iteration_counter=0;

  vec_t::iterator it;
  for(it = x.begin(); it!= x.end(); ++it)
  {
    msd.clear();
    dec2csd(*it,&msd);
    msd_vec.push_back(msd);
  }

  IF_VERBOSE(6) cout << "csd representation of " << x << ": " << msd_vec << endl;

  msd_vec_set->insert(msd_vec);
  msd_next_set = *msd_vec_set;

  do
  {
    msd_vec = (*msd_next_set.begin());
    msd_next_set.erase(msd_vec);

    for(unsigned int j=0; j < msd_vec.size(); ++j)
    {
      if(msd_vec[j].size()>2)
      {
        for(unsigned int i=0; i < msd_vec[j].size()-2; ++i)
        {
          ++iteration_counter;
          if((msd_vec[j][i]==-1) && (msd_vec[j][i+1]==0) && (msd_vec[j][i+2]==1))
          {
            msd_vec_next = msd_vec;
            msd_vec_next[j][i]=1;
            msd_vec_next[j][i+1]=1;
            msd_vec_next[j][i+2]=0;
            IF_VERBOSE(6) cout << "new msd representation of " << x << " from " << msd_vec << " found: " << msd_vec_next << endl;
            msd_next_set.insert(msd_vec_next);
            msd_vec_set->insert(msd_vec_next);
          }
          else if((msd_vec[j][i]==1) && (msd_vec[j][i+1]==0) && (msd_vec[j][i+2]==-1))
          {
            msd_vec_next = msd_vec;
            msd_vec_next[j][i]=-1;
            msd_vec_next[j][i+1]=-1;
            msd_vec_next[j][i+2]=0;
            IF_VERBOSE(6) cout << "new msd representation of " << x << " from " << msd_vec << " found: " << msd_vec_next << endl;
            msd_next_set.insert(msd_vec_next);
            msd_vec_set->insert(msd_vec_next);
          }
        }
      }
    }
  } while(!msd_next_set.empty() && (( iteration_counter < iteration_max)||(iteration_max == -1)));
}

void msd_vec2vec(sd_vec_t& msd_vec, vec_t& vec)
{
  assert(vec.size() >= msd_vec.size());
  for(unsigned i=0; i < msd_vec.size();++i)
  {
    vec[i]=0;
    for(unsigned j=0; j < msd_vec[i].size();++j)
    {
      vec[i] += msd_vec[i][j] << j;
    }
  }
}

int compute_word_size(vec_t vec, int input_wordsize)
{
  int_t sumOfFactors=0;
  for(unsigned i=0; i < vec.size(); i++)
  {
    sumOfFactors += abs(vec[i]);
  }
  return input_wordsize+log2c_64(sumOfFactors);
}

int compute_word_size(int_t x, int input_wordsize)
{
  return input_wordsize+log2c_64(x);
}



