#ifndef PAGLIB_SCOPES_H
#define PAGLIB_SCOPES_H

#include <iostream>
#include <sys/types.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <set>
#include "copa/copa.h"
#include "adder_graph.h"

using namespace std;
using namespace CoPa_clabana;

struct node_part{
vector<int64_t> shifts;
vector<vector<int64_t> > factors;
int stage;
};

class adder_tree_scope : public CoPa_Scope{
public:
    adder_graph_t* graph;
    int no_of_confs;
    int no_of_input_nodes;
    pair<vector<vector<int64_t> >,int > output_and_stage_pair;
    multimap<pair<vector<vector<int64_t> >,int >,adder_graph_base_node_t* > realized_nodes_map;
    vector<pair<vector<vector<int64_t> >,int > > node_inputs;
    map<adder_graph_base_node_t*,vector<pair<vector<vector<int64_t> >,int > > > node_inputs_map;
    std::map<std::string,std::string> parameters;

    void getInputs(adder_graph_base_node_t* node, vector<pair<adder_graph_base_node_t *, vector<bool> > > &input_nodes);
    int vecEqual( vector<int64_t> v1,vector<int64_t> v2 );

    void onEntry();
    void onEvent(int id);

    void onExit();
};

class node_scope : public CoPa_Scope{
public:
    char current_type;
    vector<node_part> parts;
    std::map<std::string,std::string> parameters;

    void onEntry();
    void buildAdder();
    void buildMux();
    void buildReg();
    void onExit();
    void onEvent(int id);
};

class node_core_scope : public CoPa_Scope{
public:
    vector<int64_t> inner_factors;
    node_part part;

    void onEntry();
    void onExit();
    void onEvent(int id);
};

class outfact_single_scope : public CoPa_Scope{
public:
    void onExit();
};

class shift_single_scope : public CoPa_Scope{
public:
    void onExit();
};

#endif // PAGLIB_SCOPES_H
