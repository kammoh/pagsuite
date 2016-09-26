/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#ifndef COMPUTE_SUCCESSOR_H
#define COMPUTE_SUCCESSOR_H


#include "type_class.h"

void compute_successor_set(int_t r1, int_t r2, int_t c_max, int_set_t *successor_set, bool erase_predecessor = true, bool two_input_adder = true, bool ternary_sign_filter = false);
void compute_successor_set(const int_set_t *r1_set, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor = true, bool two_input_adder = true, bool ternary_sign_filter = false);
void compute_successor_set(int_t r1, const int_set_t *r2_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor=true, bool two_input_adder = true, bool ternary_sign_filter = false);

void compute_successor_set(const int_set_t *realized_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor=true, bool two_input_adder = true, bool ternary_sign_filter = false);

void compute_successor_set(int_t r1, int_t r2, int_t r3, int_t c_max, int_set_t *successor_set, bool erase_predecessor=true, bool ternary_sign_filter=false);
void compute_successor_set(int_t r1, int_t r2, const int_set_t *r3_set, int_t c_max, int_set_t *successor_set, bool erase_predecessor=true, bool ternary_sign_filter = false);

void permut(int &a, int &b, int &c, int &k, int &l, int index);

void compute_successor_set(vec_t r1, vec_t r2, int_t c_max, vec_set_t *successor_set, bool erase_predecessor=true, bool two_input_adder=true,bool ternary_sign_filter=false);
void compute_successor_set(const vec_set_t *r1_set, const vec_set_t *r2_set, int_t c_max, vec_set_t *successor_set, bool erase_predecessor = true, bool two_input_adder = true, bool ternary_sign_filter = false);
void compute_successor_set(const vec_set_t *realized_set, int_t c_max, vec_set_t *successor_set, bool erase_predecessor=true, bool two_input_adder = true, bool ternary_sign_filter = false);
void compute_successor_set(vec_t r1, vec_t r2, vec_t r3, int_t c_max, vec_set_t *successor_set, bool erase_predecessor=true,  bool ternary_sign_filter=false);
#endif //COMPUTE_SUCCESSOR_H
