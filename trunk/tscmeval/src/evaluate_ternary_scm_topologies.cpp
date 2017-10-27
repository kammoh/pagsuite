#include "evaluate_ternary_scm_topologies.h"

#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <fstream>

#include "debug.h"
#include "compute_successor_set.h"
#include "rpag_functions.h"
#include "compute_realization.h"
#include "log2_64.h"

using namespace std;

const int evaluateTernarySCMTopologies::maxTopoIDs[] = {0,1,3,13};

evaluateTernarySCMTopologies::evaluateTernarySCMTopologies(int_t c_max, int noOfAddersMax)
{
  this->noOfAddersMax = noOfAddersMax;
  this->c_max = c_max;
  storeAllTopologies = true;

  //resize vectors to max. no. of adders + 1 (incl. zero):
  hset.resize(noOfAddersMax+1);
  coeffSetVecMap.resize(noOfAddersMax+1);
  topoCoeffNOFListMap.resize(noOfAddersMax+1);
  topo2AddCoeffNOFSetMap.resize(noOfAddersMax+1);

  //fill H0.1:
  coeffSetVecMap[0][1].insert(1);
  hset[0].insert(1);

}

void evaluateTernarySCMTopologies::evaluateTopology(int addCount, int topoId)
{
  cout << "computing H" << addCount << "." << topoId << " ..." << endl;

  int_set_t *nof1Set, *nof2Set, *resultSet;
  int nof1TopoId, nof2TopoId=1;
  int nof1AddCount, nof2AddCount=-1;
  bool nof2SetUsed=true;

  switch(addCount)
  {
  case 1:
    //select H0.1:
    nof1AddCount = 0;
    nof1TopoId = 1;
    nof2SetUsed = false;
    aOpArgument1 = getConstantOne;
    aOpArgument2 = getConstantOne;
    aOpArgument3 = getConstantOne;
    break;
  case 2:
    //select H1.1 for all the cases:
    nof1AddCount = 1;
    nof1TopoId = 1;
    nof2SetUsed = false;
    switch(topoId)
    {
    case 1:
      aOpArgument1 = getConstantOne;
      aOpArgument2 = getConstantOne;
      aOpArgument3 = getFirstNOF;
      break;
    case 2:
      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 3:
      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    default:
      cerr << "Error: Topology (" << addCount << "." << topoId << ") not implemented!" << endl;
      exit(-1);
    }
    break;
  case 3:
    switch(topoId)
    {
    case 1:
      //select H1.1 as NOF1:
      nof1AddCount = 1;
      nof1TopoId = 1;

      //select H1.1 as NOF2:
      nof2AddCount = 1;
      nof2TopoId = 1;

      nof2SetUsed = true;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 2:
      //select H1.1 as NOF1:
      nof1AddCount = 1;
      nof1TopoId = 1;

      //select H1.1 as NOF2:
      nof2AddCount = 1;
      nof2TopoId = 1;

      nof2SetUsed = true;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 3:
      //select H2.3:
      nof1AddCount = 2;
      nof1TopoId = 3;

      nof2SetUsed = false;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getConstantOne;
      aOpArgument3 = getFirstNOF;
      break;
    case 4:
      //select H2.3:
      nof1AddCount = 2;
      nof1TopoId = 3;

      nof2SetUsed = true;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 5:
      //select H2.1:
      nof1AddCount = 2;
      nof1TopoId = 1;

      nof2SetUsed = false;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 6:
      //select H2.1:
      nof1AddCount = 2;
      nof1TopoId = 1;

      nof2SetUsed = false;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 7:
      //select H2.2:
      nof1AddCount = 2;
      nof1TopoId = 2;

      nof2SetUsed = false;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 8:
      //select H2.3:
      nof1AddCount = 2;
      nof1TopoId = 3;

      nof2SetUsed = false;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 9:
      //select H2.1:
      nof1AddCount = 2;
      nof1TopoId = 1;

      nof2SetUsed = true;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 10:
      //select H2.2:
      nof1AddCount = 2;
      nof1TopoId = 2;

      nof2SetUsed = true;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 11:
      //select H2.3:
      nof1AddCount = 2;
      nof1TopoId = 3;

      nof2SetUsed = true;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getSecondNOF;
      break;
    case 12:
      //select H2.2:
      nof1AddCount = 2;
      nof1TopoId = 2;

      nof2SetUsed = false;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 13:
      //select H2.3:
      nof1AddCount = 2;
      nof1TopoId = 3;

      nof2SetUsed = false;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    default:
      cerr << "Error: Topology (" << addCount << "." << topoId << ") not implemented!" << endl;
      exit(-1);
    }
    break;
  case 4:
    switch(topoId)
    {
    case 1:
      //select H3.1 as NOF1:
      nof1AddCount = 3;
      nof1TopoId = 1;

      nof2SetUsed = false;

      aOpArgument1 = getConstantOne;
      aOpArgument2 = getConstantOne;
      aOpArgument3 = getFirstNOF;
      break;
    case 86:
      //select H3.2 as NOF1:
      nof1AddCount = 3;
      nof1TopoId = 2;

      nof2SetUsed = false;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    case 96:
      //select H3.13 as NOF1:
      nof1AddCount = 3;
      nof1TopoId = 13;

      nof2SetUsed = false;

      aOpArgument1 = getFirstNOF;
      aOpArgument2 = getFirstNOF;
      aOpArgument3 = getFirstNOF;
      break;
    default:
      cerr << "Error: Topology (" << addCount << "." << topoId << ") not implemented!" << endl;
      exit(-1);
    }
    break;
  default:
    cerr << "Error: Adder count (" << addCount << ") not implemented!" << endl;
    exit(-1);
  }

  int_set_t tmpset;
  int_t nof1,nof2;
  nof1Set = &(coeffSetVecMap[nof1AddCount][nof1TopoId]);
  resultSet = &(coeffSetVecMap[addCount][topoId]);
  int_set_t zeroSet; //set containing a single zero element, needed as NOF2
  zeroSet.insert(0);
  for(int_set_t::iterator it1=nof1Set->begin(); it1 != nof1Set->end(); ++it1)
  {
    nof1 = *it1;
//    cout << "." << flush;
    if(nof2SetUsed)
    {
      if(nof2AddCount < 0) //nof2AddCount < 0 means that nof2 should be chosen from the NOF of nof1
      {
        if(!topo2AddCoeffNOFSetMap[nof1AddCount][nof1TopoId][nof1].empty())
          nof2Set = &(topo2AddCoeffNOFSetMap[nof1AddCount][nof1TopoId][nof1]);
        else
          nof2Set = &(zeroSet); //no nof2 is used, use the set with single zero element
      }
      else //nof2AddCount >= 0 means that nof2 should be chosen from the coefficient set defined by nof2AddCount & nof2TopoId
      {
        nof2Set = &(coeffSetVecMap[nof2AddCount][nof2TopoId]);
      }
    }
    else
    {
      nof2Set = &(zeroSet); //no nof2 is used, use the set with single zero element
    }

    for(int_set_t::iterator it2=nof2Set->begin(); it2 != nof2Set->end(); ++it2)
    {
      nof2 = *it2;

      int_t arg1 = aOpArgument1(nof1,nof2);
      int_t arg2 = aOpArgument2(nof1,nof2);
      int_t arg3 = aOpArgument3(nof1,nof2);

      IF_VERBOSE(3) cout << "compute_successor_set(" << arg1  << "," << arg2  << "," << arg3  << ")" << endl;
      compute_successor_set(arg1, arg2, arg3, c_max, &tmpset, false);
      for(int_set_t::iterator it3=tmpset.begin(); it3 != tmpset.end(); ++it3)
      {
        int_t coeff = *it3;

        if(storeAllTopologies) coeffTopoSetMap[coeff].insert(make_pair(addCount,topoId));
        if(addCount == 2) topo2AddCoeffNOFSetMap[addCount][topoId][coeff].insert(nof1);
        else topo2AddCoeffNOFSetMap[addCount][topoId][coeff].insert(0);  //this is a workaround to store the coefficients as key in a map, could be much improved with a better data structure!!!

        if(coeffTopoMap.find(coeff) == coeffTopoMap.end())
        {
          coeffTopoMap[coeff] = make_pair(addCount,topoId);

          if(addCount > 1)
          {
            coeffNOFListMap[coeff].push_back(nof1);
            topoCoeffNOFListMap[addCount][topoId][coeff].push_back(nof1);

            if((addCount > 2) && (nof2 !=0))
            {
              coeffNOFListMap[coeff].push_back(nof2);
              topoCoeffNOFListMap[addCount][topoId][coeff].push_back(nof2);
            }
          }
        }
      }
      resultSet->insert(tmpset.begin(),tmpset.end());
      hset[addCount].insert(tmpset.begin(),tmpset.end());
      tmpset.clear();
    }
  }
  IF_VERBOSE(2) cout << endl << "H" << addCount << "." << topoId  << "=" << *resultSet << endl;

}

void evaluateTernarySCMTopologies::evaluateAll()
{
  for(int addCount=1; addCount <= noOfAddersMax; addCount++)
  {
    cout << "computing " << addCount << " adder coefficients..." << endl;
    for(int topoId=1; topoId <= maxTopoIDs[addCount]; topoId++)
    {
      evaluateTopology(addCount, topoId);
    }
  }
  if(noOfAddersMax > 3)
  {
    //evaluate selected topologies (pipelined SCM graphs)
    evaluateTopology(4, 1);
    evaluateTopology(4, 86);
    evaluateTopology(4, 96);
  }
}


int_t evaluateTernarySCMTopologies::getConstantOne(int_t nof1, int_t nof2)
{
  UNUSED(nof1);
  UNUSED(nof2);
  return 1;
}

int_t evaluateTernarySCMTopologies::getFirstNOF(int_t nof1, int_t nof2)
{
  UNUSED(nof2);
  return nof1;
}

int_t evaluateTernarySCMTopologies::getSecondNOF(int_t nof1, int_t nof2)
{
  UNUSED(nof1);
  return nof2;
}

void evaluateTernarySCMTopologies::outputCoeffRealizations()
{
  list<realization_row<int_t> > adderGraph;

  cout << "coeff (topology) : NOFs" << endl;
  int noOfAdders;
  int topologyId;
  for(int_t c=1; c < c_max; c+=2)
  {
    if(coeffTopoMap.find(c) != coeffTopoMap.end())
    {
      noOfAdders = coeffTopoMap[c].first;
      topologyId = coeffTopoMap[c].second;
      cout << c << " (" << noOfAdders << "." << topologyId << "), NOFs={" << coeffNOFListMap[c] << "}";
      if(computeRealization(c, c_max, adderGraph, coeffNOFListMap, coeffTopoMap))
      {
        cout << " : " << output_adder_graph(adderGraph, true) << endl;
        validate_adder_graph(adderGraph);
      }
      else
      {
        cerr << endl << "Error: No adder graph could be found!" << endl;
        exit(-1);
      }
      adderGraph.clear();
    }
    else
    {
      cout << c << " (?.?) : ?????????????" << endl;
    }

  }
}

void evaluateTernarySCMTopologies::getAllNOFs(map<int_t, list<int_t> >& coeffNOFListMap, int coeff, set<int> *nofset)
{
  for(list<int_t>::iterator lit = coeffNOFListMap[coeff].begin(); lit != coeffNOFListMap[coeff].end(); ++lit)
  {
    nofset->insert(*lit);
    if(coeffNOFListMap[coeff].size() < 2) //dirty hack to only include 2 NOFs
    {
      getAllNOFs(coeffNOFListMap, *lit, nofset);
    }
  }
}

void evaluateTernarySCMTopologies::saveCoeffNOFs(string filename)
{
  ofstream file;
  file.open(filename.c_str(),ios::out);
  if(file.is_open())
  {
    file.clear();
//  const int nofs[2][2]={{1,2},{3,4}};

    file << "int nofs[" << ((c_max+1)>>1) << "][2]={";
    for(int_t c=1; c < c_max; c+=2)
    {
      if(coeffTopoMap.find(c) != coeffTopoMap.end())
      {
        file << "{";
        set<int> nofset;
        getAllNOFs(coeffNOFListMap, c, &nofset);

        if(!nofset.empty())
        {
          set<int>::iterator setit = nofset.begin();
          file << *setit;
          for(++setit; setit != nofset.end(); ++setit)
          {
            file << "," << *setit;
          }
        }

        file << "}";
        if(c < c_max-2) file << ",";
      }
      else
      {
        cerr << "Error: No NOF found for coefficient!" << endl;
        exit(-1);
      }
    }
    file << "};" << endl;

    file.close();
  }
  else
  {
    std::cout << "ERROR: \t Not able to open output file." << std::endl;
    exit(-1);
  }

}

void evaluateTernarySCMTopologies::outputCoeffCoverage()
{
//  hset[3].insert(hset[2].begin(),hset[2].end()); //??

  int_t limit = c_max;

  for(int addCount=1; addCount <= noOfAddersMax; addCount++)
  {
    for(int_t c=3; c < c_max; c+=2)
    {
      if(hset[addCount].find(c) == hset[addCount].end())
      {
        limit = c-2;
        break;
      }
    }
    cout << "all odd coefficients up to " << limit << " (" << log2f_64(limit) << " bit)" << " can be realized by " << addCount << " ternary adders" << endl;
    limit = c_max;
  }
}

void evaluateTernarySCMTopologies::outputCoeffTopologies()
{
  for(int_t c=1; c < c_max; c+=2)
  {
    if(coeffTopoSetMap.find(c) != coeffTopoSetMap.end())
    {
      cout << c << ": ";
      for(set<pair<int,int> >::iterator it = coeffTopoSetMap[c].begin(); it != coeffTopoSetMap[c].end(); ++it)
      {
        cout << "(" << (*it).first << "," << (*it).second << ") ";
      }
      cout << endl;
    }
    else
    {
      cout << c << " (?.?) : ?????????????" << endl;
    }
  }
}

void evaluateTernarySCMTopologies::outputTopologiesCoverage()
{
  for(int addCount=1; addCount <= noOfAddersMax; addCount++)
  {
    int noOfTotalCoeff = hset[addCount].size();
    for(int topoId=1; topoId <= maxTopoIDs[addCount]; topoId++)
    {
      int noOfTopoCoeff = coeffSetVecMap[addCount][topoId].size();
      cout << "H" << addCount << "." << topoId << " : " << noOfTopoCoeff << " out of " << noOfTotalCoeff << " (" << ((float) 100*noOfTopoCoeff)/noOfTotalCoeff << "%)" << endl;
    }
  }
}

void evaluateTernarySCMTopologies::outputNOFSets()
{
  for(int addCount=1; addCount <= noOfAddersMax; addCount++)
  {
    for(int topoId=1; topoId <= maxTopoIDs[addCount]; topoId++)
    {
      map<int_t, set<int_t> > *coeffNOFSetMap = &(topo2AddCoeffNOFSetMap[addCount][topoId]);
      for(int_t c=1; c < c_max; c+=2)
      {
        map<int_t, set<int_t> >::iterator it = coeffNOFSetMap->find(c);
        if(it != coeffNOFSetMap->end())
        {
          cout << "H" << addCount << "." << topoId << " " << c << " : " << (*it).second << endl;
        }
      }
    }
  }

}

void evaluateTernarySCMTopologies::outputCoeffCoverageForTopology(int addCount, int topoId)
{
  int_t limit = c_max;
  map<int_t, set<int_t> > *coeffNOFSetMap = &(topo2AddCoeffNOFSetMap[addCount][topoId]);
  for(int_t c=1; c < c_max; c+=2)
  {
    map<int_t, set<int_t> >::iterator it = coeffNOFSetMap->find(c);
    if(it == coeffNOFSetMap->end())
    {
      limit = c-2;
      break;
    }
  }
  cout << "H" << addCount << "." << topoId << " can realize coefficients up to " << limit << " (" << log2f_64(limit) << " bit)" << "" << endl;

}

void evaluateTernarySCMTopologies::outputCoeffCoveragePerTopology()
{
  for(int addCount=1; addCount <= noOfAddersMax; addCount++)
  {
    for(int topoId=1; topoId <= maxTopoIDs[addCount]; topoId++)
    {
      outputCoeffCoverageForTopology(addCount, topoId);
    }
  }
  if(noOfAddersMax > 3)
  {
    outputCoeffCoverageForTopology(4, 1);
    outputCoeffCoverageForTopology(4, 86);
    outputCoeffCoverageForTopology(4, 96);
  }

}
