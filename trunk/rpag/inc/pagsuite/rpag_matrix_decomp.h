#ifndef RPAG_MATRIX_DECOMP_H
#define RPAG_MATRIX_DECOMP_H

#include "type_class.h"
#include "rpag_base.h"
#include "rpag_topologies.h"

namespace PAGSuite
{

  class rpag_matrix_decomp : public rpag_base<vec_t>
  {
  public:
    rpag_type operation_type;

    rpag_matrix_decomp(rpag_pointer *rpagp, int No_of_decompose_stages_, rpag_type operation_type) : operation_type(operation_type) //copy constructor
    {
        no_of_decomposition_stages = No_of_decompose_stages_;
        copy_parameters(rpagp);
    }

    virtual int optimize();// clasic RPAG entry point. start the optimisation

    virtual rpag_type get_rpag_type();

    virtual bool is_this_a_two_input_system(void);

    int no_of_decomposition_stages;

  protected:
    int make_all_solutions_same_depth(vector<vector<set<vec_t> > > &solutions);

    void perform_decomposition(set<vec_t> *target_fun_set);

    void insert_problems_into_pipeline_set_rev(vector<set<vec_t> > &pipeline_set_rev, vector<pair<set<vec_t>, vector<bool> > > &problems);

    void split_target_into_2(set<vec_t> target_split_set, vector<bool> partion_helper, vector<pair<set<vec_t>, vector<bool> > > &result);

    void split_target_into_3(set<vec_t> target_split_set, vector<bool> partion_helper, vector<pair<set<vec_t>, vector<bool> > > &result);

    int adder_depth(vec_t x);
  };

}

#endif // RPAG_MATRIX_DECOMP_H
