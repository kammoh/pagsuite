#include <string>

#define MAX_CONST 524287

using namespace std;

#include "scmopt.hpp"
#include "compute_successor_set.h"
#include "log2_64.h"

#include "scm_solutions.hpp"
#include "pagexponents.hpp"
#include "fundamental.h"
#include "debug.h"

SCMOpt::SCMOpt(int coeff)
{
    adder_cost=0;

    coeff = fundamental(coeff);
    this->coeff = coeff;

    IF_VERBOSE(3) cout << "odd fundamental is " << coeff << endl;
    if(this->coeff <= MAX_CONST)
    {
        buildAdderGraph(coeff);
    }
    else
    {
      cerr << "Error: (Odd) value of constant must be less than or equal to " << MAX_CONST << " (is " << coeff << ")" << endl;
      exit(-1);
    }
}

void SCMOpt::generateAOp(int a, int b, int c, int eA, int eB, int signA, int signB, int preFactor)
{
    string signAStr,signBStr;

    if(signA < signB)
    {
        //swap a and b:
        int t;
        t=a; a=b; b=t;
        t=eA; eA=eB; eB=t;
        t=signA; signA=signB; signB=t;
    }

    char optype;
    optype='A';

    if(signA > 0) signAStr = ""; else signAStr = "-";
    if(signB > 0) signBStr = ""; else signBStr = "-";
    int stage = fundamental(coeff) == preFactor*c ? 1 : 0;

    int coeffStage=1;
    int coeffOdd = fundamental(coeff);
    if((coeffOdd == preFactor*c) && (coeff == coeffOdd)) coeffStage=2; //All (even) output coefficients are defined to be in stage 2 (workaround until node type 'O' is supported)

//    adderGraph << "{'" << optype << "'," << preFactor*c << "," << coeffStage << "," << signAStr << preFactor*a << "," << (preFactor*a == 1?0:1) << "," << eA << "," << signBStr << preFactor*b << "," << (preFactor*b == 1?0:1) << "," << eB << "},";
    adderGraph << "[" << preFactor*c << "," << signAStr << preFactor*a << "," << eA << "," << signBStr << preFactor*b << "," << eB << "];";
    adder_cost++;
}

void SCMOpt::buildAdderGraph(int c, int preFactor)
{
    if((c == 1) || (c == 0)) return;
    int i = (c-1)/2-1;

    int l1;
    int_set_t leap1_candidate_set1;
    int_set_t leap1_candidate_set2;
    int_set_t leap1_candidate_set2_div;
    int_set_t leap1_set;

    switch(solutions_gt[i])
    {
    case ADD:
        int eA,eB,signA,signB;
        IF_VERBOSE(3) cout << "additive: " << c << "=" << "Aop(" << solutions_op_a[i] << "," << solutions_op_b[i] << ")" << endl;

        assert(getExponents(solutions_op_a[i],solutions_op_b[i],c,&eA,&eB,&signA,&signB));
        if(solutions_op_a[i] != 1) buildAdderGraph(solutions_op_a[i],preFactor);
        if(solutions_op_b[i] != 1) buildAdderGraph(solutions_op_b[i],preFactor);
        generateAOp(solutions_op_a[i],solutions_op_b[i],c,eA,eB,signA,signB,preFactor);
        break;
    case MUL:
        IF_VERBOSE(3) cout << "multiplicative: " << c << " = " << solutions_op_a[i] << " * " << solutions_op_b[i] << endl;
        preFactor *= solutions_op_b[i];
        if(solutions_op_a[i] != 1) buildAdderGraph(solutions_op_a[i],preFactor);
        if(solutions_op_b[i] != 1) buildAdderGraph(solutions_op_b[i]);
        break;
    case LF2:
        IF_VERBOSE(3) cout << "leapfrog-2: " << solutions_gt[i] << " with " << solutions_op_a[i] << " and " << solutions_op_b[i] << endl;

        compute_successor_set(solutions_op_a[i],1,4*c,&leap1_candidate_set1,false);

        compute_successor_set(c,solutions_op_a[i],4*c,&leap1_candidate_set2,false);

        for(int_set_t::iterator it=leap1_candidate_set2.begin(); it != leap1_candidate_set2.end(); ++it)
        {
            if(*it % solutions_op_b[i] == 0)
                leap1_candidate_set2_div.insert(*it/solutions_op_b[i]);
        }

        set_intersection(leap1_candidate_set1.begin(), leap1_candidate_set1.end(), leap1_candidate_set2_div.begin(), leap1_candidate_set2_div.end(), inserter(leap1_set, leap1_set.begin()));

#if 0 //#ifdef DEBUG
        cout << "leap1_candidate_set1=";
        for(int_set_t::iterator it=leap1_candidate_set1.begin(); it != leap1_candidate_set1.end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;

        cout << "leap1_candidate_set2=";
        for(int_set_t::iterator it=leap1_candidate_set2.begin(); it != leap1_candidate_set2.end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;

        cout << "leap1_candidate_set2_div=";
        for(int_set_t::iterator it=leap1_candidate_set2_div.begin(); it != leap1_candidate_set2_div.end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;

        cout << "leap1_set=";
        for(int_set_t::iterator it=leap1_set.begin(); it != leap1_set.end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;
#endif //DEBUG
        l1 = *(leap1_set.begin());

        if(solutions_op_a[i] != 1) buildAdderGraph(solutions_op_a[i]);
        if(solutions_op_b[i] != 1) buildAdderGraph(solutions_op_b[i],l1);

        assert(getExponents(solutions_op_a[i], l1*solutions_op_b[i], c,&eA,&eB,&signA,&signB));
        generateAOp(solutions_op_a[i],l1*solutions_op_b[i],c,eA,eB,signA,signB);

        //      cout << l1*solutions_op_b[i] << " = " << l1 << " * " << solutions_op_b[i] << endl;

        assert(getExponents(1, solutions_op_a[i],l1,&eA,&eB,&signA,&signB));
        generateAOp(1,solutions_op_a[i],l1,eA,eB,signA,signB);

        break;
    case LF3:
        cout << "Error: Solution of " << c << " requires leapfrog3 structures which are currently unsupported (sorry)" << endl;
        exit(-1);
        break;
  }
}

string SCMOpt::getAdderGraph()
{
    return string("[") + adderGraph.str().substr(0,adderGraph.str().length()-1) + string("]");
}

int SCMOpt::getAdderCost()
{
    return adder_cost;
}
