#include "mt_matree2.h"
#include "logging.h"
#include <limits>

matree2::matree2(uint node_c, uint config_c,merger* main)
{
    max_visit = 0;
    DEBUG = false;
    config_count = config_c;
    node_count = node_c;
    matrix_calc = new mt_matrix_calc(config_c,node_c);
    matrix_col_size = simple_math::pow(node_c,config_count-1);

    matrix.resize(node_c);
    vector<float> inner_matrix(matrix_col_size);
    std::fill(matrix.begin(),matrix.end(),inner_matrix);

    placements.resize(node_c);
    vector<l_int> inner_placements(matrix_col_size);
    std::fill(placements.begin(),placements.end(),inner_placements);

    placement_heaps.resize(node_c);
    for( int i = 0;i<(int)node_c;i++ )
    {
        placement_heaps[i] = new mt_matree_placement_heap( &matrix[i] );
    }

    last_stages_cost = 0;
    found_result.found = false;
    main_merger = main;
}

matree2::~matree2()
{
    for( int i = 0;i<(int)placement_heaps.size();i++ )
    {
       delete placement_heaps[i];
    }
    placement_heaps.clear();
    delete matrix_calc;
}

void matree2::create_placements()
{
    for(int i=0;i<node_count;++i)
    {
        for(int j=0;j<(int)placements[i].size();j++)
        {
            placements[i][j] = placement_heaps[i]->pop();
        }
    }

}


void matree2::rec_optimize_full(uint row_id, vector<vector<bool> > &hot_matrix, vector<vector<index_type> > &cur_matrix, float cur_cost)
{
    if(row_id == matrix.size()-1)
    {
        vector<index_type> indices;
        get_last_hot(hot_matrix,indices);

        int cur_index = 0;
        float part_cost = 0;
        if(!global_options.costmodel.strongcost)
        {
            cur_index = matrix_calc->calc_vec_to_int(indices);
            part_cost = matrix[row_id][cur_index] + cur_cost;
        }

        if ( matrix[row_id][cur_index] != numeric_limits<float>::infinity() ){
            if( global_options.costmodel.strongcost || global_options.complete || (part_cost+last_stages_cost) < *best_cost_all )
            {
                main_merger->rec_count++;
                for(uint f=0, f_end = indices.size();f<f_end;++f)
                {
                    cur_matrix[row_id][f] = indices[f];
                }
                TREE_PRINT_DOWN(merger::print_vec(indices),matrix[row_id][cur_index] );
                if(current_stage>0)
                {
                    if(global_options.costmodel.strongcost) main_merger->rec_merge_full(current_stage-1,last_stages_cost,cur_matrix);
                    else main_merger->rec_merge_full(current_stage-1,part_cost+last_stages_cost,cur_matrix);
                }
                else
                {
                    if(global_options.costmodel.strongcost) found_result.cost_all = last_stages_cost;
                    else found_result.cost_all = part_cost+last_stages_cost;
                    found_result.found = true;
                    found_result.last_merge = cur_matrix;
                }
                TREE_PRINT_UP();
            }
            else
            {
                TREE_PRINT_BREAK("cut",last_stages_cost+part_cost);
                LOG_OP::log("---cut---");

                main_merger->cut_count++;
            }
        }
    }
    else
    {
        vector<index_type> indices(config_count);
        int visited_nodes=0;
        int cur_index;
        for(uint i=0;i<matrix_col_size;++i)
        {
            cur_index = placements[row_id][i];

            matrix_calc->calc_int_to_vec(cur_index,indices,row_id);
            if (matrix[row_id][cur_index] == numeric_limits<float>::infinity())
                break;

            if( set_hot_when_for_indices(indices,hot_matrix) )
            {
                visited_nodes++;

                float part_cost = matrix[row_id][cur_index]+cur_cost;
                if( global_options.costmodel.strongcost || global_options.complete || last_stages_cost+part_cost < *best_cost_all )
                {
                    main_merger->rec_count++;
                    for(uint f=0, f_end = indices.size();f<f_end;++f)
                    {
                        cur_matrix[row_id][f] = indices[f];
                    }
                    TREE_PRINT_DOWN(merger::print_vec(indices),matrix[row_id][cur_index]);
                    if (visited_nodes>max_visit)
                        max_visit = visited_nodes;

                    rec_optimize_full(row_id+1,hot_matrix,cur_matrix,part_cost);
                    TREE_PRINT_UP();
                }
                else
                {
                    TREE_PRINT_BREAK("cut",last_stages_cost+part_cost);
                    LOG_OP::log("---cut---");
                    main_merger->cut_count++;
                }
                unset_hot_for_indices(indices,hot_matrix);
            }
            if(global_options.greedy_depth!=-1 && visited_nodes>global_options.greedy_depth )
                break;
        }
    }
}

int matree2::get_index_from_indices(vector<index_type> &indices)
{
    int exp = 1;
    int index = 0;
    for(int i=1;i<config_count;++i)
    {
        index += exp * indices[i];
        exp *= node_count;
    }
    return index;
}

void matree2::get_last_hot(vector<vector<bool> > &hot_matrix, vector<index_type>& indices)
{
    for(int i=0;i<config_count;++i)
    {
        for(int j=0;j<node_count;++j)
        {
            if( !hot_matrix[i][j] )
            {
                indices.push_back(j);
                break;
            }
        }
    }
}

vector<index_type>* matree2::get_config_indices_for_column(int index,int row_id)
{
    vector<index_type>* indices = new vector<index_type>(config_count);
    indices->push_back(row_id);
    for(int i=1;i<config_count;++i)
    {
        indices->push_back(index%node_count);
        index/=node_count;
    }
    return indices;
}

bool matree2::set_hot_when_for_indices(vector<index_type> &indices, vector<vector<bool> > &hot_matrix)
{
    for(int i=0;i<config_count;++i)
    {
        if(hot_matrix[i][indices[i]])
        {
            return false;
        }
    }
    for(int i=0;i<config_count;++i)
    {
        hot_matrix[i][indices[i]] = true;
    }
    return true;
}

void matree2::unset_hot_for_indices(vector<index_type> &indices, vector<vector<bool> > &hot_matrix)
{
    for(int i=0;i<config_count;++i)
    {
        hot_matrix[i][indices[i]] = false;
    }
}

void matree2::print_best()
{
    cout<<"cost:"<<best_cost<<endl;
}

void matree2::print_matrix(ostream &dest)
{
    dest << merger::print_mat(matrix);
}

void matree2::optimize_full()
{
    float lowest_sum=0;
    max_visit = 0;

    for(int i=0;i<node_count;++i)
    {
        lowest_sum += matrix[i][placements[i][0]];
    }

    if( global_options.costmodel.strongcost || global_options.complete || *best_cost_all > (last_stages_cost + lowest_sum) )
    {
        vector<vector<index_type> > vec(node_count);
        vector<index_type> inner_vec(config_count);
        std::fill(vec.begin(),vec.end(),inner_vec );
        vector<vector<bool> > hot_vec(config_count);
        vector<bool> inner_hot_vec(node_count,false);
        std::fill(hot_vec.begin(),hot_vec.end(),inner_hot_vec );

        rec_optimize_full(0,hot_vec,vec,0);
    }
    else
    {
        main_merger->tcut_count++;
        TREE_PRINT_BREAK("tcut",last_stages_cost + lowest_sum);
         LOG_OP::log("---tcut---");
    }
}

void matree2::matrix_set_at(uint r, uint c, float value)
{
    matrix[r][c] = value;
    placement_heaps[r]->push(c);
}

float matree2::matrix_get_at(uint r, uint c)
{
    return matrix[r][c];
}

float matree2::get_cost(vector<vector<index_type> > *vec)
{
    float cost=0;
    for(uint i=0, i_end = vec->size();i<i_end;++i)
    {
        cost += matrix[i][vec->at(i)[0]];
    }
    return cost;
}

void matree2::clear_result()
{
    found_result.cost_all = 0;
    found_result.found = false;
    found_result.last_merge.clear();
}

