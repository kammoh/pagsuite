
#include <iostream>

#include <ILP/Solver.h>
#include <ILP/Exception.h>    // ILP::Exception
#include <ILP/SolverDynamic.h> // ILP::newSolverDynamic

#include <vector>
#include <cmath>
#include <climits>
#include <sstream>

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
  int targetCoeff=-1;
  int noOfAddersMin=-1;
  int noOfAddersMax=-1;
  int shiftMax=-1;
  //    int coeffMax=(1LL << shiftMax);
  int coeffMax=-1;
  int timeout=-1; //default -1 means no timeout
  bool useIndicatorConstraints=false;
  int bigM=-1;
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
    else
    {
      if(args[i][0] != '-')
      {
        targetCoeff = atol(args[i]);
      }
      else
      {
        cout << "Error: Illegal Option: " << args[i] << endl;
        exit(-1); //ToDo: print a short help
      }
    }

  }

  if(targetCoeff <= 0)
  {
    cout << "Error: No or illegal (negative or zero) coefficient provided" << endl;
    exit(-1); //ToDo: print a short help
  }
  if(shiftMax <= 0)
  {
    shiftMax = ((int) ceil(log2(targetCoeff)))+1; //use one shift larger than coefficient word size
  }
  if(coeffMax <= 0)
  {
    coeffMax=2*targetCoeff;
  }
  if(bigM <= 0)
  {
    bigM = coeffMax*100; //!! think about a better bound !!
  }
  if(noOfAddersMin <= 0)
  {
    noOfAddersMin=1;
  }
  if(noOfAddersMax <= 0)
  {
    noOfAddersMax=INT_MAX;
  }

  cout << "settings:" << endl;
  cout << "no of adders min=" << noOfAddersMin << endl;
  cout << "no of adders max=" << noOfAddersMax << endl;
  cout << "target coefficient=" << targetCoeff << endl;
  cout << "timeout=" << timeout << endl;
  cout << "max shift=" << shiftMax << endl;
  cout << "max coeff=" << coeffMax << endl;
  cout << "big M=" << bigM << endl;

//  exit(0);//!!!

  try
  {
    for(int noOfAdders=noOfAddersMin; noOfAdders < noOfAddersMax; noOfAdders++)
    {
      cout << "******************************************************" << endl;
      cout << "  starting optimization with " << noOfAdders << " adders" << endl;
      cout << "******************************************************" << endl;
      ILP::Solver sol = ILP::Solver(ILP::newSolverDynamic({"Gurobi","CPLEX","SCIP","LPSolve"}));

      // disable solver output
      sol.quiet=false;

      // enable presolving
      sol.presolve=true;

      // print the used Solver
      cout << sol.getBackendName() << endl;

      // set the timeout of the solver
      if(timeout > 0)
      {
        sol.timeout = timeout;
      }

      //create and initialize variables:
      vector<ILP::Variable> coeff(noOfAdders+1);
      vector<ILP::Variable> coeffLeft(noOfAdders+1);
      vector<ILP::Variable> coeffRight(noOfAdders+1);
      vector<ILP::Variable> coeffShiftedLeft(noOfAdders+1);
      vector<ILP::Variable> coeffShiftedRight(noOfAdders+1);
      vector<ILP::Variable> coeffShiftedSignLeft(noOfAdders+1);
      vector<ILP::Variable> coeffShiftedSignRight(noOfAdders+1);
      vector<vector<ILP::Variable> > coeffIskLeft(noOfAdders+1, vector<ILP::Variable>(noOfAdders+1));
      vector<vector<ILP::Variable> > coeffIskRight(noOfAdders+1, vector<ILP::Variable>(noOfAdders+1));
      vector<vector<ILP::Variable> > shiftIsSLeft(noOfAdders+1, vector<ILP::Variable>(shiftMax));
      vector<vector<ILP::Variable> > shiftIsSRight(noOfAdders+1, vector<ILP::Variable>(shiftMax));
      vector<ILP::Variable> signLeft(noOfAdders+1);
      vector<ILP::Variable> signRight(noOfAdders+1);

      for(unsigned int i=0; i <= noOfAdders; i++)
      {
        coeff[i] = ILP::newIntegerVariable("c" + to_string(i),0,coeffMax);
        coeffLeft[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_l",0,coeffMax);
        coeffRight[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_r",0,coeffMax);
        coeffShiftedLeft[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_sh_l",0,coeffMax);
        coeffShiftedRight[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_sh_r",0,coeffMax);
        coeffShiftedSignLeft[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_sh_sg_l",-coeffMax,coeffMax);
        coeffShiftedSignRight[i] = ILP::newIntegerVariable("c" + to_string(i) + "_in_sh_sg_r",-coeffMax,coeffMax);
        signLeft[i] = ILP::newBinaryVariable("sign"  + to_string(i) + "l");
        signRight[i] = ILP::newBinaryVariable("sign"  + to_string(i) + "r");
        for(unsigned int k=0; k <= noOfAdders-1; k++)
        {
          coeffIskLeft[i][k] = ILP::newBinaryVariable("c"  + to_string(i) + "_in_l_is_" + to_string(k));
          coeffIskRight[i][k] = ILP::newBinaryVariable("c"  + to_string(i) + "_in_r_is_" + to_string(k));
        }
        for(unsigned int s=0; s < shiftMax; s++)
        {
          shiftIsSLeft[i][s] = ILP::newBinaryVariable("shift"  + to_string(i) + "_in_l_is_" + to_string(s));
          shiftIsSRight[i][s] = ILP::newBinaryVariable("shift"  + to_string(i) + "_in_r_is_" + to_string(s));
        }
      }

      ILP::Term obj(0);
      for(unsigned int i=1; i <= noOfAdders; i++)
      {
        obj += coeff[i];
      }
      sol.setObjective(ILP::minimize(obj));
//      sol.setObjective(ILP::maximize(obj));

      //constraint C1:
      sol << (coeff[noOfAdders] == targetCoeff);
      //constraint C2:
      sol << (coeff[0] == 1);

      for(unsigned int a=1; a <= noOfAdders; a++)
      {
        //constraints C3:
        sol << (coeff[a] - coeffShiftedSignLeft[a] - coeffShiftedSignRight[a] == 0);

        for(unsigned int k=0; k <= noOfAdders-1; k++)
        {
          //constraints C4:
          if(useIndicatorConstraints)
          {
            if(k != a)
            {
              sol << (coeffIskLeft[a][k]==1 then coeffLeft[a] - coeff[k] == 0);
              sol << (coeffIskRight[a][k]==1 then coeffRight[a] - coeff[k] == 0);
            }
            else
            {
              sol << (coeffIskLeft[a][k]==0);
              sol << (coeffIskRight[a][k]==0);
            }
          }
          else
          {
            if(k != a)
            {
//                s << (coeff[k] - BigM + BigM*coeffIskLeft[i][k] <= coeffLeft[i] <= coeff[k] + BigM - BigM*coeffIskLeft[i][k]);
//                s << (coeff[k] - BigM + BigM*coeffIskRight[i][k] <= coeffRight[i] <= coeff[k] + BigM - BigM*coeffIskRight[i][k]);
              sol << (coeff[k] - bigM + bigM*coeffIskLeft[a][k] - coeffLeft[a] <= 0);
              sol << (coeffLeft[a] - coeff[k] - bigM + bigM*coeffIskLeft[a][k] <= 0);
              sol << (coeff[k] - bigM + bigM*coeffIskRight[a][k] - coeffRight[a] <= 0);
              sol << (coeffRight[a] - coeff[k] - bigM + bigM*coeffIskRight[a][k] <= 0);
            }
            else
            {
              sol << (coeffIskLeft[a][k]==0);
              sol << (coeffIskRight[a][k]==0);
            }
          }
        }

        //constraints C5:
        ILP::Term c5l(0),c5r(0);
        for(unsigned int k=0; k <= noOfAdders-1; k++)
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
            sol << (shiftIsSRight[a][s] == 1 then coeffShiftedRight[a] - (1LL << s)*coeffRight[a] == 0 );
          }
          else
          {
            sol << (coeffShiftedLeft[a] - (1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] <= 0);
            sol << ((1LL << s)*coeffLeft[a] - bigM + bigM*shiftIsSLeft[a][s] - coeffShiftedLeft[a] <= 0);
            sol << (coeffShiftedRight[a] - (1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] <= 0);
            sol << ((1LL << s)*coeffRight[a] - bigM + bigM*shiftIsSRight[a][s] - coeffShiftedRight[a] <= 0);
          }
        }

        //constraints C7:
        ILP::Term c7l(0),c7r(0);
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
          sol << (signLeft[a] == 1 then coeffShiftedSignLeft[a] - coeffShiftedLeft[a] == 0 );
          //sign is -:
          sol << (signLeft[a] == 0 then coeffShiftedSignLeft[a] + coeffShiftedLeft[a] == 0 );
          //sign is +:
          sol << (signRight[a] == 1 then coeffShiftedSignRight[a] - coeffShiftedRight[a] == 0 );
          //sign is -:
          sol << (signRight[a] == 0 then coeffShiftedSignRight[a] + coeffShiftedRight[a] == 0 );
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

      }

      //    y==0 then x1+x2+x3==0

      sol.writeLP("oscm.lp");

      // Try to solve
      ILP::status stat = sol.solve();

      // print results
      ILP::Result res = sol.getResult();
      cout << res << endl;

      cout << "Optimization result: " << stat << endl;
      if((stat == ILP::status::FEASIBLE) || (stat == ILP::status::OPTIMAL))
      {
        cout << "no of adders:" << noOfAdders << endl;

        stringstream adderMatrix;

        adderMatrix << "[";
        for(unsigned int i=1; i <= noOfAdders; i++)
        {
          int sl,sr;
          double c = res.values[coeff[i]];
          for(int s=0; s < shiftMax; s++)
          {
            if(round(res.values[shiftIsSLeft[i][s]]) == 1)
            {
              sl=s;
            }
            if(round(res.values[shiftIsSRight[i][s]]) == 1)
            {
              sr=s;
            }
          }
          string signL,signR;
          if(round(res.values[signLeft[i]]) == 1)
            signL = "-";
          else
            signL = "";

          if(round(res.values[signRight[i]]) == 1)
            signR = "-";
          else
            signR = "+";

          double cl = res.values[coeffLeft[i]];
          double cr = res.values[coeffRight[i]];

          cout << coeff[i] << "=" << c << "=" << signL << cl << "*2^" << sl << " " << signR << " " << cr << "*2^" << sr << endl;

          int cls = signL=="-" ? -cl : cl;
          int crs = signR=="-" ? -cr : cr;
          if(c != (((int) cls) << sl) + (((int) crs) << sr))
          {
            cerr << "Error in result: " << c << " != " << (((int) cls) << sl) + (((int) crs) << sr) << endl;
          }

          adderMatrix << c << " " << signL << cl << " " << sl << " " << signR << cr << " " << sr;
          if(i != noOfAdders) adderMatrix << ";";
        }
        adderMatrix << "]";
        cout << "realization=" << adderMatrix.str() << endl;
        break;
      }
    }
  }
  catch(ILP::Exception &e)
  {
    cerr << "Error: " << e << endl;
  }

  return 0;
}
