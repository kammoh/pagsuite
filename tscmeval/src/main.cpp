/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include <iostream>
#include <cstring>

#include "debug.h"
#include "fundamental.h"
#include "evaluate_ternary_scm_topologies.h"

using namespace std;

void print_short_help()
{
    cout << "usage: tscmeval [OPTIONS] coefficients" << endl;
}

void multiply_set_elements(const int_set_t *r1_set, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set)
{
  int_set_t::iterator r1_p;
  int_set_t::iterator r2_p;

  for (r1_p=r1_set->begin(); r1_p != r1_set->end(); ++r1_p)
  {
    for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p) //!!!
    {
      int_t res = fundamental_unsave(*r1_p * *r2_p);
      if(res <= c_max)
        successor_set->insert(res);
    }
  }
}

void multiply_set_elements(int_t r1, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set)
{
  int_set_t::iterator r2_p;

  for (r2_p=r2_set->begin(); r2_p != r2_set->end(); ++r2_p) //!!!
  {
    int_t res = fundamental_unsave(r1 * *r2_p);
    if(res <= c_max)
      successor_set->insert(res);
  }
}

/**
 * Returns the value as string of a command line argument in syntax --key=value
 * @param argv the command line string
 * @param parameter the name of the parameter
 * @param value as string if parameter string was found
 * @return True if parameter string was found
 */
bool getCmdParameter(char* argv, const char* parameter, char* &value)
{
    if(strstr(argv, parameter))
    {
        value = argv+strlen(parameter);
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Returns true if a command flag in syntax --flag is defined
 * @param argv the command line string
 * @param parameter the name of the parameter
 * @return True if parameter string was found
 */
bool isCmdFlagSet(char* argv, const char* parameter)
{
  if(strstr(argv, parameter))
  {
      return true;
  }
  else
  {
      return false;
  }
}

int main(int argc, char *argv[])
{
  global_verbose = 0;
  int shiftmax = 22;
  bool computeNOFs=false;
  int noOfAddersMax=3;

  //parse command line
  for (int i = 1; i < argc; i++) {
      char* value;
      if(getCmdParameter(argv[i],"--verbose=",value))
      {
          global_verbose = atol(value);
          IF_VERBOSE(3) cout << "verbose=" << global_verbose << endl;
      }
      else if(getCmdParameter(argv[i],"--shift=",value))
      {
          shiftmax = atol(value);
          IF_VERBOSE(3) cout << "shiftmax=" << shiftmax << endl;
      }
      else if(getCmdParameter(argv[i],"--adders=",value))
      {
          noOfAddersMax = atol(value);
          IF_VERBOSE(3) cout << "max. adders=" << shiftmax << endl;
      }
      else if(isCmdFlagSet(argv[i],"--nof"))
      {
          computeNOFs = true;
          IF_VERBOSE(3) cout << "computeNOFs=" << computeNOFs << endl;
      }
      else
      {
        cout << "Error: Illegal Option: " << argv[i] << endl;
        exit(-1); //ToDo: print a short help
      }

  }

  /*
  cout << "sizeof(nofs)=" << sizeof(nofs) << endl;
  int noOfNofs = sizeof(nofs)/sizeof(int);
  cout << "noOfNofs=" << noOfNofs << endl;
  for(int coeff=1; coeff < noOfNofs; coeff += 2)
  {
    cout << "nofs[" << coeff << "]=" << nofs[(coeff-1)>>1][0] << " " << nofs[(coeff-1)>>1][1] << endl;
  }
  return 0;

  */

  cout << "starting ternary single constant multiplication evaluation" << endl;
  cout << "using max. bit shift of " << shiftmax << endl;

  int_t c_max=1LL<<shiftmax;

  evaluateTernarySCMTopologies evaluator(c_max,noOfAddersMax);


  evaluator.evaluateAll();
  /*

  //test for graph 3.9:
  evaluator.evaluateTopology(1,1);
  evaluator.evaluateTopology(2,1);
  evaluator.evaluateTopology(3,9);

  //test for pipelined adder graphs ((1.1), (2.3), (3.2) and (3.13)):
  evaluator.evaluateTopology(1,1);
  evaluator.evaluateTopology(2,3);
  evaluator.evaluateTopology(3,2);
  evaluator.evaluateTopology(3,13);
  */

  cout << "results:" << endl;
  evaluator.outputCoeffRealizations();
  evaluator.outputCoeffTopologies();
  evaluator.outputTopologiesCoverage();
//  evaluator.outputNOFSets();
  evaluator.outputCoeffCoveragePerTopology();
  evaluator.outputCoeffCoverage();

  evaluator.saveCoeffNOFs("tscm_solutions.cpp");


  return 0;
}
