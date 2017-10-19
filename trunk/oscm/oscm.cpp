
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

int main(int argc, char *args[])
{
  long targetCoeff=-1;
  int noOfAddersMin=-1;
  int noOfAddersMax=-1;
  int shiftMax=-1;
  //    int coeffMax=(1LL << shiftMax);
  long coeffMax=-1;
  int timeout=-1; //default -1 means no timeout
  bool useIndicatorConstraints=false;
  long bigM=-1;
  string ilpSolver="";

  set<long> targetCoeffs;

  //parse command line
  for (int i = 1; i < argc; i++) {
    char* value;
    if(getCmdParameter(args[i],"--timeout=",value))
    {
      timeout = atol(value);
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
    else if(getCmdParameter(args[i],"--indicator_constraints",value))
    {
      useIndicatorConstraints=true;
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
        exit(-1); //ToDo: print a short help
      }
    }

  }

  if(targetCoeffs.empty())
  {
    cout << "Error: No coefficient(s) provided" << endl;
    exit(-1); //ToDo: print a short help
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
  cout << "no of adders min=" << noOfAddersMin << endl;
  cout << "no of adders max=" << noOfAddersMax << endl;
  cout << "no of outputs=" << noOfOutputs << endl;
  cout << "timeout=" << timeout << endl;
  cout << "max shift=" << shiftMax << endl;
  cout << "max coeff=" << coeffMax << endl;
  cout << "big M=" << bigM << endl;
  cout << "use indicator constraints=" << useIndicatorConstraints << endl;

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

      // set the timeout of the solver
      if(timeout > 0)
      {
        sol.timeout = timeout;
      }

//      sol.intFeasTol = 1E-5;
//      sol.intFeasTol = 1E-9;

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

      for(unsigned int a=0; a <= noOfAdders; a++)
      {
        coeff[a] = ScaLP::newIntegerVariable("c" + to_string(a),0,coeffMax); //could be also real but then it gets numerically unstable (at least with gurobi)
        coeffLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_l",0,coeffMax);
        coeffRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_r",0,coeffMax);
        coeffShiftedLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_l",0,coeffMax);
        coeffShiftedRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_r",0,coeffMax);
        coeffShiftedSignLeft[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_sg_l",-coeffMax,coeffMax);
        coeffShiftedSignRight[a] = ScaLP::newRealVariable("c" + to_string(a) + "_in_sh_sg_r",-coeffMax,coeffMax);
        signLeft[a] = ScaLP::newBinaryVariable("sign"  + to_string(a) + "l");
        signRight[a] = ScaLP::newBinaryVariable("sign"  + to_string(a) + "r");
        for(unsigned int k=0; k <= noOfAdders-1; k++) //!!
        {
          coeffIskLeft[a][k] = ScaLP::newBinaryVariable("c"  + to_string(a) + "_in_l_is_" + to_string(k));
          coeffIskRight[a][k] = ScaLP::newBinaryVariable("c"  + to_string(a) + "_in_r_is_" + to_string(k));
        }
        for(unsigned int s=0; s < shiftMax; s++)
        {
          shiftIsSLeft[a][s] = ScaLP::newBinaryVariable("shift"  + to_string(a) + "_in_l_is_" + to_string(s));
          shiftIsSRight[a][s] = ScaLP::newBinaryVariable("shift"  + to_string(a) + "_in_r_is_" + to_string(s));
        }
      }

      //define variables for MCM:
      vector<ScaLP::Variable> output(noOfOutputs);
      vector<vector<ScaLP::Variable> > outputIsa(noOfOutputs, vector<ScaLP::Variable>(noOfAdders+1));

      ScaLP::Term obj(0);
      for(unsigned int i=1; i <= noOfAdders; i++)
      {
//        obj += coeff[i];
      }
      sol.setObjective(ScaLP::minimize(obj));
//      sol.setObjective(ScaLP::maximize(obj));

      //constraint C1:
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
          //constraint C1a:
          sol << (output[j] == targetCoeff);
          for(unsigned int a=0; a <= noOfAdders; a++)
          {
            //constraint C1b:
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
          //constraint C1c:
          ScaLP::Term c1c(0);
          for(unsigned int a=0; a <= noOfAdders; a++)
          {
            c1c += outputIsa[j][a];
          }
          sol << (c1c == 1);
          j++;
        }


      }

      //constraint C2:
      sol << (coeff[0] == 1);

      for(unsigned int a=1; a <= noOfAdders; a++)
      {
        //constraints C3:
        sol << (coeff[a] - coeffShiftedSignLeft[a] - coeffShiftedSignRight[a] == 0);

        for(unsigned int k=0; k <= a-1; k++)
        {
          //constraints C4:
          if(useIndicatorConstraints)
          {
            sol << (coeffIskLeft[a][k]==1 then coeffLeft[a] - coeff[k] == 0);
            sol << (coeffIskRight[a][k]==1 then coeffRight[a] - coeff[k] == 0);
          }
          else
          {
//                s << (coeff[k] - BigM + BigM*coeffIskLeft[i][k] <= coeffLeft[i] <= coeff[k] + BigM - BigM*coeffIskLeft[i][k]);
//                s << (coeff[k] - BigM + BigM*coeffIskRight[i][k] <= coeffRight[i] <= coeff[k] + BigM - BigM*coeffIskRight[i][k]);
            sol << (coeff[k] - bigM + bigM*coeffIskLeft[a][k] - coeffLeft[a] <= 0);
            sol << (coeffLeft[a] - coeff[k] - bigM + bigM*coeffIskLeft[a][k] <= 0);
            sol << (coeff[k] - bigM + bigM*coeffIskRight[a][k] - coeffRight[a] <= 0);
            sol << (coeffRight[a] - coeff[k] - bigM + bigM*coeffIskRight[a][k] <= 0);
          }
        }

        //constraints C5:
        ScaLP::Term c5l(0),c5r(0);
        for(unsigned int k=0; k <= a-1; k++)
        {
          c5l += coeffIskLeft[a][k];
          c5r += coeffIskRight[a][k];
        }
        sol << (c5l == 1);
        sol << (c5r == 1);

        //constraints C6:
        for(int s=0; s < shiftMax; s++)
        {
          if(useIndicatorConstraints)
          {
            sol << (shiftIsSLeft[a][s] == 1 then coeffShiftedLeft[a] - (1LL << s)*coeffLeft[a] == 0 );
//            sol << (shiftIsSRight[a][s] == 1 then coeffShiftedRight[a] - (1LL << s)*coeffRight[a] == 0 );
          }
          else
          {
            sol << (coeffShiftedLeft[a] - (1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] <= 0);
            sol << ((1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] - coeffShiftedLeft[a] <= 0);
/*
            sol << (coeffShiftedRight[a] - (1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] <= 0);
            sol << ((1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] - coeffShiftedRight[a] <= 0);
*/
          }
        }
        sol << (coeffShiftedRight[a] - coeffRight[a] == 0 ); //right shift is disabled per default
        //constraints C7:
        ScaLP::Term c7l(0),c7r(0);
        for(int s=0; s < shiftMax; s++)
        {
          c7l += shiftIsSLeft[a][s];
          c7r += shiftIsSRight[a][s];
        }
        sol << (c7l == 1);
        sol << (c7r == 1);

        //constraint C8:
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
        }
        else
        {
          //sign is +:
          sol << (coeffShiftedSignLeft[a] - coeffShiftedLeft[a] - bigM*signLeft[a] <= 0);
          sol << (coeffShiftedLeft[a] - bigM*signLeft[a] - coeffShiftedSignLeft[a]<= 0);
          sol << (coeffShiftedSignRight[a] - coeffShiftedRight[a] - bigM*signRight[a] <= 0);
          sol << (coeffShiftedRight[a] - bigM*signRight[a] - coeffShiftedSignRight[a]<= 0);

          //sign is -:
          sol << (coeffShiftedSignLeft[a] + coeffShiftedLeft[a] - bigM + bigM*signLeft[a] <= 0);
          sol << (-coeffShiftedLeft[a] - bigM + bigM*signLeft[a] - coeffShiftedSignLeft[a] <= 0);
          sol << (coeffShiftedSignRight[a] + coeffShiftedRight[a] - bigM + bigM*signRight[a] <= 0);
          sol << (-coeffShiftedRight[a] - bigM + bigM*signRight[a] - coeffShiftedSignRight[a] <= 0);
        }

        //constraint C9:
        sol << (signLeft[a] + signRight[a] <= 1);
      }

      //    y==0 then x1+x2+x3==0

      sol.writeLP("oscm.lp");

      // Try to solve
      ScaLP::status stat = sol.solve();

      // print results
      ScaLP::Result res = sol.getResult();
      cout << res << endl;

//      std::setprecision(20);

      cout << "Optimization result: " << stat << endl;
      if((stat == ScaLP::status::FEASIBLE) || (stat == ScaLP::status::OPTIMAL) || (stat == ScaLP::status::TIMEOUT))
      {
        cout << "no of adders:" << noOfAdders << endl;

        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          cout << coeff[a] << "=" << res.values[coeff[a]] << "=Aop(" << res.values[coeffLeft[a]] << "," << res.values[coeffRight[a]] << ")" << endl;
          for(int s=0; s < shiftMax; s++)
          {
            if(round(res.values[shiftIsSRight[a][s]]) == 1)
              cout << "   " << coeff[a] << " shift right is " << s << endl;
            if(round(res.values[shiftIsSLeft[a][s]]) == 1)
              cout << "   " << coeff[a] << " shift left is " << s << endl;
          }
          cout << "   " << coeff[a] << " sign left is " << res.values[signLeft[a]] << endl;
          cout << "   " << coeff[a] << " sign left is " << res.values[signRight[a]] << endl;

          cout << "   " << coeffShiftedLeft[a] << "=" << res.values[coeffShiftedLeft[a]] << endl;
          cout << "   " << coeffShiftedRight[a] << "=" << res.values[coeffShiftedRight[a]] << endl;
          cout << "   " << coeffShiftedSignLeft[a] << "=" << res.values[coeffShiftedSignLeft[a]] << endl;
          cout << "   " << coeffShiftedSignRight[a] << "=" << res.values[coeffShiftedSignRight[a]] << endl;
        }

        stringstream adderMatrix;

        adderMatrix << "[";
        for(unsigned int a=1; a <= noOfAdders; a++)
        {
          int sl,sr;
          double c = round(res.values[coeff[a]]);
          for(int s=0; s < shiftMax; s++)
          {
            if(round(res.values[shiftIsSLeft[a][s]]) == 1)
            {
              sl=s;
            }
            if(round(res.values[shiftIsSRight[a][s]]) == 1)
            {
              sr=s;
            }
          }
          string signL,signR;
          if(round(res.values[signLeft[a]]) == 1)
            signL = "-";
          else
            signL = "";

          if(round(res.values[signRight[a]]) == 1)
            signR = "-";
          else
            signR = "+";

          double cl = round(res.values[coeffLeft[a]]);
          double cr = round(res.values[coeffRight[a]]);

          cout << coeff[a] << "=" << c << "=" << signL << cl << "*2^" << sl << " " << signR << " " << cr << "*2^" << sr << endl;

          int cls = signL=="-" ? -cl : cl;
          int crs = signR=="-" ? -cr : cr;
          if(c != (((int) cls) << sl) + (((int) crs) << sr))
          {
            cerr << "Error in result: " << c << " != " << (((int) cls) << sl) + (((int) crs) << sr) << endl;
          }

          adderMatrix << c << " " << signL << cl << " " << sl << " " << signR << cr << " " << sr;
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
