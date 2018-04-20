/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef RPAG_FUNCTIONS
#define RPAG_FUNCTIONS

#include <cmath>

#include "type_class.h"
#include "fundamental.h"

template <class T>
class realization_row
{
public:
  realization_row();
  virtual ~realization_row();

  bool use_ternary_adders();
  char type();
  unsigned int size();
  string operator[](unsigned int i);
  bool operator== (const realization_row<T> rhs) const;
  const realization_row<T> operator=(const realization_row<T> rhs);
  string to_ss_standard();
  string to_ss_matlab();

  T W;
  T A;
  T B;
  T C;

  int stageW;
  int stageA;
  int stageB;
  int stageC;

  int eA;
  int eB;
  int eC;
};

template<class T>
realization_row<T>::realization_row()
{
  W=0;
  A=0;
  B=0;
  C=0;

  stageW=0;
  stageA=0;
  stageB=0;
  stageC=0;

  eA=0;
  eB=0;
  eC=0;
}

template<class T>
realization_row<T>::~realization_row()
{}

template<class T>
bool realization_row<T>::use_ternary_adders()
{
  if(C == 0)
    return false;
  else
    return true;
}

template<class T>
char realization_row<T>::type()
{
  //if((stageW == stageA) && (W == A)) //with this line, shifts at the output crashes!
  if(stageW == stageA)
  {
    return 'O';
  }
  else if((B == 0)&&(C == 0))
  {
    return 'R';
  }
  else
  {
    return 'A';
  }
}

template<class T>
unsigned int realization_row<T>::size()
{
  if(use_ternary_adders())
    return 11;
  else
    return 8;
}

template<class T>
string realization_row<T>::operator[](unsigned int i)
{
  stringstream s;
  switch(i)
  {
    case 0:  s << W; break;
    case 1:  s << stageW; break;
    case 2:  s << A; break;
    case 3:  s << stageA; break;
    case 4:  s << eA; break;
    case 5:  s << B; break;
    case 6:  s << stageB; break;
    case 7:  s << eB; break;
    case 8:  s << C; break;
    case 9:  s << stageC; break;
    case 10: s << eC; break;
    default: std::cout << "realization_row operator[] undefined argument" << std::endl;  exit(-1);
  }
  return s.str();
}

template<class T>
bool realization_row<T>::operator== (const realization_row<T> rhs) const
{
  return(
  (W == rhs.W)&&
  (A == rhs.A)&&
  (B == rhs.B)&&
  (C == rhs.C)&&
  (stageW == rhs.stageW)&&
  (stageA == rhs.stageA)&&
  (stageB == rhs.stageB)&&
  (stageC == rhs.stageC)&&
  (eA == rhs.eA)&&
  (eB == rhs.eB)&&
  (eC == rhs.eC));
}

template<class T>
const realization_row<T> realization_row<T>::operator=(const realization_row<T> rhs)
{
  W = rhs.W;
  A = rhs.A;
  B = rhs.B;
  C = rhs.C;

  stageW = rhs.stageW;
  stageA = rhs.stageA;
  stageB = rhs.stageB;
  stageC = rhs.stageC;

  eA = rhs.eA;
  eB = rhs.eB;
  eC = rhs.eC;
  return (*this);
}


template<class T>
string realization_row<T>::to_ss_standard()
{
  stringstream s;
  if(type() == 'O')
  {
    s << "{\'O\',[" << W << "]," << stageW << ",[" << A << "]," << stageA << "," << eA << "}";
  }
  else if(type() == 'R')
  {
    s << "{\'R\',[" << W << "]," << stageW << ",[" << A << "]," << stageA << "}";
  }
  else if(type() == 'A')
  {
    s << "{\'A\',[" << W << "]," << stageW << ",[" << A << "]," << stageA << "," << eA << ",[" << B << "]," << stageB << "," << eB;
    if(C != 0) {s << ",[" << C << "]," << stageC << "," << eC;}//for ternary adders
    s << "}";
  }
  else
  {
    std::cout << "ERROR in realization_row::to_ss_standard()" << std::endl;
    std::cout << "the row is not a Register or an adder" << std::endl;
    exit(-1);
  }
  return s.str();
}

template<class T>
string realization_row<T>::to_ss_matlab()
{
  stringstream s;
  if(vec_t::default_elem_count != 1)// this was set to the number of elements in the vector if it is used. the default case is 1;
  {// CMM
    s << "{[" << W << "]," << stageW << ",[" << A << "]," << stageA << "," << eA << ",[" << B << "]," << stageB << "," << eB;
    if(C != 0) {s << ",[" << C << "]," << stageC << "," << eC;}//for ternary adders
  }
  else
  {//MCM
    s << "{" << W << "," << stageW << "," << A << "," << stageA << "," << eA << "," << B << "," << stageB << "," << eB;
    if(C != 0) {s << "," << C << "," << stageC << "," << eC;}//for ternary adders
  }
  s << "}";
  return s.str();
}


int_t max_elem(const vec_t &rhs);
int_t max_elem(const int_t &rhs);

vec_t my_atoll(vec_t to, const char* input);
int_t my_atoll(int_t to, const char* input);

void test_target_size(const char*, int &size);

string rpag_info(vector<string> &start_arguments, bool new_line = true);

int compute_k_max(int_t &r1, int_t &r2, int_t &c_max);
int compute_k_max(int_t &r1, int_t &r2, int_t &r3, int_t &c_max);
int compute_k_max(vec_t &r1, vec_t &r2, int_t &c_max);
int compute_k_max(vec_t &r1, vec_t &r2, vec_t r3, int_t &c_max);

bool just_one_negative_sign(bool a, bool b, bool c);
bool just_one_negative_sign(int &a, int &b, int &c);
bool just_one_negative_sign(vec_t &a, vec_t &b, vec_t &c);
bool just_one_negative_sign(int_t &a, int_t &b, int_t &c);

void create_unit_element(vec_t &var, unsigned int i);
void create_unit_element(int_t &var, unsigned int i);

void create_null_element(vec_t &var);
void create_null_element(int_t &var);


template <class T>
bool getExponents(T &a,T &b, T &w, int *eA, int *eB, int *signA, int *signB)
{
  bool exponentsFound=false;
  int e; //exponent

  //check left shift solutions:
  if(fundamental_count(w-a,e) == b)
  {
    IF_VERBOSE(8)  cout << "case 01" << endl;
    //c = a + 2^eB b => fundamental(c-a) == b
    *eA=0; *eB=e; *signA=1; *signB=1; exponentsFound=true;
  }
  else if(fundamental_count(w-b,e) == a)
  {
    IF_VERBOSE(8)  cout << "case 02" << endl;
    //c = 2^eA a + b => fundamental(c-b) == a
    *eA=e; *eB=0; *signA=1; *signB=1; exponentsFound=true;
  }
  else if(fundamental_count(w+a,e) == b)
  {
    IF_VERBOSE(8)  cout << "case 03" << endl;
    //c = -a + 2^eB b => fundamental(c+a) == b
    *eA=0; *eB=e; *signA=-1; *signB=1; exponentsFound=true;
  }
  else if(-fundamental_count(w-a,e) == b)
  {
    IF_VERBOSE(8)  cout << "case 04" << endl;
    //c = a - 2^eB b => -fundamental(c-a) == b
    *eA=0; *eB=e; *signA=1; *signB=-1; exponentsFound=true;
  }
  else if(fundamental_count(b-w,e) == a)
  {
    IF_VERBOSE(8)  cout << "case 05" << endl;
    //c = -2^eA a + b => fundamental(b-c) == a
    *eA=e; *eB=0; *signA=-1; *signB=1; exponentsFound=true;
  }
  else if(fundamental_count(w+b,e) == a)
  {
    IF_VERBOSE(8)  cout << "case 06" << endl;
    //c = 2^eA a - b => fundamental(c+b) == a
    *eA=e; *eB=0; *signA=1; *signB=-1; exponentsFound=true;
  }
  //check right shift solutions:
  else if(fundamental_count(a+b,e) == w)
  {
    IF_VERBOSE(8)  cout << "case 07" << endl;
    //c = 2^-eA a + 2^-eB b (with eA==eB)
    *eA = -e; *eB = *eA; *signA = 1; *signB = 1;
    exponentsFound=true;
  }
  else if(fundamental_count((a-b),e) == w)
  {
    IF_VERBOSE(8)  cout << "case 08" << endl;
    //c = 2^-eA a - 2^-eB b (with eA==eB)
    *eA = -e; *eB = *eA;
    *signA = 1; *signB = -1;
    exponentsFound=true;
  }
  else if(fundamental_count((b-a),e) == w)
  {
    IF_VERBOSE(8)  cout << "case 09" << endl;
    //c = -2^-eA a + 2^-eB b (with eA==eB)
    *eA = -e; *eB = *eA;
    *signA = -1; *signB = 1;
    exponentsFound=true;
  }

  if(exponentsFound)
  {
    //check results:
    if(*eA >= 0)
    {
      if(!((*signA * (a << (*eA))) + *signB * (b << (*eB)) == w))
      {
        cout << "Error: Exponens invalid: (" << *signA << ")*" << a << "*2^" << (*eA) << " + (" << *signB << ")*" << b << "*2^" << (*eB) << "=" << (*signA * (a << (*eA))) + *signB * (b << (*eB)) << " != " << w << endl;
        exit(-1);
      }
      return true;
    }
    else
    {
      if(!(((*signA * a + *signB * b) >> abs(*eA)) == w))
      {
        cout << "Error: Exponens invalid: " << ((*signA * a + *signB * b) >> abs(*eA)) << " != " << w << endl;
        exit(-1);
      }
      return true;
    }
  }
  else
  {
    // this case is possible until searching for a adder graph
    return false;
  }
}


template <class T>
bool getExponents_for_sign_filter(T &a,T &b, T &w, int *eA, int *eB, int *signA, int *signB, int use_input)
{
  bool exponentsFound=false;
  int e; //exponent
  int use=0;

  //check left shift solutions:
  if(fundamental_count(w-a,e) == b)
  {
    ++use;
    if(use == use_input)
    {
      //IF_VERBOSE(8)  cout << "case 01" << endl;
      //c = a + 2^eB b => fundamental(c-a) == b
      *eA=0; *eB=e; *signA=1; *signB=1; exponentsFound=true;
      IF_VERBOSE(8)  cout << "case 01" << "w=" << w << "a=" <<a << "b=" << b << "eA =" << *eA << "eB =" << *eB<< "signA =" << *signA<< "signB =" << *signB << endl;
    }
  }
  if(fundamental_count(w-b,e) == a)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 02" << endl;
      //c = 2^eA a + b => fundamental(c-b) == a
      *eA=e; *eB=0; *signA=1; *signB=1; exponentsFound=true;
    }
  }
  if(fundamental_count(w+a,e) == b)
  {
  ++use;
  if(use == use_input)
  {
      IF_VERBOSE(8)  cout << "case 03" << endl;
      //c = -a + 2^eB b => fundamental(c+a) == b
      *eA=0; *eB=e; *signA=-1; *signB=1; exponentsFound=true;
    }
  }
  if(-fundamental_count(w-a,e) == b)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 04" << endl;
      //c = a - 2^eB b => -fundamental(c-a) == b
      *eA=0; *eB=e; *signA=1; *signB=-1; exponentsFound=true;
    }
  }
  if(fundamental_count(b-w,e) == a)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 05" << endl;
      //c = -2^eA a + b => fundamental(b-c) == a
      *eA=e; *eB=0; *signA=-1; *signB=1; exponentsFound=true;
    }
  }
  if(fundamental_count(w+b,e) == a)
  {
    ++use;
    if(use == use_input)
    {
      //IF_VERBOSE(8)  cout << "case 06" << endl;
      //c = 2^eA a - b => fundamental(c+b) == a
      *eA=e; *eB=0; *signA=1; *signB=-1; exponentsFound=true;
      IF_VERBOSE(8)  cout << "case 06" << "w=" << w << "a=" <<a << "b=" << b << "eA =" << *eA << "eB =" << *eB<< "signA =" << *signA<< "signB =" << *signB << endl;
    }
  }
  //check right shift solutions:
  if(fundamental_count(a+b,e) == w)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 07" << endl;
      //c = 2^-eA a + 2^-eB b (with eA==eB)
      *eA = -e; *eB = *eA; *signA = 1; *signB = 1;
      exponentsFound=true;
    }
  }
  if(fundamental_count((a-b),e) == w)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 08" << endl;
      //c = 2^-eA a - 2^-eB b (with eA==eB)
      *eA = -e; *eB = *eA;
      *signA = 1; *signB = -1;
      exponentsFound=true;
    }
  }
  if(fundamental_count((b-a),e) == w)
  {
    ++use;
    if(use == use_input)
    {
      IF_VERBOSE(8)  cout << "case 9" << endl;
      //c = -2^-eA a + 2^-eB b (with eA==eB)
      *eA = -e; *eB = *eA;
      *signA = -1; *signB = 1;
      exponentsFound=true;
    }
  }

  if(exponentsFound)
  {
    //check results:
    if(*eA >= 0)
    {
      if(!((*signA * (a << (*eA))) + *signB * (b << (*eB)) == w))
      {
        cout << "Error: Exponens invalid: (" << *signA << ")*" << a << "*2^" << (*eA) << " + (" << *signB << ")*" << b << "*2^" << (*eB) << "=" << (*signA * (a << (*eA))) + *signB * (b << (*eB)) << " != " << w << endl;
        exit(-1);
      }
      return true;
    }
    else
    {
      if(!(((*signA * a + *signB * b) >> abs(*eA)) == w))
      {
        cout << "Error: Exponens invalid: " << ((*signA * a + *signB * b) >> abs(*eA)) << " != " << w << endl;
        exit(-1);
      }
      return true;
    }
  }
  else
  {
    // this case is possible until searching for a adder graph
    return false;
  }
}

// nessesary for getExponents(3 adder) to support vec_t and int_t
inline bool is_it_negative(int_t Z){return (Z < 0);}
inline bool is_it_negative(vec_t Z)
{
    for(int_t i: Z)
    {
        if(i != 0){return (i < 0);}
    }
    return 0;
}
inline bool is_it_negative(int Z){return (Z < 0);}// nessesary for sign_filter option
inline bool is_it_negative(bool Z){return Z;}// nessesary for sign_filter option // false = positive sign; true= negative sign

template<typename T>
bool getExponents(T &a, T &b, T &c, T &w, int *eA, int *eB, int *eC, int *signA, int *signB, int *signC, int_t c_max, bool ternary_sign_filter)
{
  T Z;
  int k_max  = compute_k_max(a,b,c,c_max);
  bool found = false;
  bool neg   = false;
  int permut = 0;

  for(int r_shift = 0; r_shift <= k_max; ++r_shift)
  {
    for(permut = 0; permut <= 2; ++permut)// right shifts can only be detected between a and b. If ther is one between c and another thy have to be switched.
    {
      switch (permut)
      {
        case 0: break;//initial set. test for right shift beetween the original a and b.
        case 1: swap(a,c); swap(*eA,*eC); swap(*signA,*signC); break; // test for right shift beetween the original c and b.
        case 2: swap(b,c); swap(*eB,*eC); swap(*signB,*signC); break; // test for right shift beetween the original c and a.
        default: std::cout << "error in getExponents permut is not 0,1 or 2" << endl; exit(-1);
      }

      for ((*eC)=0; (*eC) <= k_max; ++(*eC))
      {
        for(int sign_loop=0; sign_loop <= 1; ++sign_loop)
        {
          switch (sign_loop)
          {
            case 0: (*signC) =  1; break;
            case 1: (*signC) = -1; break;
            default: std::cout << "error in getExponents sign_loop is not 0 or 1" << endl; exit(-1);
          }

          //Z = w - ((c << (*eC)) * (*signC));
          Z = (w << (r_shift)) - ((c << (*eC)) * (*signC));

          if (is_it_negative(Z))// this have to be different for the rpag_vector
          {
            Z = -Z;//the getExponent function does not support negativ values.
            neg = true;
          }
          else
          {
            neg = false;
          }

          if(ternary_sign_filter)
          {
            int use_solution = 1;
            do
            {
              if(Z % 2 == 0)// if Z is evan
              {
                T Z_odd;
                int eZ_odd;
                Z_odd = fundamental_count(Z, eZ_odd);
                found = getExponents_for_sign_filter(a,b,Z_odd,eA,eB,signA,signB,use_solution);

                if(neg)//the getExponent function does not support negativ values.
                {
                  (*signA) = -(*signA);
                  (*signB) = -(*signB);
                }
                (*eA) = (*eA) + eZ_odd;
                (*eB) = (*eB) + eZ_odd;
              }
              else // if Z is odd
              {
                found = getExponents_for_sign_filter(a,b,Z,eA,eB,signA,signB,use_solution);
                if(neg)//the getExponent function does not support negativ values.
                {
                  (*signA) = -(*signA);
                  (*signB) = -(*signB);
                }
              }

              ++use_solution;
            }while( (found == true) && (use_solution < 10) && (just_one_negative_sign((*signA),(*signB),(*signC)) == false) );

            if(just_one_negative_sign((*signA),(*signB),(*signC)) == false) {found = false;}
          }
          else
          {
            if(Z % 2 == 0)// if Z is evan
            {
              T Z_odd;
              int eZ_odd;
              Z_odd = fundamental_count(Z, eZ_odd);
              found = getExponents(a,b,Z_odd,eA,eB,signA,signB);

              if(neg)//the getExponent function does not support negativ values.
              {
                (*signA) = -(*signA);
                (*signB) = -(*signB);
              }
              (*eA) = (*eA) + eZ_odd;
              (*eB) = (*eB) + eZ_odd;
            }
            else // if Z is odd
            {
              found = getExponents(a,b,Z,eA,eB,signA,signB);

              if(neg)//the getExponent function does not support negativ values.
              {
                (*signA) = -(*signA);
                (*signB) = -(*signB);
              }
            }
          }
          if(((*eA) > k_max) || ((*eB) > k_max) || ((*eC) > k_max)){found = false;}
          if(found == true)
          {
              (*eA) -= r_shift;
              (*eB) -= r_shift;
              (*eC) -= r_shift;
              switch (permut)
              {
                case 2: swap(b,c); swap(*eB,*eC); swap(*signB,*signC); // switch b&c and a&c to create the original set
                case 1: swap(a,c); swap(*eA,*eC); swap(*signA,*signC); // switch         a&c to create the original set
                case 0: break;                                         // initial set.                     no switching
                default: std::cout << "error in getExponents permut is not 0,1 or 2" << endl; exit(-1);
              }
              return true;
          }
        }
      }
    }
  }

  //the variable's has to be in the correct order.
  swap(b,c); swap(*eB,*eC); swap(*signB,*signC);
  swap(a,c); swap(*eA,*eC); swap(*signA,*signC);

  return false;
}

template<typename T>
bool getExponents(realization_row<T>& row, int_t c_max, bool ternary_sign_filter)
{
  int signA, signB, signC;

  if(getExponents(row.A, row.B, row.W, &row.eA, &row.eB, &signA, &signB))
  {
    row.A *= signA;
    row.B *= signB;
    row.C = 0; //two-input adder is possible, so reset C to zero

    return true;
  }
  else if(getExponents(row.A, row.B, row.C, row.W, &row.eA, &row.eB, &row.eC, &signA, &signB, &signC, c_max, ternary_sign_filter))
  {
    row.A *= signA;
    row.B *= signB;
    row.C *= signC;

    return true;
  }
  else
  {
    return false;
  }
}

template <class T>
void pipeline_set_to_adder_graph(vector< set<T> > &pipeline_set, list< realization_row<T> > &pipelined_adder_graph, bool is_this_a_two_input_system, int_t c_max, bool ternarry_sign_filter=false)
{
  bool ternary_adders = !is_this_a_two_input_system; //the funktion ask for a two input system. And this funktion works with a ternary adder flag... so it has to be inverted

  IF_VERBOSE(8) cout << "pipeline_set=" << pipeline_set << endl;
  typename set<T>::const_iterator working_set_iter,predecessor_set_iter1,predecessor_set_iter2, predecessor_set_iter3;
  int eA, eB, eC, signA, signB, signC;
  T a,b,c,w;
  bool exponentsFound;

  {// to delete base_set when it is not needed anymore.
    set<T> base_set;
    for(unsigned int i=0; i < vec_t::default_elem_count; ++i)
    {
        T base;
        base = 0;// it is not possible to set the elemts before they are created.
        create_unit_element(base,i);
        base_set.insert(base);
    }
    pipeline_set.insert(pipeline_set.begin(),base_set);
  }

  for(unsigned int s = 1; s < pipeline_set.size(); s++)
  {
    for(working_set_iter = pipeline_set[s].begin(); working_set_iter != pipeline_set[s].end(); ++working_set_iter)
    {
      exponentsFound=false;
      w=*working_set_iter;// should not called c, because c is used in case of 3 addder's.( posseble w? than a &b vor 2 adders and a, b and c for 3 adders) should we chanche the name in the getExponent funktion? the it is unic
      {
        //check for register:
        c=0; signC=1; eC=0;
        for(predecessor_set_iter1 = (pipeline_set)[s-1].begin(); predecessor_set_iter1 != (pipeline_set)[s-1].end(); ++predecessor_set_iter1)
        {
          if(w == *predecessor_set_iter1)
          {
            a=*predecessor_set_iter1;
            signA=1; eA=0;
            b=0; signB=1; eB=0;
            exponentsFound = true;
            break;
          }
        }

        if(!exponentsFound)
        { 
          //it is possible that there is a two adder solution for three adders so the two adders have to be tested.
          signC = 0; c=0; eC=0;
          for(predecessor_set_iter1 = (pipeline_set)[s-1].begin(); predecessor_set_iter1 != (pipeline_set)[s-1].end(); ++predecessor_set_iter1)
          {
            for(predecessor_set_iter2 = predecessor_set_iter1; predecessor_set_iter2 != (pipeline_set)[s-1].end(); ++predecessor_set_iter2)
            {
              a=*predecessor_set_iter1;
              b=*predecessor_set_iter2;
              exponentsFound = getExponents(a,b,w, &eA, &eB, &signA, &signB);
              if(exponentsFound) break;
            }
            if(exponentsFound) break;
          }

          if(ternary_adders && (!exponentsFound))
          {
            //new loop for the third elemnt and getExponent(3 adder)
            for(predecessor_set_iter1 = (pipeline_set)[s-1].begin(); predecessor_set_iter1 != (pipeline_set)[s-1].end(); ++predecessor_set_iter1)
            {
              for(predecessor_set_iter2 = predecessor_set_iter1; predecessor_set_iter2 != (pipeline_set)[s-1].end(); ++predecessor_set_iter2)
              {
                for(predecessor_set_iter3 = predecessor_set_iter2; predecessor_set_iter3 != (pipeline_set)[s-1].end(); ++predecessor_set_iter3)
                {
                  a = *predecessor_set_iter1;
                  b = *predecessor_set_iter2;
                  c = *predecessor_set_iter3;
                  exponentsFound = getExponents(a,b,c,w, &eA, &eB, &eC, &signA, &signB, &signC, c_max, ternarry_sign_filter);
                  if(ternarry_sign_filter && !just_one_negative_sign(signA,signB,signC)) { exponentsFound = false; }//do not use ternarry adders with more than one negative sign;
                  if(exponentsFound) break;
                }
                if(exponentsFound) break;
              }
              if(exponentsFound) break;
            }
          }
        }
      }

      if(exponentsFound)
      {
        if(ternary_adders)
        {
          IF_VERBOSE(8) cout << w << " (stage " << s << ") = " << signA << " * " << a << " *2^" << eA << " + " << signB << " * " << b << " *2^" << eB << endl << signC << " * " << c << " *2^" << eC << endl;
          a = signA * a;
          b = signB * b;
          c = signC * c;

          //normalize sign
          if((signA < 0)&&(signB >= 0))
          {
            swap(a,b);
            swap(eA,eB);
          }
          else if((signA < 0)&&(signB < 0))
          {
            swap(a,c);
            swap(eA,eC);
          }

          realization_row<T> row;
          row.W       = w;
          row.stageW  = s;
          row.A       = a;
          row.stageA  = s-1;
          row.eA      = eA;
          row.B       = b;
          row.stageB  = s-1;
          row.eB      = eB;
          row.C       = c;
          row.stageC  = s-1;
          row.eC      = eC;
          pipelined_adder_graph.push_back(row);

        }
        else
        {
          IF_VERBOSE(8) cout << w << " (stage " << s << ") = " << signA << " * " << a << " *2^" << eA << " + " << signB << " * " << b << " *2^" << eB << endl;
          a = signA * a;
          b = signB * b;

          //normalize sign
          if(signA < 0)
          {
            swap(a,b);
            swap(eA,eB);
          }

          realization_row<T> row;
          row.W       = w;
          row.stageW  = s;
          row.A       = a;
          row.stageA  = s-1;
          row.eA      = eA;
          row.B       = b;
          row.stageB  = s-1;
          row.eB      = eB;
          pipelined_adder_graph.push_back(row);
        }
      }
      else
      {
        cout << "Error: No solution could be found to realize " << w << endl;
        exit(-1);
      }
    }
  }
}

template <class T>
void validate_adder_graph(list< realization_row<T> >& adder_graph)
{
  typename list< realization_row<T> >::iterator iter;
  for(iter = adder_graph.begin(); iter != adder_graph.end(); ++iter)
  {
//    if((*iter).W != ((*iter).A << (*iter).eA) + ((*iter).B << (*iter).eB) + ((*iter).C << (*iter).eC))
    if(((double) (*iter).W) != (*iter).A * pow(2.0,(double) (*iter).eA) + (*iter).B * pow(2.0,(double) (*iter).eB) + (*iter).C *pow(2.0,(double) (*iter).eC))
    {
      cerr << "Error: adder graph " << (*iter).to_ss_standard() << " is inconsistent" << endl;
      exit(-1);
    }
  }
}

template <class T>
void append_targets_to_adder_graph(vector< set<T> > &pipeline_set, list< realization_row<T> > &pipelined_adder_graph, set<T> &target_set)
{
  //determine no. of stages:
  int no_of_pipeline_stages=0;
  typename list< realization_row<T> >::iterator iter;
  for(iter = pipelined_adder_graph.begin(); iter != pipelined_adder_graph.end(); ++iter)
  {
    if(no_of_pipeline_stages < (*iter).stageW)
      no_of_pipeline_stages = (*iter).stageW;
  }

  //append output coefficients:
  for(typename set<T>::iterator t_iter = target_set.begin(); t_iter != target_set.end(); ++t_iter)
  {
    T w = *t_iter;

    if(w == 0) continue; //skip zero ouputs

    int shift;
    T t_fun;
    t_fun = fundamental_count(w, shift);

    bool wFound=false;
    for(unsigned s=0; s <= no_of_pipeline_stages; s++)
    {
        if(pipeline_set[s].find(w) != pipeline_set[s].end())
        {
            wFound=true;
        }
        else
        {
            if(wFound)
            {
                //w was computed before and has to be repeaded now until the output
                realization_row<T> row;
                row.W       = w;
                row.stageW  = s;
                row.A       = t_fun;
                row.stageA  = s-1;
                row.eA      = shift;

                IF_VERBOSE(4) cout << "adding target: " << row.to_ss_matlab() << endl;
                pipelined_adder_graph.push_back(row);
            }
        }
    }

    realization_row<T> row;
    row.W       = w;
    row.stageW  = no_of_pipeline_stages;
    row.A       = t_fun;
    row.stageA  = no_of_pipeline_stages;
    row.eA      = shift;

    IF_VERBOSE(4) cout << "adding target: " << row.to_ss_matlab() << endl;
    pipelined_adder_graph.push_back(row);

  }
}

template <class T>
void remove_redundant_inputs_from_adder_graph(list< realization_row<T> > &pipelined_adder_graph, vector< set<T> > &pipeline_set_best)
{
    //prepare a set of all bases (input vectors):
    set<T> base_set;
    for(unsigned int i=0; i < vec_t::default_elem_count; ++i)
    {
        T base;//in CMM case, this is a Vector of Dimension 1 filled with zeros
        base = 0;// it is not possible to set the elements before they are created.
        create_unit_element(base,i);
        base_set.insert(base);
    }

    T null_element;
    create_null_element(null_element);

    vector<set<T> > required_base_nodes;
    required_base_nodes.resize(pipeline_set_best.size());

    typename list< realization_row<T> >::iterator it;
    for(it = pipelined_adder_graph.begin(); it != pipelined_adder_graph.end(); ++it)
    {
        realization_row<T> & row = *it;
        IF_VERBOSE(4) cout << "processing row: " << row.to_ss_matlab() << endl;

        //check whether element is an input or not:
        if(base_set.find(row.W) == base_set.end())
        {
            IF_VERBOSE(5) cout << "  row is not an input" << endl;
            if(row.stageA > 0)
            {
                if(base_set.find(norm(row.A)) != base_set.end())
                {
                    IF_VERBOSE(5) cout << "    input " << row.A << " is required in stage " << row.stageA << endl;
                    for(unsigned s=row.stageA; s > 0; s--)
                        required_base_nodes[s].insert(norm(row.A));
                }
                if(norm(row.B) != null_element)
                {
                    if(base_set.find(norm(row.B)) != base_set.end())
                    {
                        IF_VERBOSE(5) cout << "    input " << row.B << " is required in stage " << row.stageB << endl;
                        for(unsigned s=row.stageB; s > 0; s--)
                            required_base_nodes[s].insert(norm(row.B));
                    }
                }
                if(norm(row.C) != null_element)
                {
                    if(base_set.find(norm(row.C)) != base_set.end())
                    {
                        IF_VERBOSE(5) cout << "    input " << row.C << " is required in stage " << row.stageC << endl;
                        for(unsigned s=row.stageC; s > 0; s--)
                            required_base_nodes[s].insert(norm(row.C));
                    }
                }
            }
        }
    }

    //now, filter out all nodes that are not in required_base_nodes
    for(it = pipelined_adder_graph.begin(); it != pipelined_adder_graph.end();)
    {
        realization_row<T> & row = *it;
        IF_VERBOSE(6) cout << "processing row: " << row.to_ss_matlab() << endl;
        if(base_set.find(norm(row.W)) != base_set.end())
        {
            IF_VERBOSE(5) cout << "  row is an input" << endl;
            if(required_base_nodes[row.stageW].find(norm(row.W)) == required_base_nodes[row.stageW].end())
            {
                IF_VERBOSE(4) cout << "  row " << row.to_ss_matlab() << " is not required and will be removed from adder graph" << endl;
                typename set<T>::iterator it2 = pipeline_set_best[row.stageW].find(row.W);
                if(it2 != pipeline_set_best[row.stageW].end())
                {
                    pipeline_set_best[row.stageW].erase(it2);
                }
                pipelined_adder_graph.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

template <class T>
string output_adder_graph(list< realization_row<T> > &pipelined_adder_graph, bool standard_1_or_matlab_0_style)
{
  stringstream output;
  output << "{";
  typename list< realization_row<T> >::iterator iter;
  for(iter = pipelined_adder_graph.begin(); iter != pipelined_adder_graph.end(); ++iter)
  {
    if(iter != pipelined_adder_graph.begin())
      output << ",";

    if(standard_1_or_matlab_0_style)
    {
      output << (*iter).to_ss_standard();
    }
    else
    {
      output << (*iter).to_ss_matlab();
    }
  }
  output << "}";
  return output.str();
}



#endif //RPAG_FUNCTIONS

