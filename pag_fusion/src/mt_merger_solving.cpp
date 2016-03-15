#include "mt_merger.h"
#include <limits>

void merger::solve()
{
    if(configurations.size()>1 && check_limits())
    {
        TREE_PRINT_INIT("Start");

        calculate_possibilities();
        calculate_decision_count();

        prepare_trees();

        best_cost_all = INT_MAX;

        run_time.start();

        rec_merge_full(stage_count-1,0,start_merging);

        run_time.stop();

        print_result();
        print_stats();
        create_muxed_graph();

        if(global_options.printing_options.perfFile)
        {
            write_statistics_file();
        }
    }
    else
    {
        cerr << "Aborted due to limits" << endl;
        exit_code = -1;

    }
}

void merger::rec_merge_full(index_type cur_stage, float cur_cost, vector<vector<index_type> > &merge_matrix)
{
    if ( global_options.timeout != 0 || EXIT_REQUEST){
        run_time.stop();
        if( run_time.time_elapsed > global_options.timeout )
            exit_call();
    }

    if(cur_stage==0){	TREE_PRINT_DOWN("last stage",cur_cost);}
    else{	TREE_PRINT_DOWN("next stage",cur_cost); }
    bool break_merging=false;

    merge_tree_n[cur_stage+1] = merge_matrix;

    temp_merge(cur_stage,merge_matrix);

    if(global_options.costmodel.strongcost)
    {
        if(cur_stage < stage_count-1){
            cur_cost += compute_strong_cost(cur_stage);
            if(cur_cost > best_cost_all && !global_options.complete) break_merging=true;
        }
    }

    if(break_merging)
    {
        TREE_PRINT_BREAK("scut",cur_cost);
        LOG_OP::log("---str cut---");
        scut_count++;
    }
    else
    {
        if(cur_stage>=stage_count-1 && global_options.printing_options.graphs) print_graphs();

        if(global_options.printing_options.operations)
        {
            for(int i=configurations[0]->stages.size(), i_end = cur_stage+1;i>i_end;--i)
                LOG() << "---";
            LOG()<<">########## stage "<<cur_stage<<" ##########"<<endl;
            LOG()<<"current cost:"<<cur_cost<<endl;
        }

        matree2* mat;
        if(matree_tree_n[cur_stage] == NULL)
        {
            matree_tree_n[cur_stage] = new matree2(stage_node_count[cur_stage]-stage_node_count_ff[cur_stage],configurations.size(),this); // FFNODE: angepasst
        }

        mat = matree_tree_n[cur_stage];
        mat->current_stage = cur_stage;
        mat->constant_add_cost = 0;
        current_stage = cur_stage;
        mat->last_stages_cost = cur_cost;
        mat->best_cost_all = &best_cost_all;

        if(global_options.printing_options.operations) LOG()<<"fill matrix..."<<endl;
        vector<index_type> tmp(configurations.size());
        prepare_decision_nodes( cur_stage );
        fill_matrix(cur_stage,0,tmp,mat);
        clear_decision_nodes();

        if(global_options.printing_options.merge)
        {
            LOG()<<"used merging:"<<endl;
            LOG()<<print_mat(merge_matrix);
        }
        if(global_options.printing_options.cost)
        {
            LOG()<<"cost matrix:"<<endl;
            mat->print_matrix(LOG());
        }

        if(global_options.printing_options.operations) LOG()<<"create placements..."<<endl;

        mat->create_placements();

        if(stage_node_count[cur_stage]>1)
        {
            if(cur_stage==0)
            {

                if(global_options.printing_options.operations) LOG()<<"last stage..."<<endl;

                float part_cost = 0;
                if(global_options.costmodel.strongcost)
                {
                    part_cost = compute_strong_cost(cur_stage-1);
                }
                else
                {
                    ushort t_node_count = stage_node_count[cur_stage];
                    vector<index_type> indices( configurations.size() );
                    for( index_type i=0;i<t_node_count;++i )
                    {
                        std::fill(indices.begin(),indices.end(),i);
                        part_cost += mat->matrix_get_at((uint)i,(uint)get_index_from_indices( indices,t_node_count ));
                    }
                }
                mat->found_result.cost_all = mat->last_stages_cost + part_cost;
                vector<index_type> inner_last_merge(configurations.size());
                mat->found_result.last_merge.clear();
                for(unsigned int i=0, i_end = stage_node_count[0];i< i_end;++i)
                {
                    std::fill(inner_last_merge.begin(),inner_last_merge.end(),i);
                    mat->found_result.last_merge.push_back(inner_last_merge);
                }
                mat->found_result.found = true;
            }
            else
            {
                if(global_options.printing_options.operations)
                    LOG()<<"step in..."<<endl;

                stepin_count++;
                mat->optimize_full();
            }
        }
        else
        {
            if(global_options.printing_options.operations) LOG()<<"1 node stage..."<<endl;
            float part_cost = 0.0;
            if(global_options.costmodel.strongcost)
            {
                part_cost = compute_strong_cost((int)cur_stage-1);
            }
            else
            {
                part_cost = mat->matrix_get_at(0,0);
            }
            if(cur_stage==0)
            {
                if(global_options.printing_options.operations) LOG()<<"last stage..."<<endl;
                mat->found_result.cost_all = mat->last_stages_cost + part_cost;
                mat->found_result.found = true;
            }
            else if( global_options.complete || (cur_cost+part_cost) < best_cost_all )
            {
                TREE_PRINT_DOWN("skip stage",-1);
                vector<vector<index_type> > merge;
                vector<index_type> inner_merge(configurations.size());
                merge.push_back(inner_merge);
                if(global_options.costmodel.strongcost) rec_merge_full(cur_stage-1,cur_cost,merge);
                else rec_merge_full(cur_stage-1,cur_cost+ part_cost,merge);

                TREE_PRINT_UP();
            }
        }
        if(mat->found_result.found)
        {
            if(best_cost_all > mat->found_result.cost_all)
            {
                if(stage_node_count[cur_stage]>1)
                {
                    merge_tree_n[cur_stage] = mat->found_result.last_merge;
                }
                else
                {
                    vector<vector<index_type> > t_zero(1);
                    vector<index_type> inner_t_zero(configurations.size());
                    t_zero[0] = inner_t_zero;
                    merge_tree_n[cur_stage] = t_zero;
                }

                copy_current_tree();

                best_cost_all = mat->found_result.cost_all;
                TREE_PRINT_BREAK("new",best_cost_all);
                 LOG_OP::log("---NEW---");
                if(global_options.printing_options.newOcc) LOG()<<".";
                new_count++;
                if(global_options.printing_options.operations) LOG()<<"cost:"<<mat->found_result.cost_all<<endl;
            }
            else
            {
                ign_count++;
                 LOG_OP::log("---ign---");
                TREE_PRINT_BREAK("ign",mat->found_result.cost_all);
            }
            mat->clear_result();
        }
        //if(go_up){ TREE_PRINT_UP();}
    }
    TREE_PRINT_UP();
}

void merger::fill_matrix(index_type cur_stage, index_type cur_config, vector<index_type> &current, matree2* matrix, bool has_fixed)
{
    if(cur_config<configurations.size())
    {
        mt_stage* stage = configurations[cur_config]->stages[cur_stage];
        for(vector<mt_node*>::iterator iter = stage->nodes.begin();iter!=stage->nodes.end();++iter)
        {
            current[cur_config] = (*iter)->id;
            bool has_fixedt = (has_fixed | (*iter)->is_fixed);
            fill_matrix(cur_stage,cur_config+1,current,matrix,has_fixedt);
        }
    }
    else
    {
         float cost = 0;
         if( has_fixed ){
             bool fully_fixed = true;
             bool has_fully_fixed = false;
             for(int i=0;i<current.size();++i){
                 if( !configurations[i]->stages[cur_stage]->nodes[current[i]]->is_fully_fixed ){
                     fully_fixed = false;
                 }else{
                     has_fully_fixed = true;
                 }
             }
             if(fully_fixed){  // <= All nodes fully fixed, check if they fit together
                 mt_node *f = configurations[0]->stages[cur_stage]->nodes[current[0]];
                 bool fully_fixed=true;
                 for(int i=0;i<f->fixed_to.size();++i){
                    if( current[ f->fixed_to[i]->cfg_id ] != f->fixed_to[i]->id ){
                        fully_fixed = false;
                        break;
                    }
                 }
                 if(fully_fixed){
                     cost = path_combine(current,cur_stage);
                     matrix->constant_add_cost += cost;
                 }else{
                     // <= its not allowed to enter ff-nodes as they don't exist in matrix
                 }
             }else if( !has_fully_fixed ){ // <= now we need to check if the fixed nodes fit together
                 bool matched_fixing = true;
                 for(int i=0;i<current.size();++i){
                     mt_node *f = configurations[0]->stages[cur_stage]->nodes[current[0]];
                     if( f->is_fixed ){
                         for(int j=0;j<f->fixed_to.size();++f){     // <= i now this will create redundancy, but it's securerer
                             if( current[ f->fixed_to[i]->cfg_id ] != f->fixed_to[i]->id ){
                                 matched_fixing = false;
                                 break;
                             }
                         }
                         if( !matched_fixing )
                             break;
                     }
                 }
                 if( matched_fixing ){
                     cost = path_combine(current,cur_stage);
                     matrix->matrix_set_at( current[0],matrix->matrix_calc->calc_vec_to_int(current),cost );
                 }else{
                     cost = numeric_limits<float>::infinity();
                     matrix->matrix_set_at( current[0],matrix->matrix_calc->calc_vec_to_int(current),cost );
                 }
             }
         }else{
             cost = path_combine(current,cur_stage);
             matrix->matrix_set_at( current[0],matrix->matrix_calc->calc_vec_to_int(current),cost );
         }
    }
}

void merger::temp_merge(index_type cur_stage, vector<vector<index_type> > &mat)
{
    for(uint i=1, i_end = configurations.size();i<i_end;++i)
    {
        mt_stage* stage = configurations[i]->stages[cur_stage];
        vector<index_type> t_vec;
        mt_matrix_calc::make_vec_from_column(mat,i,t_vec);

        for(vector<mt_node*>::iterator node_iter=stage->nodes.begin(), node_iter_end = stage->nodes.end();
            node_iter!=node_iter_end;
            ++node_iter)
        {
            for(vector<mt_path*>::iterator path_iter=(*node_iter)->paths_down.begin(), path_iter_end = (*node_iter)->paths_down.end();
                path_iter!= path_iter_end;
                ++path_iter)
            {
                for(uint j=0, j_end = t_vec.size();j<j_end;++j)
                {
                    if(t_vec[j]==(*path_iter)->target->id)
                    {
                        (*path_iter)->target->id_merged = j;
                        break;
                    }
                }
            }
        }
        stage = configurations[i]->stages[cur_stage+1];
        for(vector<mt_node*>::iterator node_iter=stage->nodes.begin(), node_iter_end = stage->nodes.end();
            node_iter!=node_iter_end;
            ++node_iter)
        {
            for(uint j=0, j_end = t_vec.size();j<j_end;++j)
            {
                    if( (*node_iter)->id == t_vec[j] )
                    {
                        (*node_iter)->id_merged = j;
                         break;
                    }
            }
        }
    }
}

float merger::compute_strong_cost(int cur_stage_id)
{
    vector< mt_decision_node* > decision_nodes( stage_node_count[cur_stage_id+2] );

    for( uint i=0, i_end = decision_nodes.size();i<i_end;++i)
        decision_nodes[i] = new mt_decision_node(configurations.size());

    map<int,vector<mt_node*> >::iterator ghost_it;
    if( (ghost_it = ghost_nodes_map.find(cur_stage_id+2)) != ghost_nodes_map.end() )
    {
        for( vector<mt_node*>::iterator iter = (*ghost_it).second.begin(), iter_end = (*ghost_it).second.end();iter!=iter_end;++iter  )
        {
            decision_nodes[(*iter)->id_merged]->no_of_dont_cares++;
            decision_nodes[(*iter)->id_merged]->decision_types[ (*iter)->cfg_id ] = DECISION_DONT_CARE;
        }
    }

    for(uint c=0, c_end = configurations.size();c<c_end;++c)
    {
        mt_stage* cur_stage = configurations[c]->stages[cur_stage_id+2];
        for( vector<mt_node*>::iterator node_it=cur_stage->nodes.begin(), node_it_end = cur_stage->nodes.end();node_it!=node_it_end;++node_it  )
        {
            mt_decision_node* dec_node = decision_nodes[(*node_it)->id_merged];

            if(dec_node->outputs_ws < (*node_it)->wordsize )
                dec_node->outputs_ws = (*node_it)->wordsize;
            if( (*node_it)->is_ternary ) dec_node->has_ternary = true;
            for( vector<mt_path*>::iterator path_it=(*node_it)->paths_up.begin(), path_it_end = (*node_it)->paths_up.end();path_it!=path_it_end;++path_it)
            {
                mt_path* t = *path_it;

                dec_node->decision_paths[c].push_back(t);
                if(t->isreg) dec_node->decision_types[c] = DECISION_REG;
                else dec_node->decision_types[c] = DECISION_ADD;
            }
        }
    }

    float cost_complete = 0.0;
    for(uint i =0, i_end = decision_nodes.size();i<i_end;++i)
    {
        mt_decision_node* t = decision_nodes[i];
        t->expand();
        t->get_best();
        float cost = t->result.cost;
        if(cost != FLT_MAX)
        {
            cost_complete += cost;
            switch(t->result.type)
            {
            case 'R': cost_complete += t->outputs_ws * COST_REG; break;
            case 'A': cost_complete += t->outputs_ws * COST_ADD; break;
            case 'S': cost_complete += t->outputs_ws * COST_SUB; break;
            case 'C': cost_complete += t->outputs_ws * COST_ADDSUB; break;
            }
        }
        delete t;
    }

    decision_nodes.clear();

    return cost_complete;
}

