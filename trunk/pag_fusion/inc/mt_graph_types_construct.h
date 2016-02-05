#ifndef MT_GRAPH_TYPES_CONSTRUCT_H
#define MT_GRAPH_TYPES_CONSTRUCT_H


#include <vector>
#include <map>
#include <stdexcept>
#include "mt_graph_types.h"
#include "adder_graph.h"

using namespace std;

enum adder_graph_node_type{adder_graph_node_type_reg,adder_graph_node_type_add,adder_graph_node_type_sub,adder_graph_node_type_conf_addsub};
enum path_construct_type{path_construct_type_none,path_construct_type_mux,path_construct_type_path};

class mt_path_construct{
public:
    map<l_int,mt_path*> paths;
    path_construct_type paths_type;

    mt_path_construct(){
    }
};

class mt_node_construct{
public:
    map<l_int,mt_node*> nodes;
    vector<mt_path_construct*> inputs;



    mt_node_construct(){}
    ~mt_node_construct()
    {
       while(inputs.size() > 0){
           delete inputs.back();
           inputs.pop_back();
       }
    }
};

class mt_graph_construct{
public:
    vector< vector<mt_node_construct*>* >* stages;
    vector< bool >* stage_has_mux;
    index_type conf_count;
    index_type inp_count;
    index_type no_of_stages;

    vector< vector<register_node_t*>* >* stage_mux_register;
    vector< vector<mux_node_t*>* >* stage_mux;
    list< mux_node_t* > locked_mux;

    mt_graph_construct(int stages_count);

    ~mt_graph_construct();

    void convert(adder_graph_t* new_graph,int no_of_inputs=1);
    bool is_node_reg(mt_node_construct* node);
    int set_node_outputs( adder_graph_base_node_t* new_node,mt_node_construct* node );
    void set_conf_adder( conf_adder_subtractor_node_t* new_node,mt_node_construct* node );
    adder_graph_node_type get_adder_type(mt_node_construct* node, vector<bool> &is_negative);
    register_node_t* get_mux_reg(mt_path_construct* cur_path, map<int, adder_graph_base_node_t *> &pre_stage, int64_t& input_shift );
    mux_node_t* get_mux( mt_path_construct* cur_path, map<int,adder_graph_base_node_t*>* pre_stage );
    void normalize_mux_shift( adder_subtractor_node_t* node,mux_node_t* mux);
    void normalize_all(adder_graph_t *new_graph );

    ///
    /// \brief Untersucht ob das Register einem vorhandenen entspricht
    /// \param Zu suchendes Register
    /// \param Stufe in der Gesucht werden soll
    /// \return true, wenn ein Register gefunden wurde; false, wenn nicht
    ///
    bool combine_reg(register_node_t** reg, int stage);
    bool combine_mux(mux_node_t **mux, int stage);
};






#endif // MT_GRAPH_TYPES_CONSTRUCT_H
