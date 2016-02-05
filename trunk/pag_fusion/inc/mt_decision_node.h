#ifndef MT_DECISION_NODE_H
#define MT_DECISION_NODE_H

#include <cfloat>
#include <vector>
#include <algorithm>
#include "mt_cost_functions.h"
#include "mt_graph_types.h"
#include "mt_graph_types_construct.h"
#include "logging.h"
#include "mt_options.h"
using namespace std;

extern cmd_options global_options;

enum DECISION_NODE_TYPE{
    DECISION_ADD,
    DECISION_ADD3,
    DECISION_REG,
    DECISION_DONT_CARE,
    DECISION_NONE
};

class mt_decision_node{
public:
    vector<vector<mt_path*> > decision_paths;
    vector<vector<int> > paths;
    vector<DECISION_NODE_TYPE> decision_types;

    int no_of_dont_cares; //!< dont care */
    bool has_ternary;
    bool construct;
    int noLines;
    short neg_shift;
    unsigned short outputs_ws;
    struct{
        float cost;
        unsigned char type;
        vector<vector<int> > paths_copy;
    } result;
    float best_cost;
    unsigned char cur_type;

    float (*costfunction)(mt_decision_node*);

    mt_decision_node(int size);
    void add_path(mt_path* new_path);
    void clear();
    bool swap_cfg(int cfg);
    void expand();
    void get_best( );
    void get_best( unsigned int cur_cfg );
    void swap_to_best_state();
    bool construct_best(mt_node_construct* new_output_node);
    void print_state(float cost);
};

#endif // MT_DECISION_NODE_H
