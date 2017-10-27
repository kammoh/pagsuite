#include "compute_realization.h"


bool computeRealization(int_t coeff, int_t c_max, list<realization_row<int_t> >& realization, map<int_t, list<int_t> >& coeffNOFListMap, map<int_t, pair<int,int> >& coeffTopoMap)
{
  if(coeff == 1) return true;

  list<int_t>& nofs = coeffNOFListMap[coeff];
  int noOfAdders = coeffTopoMap[coeff].first;
  int topologyId = coeffTopoMap[coeff].second;

  list<int_t>::iterator it = nofs.begin();

  //initialize NOFs:
  int_t nof1 = *it;
  int_t nof2 = -1;
  if(++it != nofs.end())
  {
    nof2 = *it;
  }

  realization_row<int_t> row;

  switch(noOfAdders)
  {
  case 1:
    if(topologyId != 1)
    {
      cerr << "Error: number of adders (" << noOfAdders << ") must be between 1 and 3" << endl;
      exit(-1);
    }
    row.A=1; row.B=1; row.C=1;
    row.W=coeff;
    if(getExponents<int_t>(row, c_max, false))
    {
      realization.push_back(row);
      return true;
    }
    break;
  case 2:
    switch(topologyId)
    {
    case 1:
      row.A=1; row.B=1; row.C=nof1;
      break;
    case 2:
      row.A=1; row.B=nof1; row.C=nof1;
      break;
    case 3:
      row.A=nof1; row.B=nof1; row.C=nof1;
      break;
    default:
      cerr << "Error: topology " << noOfAdders << "." << topologyId << " does not exist" << endl;
      exit(-1);
    }

    row.W=coeff;
    if(getExponents<int_t>(row, c_max, false))
    {
      realization.push_back(row);

      row.A=1; row.B=1; row.C=1;
      row.W=nof1;
      if(getExponents<int_t>(row, c_max, false))
      {
        realization.push_back(row);
        return true;
      }
    }

    break;
  case 3:
    switch(topologyId)
    {
    case 1:
      row.A=1; row.B=nof1; row.C=nof2;
      break;
    case 2:
      row.A=nof1; row.B=nof1; row.C=nof2;
      break;
    case 3:
      row.A=1; row.B=1; row.C=nof1;
      break;
    case 4:
      row.A=1; row.B=nof1; row.C=nof2;
      break;
    case 5:
      row.A=nof1; row.B=nof1; row.C=nof1;
      break;
    case 6:
      row.A=1; row.B=nof1; row.C=nof1;
      break;
    case 7:
      row.A=1; row.B=nof1; row.C=nof1;
      break;
    case 8:
      row.A=1; row.B=nof1; row.C=nof1;
      break;
    case 9:
      row.A=nof1; row.B=nof1; row.C=nof2;
      break;
    case 10:
      row.A=nof1; row.B=nof1; row.C=nof2;
      break;
    case 11:
      row.A=nof1; row.B=nof1; row.C=nof2;
      break;
    case 12:
      row.A=nof1; row.B=nof1; row.C=nof1;
      break;
    case 13:
      row.A=nof1; row.B=nof1; row.C=nof1;
      break;
    default:
      cerr << "Not implemented so far..." << endl;
//      cerr << "Error: topology " << noOfAdders << "." << topologyId << " does not exist" << endl;
      exit(-1);
    }

    row.W=coeff;

    if(getExponents<int_t>(row, c_max, false))
    {
      realization.push_back(row);

      //add the computation of NOF 1 to adder graph:
      computeRealization(nof1, c_max, realization, coeffNOFListMap, coeffTopoMap);

      //add the computation of NOF 2 to adder graph, but only if not already present in adder graph:
      bool nof2found = false;
      if(nof2 > 0)
      {
        for(list<realization_row<int_t> >::iterator it = realization.begin(); it != realization.end(); ++it)
        {
          if((*it).W == nof2)
          {
            nof2found = true;
            break;
          }

        }
        if(!nof2found) computeRealization(nof2, c_max, realization, coeffNOFListMap, coeffTopoMap);
      }
      return true;
    }
    break;
  default:
    cerr << "Error: number of adders (" << noOfAdders << ") must be between 1 and 3" << endl;
    exit(-1);
  }
  return false;
}
