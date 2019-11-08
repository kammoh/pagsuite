
#include <iostream>

#include <ScaLP/Solver.h>
#include <ScaLP/Exception.h>    // ScaLP::Exception
#include <ScaLP/SolverDynamic.h> // ScaLP::newSolverDynamic

#include <vector>
#include <cmath>
#include <climits>
#include <sstream>
#include <iomanip>
#include <set>
#include <cassert>

// This example shows you, how to use the dynamic backend.
//
// It is a minimal version of simple_gurobi.
// The only relevant change is in line 24.

using namespace std;

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

void print_short_help()
{
    cout << "usage: omcm [OPTIONS] coefficient(s)" << endl;
    cout << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "General Options:" << endl;
    cout << "Option                                  Meaning" << endl;
    cout << "--solver=[Gurobi|CPLEX|SCIP|LPSolve]    ILP solver (if available in ScaLP library), also a comma separated wish list of solvers can be provided" << endl;
    cout << "--timeout=[int]                         ILP solver Timeout in seconds" << endl;
    cout << "--threads=[int]                         Number of threads for the ILP solver" << endl;
    cout << "--max_shift=[int]                       Maximum bit shift which is considered in the adder graph" << endl;
    cout << "--max_coeff=[int]                       Maximum coefficient that is expected in an adder node" << endl;
    cout << "--big_M=[int]                           Value of the big-M variable (default: max. coeff + 1)" << endl;
    cout << "--int_feas_tol=[float]                  Sets the integer feasible tolerance of the solver. Reduce this value for large constants if errors occur (default: min(1E-5,1/(2*big-M)))" << endl;
    cout << "--indicator_constraints                 When specified, indicator constraints are used instead of big-M constraints" << endl;
    cout << "--relaxations                           When specified, relaxations to real variables are used where possible" << endl;
    cout << "--ternary_add                           When specified, ternary adders are considered instead of 2-input adders" << endl;
    cout << "--no_of_adders_min=[int]                Minimum number of adders for which a solution is searched (default is the number of unique odd coefficients as lower bound)" << endl;
    cout << "--no_of_adders_max=[int]                Maximum number of adders for which a solution is searched (default is infinity)" << endl;
    cout << "--objective=[none|minAccSum|minGPC]     Secondary objective, none: only the adders are minimized, minAccSum: the sum of coefficients is minimized leading to low word size solutions, minGPC: the glitch path cound (GPC) is minimized" << endl;
    cout << endl;

}
int main(int argc, char *args[])
{
  long targetCoeff=-1;
  int noOfAddersMin=-1;
  int noOfAddersMax=-1;
  int shiftMax=-1;
  int shiftMin=0;
  //    int coeffMax=(1LL << shiftMax);
  long coeffMax=-1;
  int timeout=-1; //default -1 means no timeout
  bool useIndicatorConstraints=false;
  long bigM=-1;
  long bigMGPC=1000; //should be enough
  string ilpSolver="";
  int threads=0;
  string objective="none";
  bool useRelaxation=false;
  bool useTernaryAdder=false;
  double intFeasTol=1E-5;

  set<long> targetCoeffs;

  //parse command line
  for (int i = 1; i < argc; i++) {
    char* value;
    if(getCmdParameter(args[i],"--timeout=",value))
    {
      timeout = atol(value);
    }
    if(getCmdParameter(args[i],"--threads=",value))
    {
      threads = atol(value);
    }
    else if(getCmdParameter(args[i],"--max_shift=",value))
    {
      shiftMax = atol(value);
    }
    else if(getCmdParameter(args[i],"--max_coeff=",value))
    {
      coeffMax = atol(value);
    }
    else if(getCmdParameter(args[i],"--big_M=",value))
    {
      bigM = atol(value);
    }
    else if(getCmdParameter(args[i],"--no_of_adders_min=",value))
    {
      noOfAddersMin = atol(value);
    }
    else if(getCmdParameter(args[i],"--no_of_adders_max=",value))
    {
      noOfAddersMax = atol(value);
    }
    else if(getCmdParameter(args[i],"--int_feas_tol=",value))
    {
      intFeasTol = atof(value);
    }
    else if(getCmdParameter(args[i],"--indicator_constraints",value))
    {
      useIndicatorConstraints=true;
    }
    else if(getCmdParameter(args[i],"--ternary_add",value))
    {
      useTernaryAdder=true;
    }
    else if(getCmdParameter(args[i],"--relaxations",value))
    {
      useRelaxation=true;
    }
    else if(getCmdParameter(args[i],"--objective=",value))
    {
      objective=value;
    }
    else if(getCmdParameter(args[i],"--solver=",value))
    {
      ilpSolver = string(value);
    }
    else
    {
      if(args[i][0] != '-')
      {
        long c = atol(args[i]);
        if(c <= 0)
        {
          cout << "Error: Invalid (negative or zero) coefficient provided" << endl;
          exit(-1); //ToDo: print a short help
        }
        targetCoeffs.insert(c);
      }
      else
      {
        cout << "Error: Illegal Option: " << args[i] << endl;
        print_short_help();
        exit(-1);
      }
    }

  }

  if(targetCoeffs.empty())
  {
    cout << "Error: No coefficient(s) provided" << endl;
    print_short_help();
    exit(-1);
  }

  long targetCoeffMax = -1;
  for(long c : targetCoeffs)
  {
    if(c > targetCoeffMax) targetCoeffMax=c;
  }

  if(shiftMax <= 0)
  {
    shiftMax = ((int) ceil(log2(targetCoeffMax)))+1; //use one shift larger than coefficient word size
  }
  if(coeffMax <= 0)
  {
    coeffMax=2*targetCoeffMax;
  }
  if(bigM <= 0)
  {
    bigM = coeffMax+1; //set big M to the minimum possible
  }
  if(noOfAddersMin <= 0)
  {
    noOfAddersMin=targetCoeffs.size();
  }
  if(noOfAddersMax <= 0)
  {
    noOfAddersMax=INT_MAX;
  }

  int noOfOutputs = targetCoeffs.size();

  cout << "problem:";
  if(noOfOutputs == 1)
  {
    cout << "SCM";
    targetCoeff = *(targetCoeffs.begin());
  }
  else
  {
    cout << "MCM";
  }
  cout << endl;

  cout << "settings:" << endl;
  cout << "objective=" << objective << endl;
  cout << "no of adders min=" << noOfAddersMin << endl;
  cout << "no of adders max=" << noOfAddersMax << endl;
  cout << "no of outputs=" << noOfOutputs << endl;
  cout << "timeout=" << timeout << endl;
  cout << "max shift=" << shiftMax << endl;
  cout << "max coeff=" << coeffMax << endl;
  cout << "big M=" << bigM << endl;
  cout << "use indicator constraints=" << useIndicatorConstraints << endl;
  cout << "use relaxations=" << useRelaxation << endl;
  cout << "threads=" << threads << endl;
  cout << "target coefficients=";
  for(long c : targetCoeffs)
  {
    cout << c << " ";
  }
  cout << endl;

//  exit(0);//!!!

  try
  {
    for(int noOfAdders=noOfAddersMin; noOfAdders <= noOfAddersMax; noOfAdders++)
    {
      cout << "******************************************************" << endl;
      cout << "  starting optimization with " << noOfAdders << " adders" << endl;
      cout << "******************************************************" << endl;
      ScaLP::Solver sol = ilpSolver.empty()? ScaLP::Solver(ScaLP::newSolverDynamic({"Gurobi","CPLEX","SCIP","LPSolve"})) : ScaLP::Solver(ScaLP::newSolverDynamic({ilpSolver.c_str()}));

      // print the used Solver
      cout << "solver=" << sol.getBackendName() << endl;

      // disable solver output
      sol.quiet=false;

      // enable presolving
      sol.presolve=true;
//      sol.presolve=false; //!!!

      //set threads
      sol.threads=threads;

      // set the timeout of the solver
      if(timeout > 0)
      {
        sol.timeout = timeout;
      }

/*
      //does not work with SCIP!
      sol.intFeasTol = intFeasTol;

      double minTol = 1/((double) 2*bigM); //the minimum tolerance accepted by the model
      if(sol.intFeasTol > minTol)
        sol.intFeasTol = minTol;

      cout << "Int feasibility tolerance set to " << sol.intFeasTol << endl;
*/
      //define and initialize variables:
      vector<ScaLP::Variable> coeff(noOfAdders+1);
      vector<ScaLP::Variable> coeffLeft(noOfAdders+1);
      vector<ScaLP::Variable> coeffRight(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedLeft(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedRight(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedSignLeft(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedSignRight(noOfAdders+1);
      vector<vector<ScaLP::Variable> > coeffIskLeft(noOfAdders+1, vector<ScaLP::Variable>(noOfAdders+1));
      vector<vector<ScaLP::Variable> > coeffIskRight(noOfAdders+1, vector<ScaLP::Variable>(noOfAdders+1));
      vector<vector<ScaLP::Variable> > shiftIsSLeft(noOfAdders+1, vector<ScaLP::Variable>(shiftMax));
      vector<vector<ScaLP::Variable> > shiftIsSRight(noOfAdders+1, vector<ScaLP::Variable>(shiftMax));
      vector<ScaLP::Variable> signLeft(noOfAdders+1);
      vector<ScaLP::Variable> signRight(noOfAdders+1);

      //the 'middle' input is used for ternary adders
      vector<ScaLP::Variable> coeffMiddle(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedMiddle(noOfAdders+1);
      vector<ScaLP::Variable> coeffShiftedSignMiddle(noOfAdders+1);
      vector<vector<ScaLP::Variable> > coeffIskMiddle(noOfAdders+1, vector<ScaLP::Variable>(noOfAdders+1));
      vector<vector<ScaLP::Variable> > shiftIsSMiddle(noOfAdders+1, vector<ScaLP::Variable>(shiftMax));
      vector<ScaLP::Variable> signMiddle(noOfAdders+1);

      for(unsigned int a=0; a <= noOfAdders; a++)
      {
        coeff[a] = ScaLP::newIntegerVariable("c" + to_string(a),0,coeffMax); //could be also real but then it gets numerically unstable (at least with gurobi)

        if(useRelaxation)
        {
            coeffLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_l",0,coeffMax);
            coeffRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_r",0,coeffMax);
            coeffShiftedLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_l",0,coeffMax);
            coeffShiftedRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_r",0,coeffMax);
            coeffShiftedSignLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_sg_l",-coeffMax,coeffMax);
            coeffShiftedSignRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_sg_r",-coeffMax,coeffMax);
            if(useTernaryAdder)
            {
              coeffMiddle[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_m",0,coeffMax);
              coeffShiftedMiddle[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_m",0,coeffMax);
              coeffShiftedSignMiddle[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_sg_m",-coeffMax,coeffMax);
            }
        }
        else
        {
            coeffLeft[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_l",0,coeffMax);
            coeffRight[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_r",0,coeffMax);
            coeffShiftedLeft[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_l",0,coeffMax);
            coeffShiftedRight[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_r",0,coeffMax);
            coeffShiftedSignLeft[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_sg_l",-coeffMax,coeffMax);
            coeffShiftedSignRight[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_sg_r",-coeffMax,coeffMax);
            if(useTernaryAdder)
            {
              coeffMiddle[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_m",0,coeffMax);
              coeffShiftedMiddle[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_m",0,coeffMax);
              coeffShiftedSignMiddle[a] = ScaLP::newIntegerVariable("c" + to_string(a) + "_in_sh_sg_m",-coeffMax,coeffMax);
            }
        }
        signLeft[a] = ScaLP::newBinaryVariable("sign"  + to_string(a) + "l");
        signRight[a] = ScaLP::newBinaryVariable("sign"  + to_string(a) + "r");
        if(useTernaryAdder)
        {
          signMiddle[a] = ScaLP::newBinaryVariable("sign"  + to_string(a) + "m");
        }
        for(unsigned int k=0; k <= noOfAdders-1; k++) //!!
        {
          coeffIskLeft[a][k] = ScaLP::newBinaryVariable("c"  + to_string(a) + "_in_l_is_" + to_string(k));
          coeffIskRight[a][k] = ScaLP::newBinaryVariable("c"  + to_string(a) + "_in_r_is_" + to_string(k));
          if(useTernaryAdder)
          {
            coeffIskMiddle[a][k] = ScaLP::newBinaryVariable("c"  + to_string(a) + "_in_m_is_" + to_string(k));
          }
        }
        for(unsigned int s=0; s < shiftMax; s++)
        {
          shiftIsSLeft[a][s] = ScaLP::newBinaryVariable("shift"  + to_string(a) + "_in_l_is_" + to_string(s));
          shiftIsSRight[a][s] = ScaLP::newBinaryVariable("shift"  + to_string(a) + "_in_r_is_" + to_string(s));
          if(useTernaryAdder)
          {
            shiftIsSMiddle[a][s] = ScaLP::newBinaryVariable("shift"  + to_string(a) + "_in_m_is_" + to_string(s));
          }
        }
      }

      //define variables for MCM:
      vector<ScaLP::Variable> output(noOfOutputs);
      vector<vector<ScaLP::Variable> > outputIsa(noOfOutputs, vector<ScaLP::Variable>(noOfAdders+1));

      //define variables for minGPC objective:
      vector<ScaLP::Variable> gpc(noOfAdders+1);
      vector<ScaLP::Variable> gpcLeft(noOfAdders+1);
      vector<ScaLP::Variable> gpcRight(noOfAdders+1);
      vector<ScaLP::Variable> gpcMiddle(noOfAdders+1);

      ScaLP::Term obj(0); //this corresponds to objective == "none"
      if(objective == "none")
      {
        //nothing to do
      }
      else if(objective == "minAccSum")
      {
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          obj += coeff[a];
        }
      }
      else if(objective == "minGPC")
      {
        for(unsigned int a=0; a <= noOfAdders; a++)
        {
          //initialize the GPC variables:
          gpc[a] = ScaLP::newIntegerVariable("gpc" + to_string(a)); //could be also real but then it gets numerically unstable (at least with gurobi)
          gpcLeft[a] = ScaLP::newRealVariable("gpc_l" + to_string(a));
          gpcRight[a] = ScaLP::newRealVariable("gpc_r" + to_string(a));
          if(useTernaryAdder)
          {
            gpcMiddle[a] = ScaLP::newRealVariable("gpc_m" + to_string(a));
          }
        }
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          obj += gpc[a];
        }
      }
      else
      {
        cout << "Error: objective " << objective << " unknown" << endl;
        exit(-1); //ToDo: print a short help
      }

      sol.setObjective(ScaLP::minimize(obj));
//      sol.setObjective(ScaLP::maximize(obj));

      //constraint C6:
      if(noOfOutputs == 1) //SCM problem
      {
        sol << (coeff[noOfAdders] == targetCoeff);
      }
      else //MCM problem
      {
        //initialize missing variables:
        for(unsigned int j=0; j < noOfOutputs; j++)
        {
          output[j] = ScaLP::newRealVariable("o" + to_string(j),0,coeffMax);
          for(unsigned int a=0; a <= noOfAdders; a++)
          {
            outputIsa[j][a] = ScaLP::newBinaryVariable("o" + to_string(j+1) + "_is_" + to_string(a));
          }
        }

        unsigned int j=0;
        for(long targetCoeff : targetCoeffs)
        {
          //constraint C6a:
          for(unsigned int a=0; a <= noOfAdders; a++)
          {
            if(useIndicatorConstraints)
            {
              sol << (outputIsa[j][a]==1 then output[j] - coeff[a] == 0);
            }
            else
            {
              sol << (output[j] - coeff[a] - bigM + bigM*outputIsa[j][a] <= 0);
              sol << (-output[j] + coeff[a] - bigM + bigM*outputIsa[j][a]  <= 0);
            }
          }
          //constraint C6b:
          sol << (output[j] == targetCoeff);
          //constraint C6c:
          ScaLP::Term c1c(0);
          for(unsigned int a=0; a <= noOfAdders; a++)
          {
            c1c += outputIsa[j][a];
          }
          sol << (c1c == 1);
          j++;
        }


      }

      //constraint C1:
      sol << (coeff[0] == 1);

      for(unsigned int a=1; a <= noOfAdders; a++)
      {
        //constraints C2:
        if(useTernaryAdder)
        {
          sol << (coeff[a] - coeffShiftedSignLeft[a] - coeffShiftedSignMiddle[a] - coeffShiftedSignRight[a] == 0);
        }
        else
        {
          sol << (coeff[a] - coeffShiftedSignLeft[a] - coeffShiftedSignRight[a] == 0);
        }

        for(unsigned int k=0; k <= a-1; k++)
        {
          //constraints C3a:
          if(useIndicatorConstraints)
          {
            sol << (coeffIskLeft[a][k]==1 then coeffLeft[a] - coeff[k] == 0);
            sol << (coeffIskRight[a][k]==1 then coeffRight[a] - coeff[k] == 0);
            if(useTernaryAdder)
              sol << (coeffIskMiddle[a][k]==1 then coeffMiddle[a] - coeff[k] == 0);
          }
          else
          {
//                s << (coeff[k] - BigM + BigM*coeffIskLeft[i][k] <= coeffLeft[i] <= coeff[k] + BigM - BigM*coeffIskLeft[i][k]);
//                s << (coeff[k] - BigM + BigM*coeffIskRight[i][k] <= coeffRight[i] <= coeff[k] + BigM - BigM*coeffIskRight[i][k]);
            sol << (coeff[k] - bigM + bigM*coeffIskLeft[a][k] - coeffLeft[a] <= 0);
            sol << (coeffLeft[a] - coeff[k] - bigM + bigM*coeffIskLeft[a][k] <= 0);
            sol << (coeff[k] - bigM + bigM*coeffIskRight[a][k] - coeffRight[a] <= 0);
            sol << (coeffRight[a] - coeff[k] - bigM + bigM*coeffIskRight[a][k] <= 0);
            if(useTernaryAdder)
            {
              sol << (coeff[k] - bigM + bigM*coeffIskMiddle[a][k] - coeffMiddle[a] <= 0);
              sol << (coeffMiddle[a] - coeff[k] - bigM + bigM*coeffIskMiddle[a][k] <= 0);
            }
          }
        }

        //constraints C3b:
        ScaLP::Term c3bl(0),c3bm(0),c3br(0);
        for(unsigned int k=0; k <= a-1; k++)
        {
          c3bl += coeffIskLeft[a][k];
          c3br += coeffIskRight[a][k];
          if(useTernaryAdder)
            c3bm += coeffIskMiddle[a][k];
        }
        sol << (c3bl == 1);
        sol << (c3br == 1);
        if(useTernaryAdder)
          sol << (c3bm == 1);

        //constraints C4a:
        for(int s=shiftMin; s < shiftMax; s++)
        {
          if(useIndicatorConstraints)
          {
            sol << (shiftIsSLeft[a][s] == 1 then coeffShiftedLeft[a] - (1LL << s)*coeffLeft[a] == 0 );
//            sol << (shiftIsSRight[a][s] == 1 then coeffShiftedRight[a] - (1LL << s)*coeffRight[a] == 0 );
            if(useTernaryAdder)
              sol << (shiftIsSMiddle[a][s] == 1 then coeffShiftedMiddle[a] - (1LL << s)*coeffMiddle[a] == 0 );
          }
          else
          {
            sol << (coeffShiftedLeft[a] - (1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] <= 0);
            sol << ((1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] - coeffShiftedLeft[a] <= 0);
            if(useTernaryAdder)
            {
              sol << (coeffShiftedMiddle[a] - (1LL << s)*coeffMiddle[a] - bigM + bigM*shiftIsSMiddle[a][s] <= 0);
              sol << ((1LL << s)*coeffMiddle[a] - bigM + bigM*shiftIsSMiddle[a][s] - coeffShiftedMiddle[a] <= 0);
            }
/*
            sol << (coeffShiftedRight[a] - (1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] <= 0);
            sol << ((1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] - coeffShiftedRight[a] <= 0);
*/
          }
          //constraint C4c/d:
          if(s > 0)
            sol << (shiftIsSRight[a][s] == 0 ); //shift at right input is zero for all positive shifts
          else
            sol << (shiftIsSRight[a][s] == 1 ); //right shift at right input is currently disabled
//            sol << (shiftIsSRight[a][s] - shiftIsSLeft[a][s] == 0 ); //for right shifts, both have to be equal
        }
        sol << (coeffShiftedRight[a] - coeffRight[a] == 0 ); //right shift is currently disabled

        //constraints C4b:
        ScaLP::Term c7l(0),c7m(0),c7r(0);
        for(int s=0; s < shiftMax; s++)
        {
          c7l += shiftIsSLeft[a][s];
          c7r += shiftIsSRight[a][s];
          if(useTernaryAdder)
            c7m += shiftIsSMiddle[a][s];
        }
        sol << (c7l == 1);
        sol << (c7r == 1);
        if(useTernaryAdder)
          sol << (c7m == 1);

        //constraint C5a/b:
        if(useIndicatorConstraints)
        {
          //sign is +:
          sol << (signLeft[a] == 1 then coeffShiftedSignLeft[a] + coeffShiftedLeft[a] == 0 );
          //sign is -:
          sol << (signLeft[a] == 0 then coeffShiftedSignLeft[a] - coeffShiftedLeft[a] == 0 );
          //sign is +:
          sol << (signRight[a] == 1 then coeffShiftedSignRight[a] + coeffShiftedRight[a] == 0 );
          //sign is -:
          sol << (signRight[a] == 0 then coeffShiftedSignRight[a] - coeffShiftedRight[a] == 0 );
          if(useTernaryAdder)
          {
            //sign is +:
            sol << (signMiddle[a] == 1 then coeffShiftedSignMiddle[a] + coeffShiftedMiddle[a] == 0 );
            //sign is -:
            sol << (signMiddle[a] == 0 then coeffShiftedSignMiddle[a] - coeffShiftedMiddle[a] == 0 );
          }
        }
        else
        {
          //sign is +:
          sol << (coeffShiftedSignLeft[a] - coeffShiftedLeft[a] - bigM*signLeft[a] <= 0);
          sol << (coeffShiftedLeft[a] - bigM*signLeft[a] - coeffShiftedSignLeft[a]<= 0);
          //sign is -:
          sol << (coeffShiftedSignLeft[a] + coeffShiftedLeft[a] - bigM + bigM*signLeft[a] <= 0);
          sol << (-coeffShiftedLeft[a] - bigM + bigM*signLeft[a] - coeffShiftedSignLeft[a] <= 0);

          //sign is +:
          sol << (coeffShiftedSignRight[a] - coeffShiftedRight[a] - bigM*signRight[a] <= 0);
          sol << (coeffShiftedRight[a] - bigM*signRight[a] - coeffShiftedSignRight[a]<= 0);
          //sign is -:
          sol << (coeffShiftedSignRight[a] + coeffShiftedRight[a] - bigM + bigM*signRight[a] <= 0);
          sol << (-coeffShiftedRight[a] - bigM + bigM*signRight[a] - coeffShiftedSignRight[a] <= 0);

          if(useTernaryAdder)
          {
            //sign is +:
            sol << (coeffShiftedSignMiddle[a] - coeffShiftedMiddle[a] - bigM*signMiddle[a] <= 0);
            sol << (coeffShiftedMiddle[a] - bigM*signMiddle[a] - coeffShiftedSignMiddle[a]<= 0);
            //sign is -:
            sol << (coeffShiftedSignMiddle[a] + coeffShiftedMiddle[a] - bigM + bigM*signMiddle[a] <= 0);
            sol << (-coeffShiftedMiddle[a] - bigM + bigM*signMiddle[a] - coeffShiftedSignMiddle[a] <= 0);
          }

        }

        //constraint C5c:
        if(useTernaryAdder)
        {
          sol << (signLeft[a] + signMiddle[a] + signRight[a] <= 2);
        }
        else
        {
          sol << (signLeft[a] + signRight[a] <= 1);
        }
      }

      //    y==0 then x1+x2+x3==0

      //optional GPC constraints:
      if(objective == "minGPC")
      {
        //constraint C7a:
        sol << (gpc[0] == 0);

        //constraint C7b:
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          for(unsigned int k=0; k <= a-1; k++)
          {
            if(useIndicatorConstraints)
            {
              sol << (coeffIskLeft[a][k]==1 then gpcLeft[a] - gpc[k] == 0);
              sol << (coeffIskRight[a][k]==1 then gpcRight[a] - gpc[k] == 0);
              if(useTernaryAdder)
                sol << (coeffIskMiddle[a][k]==1 then gpcMiddle[a] - gpc[k] == 0);
            }
            else
            {
              sol << (gpc[k] - bigMGPC + bigMGPC*coeffIskLeft[a][k] - gpcLeft[a] <= 0);
              sol << (gpcLeft[a] - gpc[k] - bigMGPC + bigMGPC*coeffIskLeft[a][k] <= 0);
              sol << (gpc[k] - bigMGPC + bigMGPC*coeffIskRight[a][k] - gpcRight[a] <= 0);
              sol << (gpcRight[a] - gpc[k] - bigMGPC + bigMGPC*coeffIskRight[a][k] <= 0);
              if(useTernaryAdder)
              {
                sol << (gpc[k] - bigMGPC + bigMGPC*coeffIskMiddle[a][k] - gpcMiddle[a] <= 0);
                sol << (gpcMiddle[a] - gpc[k] - bigMGPC + bigMGPC*coeffIskMiddle[a][k] <= 0);
              }
            }
          }

          //constraint C7c:
          if(useTernaryAdder)
            sol << (gpc[a] - gpcLeft[a] - gpcMiddle[a] - gpcRight[a] == 1);
          else
            sol << (gpc[a] - gpcLeft[a] - gpcRight[a] == 1);
        }
      }

      sol.writeLP("omcm.lp");

      // Try to solve
      ScaLP::status stat = sol.solve();

      // print results
      ScaLP::Result res = sol.getResult();
      cout << res << endl;

//      std::setprecision(20);

      cout << "Optimization result: " << stat << endl;
      if((stat == ScaLP::status::FEASIBLE) || (stat == ScaLP::status::OPTIMAL) || (stat == ScaLP::status::TIMEOUT_FEASIBLE))
      {
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          cout << coeff[a] << "=" << res.values[coeff[a]] << "=Aop(" << res.values[coeffLeft[a]] << "," << res.values[coeffRight[a]] << ")" << endl;
          for(int s=0; s < shiftMax; s++)
          {
            if(round(res.values[shiftIsSLeft[a][s]]) == 1)
              cout << "   " << coeff[a] << " shift left is " << s << endl;
            if(useTernaryAdder)
            {
              if(round(res.values[shiftIsSMiddle[a][s]]) == 1)
                cout << "   " << coeff[a] << " shift middle is " << s << endl;
            }
            if(round(res.values[shiftIsSRight[a][s]]) == 1)
              cout << "   " << coeff[a] << " shift right is " << s << endl;
          }
          cout << "   " << coeff[a] << " sign left is " << res.values[signLeft[a]] << endl;
          if(useTernaryAdder)
            cout << "   " << coeff[a] << " sign middle is " << res.values[signMiddle[a]] << endl;
          cout << "   " << coeff[a] << " sign right is " << res.values[signRight[a]] << endl;

          cout << "   " << coeffShiftedLeft[a] << "=" << res.values[coeffShiftedLeft[a]] << endl;
          cout << "   " << coeffShiftedSignLeft[a] << "=" << res.values[coeffShiftedSignLeft[a]] << endl;
          if(useTernaryAdder)
          {
            cout << "   " << coeffShiftedMiddle[a] << "=" << res.values[coeffShiftedMiddle[a]] << endl;
            cout << "   " << coeffShiftedSignMiddle[a] << "=" << res.values[coeffShiftedSignMiddle[a]] << endl;
          }
          cout << "   " << coeffShiftedRight[a] << "=" << res.values[coeffShiftedRight[a]] << endl;
          cout << "   " << coeffShiftedSignRight[a] << "=" << res.values[coeffShiftedSignRight[a]] << endl;

        }

        stringstream adderMatrix;

        cout << "no of adders:" << noOfAdders << endl;

        adderMatrix << "[";
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          int sl,sm,sr;
          double c = res.values[coeff[a]];
          int cRounded = round(c);
          for(int s=0; s < shiftMax; s++)
          {
            if(round(res.values[shiftIsSLeft[a][s]]) == 1)
            {
              sl=s;
            }
            if(useTernaryAdder)
            {
              if(round(res.values[shiftIsSMiddle[a][s]]) == 1)
              {
                sm=s;
              }
            }
            if(round(res.values[shiftIsSRight[a][s]]) == 1)
            {
              sr=s;
            }
          }
          string signL,signM,signR;
          if(round(res.values[signLeft[a]]) == 1)
            signL = "-";
          else
            signL = "";

          if(useTernaryAdder)
          {
            if(round(res.values[signMiddle[a]]) == 1)
              signM = "-";
            else
              signM = "+";
          }

          if(round(res.values[signRight[a]]) == 1)
            signR = "-";
          else
            signR = "+";

          double cl = res.values[coeffLeft[a]];
          double cr = res.values[coeffRight[a]];
          double cm=0;
          if(useTernaryAdder)
            cm = res.values[coeffMiddle[a]];

          int clRounded = round(cl);
          int crRounded = round(cr);
          int cmRounded = round(cm);

          cout << coeff[a] << "=" << cRounded << "=" << signL << clRounded << "*2^" << sl << " ";
          if(useTernaryAdder)
          {
            cout << signM << " " << cmRounded << "*2^" << sm;
          }
          cout << signR << " " << crRounded << "*2^" << sr << endl;

//          std::setprecision(20);

          int cls = signL=="-" ? -clRounded : clRounded;
          int crs = signR=="-" ? -crRounded : crRounded;
          int cms = signM=="-" ? -cmRounded : cmRounded;
          if(useTernaryAdder)
          {
            if(cRounded != (((int) cls) << sl) + (((int) cms) << sm) + (((int) crs) << sr))
            {
              cerr << "Error in result: " << cRounded << " != " << (((int) cls) << sl) + (((int) cms) << sm) + (((int) crs) << sr) << endl;
            }
          }
          else
          {
            if(cRounded != (((int) cls) << sl) + (((int) crs) << sr))
            {
              cerr << "Error in result: " << cRounded << " != " << (((int) cls) << sl) + (((int) crs) << sr) << endl;
            }
          }

          adderMatrix << cRounded << " " << signL << clRounded << " " << sl << " ";
          if(useTernaryAdder)
            adderMatrix << signM << cmRounded << " " << sm << " ";
          adderMatrix << signR << crRounded << " " << sr;

          if(a != noOfAdders) adderMatrix << ";";
        }
        adderMatrix << "]";
        cout << "realization=" << adderMatrix.str() << endl;
        break;
      }
    }
  }
  catch(ScaLP::Exception &e)
  {
    cerr << "Error: " << e << endl;
  }

  return 0;
}
