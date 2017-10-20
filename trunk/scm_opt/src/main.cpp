#include <iostream>

using namespace std;

#include "scmopt.hpp"
#include "debug.h"
#include "fundamental.h"


int main(int argc, char* argv[] )
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " coefficient" << endl;
        return 1;
    }

    int coeff = stoi(argv[1]);

    global_verbose = 0;

    SCMOpt scmo(coeff);

    cout << "adder_graph=" << scmo.getAdderGraph() << ";" << endl;
    cout << "adder_cost=" << scmo.getAdderCost() << ";" << endl;
    return 0;
}

