#ifndef SCMOPT_H
#define SCMOPT_H

#include <sstream>

using namespace std;

class SCMOpt
{
public:
    SCMOpt(int coeff, bool useOldAdderGraph=false);

    string getAdderGraph();
    int getAdderCost();

private:
    int coeff;  /**< The constant */
    int wIn;

    void generateAOp(int a, int b, int c, int eA, int eB, int signA, int signB, int preFactor=1);
    void buildAdderGraph(int c, int preFactor=1);

    bool useOldAdderGraph;
    stringstream adderGraph;

    int adder_cost;
};

#endif // SCMOPT_H
