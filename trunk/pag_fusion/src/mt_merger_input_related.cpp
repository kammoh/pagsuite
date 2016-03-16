#include "mt_merger.h"
#include "mt_types.h"
#include "parsers/inputparser.h"

merger::merger(string file_name)
{
    init();

    InputParser inp(file_name);
    if(inp.parse()){
        for(int i=0;i<inp.graphs.size();++i){
            add_graph( inp.graphs[i] );
        }

        vector<index_type> inner_start_merging( configurations.size() );
        start_merging.resize( configurations.front()->stages[stage_count]->nodes.size() );
        std::fill(start_merging.begin(),start_merging.end(),inner_start_merging);
        start_merge( inp.start_merging );

        // Do something with fixed nodes

        input_reorder();
        stage_node_count.clear();
        stage_node_count.resize(stage_count+1);
        stage_node_count_ff.resize(stage_count+1,0);
        start_node_count_check();

        fix_nodes(inp.fixed_nodes);
        move_fixed_nodes();
    }
    else
    {
        throw runtime_error("no input file found");
    }
}

void merger::fix_nodes(const vector< InputParser::fix_node >& fnodes){
    for( vector< InputParser::fix_node >::const_iterator it_f = fnodes.begin();it_f!=fnodes.end();++it_f ){
        int cfg=0,node_id=-1;
        for( vector<vector<int64_t> >::const_iterator it_cfg = it_f->outputs.begin();it_cfg!=it_f->outputs.end();++it_cfg,++cfg ){
            if( !it_cfg->empty() ){
                mt_node* node = find_node(cfg,it_f->stage,*it_cfg );
                if( node!=NULL ){
                    node->is_fixed = true;
                    if(node_id==-1){
                        node_id = node->id;
                    }else if(node_id != node->id){
                        std::swap( configurations[cfg]->stages[it_f->stage]->nodes[node_id]->id_merged, configurations[cfg]->stages[it_f->stage]->nodes[node->id]->id_merged );
                        std::swap( configurations[cfg]->stages[it_f->stage]->nodes[node_id], configurations[cfg]->stages[it_f->stage]->nodes[node->id] );
                        std::swap( configurations[cfg]->stages[it_f->stage]->nodes[node_id]->id, configurations[cfg]->stages[it_f->stage]->nodes[node->id]->id );

                        for(int i=cfg-1;i>=0;--i){
                            configurations[cfg]->stages[it_f->stage]->nodes[node_id]->fixed_to.push_back( configurations[i]->stages[it_f->stage]->nodes[node_id] );
                            configurations[i]->stages[it_f->stage]->nodes[node_id]->fixed_to.push_back( configurations[cfg]->stages[it_f->stage]->nodes[node_id] );
                        }
                    }
                }
            }
        }
    }
}

void merger::move_fixed_nodes(){
    for( int stage = 0; stage < stage_count ; ++stage){
        for( int node_id=0;node_id<(stage_node_count[stage]-stage_node_count_ff[stage]);++node_id ){
            bool all_fixed = true;
            for(int cfg=0;cfg<configurations.size();++cfg){
                mt_node* n = configurations[cfg]->stages[stage]->nodes[node_id];
                if(!n->is_fixed){
                    all_fixed=false;
                    break;
                }
            }
            if(all_fixed){
                for(int cfg=0;cfg<configurations.size();++cfg){
                    mt_stage* st = configurations[cfg]->stages[stage];
                    st->nodes[node_id]->is_fully_fixed = true;
                    int i=st->nodes.size()-1;
                    while( i>node_id && st->nodes[i]->is_fully_fixed ){
                        --i;
                    }
                    if(i!=node_id){
                        std::swap( st->nodes[node_id],st->nodes[i] );
                        std::swap( st->nodes[node_id]->id,st->nodes[i]->id );
                        std::swap( st->nodes[node_id]->id_merged,st->nodes[i]->id_merged );
                    }
                }
                //--stage_node_count[node_id];
                ++stage_node_count_ff[stage];
            }
        }
    }
}

void merger::add_graph(string graph_string)
{
    adder_graph_t* pipelined_adder_graph = new adder_graph_t();
    if( !pipelined_adder_graph->parse_to_graph(graph_string) )
    {
        throw runtime_error("PARSE: failed");
    }

    pipelined_adder_graph->check_and_correct(graph_string);
    convert_graph(pipelined_adder_graph);
    delete pipelined_adder_graph;
}

void merger::convert_graph(adder_graph_t *graph)
{
    mt_graph* new_graph = new mt_graph();

    if( inp_count == 0) inp_count = graph->nodes_list.front()->output_factor[0].size();


    //Zähle Knoten auf den Stufen
    map<int,int> new_stage_node_count;

    for(list<adder_graph_base_node_t*>::iterator it_node= graph->nodes_list.begin() , it_node_end = graph->nodes_list.end();
        it_node != it_node_end;
        ++it_node)
    {
        adder_graph_base_node_t* t= *it_node;
        pair<map<int,int>::iterator,bool> pr = new_stage_node_count.insert(make_pair(t->stage,1));
        if( !pr.second )
        {
            ((*pr.first).second)++;
        }
    }
    //Erstelle Speicher für die Knoten- und Stufenanzahl
    if(configurations.size() == 0)
    {
        stage_node_count.resize(new_stage_node_count.size());
        stage_count = new_stage_node_count.size()-1;
    }

    //Erstelle interne Stufen
    for(uint i=0, i_end = new_stage_node_count.size();i<i_end;++i)
    {       
        if( stage_node_count[i] < (*new_stage_node_count.find(i)).second )
            stage_node_count[i] = (*new_stage_node_count.find(i)).second;

        mt_stage* new_stage = new mt_stage(i);
        new_graph->add(new_stage);
    }

    //Erstelle interne Knotenliste und füge sie internen Stufen zu; neue Knoten werden mit alten verknüpft
    map<adder_graph_base_node_t*,mt_node*> node_cor;
    for(list<adder_graph_base_node_t*>::iterator it_node= graph->nodes_list.begin(), it_node_end = graph->nodes_list.end();
        it_node != it_node_end;
        ++it_node)
    {
        adder_graph_base_node_t* t= *it_node;
        mt_node* new_node = new mt_node( new_graph->stages[t->stage]->nodes.size() );
        new_node->cfg_id = configurations.size();
        new_graph->stages[t->stage]->nodes.push_back(new_node);
        if( is_a<adder_subtractor_node_t>(*t) ){
            for(int i=0;i<(int)((adder_subtractor_node_t*)t)->input_shifts.size();i++){
                if( ((adder_subtractor_node_t*)t)->input_shifts[i]<0 && ((adder_subtractor_node_t*)t)->input_shifts[i]<new_node->neg_shift ){
                    new_node->neg_shift = ((adder_subtractor_node_t*)t)->input_shifts[i];
                }
            }

        }

        new_node->values.insert(new_node->values.begin(),t->output_factor.front().begin(),t->output_factor.front().end());
        new_node->wordsize = computeWordSize(t,0);
        //node_cor.insert(make_pair<adder_graph_base_node_t*,mt_node*>(t,new_node));
        node_cor.insert(make_pair(t,new_node));
    }

    resort_inputs( new_graph->stages[0] );

    //Ordne Knoten
    for(int stage_id=1;stage_id < (int)new_graph->stages.size()-1;stage_id++ ){
        map<int,mt_node*> new_sorting;

        for( vector<mt_node*>::iterator node_iter = new_graph->stages[stage_id]->nodes.begin();
                 node_iter != new_graph->stages[stage_id]->nodes.end();
                 ++node_iter){
            mt_node* node_it = *node_iter;
            int new_id=-1;
            for(int cfg=0;cfg< (int)configurations.size();cfg++ ){
                int new_id_count=0;
                for(int node_id=0;node_id < (int) configurations[cfg]->stages[stage_id]->nodes.size();node_id++){
                    mt_node* node_it2 = configurations[cfg]->stages[stage_id]->nodes[node_id];
                    if( node_it2!=NULL && node_it!=NULL &&  node_it2->values == node_it->values ){
                        new_id = node_id;
                        // TODO switch here
                        if( global_options.fix_by_value ){
                            node_it->is_fixed = true;
                            node_it2->is_fixed = true;
                        }
                        new_sorting.insert( make_pair(new_id,node_it) );
                        break;
                    }
                    new_id_count++;
                }
                if( new_id != -1 ) break;
            }
        }

        vector<mt_node*> new_node_vector( stage_node_count[stage_id],NULL );
        for( map<int,mt_node*>::iterator item = new_sorting.begin();item!=new_sorting.end();item++ ){
            new_node_vector[ (*item).first ] = (*item).second;
            (*item).second->id = (*item).first;
            (*item).second->id_merged = (*item).first;
        }
        for( vector<mt_node*>::iterator node_it = new_graph->stages[stage_id]->nodes.begin();
             node_it != new_graph->stages[stage_id]->nodes.end();
             ++node_it){
            if( std::find( new_node_vector.begin(),new_node_vector.end(),(*node_it) ) == new_node_vector.end() ){
                int cur_id=0;
                for( vector<mt_node*>::iterator new_place = new_node_vector.begin();new_place != new_node_vector.end();++new_place ){
                    if( (*new_place) == NULL ){
                        (*new_place) = (*node_it);
                        (*node_it)->id = cur_id;
                        (*node_it)->id_merged = cur_id;
                        break;
                    }
                    cur_id++;
                }
            }
        }
        new_graph->stages[stage_id]->nodes = new_node_vector;
    }

    //Erstelle Pfade zwischen den Stufen
    for(list<adder_graph_base_node_t*>::iterator it_node = graph->nodes_list.begin(), it_node_end= graph->nodes_list.end();
        it_node != it_node_end;
        ++it_node)
    {
        adder_graph_base_node_t* t= *it_node;
        index_type node_to_index = (*node_cor.find(t)).second->id;
        if( is_a<adder_subtractor_node_t>(*t) )
        {
            adder_subtractor_node_t* tt = (adder_subtractor_node_t*)t;
            if(tt->inputs.size() == 3)
            {
                (*node_cor.find(tt)).second->is_ternary = true;
                global_options.ternary_mode = true;
            }
            for(uint i=0, i_end = tt->inputs.size();i<i_end;++i)
            {
                if(tt->inputs[i]!=NULL)
                {
                    mt_path* new_path = new mt_path(node_to_index,tt->input_shifts.at(i),false);
                    //new_path->to_merged = new_path->to;
                    new_path->parent = (*node_cor.find(tt->inputs.at(i))).second;
                    new_path->parent->paths_down.push_back(new_path);
                    new_path->target = (*node_cor.find(tt)).second;
                    new_path->target->paths_up.push_back(new_path);

                    if(tt->input_is_negative.at(i)) new_path->is_neg = true;
                    else new_path->is_neg = false;
                }
            }
        }
        else if( is_a<register_node_t>(*t) )
        {
            register_node_t* tt = (register_node_t*)t;
            mt_path* new_path = new mt_path(node_to_index,tt->input_shift,true);
            //new_path->to_merged = new_path->to;
            new_path->parent = (*node_cor.find(tt->input)).second;
            new_path->parent->paths_down.push_back(new_path);
            new_path->target = (*node_cor.find(tt)).second;
            new_path->target->paths_up.push_back(new_path);
            new_path->is_neg = false;
        }
        else if( is_a<input_node_t>(*t) )
        {
            //NICHTS TUN - AUCH MAL GUT
        }
        else
        {
            throw runtime_error("INPUT CONVERSION: NO VALID NODE TYPE");
        }
    }

    configurations.push_back(new_graph);
}

void merger::start_merge(vector<vector<vector<int64_t> > >& start_nodes)
{
    vector<bool> used( start_nodes.size()*start_nodes.front().size() );

    for(unsigned int m_node_id=0, m_node_id_end = start_nodes.size();m_node_id< m_node_id_end;++m_node_id)
    {
        mt_stage* t_stage = configurations[0]->stages[stage_count];
        mt_node* t_node = t_stage->nodes[m_node_id];
        start_merging[m_node_id][0] = m_node_id;
        int used_node_index = -1;
        for(unsigned int i=0, i_end = start_nodes.size();i<i_end;++i)
        {
            if( t_node->values == start_nodes[i][0] )
            {
                used_node_index = i;
            }
        }

        for(unsigned int cfg=1, cfg_end = start_nodes[used_node_index].size();cfg<cfg_end;++cfg)
        {
            mt_stage* tt_stage = configurations[cfg]->stages[stage_count];
            for(unsigned int node_id = 0, node_id_end = t_stage->nodes.size();node_id<node_id_end;++node_id)
            {
                if( tt_stage->nodes[node_id]->values == start_nodes[used_node_index][cfg] && !used[m_node_id+cfg*start_nodes.size()])
                {
                    start_merging[m_node_id][cfg] = tt_stage->nodes[node_id]->id;
                    used[m_node_id+cfg*start_nodes.size()] = true;
                }
            }
        }
    }
}

void merger::start_normalize(vector<vector<vector<int64_t> > > &start_nodes)
{
    for(unsigned int ni=0, ni_end = start_nodes.size();ni<ni_end;++ni)
    {
        for(unsigned int ci=0, ci_end = start_nodes[ni].size();ci<ci_end;++ci)
        {
            bool normalize=false;
            do{
                if(normalize)
                {
                    for(unsigned int j=0, j_end = start_nodes[ni][ci].size();j<j_end;++j)
                    {
                        start_nodes[ni][ci][j] /= 2;
                    }
                }
                normalize = true;
                for(unsigned int j=0, j_end = start_nodes[ni][ci].size();j<j_end;++j)
                {
                    if( start_nodes[ni][ci][j]%2==1 )
                    {
                        normalize = false;
                        break;
                    }
                }
            }while(normalize);
        }
    }
}

void merger::start_node_count_check()
{
    has_ghosts = true;
    for(uint stage_id=0;stage_id<=stage_count;++stage_id)
    {
        uint max=0;
        for(uint conf_id=0, conf_id_end = configurations.size();conf_id<conf_id_end;++conf_id)
        {
            if( max < configurations[conf_id]->stages[stage_id]->nodes.size() )
            {
                max = configurations[conf_id]->stages[stage_id]->nodes.size();
            }
        }
        stage_node_count[stage_id] = max;
        for(uint conf_id=0, conf_id_end = configurations.size();conf_id<conf_id_end;++conf_id)
        {
            int cur_node_id=0;
            for( vector<mt_node*>::iterator item = configurations[conf_id]->stages[stage_id]->nodes.begin();
                 item != configurations[conf_id]->stages[stage_id]->nodes.end();
                 ++item
                 ){
                if( (*item) == NULL ){
                    has_ghosts = true;
                    mt_node* node = new mt_node(cur_node_id);
                    node->id_merged=node->id;
                    node->cfg_id = conf_id;
                    node->wordsize=0;
                    node->values.resize(inp_count,DONT_CARE);
                    node->is_ghost = true;
                    map<int,vector<mt_node*> >::iterator ghost_vec;
                    if( ( ghost_vec =  ghost_nodes_map.find(stage_id) ) != ghost_nodes_map.end())
                    {
                        (*ghost_vec).second.push_back(node);
                    }
                    else
                    {
                        vector<mt_node*> new_ghost_vec;
                        new_ghost_vec.push_back(node);
                        //ghost_nodes_map.insert(make_pair<int,vector<mt_node*> >(stage_id,new_ghost_vec) );
                        ghost_nodes_map.insert(make_pair(stage_id,new_ghost_vec) );
                    }
                    configurations[conf_id]->stages[stage_id]->nodes[cur_node_id] = node;
                }
                cur_node_id++;
            }

            while( max > configurations[conf_id]->stages[stage_id]->nodes.size() )
            {
                has_ghosts = true;
                mt_node* node = new mt_node(configurations[conf_id]->stages[stage_id]->nodes.size());
                node->id_merged=node->id;
                node->cfg_id = conf_id;
                node->wordsize=0;
                node->values.resize(inp_count,DONT_CARE);
                node->is_ghost = true;
                map<int,vector<mt_node*> >::iterator ghost_vec;
                if( ( ghost_vec =  ghost_nodes_map.find(stage_id) ) != ghost_nodes_map.end())
                {
                    (*ghost_vec).second.push_back(node);
                }
                else
                {
                    vector<mt_node*> new_ghost_vec;
                    new_ghost_vec.push_back(node);
                    //ghost_nodes_map.insert(make_pair<int,vector<mt_node*> >(stage_id,new_ghost_vec) );
                    ghost_nodes_map.insert(make_pair(stage_id,new_ghost_vec) );
                }
                configurations[conf_id]->stages[stage_id]->add(node);
            }
        }
    }
}

void merger::resort_inputs( mt_stage* stage )
{
    map<index_type,mt_node*> new_order;

    for(index_type j =0, j_end = stage->nodes.size();j < j_end;++j)
    {
        mt_node* node = stage->nodes[j];
        int needed_index = -1;

        for(index_type f=0, f_end = node->values.size();f<f_end;++f)
        {
            if( node->values[f]==1 )
            {
                needed_index = f;
                break;
            }
        }
        if(needed_index==-1) throw runtime_error("RESORT_INPUTS: index not found");
        node->id = needed_index;
        node->id_merged = needed_index;
        //new_order.insert(make_pair<index_type,mt_node*>(needed_index,node));
        new_order.insert(make_pair(needed_index,node));
    }
    stage->nodes.clear();
    for(map<index_type,mt_node*>::iterator iter = new_order.begin(), iter_end = new_order.end();iter!=iter_end;++iter)
    {
        stage->nodes.push_back( (*iter).second );
    }
}

