#include "mt_merger.h"
#include <cmath>

void merger::print_graphs()
{
    for(uint i=0, i_end = configurations.size();i<i_end;++i)
    {
        configurations[i]->print(i,LOG());
        LOG()<<endl;
    }
}

int merger::get_index_from_indices(vector<index_type> &indices, ushort node_count)
{
    int exp = 1;
    int index = 0;
    for(uint i=1, i_end = configurations.size();i<i_end;++i)
    {
        index += exp * indices[i];
        exp *= node_count;
    }
    return index;
}

void merger::input_reorder()
{
    int count = start_merging.front().size();
    for(uint i = 1, i_end = start_merging.size(); i<i_end;++i)
    {
        int old_i = i;
        while( i>0 && start_merging[i][0] < start_merging[i-1][0] )
        {
            for(int j=0;j<count;++j)
            {
                int64_t t = start_merging[i][j];
                start_merging[i][j] = start_merging[i-1][j];
                start_merging[i-1][j] = t;
            }
            i--;
        }
        i = old_i;
    }
}

void merger::copy_current_tree()
{
    max_visit=0;
    for(int i=0;i<matree_tree_n.size();i++){
        if( matree_tree_n[i] != NULL){
            if( matree_tree_n[i]->max_visit>max_visit ){
                max_visit = matree_tree_n[i]->max_visit;
            }
        }
    }

    best_merge_tree_n = merge_tree_n;
}

void merger::print_result()
{
    LOG()<<endl<<"########################## RESULT ##################################"<<endl;
    LOG()<<"TOTAL COST:"<<best_cost_all<<endl;

    for(int i=0;i<=stage_count;++i)
    {
        int stage = i;
        LOG()<<endl<<"-------------------stage"<<stage<<"------------------------"<<endl;
        //vector<vector<index_type> >* mat = best_merge_tree_n->get_at(i);
        int cur_conf=0;
        for(vector<mt_graph*>::iterator conf_iter=configurations.begin(), conf_iter_end = configurations.end();
            conf_iter!=conf_iter_end;
            ++conf_iter)
        {
            LOG()<<"|";
            mt_stage* stage_p = (*conf_iter)->stages[stage];
            for(uint r=0, r_end = best_merge_tree_n[i].size();r<r_end;++r)
            {
                int index = best_merge_tree_n[i][r][cur_conf];
                if(!stage_p->nodes[index]->is_ghost) LOG()<<"\t"<<stage_p->nodes[index]->print_values();
                else cout<<"\t-";
            }
            LOG()<<"\t|"<<endl;
            cur_conf++;
        }
    }
}

string merger::trim_string(string txt)
{
    txt = txt.substr( txt.find_first_not_of("\n\r \t") );
    txt = txt.substr( 0,txt.find_last_not_of("\n\r \t") );
    return txt;
}

void merger::prepare_trees()
{
    merge_tree_n.resize(stage_count+1);
    best_merge_tree_n.resize(stage_count+1);
    matree_tree_n.resize(stage_count+1);

    std::fill( matree_tree_n.begin(),matree_tree_n.end(),(matree2*)NULL );
}

void merger::calculate_decision_count()
{
    /*
    vector<double> dec_c( stage_count );
    vector<double> poss_c( stage_count );


    double N = configurations.size();
    for(int s=1;s<stage_count;++s)
    {
        double K_s = stage_node_count[s];

        poss_c[s] = simple_math::pow(simple_math::fak(K_s),(N-1));

        double sum=0;
        for(unsigned int j=1;j<K_s;j++)
        {
            sum += simple_math::pow( (1.0/simple_math::fak((double)j)),N-1 );
        }

        dec_c[s] = (unsigned long)(sum * poss_c[s]);
    }

    double sum_all = 0.0;
    for(int i=1;i<stage_count;++i)
    {
        double prod = 1.0;
        for(int j=1;j<=i-1;++j)
        {
            prod *= poss_c[j];
        }
        sum_all += prod * dec_c[i];
    }
    dec_count = (unsigned long)sum_all;
    */
    long sum_total = 0;
    long stage_multiplier = 1;
    long c = configurations.size();

    for(int s=stage_count-1;s>0;s--)
    {
        long dec_multiplier = 1;
        long sum = 0;
        for(int k=stage_node_count[s];k>0;k--)
        {
            sum +=  simple_math::pow(k,c-1) * dec_multiplier;

            dec_multiplier *= simple_math::pow(k,c-1);
        }
        sum_total += sum * stage_multiplier;
        stage_multiplier *= dec_multiplier;
    }
    dec_count = sum_total;
}

void merger::calculate_possibilities()
{
    long long possibilities=1;
    for(int i=stage_count-1;i>0;--i)
    {
        possibilities *= simple_math::pow( simple_math::fak(stage_node_count[i]),configurations.size()-1 );
    }
    pos_count = possibilities;
}

void merger::print_stats()
{
    if(global_options.stats)
    {
        max_col_size=0;
        for(int i=0;i<matree_tree_n.size();i++){
            if( matree_tree_n[i] != NULL){
                if( matree_tree_n[i]->matrix_col_size > max_col_size ){
                    max_col_size = matree_tree_n[i]->matrix_col_size;
                }
            }
        }

        LOG()<<endl<<"______________________STATISTICS________________________"<<endl;
        LOG()<<"CUT  COUNT:"<<cut_count<<endl;
        LOG()<<"TCUT COUNT:"<<tcut_count<<endl;
        if(global_options.costmodel.strongcost)
            LOG()<<"SCUT COUNT:"<<scut_count<<endl;

        LOG()<<"IGN  COUNT:"<<ign_count<<endl;
        LOG()<<"NEW  COUNT:"<<new_count<<endl;
        LOG()<<"DECISIONS MADE:"<<rec_count<<endl;
        LOG()<<"FULL TREE DECISIONS:"<<dec_count<<endl;
        LOG()<<"MAPPING POSSIBILITIES:"<<pos_count<<endl;
        LOG()<<"MAX NODE INDEX:"<<max_visit <<endl;
		LOG()<<"MAX COL SIZE:"<<max_col_size <<endl;
    }
    LOG()<<"EXECUTION TIME: " << run_time.print_time() << endl;
}

void merger::walk_complete_tree(low_tree *leave, int tree_stage)
{
    if(leave->label[0] == '|')
    {
        int cfg=0;
        string label = leave->label.substr(2,leave->label.size()-3);
        string new_label;
        while(label.size() > 0)
        {
            if(cfg>0) new_label += "\\n";
            size_t pos = label.find("\t");
            int index = -1;
            if( pos != string::npos )
            {
                 index = atoi( label.substr(0,pos).c_str() );
                 label = label.substr(label.find("\t")+1);
            }
            else
            {
                index = atoi( label.c_str() );
                label = "";
            }
            new_label += configurations[cfg]->stages[tree_stage]->nodes[index]->print_values();

            cfg++;
        }
        leave->label = new_label;
    }
    else if( leave->label == "next stage" )
    {
        tree_stage--;
    }

    for(unsigned int i=0, i_end = leave->leaves.size();i<i_end;++i)
    {
        walk_complete_tree(leave->leaves[i],tree_stage);
    }
}

void merger::exit_call()
{
    run_time.stop();
    run_time.time_elapsed *= -1;
    LOG()<<endl<<"########################## TERMINATED ##################################"<<endl;
    if(best_cost_all != INT_MAX) print_result();
    else cout << "No result found yet" << endl;
    print_stats();

    if(best_cost_all != INT_MAX)
    {
        create_muxed_graph();
        if(global_options.printing_options.perfFile)
            write_statistics_file();
    }
    exit(0);
}

void merger::write_statistics_file()
{
    //convert exec_time to ms
    long long exec_time_msec = (run_time.time_elapsed / 1000);
    double exec_time_sec = ((double)exec_time_msec / 1000.0);
	


    fstream stat_file;
    stat_file.open("pag_fusion_profile.csv",ios_base::out|ios_base::app);

    stat_file<< statistics_mem["gshape"] << ";"
             << statistics_mem["#coeff"] << ";"
             << statistics_mem["#confs"] << ";"
             << statistics_mem["#mux"] << ";"
             << statistics_mem["cost_mux"] << ";"
             << statistics_mem["#mux_reg"] << ";"
             << statistics_mem["cost_mux_reg"] << ";"
             << statistics_mem["#add_total"] << ";"
             << statistics_mem["cost_approx"] << ";"
             << statistics_mem["cost_computed"] << ";"
             << statistics_mem["#dec"] << ";"
             << statistics_mem["#rec"] << ";"
             << statistics_mem["#poss"] << ";"
             << exec_time_sec ;

    stat_file.close();
}

void merger::get_pag_info(adder_graph_t *graph)
{
    int no_of_inputs=0,no_of_cfgs=0,no_of_mux=0,no_of_add=0,no_of_cfgadd=0,no_of_reg=0,no_of_mux_reg=0,real_cost=0,mux_cost=0,max_ws=0,cost_mux_reg=0;

    map<int,int> reg_count;
    map<int,int> reg_cost;
    set<int> mux_stages;

    no_of_cfgs = graph->nodes_list.front()->output_factor.size();
    no_of_inputs = graph->nodes_list.front()->output_factor[0].size();

    for(list<adder_graph_base_node_t*>::iterator op = graph->nodes_list.begin();op!=graph->nodes_list.end();++op)
    {
        int current_ws = computeWordSize(*op,0);
        if(current_ws>max_ws) max_ws = current_ws;
        if (is_a<adder_subtractor_node_t>(*(*op)))
        {
           no_of_add++;
           adder_subtractor_node_t* t = (adder_subtractor_node_t*)(*op);
           bool has_neg=false;
           for(int i=0;i<(int)t->input_is_negative.size();i++)
           {
               if(t->input_is_negative[i]) has_neg = true;
           }
           if(has_neg) real_cost += COST_SUB * current_ws;
           else real_cost += COST_ADD * current_ws;
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*op)))
        {
           no_of_cfgadd++;
           //conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)(*op);
           real_cost += COST_ADDSUB * current_ws;
        }
        else if (is_a<mux_node_t>(*(*op)))
        {
           no_of_mux++;
           mux_node_t* t = (mux_node_t*)(*op);
           mux_stages.insert(t->stage);
           pair<adder_graph_base_node_t*,int> node_shift_pair;
           set<pair<adder_graph_base_node_t*,int> > base_mux;
           for(int i =0;i<(int)t->inputs.size();i++)
           {
               node_shift_pair.first = t->inputs[i];
               node_shift_pair.second = t->input_shifts[i];
               base_mux.insert(node_shift_pair);
           }
           int mux_count = base_mux.size();
           real_cost += COST_MUX * current_ws * (mux_count-1);
           mux_cost += COST_MUX * current_ws * (mux_count-1);
        }
        else if (is_a<register_node_t>(*(*op)))
        {
           no_of_reg++;
           register_node_t* t = (register_node_t*)(*op);
           real_cost += COST_REG * current_ws;

           if( reg_count.find(t->stage) != reg_count.end() )
           {
               reg_count[t->stage]++;
               reg_cost[t->stage] += COST_REG * current_ws;
           }
           else
           {
               reg_count[t->stage] = 1;
                reg_cost[t->stage] = COST_REG * current_ws;
           }
        }
    }


    for(set<int>::iterator item = mux_stages.begin();item!=mux_stages.end();++item)
    {
        no_of_mux_reg += reg_count[*item];
        cost_mux_reg += reg_cost[*item];
    }

    add_to_stats("#confs",no_of_cfgs );
    add_to_stats("#inputs",no_of_inputs);
    add_to_stats("#coeff",stage_node_count.back() );
    add_to_stats("#mux",no_of_mux);
    add_to_stats("#mux_reg",no_of_mux_reg );
    add_to_stats("#add",no_of_add);
    add_to_stats("#cfgadd",no_of_cfgadd);
    add_to_stats("#add_total",(no_of_add+no_of_cfgadd));
    add_to_stats("#reg",no_of_reg);
    add_to_stats("cost_computed",real_cost);
    add_to_stats("cost_approx",best_cost_all);
    add_to_stats("cost_mux",mux_cost);
    add_to_stats("cost_mux_reg",cost_mux_reg);
    add_to_stats("#poss",pos_count);
    add_to_stats("#dec",dec_count);
    add_to_stats("#rec",rec_count);
    add_to_stats("#tcut",tcut_count);
    add_to_stats("#scut",scut_count);
    add_to_stats("#cut",cut_count);
    add_to_stats("#ign",ign_count);
    add_to_stats("#new",new_count);
    add_to_stats("max_ws",max_ws);

    stringstream shape;
    shape << "[";
    for(uint i =0, i_end = stage_node_count.size();i< i_end;++i)
    {
        if(i>0) shape << ",";

        shape << stage_node_count[i];
    }
    shape << "]";
    add_to_stats("gshape",shape.str());
    add_to_stats("#stages",stage_count-1);

    string cost_model;
    if(global_options.costmodel.strongcost) cost_model += "s";
    if(global_options.costmodel.ignorezero) cost_model += "z";
    if(global_options.costmodel.lowcost) cost_model += "l";
    add_to_stats("costmodel",cost_model);
}

bool merger::check_limits()
{
    float sum=0;
    for( int i=1;i<(int)stage_node_count.size()-1;i++ )
    {
        double pp = pow((double)stage_node_count[i],(double)configurations.size());
        sum += pp * 8.0;
    }

    return (sum<global_options.mem_limit);
}

mt_node *merger::find_node(const int &cfgid,const int &stageid, const vector<int64_t>& outputs){
    mt_stage* stage = configurations.at(cfgid)->stages.at(stageid);
    for( std::vector<mt_node*>::iterator it = stage->nodes.begin();it!=stage->nodes.end();++it ){
        if( (*it)->values == outputs ){
            return (*it);
        }
    }
    return NULL;
}
