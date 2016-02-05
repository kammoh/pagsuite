#ifndef MERGER_H
#define MERGER_H

#include "mt_debug.h"
#include "logging.h"
#include "mt_options.h"
extern cmd_options global_options;
extern bool EXIT_REQUEST;
#include "mt_cost_functions.h"
#include "abstraction_time.h"

#include "adder_graph.h"
#include "mt_matree2.h"

#include "mt_tree_printer.h"
#include "mt_graph_types_construct.h"
#include "mt_types.h"
#include "mt_decision_node.h"
#include "pthread.h"
#include "mt_limits.h"
#include "parsers/inputparser.h"

#include <limits.h>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <cstring>

using namespace std;

/*!
 * \struct cmd_options
 * \brief Struktur für die über die Commandozeile übergebenen Parameter
 *
 */

struct path_mux_decision_state;
class matree2;

/*!
 * \defgroup main Hauptprogrammteile
 * \class class merger
 * \brief Hauptklasse der Optimierung
 *
 * Erstellt und durchläuft Graphen und steuert den
 * Optimierungsvorgang. Konstruiert neue gemergte Graphen
 * auf Grundlage des Optimierungsvorgangs.
 */
class merger
{
public:
	int exit_code;

    unsigned long tcut_count;
    unsigned long cut_count;
    unsigned long ign_count;
    unsigned long new_count;
    unsigned long stepin_count;
    unsigned long rec_count;
    unsigned long dec_count;
    unsigned long pos_count;
    unsigned long scut_count;
    unsigned long max_visit;
    unsigned long max_col_size;

    //mt_merger
    merger();
    ~merger();

    //mt_merger_input_related
    merger(string);

    //mt_merger_solving
    void solve();
    void rec_merge_full(index_type cur_stage, float cur_cost, vector<vector<index_type> > &merge_matrix);

    //mt_merger_misc
    void write_statistics_file();
    void print_graphs();

    void print_stats();
    void exit_call();

    template<typename T>
    static string print_vec(vector<T>& vec){
        stringstream ss;
        ss << "|\t";
        for(uint i=0, i_end = vec.size();i<i_end;++i )
            ss << vec[i] << "\t";
        ss << "|";
        return ss.str();
    }

    template<typename T>
    static string print_mat(vector<vector<T> >& mat){
        stringstream ss;
        for(uint i = 0, i_end = mat.size();i<i_end;++i)
        {
            ss << "|\t";
            for(uint j = 0, j_end = mat[i].size();j<j_end;++j )
                ss << mat[i][j] << "\t";
            ss << "|\n";
        }
        return ss.str();
    }

private:
    time_messure run_time;
    bool has_ghosts;
    index_type current_stage;
    index_type stage_count;
    index_type inp_count;
    float best_cost_all;
    mt_graph_construct* new_graph;
    mt_path* path_ghost;
    mt_path* path_nop;

    vector<mt_graph*> configurations;

    map<int,vector<mt_node*> > ghost_nodes_map;
    map<int,vector<vector<index_type> > > current_merge_tree;

    vector<index_type> stage_node_count;
    vector<vector<index_type> > start_merging;

    vector<vector<vector<index_type> > > best_merge_tree_n;
    vector<vector<vector<index_type> > > merge_tree_n;
    vector<matree2*> matree_tree_n;

    vector<mt_decision_node*> stage_decision_nodes;

    map<string,string> statistics_mem;

    template<typename T>
    void add_to_stats(string name,T val)
    {
        stringstream st;
        st << val;
        //statistics_mem.insert( make_pair<string,string>( name,st.str() )  );
        statistics_mem.insert( make_pair( name,st.str() )  );
    }

    //mt_merger
    void init();

    //mt_merger_contruction
    void create_muxed_graph();

    //mt_merger_input_related
    void start_node_count_check();
    void start_merge(vector<vector<vector<int64_t> > >& start_nodes);
    void add_graph(string graph_string);
    void convert_graph(adder_graph_t *graph);
    void resort_inputs(mt_stage *stage);
    void start_normalize(vector<vector<vector<int64_t> > >& start_nodes);
    void fix_nodes(const vector< InputParser::fix_node >& fnodes);

    //mt_merger_solving
    void temp_merge(index_type cur_stage, vector<vector<index_type> > &mat);
    float compute_strong_cost(int cur_stage_id);
    void fill_matrix(index_type cur_stage, index_type cur_config, vector<index_type> &current, matree2* matrix, bool allowed = true);

    //mt_merger_solving_cost
    float path_combine(vector<index_type> &cur_nodes, index_type cur_stage);
    void clear_decision_nodes();
    void prepare_decision_nodes(index_type cur_stage);
    void collect_paths(vector<index_type> &cur_nodes, int cur_stage);

    //mt_merger_misc
    int get_index_from_indices(vector<index_type> &indices, ushort node_count);
    void input_reorder();
    void copy_current_tree();
    void print_result();
    string trim_string(string txt);
    void prepare_trees();
    void calculate_decision_count();
    void calculate_possibilities();
    void walk_complete_tree(low_tree* leave,int tree_stage);
    void get_pag_info(adder_graph_t* graph);
    bool check_limits();
    mt_node* find_node(const int& cfgid, const int &stageid, const vector<int64_t> &outputs );

};

#endif // MERGER_H
