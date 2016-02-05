#ifndef PAG_SPLITTER_H
#define PAG_SPLITTER_H

#include "adder_graph.h"

#include <string>
#include <iostream>
#include <list>
#include <stdlib.h>
#include <stdexcept>
#include <sstream>
#include <cstring>

using namespace std;

class pag_splitter
{
private:
    string instance;
    list<adder_graph_base_node_t*> source_graph_output_nodes;
    list<list<adder_graph_base_node_t*> > groups;

    list<adder_graph_t*> split_graphs;

    void walk_graph(adder_graph_base_node_t* t,adder_graph_t* graph);
    void get_output_nodes();

public:
    adder_graph_t* source_graph;
    pag_splitter(string complete_graph, string instance_name);
    ~pag_splitter();

    void inflate_graph(vector<vector<vector<int64_t> > >& groups);
    void add_group(vector<vector<int64_t> >& group_nodes);
    void add_no_group();
    void detox_graph();

    void split();
    void print_all_splitted(string instance_name);
    void generate_input_file(string instance_name, vector<vector<vector<int64_t> > > &groups);
};


#endif // PAG_SPLITTER_H
