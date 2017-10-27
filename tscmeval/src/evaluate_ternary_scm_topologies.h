#ifndef EVALUATE_TERNARY_SCM_TOPOLOGIES_H
#define EVALUATE_TERNARY_SCM_TOPOLOGIES_H

#include "debug.h"

class evaluateTernarySCMTopologies
{
public:
  evaluateTernarySCMTopologies(int_t c_max, int noOfAddersMax);

  void evaluateAll();
  void evaluateTopology(int addCount, int topoId);

  void outputCoeffRealizations();
  void saveCoeffNOFs(string filename);
  void outputCoeffCoverage();
  void outputCoeffTopologies();
  void outputTopologiesCoverage();
  void outputNOFSets();
  void outputCoeffCoverageForTopology(int addCount, int topoId);
  void outputCoeffCoveragePerTopology();

  bool storeAllTopologies; //if true, all topologies for a given coefficient are stored in coeffTopoSetMap

  int noOfAddersMax;
private:
  int_t c_max;

  map<int_t, list<int_t> > coeffNOFListMap;
  map<int_t, pair<int,int> > coeffTopoMap;

  map<int_t, set<pair<int,int> > > coeffTopoSetMap;

  vector<map<int, map<int_t, list<int_t> > > > topoCoeffNOFListMap; //maps #add & topology to a map which maps the coefficient to a list of NOFs, access NOF list via topoCoeffNOFListMap[#add][topoId][coeff]
  vector<map<int, map<int_t, set<int_t> > > > topo2AddCoeffNOFSetMap; //maps #add & topology to a map which maps the coefficient to a set of NOFs (containing single NOFs of two adder graphs), access NOF set via topoCoeffNOFListMap[#add][topoId][coeff]
  vector<map<int, int_set_t> > coeffSetVecMap; //maps #add & topology to coefficient set, access coefficient set via coeffSetVecMap[#add][topoId]
  vector<int_set_t> hset; //set of all coefficients with a given #add, index is #add

  static int_t getConstantOne(int_t nof1, int_t nof2);
  static int_t getFirstNOF(int_t nof1, int_t nof2);
  static int_t getSecondNOF(int_t nof1, int_t nof2);

  static void getAllNOFs(map<int_t, list<int_t> >& coeffNOFListMap, int coeff, set<int> *nofset);

  int_t (*aOpArgument1)(int_t nof1, int_t nof2);
  int_t (*aOpArgument2)(int_t nof1, int_t nof2);
  int_t (*aOpArgument3)(int_t nof1, int_t nof2);

  static const int maxTopoIDs[]; //arry containing the number of different topologies for a given numer of adders

};


#endif // EVALUATE_TERNARY_SCM_TOPOLOGIES_H
