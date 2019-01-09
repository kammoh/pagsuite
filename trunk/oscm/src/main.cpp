#include <iostream>

using namespace std;

#include "pagsuite/oscm.hpp"
#include "pagsuite/debug.h"
#include "pagsuite/fundamental.h"


int main(int argc, char *argv[])
{
  bool useOldAdderGraph = false;
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " coefficient" << endl;
    return 1;
  }

  int coeff = stoi(argv[1]);

  PAGSuite::global_verbose = 0;

  PAGSuite::OptimalSCM scmo(coeff, useOldAdderGraph);

  cout << "adder_graph=" << scmo.getAdderGraph() << ";" << endl;
  cout << "adder_cost=" << scmo.getAdderCost() << ";" << endl;
  return 0;
}

