#ifndef PAGEXPONENTS_H
#define PAGEXPONENTS_H

#include <set>
#include <string>
//#include <cstdint>

//typedef long long int int64_t;
//typedef long long int int_t;

#include "compute_successor_set.h"
#include "types.h"

using namespace std;
//using namespace rpag;

bool getExponents(int_t a, int_t b, int_t c, int *eA, int *eB, int *signA, int *signB);
//bool getExponents(int_t &a, int_t &b, int_t &w, int *eA, int *eB, int *signA, int *signB);
//bool getExponents(int_t &a, int_t &b, int_t &c, int_t &w, int *eA, int *eB, int *eC, int *signA, int *signB, int *signC, int c_max);
bool getExponents(int_t &a, int_t &b, int_t &c, int_t &w, int *eA, int *eB, int *eC, int *signA, int *signB, int *signC, int_t c_max, bool ternary_sign_filter=false);
void computeAdderGraphTernary(set<int> &nof_set, set<int> &coefficient_set, string &adderGraphString);
#endif // PAGEXPONENTS_H
