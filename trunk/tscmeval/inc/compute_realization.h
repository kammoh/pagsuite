#ifndef COMPUTE_REALIZATION_H
#define COMPUTE_REALIZATION_H

#include "types.h"
#include "rpag_functions.h"

bool computeRealization(int_t coeff, int_t c_max, list<realization_row<int_t> >& realization, map<int_t, list<int_t> >& coeffNOFListMap, map<int_t, pair<int,int> >& coeffTopoMap);

#endif // COMPUTE_REALIZATION_H
