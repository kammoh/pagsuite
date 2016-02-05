#ifndef MATREE2_H
#define MATREE2_H

#include "logging.h"
#include "mt_options.h"
extern cmd_options global_options;

#include "mt_types.h"
#include "mt_merger.h"
#include "simple_math.h"
#include "mt_matrix_calc.h"
#include "mt_matree_placement_heap.h"

#include <algorithm>
#include <time.h>
#include <vector>
#include <map>

using namespace std;

class merger;

class matree2
{
private:
    vector<vector<float> > matrix;
    vector<vector<l_int> > placements;
    vector<mt_matree_placement_heap*> placement_heaps;
    bool DEBUG;

    index_type config_count;
    index_type node_count;
    merger* main_merger;

    vector<l_int>* placement_at(int i);
    //vector<index_type>* get_last_hot(vector<vector<bool> > *hot_matrix);
    void get_last_hot(vector<vector<bool> > &hot_matrix, vector<index_type>& indices);
    vector<index_type>* get_config_indices_for_column(int index, int row_id);
    int get_index_from_indices(vector<index_type> &indices);
    bool set_hot_when_for_indices(vector<index_type> &indices, vector<vector<bool> > &hot_matrix);
    void unset_hot_for_indices(vector<index_type> &indices, vector<vector<bool> > &hot_matrix);

    void rec_optimize_full(uint row_id, vector<vector<bool> > &hot_matrix, vector<vector<index_type> >& cur_matrix, float cur_cost);
public:
    uint matrix_col_size;
    //matree2(uint size_r,uint size_c,bool dbg=false);
    matree2(uint node_c, uint config_c,merger* main);
    ~matree2();
    void fill_random(int f);
    void create_placements();
    //void create_placements2();
    void quicksort(int left, int right, int row);
    void print_matrix(ostream& dest=cout);
    void print_best();
    void optimize_full();
    void clear_result();
    void next_partition(int row=-1);
    float get_cost(vector<vector<index_type> > *vec);

    void matrix_set_at(uint r, uint c, float value);
    float matrix_get_at(uint r, uint c);

    int max_visit;
    float best_cost;
    float last_stages_cost;
    float *best_cost_all;
    index_type current_stage;

    mt_matrix_calc* matrix_calc;

    struct merge_result
    {
        bool found;
        float cost_all;
        vector<vector<index_type> > last_merge;
    }found_result;
};

#endif // MATREE2_H
