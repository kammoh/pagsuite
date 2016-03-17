#ifndef GRAPH_TYPES_H
#define GRAPH_TYPES_H

#include <vector>
#include <sstream>
#include <set>
#include <cstdlib>
#include <iostream>
#include <cfloat>
#include "mt_types.h"
#include "adder_graph.h"
using namespace std;
class mt_node;

class mt_path
{
public:
    mt_node* parent;
    mt_node* target;
    bool is_ghost;
    bool is_nop;
    bool isreg;
    bool is_neg;
   // index_type to;
   // index_type to_merged;
    l_int shift;

    mt_path();

    mt_path(index_type t,short s,bool reg);
    mt_path(bool ghost);
    void print(ostream& dest = cout, std::string pre = "");
};

class mt_node
{
public:
    index_type cfg_id;
    index_type id;
    bool is_fixed;
    bool is_fully_fixed;
    l_int id_merged;
    l_int neg_shift;
    vector<int64_t> values;
    bool is_ghost;
    bool is_ternary;
    ushort wordsize;
    vector<mt_path*> paths_up;
    vector<mt_path*> paths_down;

    vector<mt_node*> fixed_to;

    mt_node(index_type index);
    ~mt_node();

    void add(index_type t,ushort s,bool isreg=false);
    string print_values();
    void print(ostream& dest = cout,std::string pre="");
};

class mt_stage
{
public:
    index_type id;
    vector<mt_node*> nodes;
    vector<mt_node*> fully_fixed_nodes;
    mt_stage(index_type index);

    ~mt_stage();
    void add(mt_node* node);
    void print(ostream& dest = cout,std::string pre="");
};

class mt_graph{
public:
    vector<mt_stage*> stages;

    mt_graph();
    ~mt_graph();
    void add(mt_stage* stage);
    void print(index_type id,ostream& dest = cout);
};

struct temp_path_cost{
    index_type used_id;
    uint part_cost;
    bool used;
};

#endif // GRAPH_TYPES_H
