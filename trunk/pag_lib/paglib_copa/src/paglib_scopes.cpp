#include "paglib_scopes.h"
#include <functional>
#include <algorithm>

void outfact_single_scope::onExit(){
    node_core_scope* par = getParentScope<node_core_scope>();
    if( hasValue(0) ){
        par->inner_factors.push_back( getValueSingle(0).toLLong() );
    }
    else{
        par->inner_factors.push_back( DONT_CARE );
    }
}


void shift_single_scope::onExit(){
    node_scope* par = getParentScope<node_scope>();
    if( hasValue(1) ){
        par->parts.back().shifts.push_back( getValueSingle(1).toInt() );
    }
    else{
        par->parts.back().shifts.push_back( DONT_CARE );
    }
}

void node_core_scope::onEntry(){
    part.factors.clear();
    part.shifts.clear();
    part.stage = -1;
}

void node_core_scope::onExit(){
    part.stage = getValueSingle(2).toInt();
    part.factors.push_back(inner_factors);
    inner_factors.clear();

    node_scope* par = getParentScope<node_scope>();

    int64_t sum=0;
    for(int i=0;i<(int)part.factors.size();i++){
        for( int j=0;j<(int)part.factors[i].size();j++ ){
            sum+=abs(part.factors[i][j]);
        }
    }
    if( !(part.stage==0 && sum == 0))
        par->parts.push_back(part);
}

void node_core_scope::onEvent(int id){
    if( id == 0 ){
        part.factors.push_back(inner_factors);
        inner_factors.clear();
    }
}


void node_scope::onEntry(){
    parts.clear();
    parameters.clear();
    current_type = 0;
}

void node_scope::buildAdder(){
    adder_tree_scope* tree = getParentScope<adder_tree_scope>();

    if(tree->no_of_input_nodes==-1 && tree->no_of_confs==-1)
    {
        tree->no_of_confs = parts[0].factors.size();
        tree->no_of_input_nodes = parts[0].factors[0].size();
    }

    adder_subtractor_node_t* new_node = new adder_subtractor_node_t();
    new_node->specific_parameters = parameters;

    for( unsigned int inp=0;inp< parts.size();inp++)
    {
        if(inp!=0)
        {
            new_node->input_shifts.push_back( parts[inp].shifts[0] );
        }

        tree->output_and_stage_pair.first = parts[inp].factors;
        tree->output_and_stage_pair.second = parts[inp].stage;
        if(inp==0)
        {
            tree->realized_nodes_map.insert(make_pair(tree->output_and_stage_pair,new_node));
        }
        else
        {
            tree->node_inputs.push_back(tree->output_and_stage_pair);
        }
    }
    tree->node_inputs_map[new_node] = tree->node_inputs;

    new_node->output_factor = parts[0].factors;
    new_node->stage = parts[0].stage;
    tree->graph->nodes_list.push_back(new_node);
    tree->node_inputs.clear();
}

void node_scope::buildMux(){
    adder_tree_scope* tree=getParentScope<adder_tree_scope>();

    if(tree->no_of_input_nodes==-1 && tree->no_of_confs==-1)
    {
        tree->no_of_confs = parts[0].factors.size();
        tree->no_of_input_nodes = parts[0].factors[0].size();
    }
    int no_of_confs = tree->no_of_confs;

    mux_node_t* new_node = new mux_node_t();
    new_node->specific_parameters = parameters;

    map<int,int> input_cfg_map;   // first is configuration; second is id in current_nodes

    vector<bool> output_reset;
    for( int cfg=0;cfg < no_of_confs;cfg++)
    {
        bool is_rst=true;
        for( unsigned int inp=1;inp < parts.size();inp++)
        {
            if( parts[inp].shifts[cfg] != DONT_CARE )
                is_rst = false;
        }
        output_reset.push_back(is_rst);
    }

    for( int cfg=0;cfg < no_of_confs;cfg++)
    {
        if( output_reset[cfg] )
        {
            input_cfg_map[cfg] = -1;
            continue;
        }
        for( unsigned int inp=1;inp< parts.size();inp++)
        {
            if( parts[inp].shifts[cfg] != DONT_CARE )
            {
                input_cfg_map[cfg] = inp;
                break;
            }
        }
    }

    for( int cfg=0;cfg < no_of_confs;cfg++ )
    {
        if(input_cfg_map[cfg] == -1)
        {
            tree->output_and_stage_pair.first = parts[0].factors;
            tree->output_and_stage_pair.second = -1;
            tree->node_inputs.push_back(tree->output_and_stage_pair);
            new_node->input_shifts.push_back(0);
        }
        else
        {
            tree->output_and_stage_pair.first = parts[input_cfg_map[cfg]].factors;
            tree->output_and_stage_pair.second = parts[input_cfg_map[cfg]].stage;
            tree->node_inputs.push_back(tree->output_and_stage_pair);
            new_node->input_shifts.push_back( parts[input_cfg_map[cfg]].shifts[cfg] );
        }
    }
    tree->node_inputs_map[new_node] = tree->node_inputs;

    tree->output_and_stage_pair.first = parts[0].factors;
    tree->output_and_stage_pair.second = parts[0].stage;
    tree->realized_nodes_map.insert(make_pair(tree->output_and_stage_pair,new_node));

    new_node->output_factor = parts[0].factors;
    new_node->stage = parts[0].stage;

    tree->graph->nodes_list.push_back(new_node);
    tree->node_inputs.clear();
}

void node_scope::buildReg(){
    adder_tree_scope* tree=getParentScope<adder_tree_scope>();

    if(tree->no_of_input_nodes==-1 && tree->no_of_confs==-1)
    {
        tree->no_of_confs = parts[0].factors.size();
        tree->no_of_input_nodes = parts[0].factors[0].size();
    }

    register_node_t* new_node = new register_node_t();
    new_node->specific_parameters = parameters;

    for( unsigned int inp=0;inp< parts.size();inp++)
    {
        tree->output_and_stage_pair.first = parts[inp].factors;
        tree->output_and_stage_pair.second = parts[inp].stage;
        if(inp==0)
        {
            tree->realized_nodes_map.insert(make_pair(tree->output_and_stage_pair,new_node));
        }
        else
        {
            tree->node_inputs.push_back(tree->output_and_stage_pair);
        }
    }
    tree->node_inputs_map[new_node] = tree->node_inputs;

    new_node->output_factor = parts[0].factors;
    new_node->stage = parts[0].stage;
    if ( parts[1].shifts.size()>0 )
        new_node->input_shift = parts[1].shifts[0];
    else
        new_node->input_shift = 0;

    tree->graph->nodes_list.push_back(new_node);
    tree->node_inputs.clear();
}

void node_scope::onExit(){
    if( current_type == 'A' && parts.size()==2 ){
        current_type = 'R';
    }

    switch(current_type){
    case 'A':
        buildAdder();break;
    case 'R':
        buildReg();break;
    case 'M':
        buildMux();break;
    default:
        throw runtime_error("Invalid node type");
    }
    parameters.clear();
}

void node_scope::onEvent(int id){
    if( id == 1 ){
        current_type = 'A';
    }
    else if(id == 2){
        current_type = 'R';
    }
    else if(id == 3){
        current_type = 'M';
    }else if(id == 10){
        std::vector< CoPa_Value > vals = getValueGroup(5);
        if(vals.size()==2){
            parameters.insert( make_pair<std::string,std::string>(vals[0].toString(),vals[1].toString()) );
        }
    }
}


void adder_tree_scope::onEntry(){
    no_of_confs = -1;
    no_of_input_nodes = -1;
    graph = new adder_graph_t;
}

void adder_tree_scope::onEvent(int id){
    if(id == 10){
        std::vector< CoPa_Value > vals = getValueGroup(5);
        if(vals.size()==2){
            parameters.insert( make_pair<std::string,std::string>(vals[0].toString(),vals[1].toString()) );
        }
    }
}

void adder_tree_scope::onExit(){
    vector<int64_t> inner_of_input;

    for( int inp=0;inp < no_of_input_nodes;inp++ )
    {
        input_node_t* new_node = new input_node_t();
        for( int cfg=0;cfg < no_of_confs;cfg++)
        {
            for( int i=0;i < no_of_input_nodes;i++ )
            {
                if( i==inp ) inner_of_input.push_back(1);
                else inner_of_input.push_back(0);
            }
            new_node->output_factor.push_back(inner_of_input);
            inner_of_input.clear();
        }
        output_and_stage_pair.first = new_node->output_factor;
        output_and_stage_pair.second = new_node->stage;
        realized_nodes_map.insert(make_pair(output_and_stage_pair,new_node));
        graph->nodes_list.insert(graph->nodes_list.begin(),new_node);
    }

    map<adder_graph_base_node_t*,adder_graph_base_node_t*> replace_map;

    for(list<adder_graph_base_node_t*>::iterator it = graph->nodes_list.begin(); it != graph->nodes_list.end(); it++)
    {
        adder_graph_base_node_t* current_node = *it;
        vector<pair<adder_graph_base_node_t*,vector<bool> > > input_nodes;

        getInputs( current_node,input_nodes );

        if (is_a<adder_subtractor_node_t>(*current_node))
        {
            for(vector<pair<adder_graph_base_node_t*,vector<bool> > >::iterator input=input_nodes.begin();input!=input_nodes.end();++input ){
                    if( input->second.size() > 1){
                        conf_adder_subtractor_node_t *new_node = new conf_adder_subtractor_node_t;
                        new_node->output_factor = ((adder_subtractor_node_t*)(current_node))->output_factor;
                        new_node->stage = ((adder_subtractor_node_t*)(current_node))->stage;
                        new_node->input_shifts = ((adder_subtractor_node_t*)(current_node))->input_shifts;
                        new_node->input_is_negative.resize(no_of_confs,vector<bool>());
                        replace_map.insert( make_pair<adder_graph_base_node_t*,adder_graph_base_node_t*>(current_node,new_node) );
                        current_node = new_node;
                        break;
                    }
            }
        }

        for(vector<pair<adder_graph_base_node_t*,vector<bool> > >::iterator input=input_nodes.begin();input!=input_nodes.end();++input ){
            if (is_a<adder_subtractor_node_t>(*current_node))
            {
                ((adder_subtractor_node_t*)(current_node))->inputs.push_back(input->first);
                ((adder_subtractor_node_t*)(current_node))->input_is_negative.push_back(input->second[0]);
            }
            else if (is_a<conf_adder_subtractor_node_t>(*(current_node)))
            {
                ((conf_adder_subtractor_node_t*)(current_node))->inputs.push_back( input->first );
                if(input->second.size() == 1)
                    input->second.resize(no_of_confs,input->second[0]);

                for(int j=0;j<no_of_confs;++j){
                    ((conf_adder_subtractor_node_t*)(current_node))->input_is_negative[j].push_back( input->second[j] );
                }
            }
            else if (is_a<mux_node_t>(*(current_node)))
            {
                ((mux_node_t*)(current_node))->inputs.push_back( input->first );
            }
            else if (is_a<register_node_t>(*(current_node)))
            {
                ((register_node_t*)(current_node))->input = input->first;
            }
        }
    }

    for(map<adder_graph_base_node_t*,adder_graph_base_node_t*>::iterator replace_it=replace_map.begin();replace_it!=replace_map.end();++replace_it  )
        graph->nodes_list.push_back( replace_it->second );

    for(map<adder_graph_base_node_t*,adder_graph_base_node_t*>::iterator replace_it=replace_map.begin();replace_it!=replace_map.end();++replace_it  ){

        //cerr << ">>replacing " << replace_it->first << " with " << replace_it->second << endl;

        for(list<adder_graph_base_node_t*>::iterator it = graph->nodes_list.begin(); it != graph->nodes_list.end(); it++)
        {
            if (is_a<adder_subtractor_node_t>(**it))
            {
                for(int i=0;i<(int)((adder_subtractor_node_t*)(*it))->inputs.size() ;++i ){
                    if( ((adder_subtractor_node_t*)(*it))->inputs[i] == replace_it->first ){
                        //cerr << "found input at " << *it << "[" << i << "]. now replacing..." << ((adder_subtractor_node_t*)(*it))->inputs[i] << endl;
                        ((adder_subtractor_node_t*)(*it))->inputs[i] = replace_it->second;
                        //cerr << "set to " << ((adder_subtractor_node_t*)(*it))->inputs[i] << endl;
                    }
                }
            }
            else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
            {
                for(int i=0;i<(int)((conf_adder_subtractor_node_t*)(*it))->inputs.size() ;++i ){
                    if( ((conf_adder_subtractor_node_t*)(*it))->inputs[i] == replace_it->first ){
                        //cerr << "found input at " << *it << "[" << i << "]. now replacing..." << ((conf_adder_subtractor_node_t*)(*it))->inputs[i] << endl;
                        ((conf_adder_subtractor_node_t*)(*it))->inputs[i] = replace_it->second;
                        //cerr << "set to " << ((conf_adder_subtractor_node_t*)(*it))->inputs[i] << endl;
                    }
                }
            }
            else if (is_a<mux_node_t>(*(*it)))
            {
                for(int i=0;i<(int)((mux_node_t*)(*it))->inputs.size() ;++i ){
                    if( ((mux_node_t*)(*it))->inputs[i] == replace_it->first ){
                        //cerr << "found input at " << *it << "[" << i << "]. now replacing..." << ((mux_node_t*)(*it))->inputs[i] << endl;
                        ((mux_node_t*)(*it))->inputs[i] = replace_it->second;
                        //cerr << "set to " << ((mux_node_t*)(*it))->inputs[i] << endl;
                    }
                }
            }
            else if (is_a<register_node_t>(*(*it)))
            {
                if( ((register_node_t*)(*it))->input == replace_it->first ){
                    //cerr << "found input at " << *it << "[" << 0 << "]. now replacing..." << ((register_node_t*)(*it))->input << endl;
                    ((register_node_t*)(*it))->input = replace_it->second;
                    //cerr << "set to " << ((register_node_t*)(*it))->input << endl;
                }
            }
        }
        list<adder_graph_base_node_t*>::iterator it = std::find(graph->nodes_list.begin(),graph->nodes_list.end(), replace_it->first);
        if( it != graph->nodes_list.end() ){
            delete (*it);
            graph->nodes_list.erase(it);
        }
    }
}

void adder_tree_scope::getInputs(adder_graph_base_node_t* node, vector<pair<adder_graph_base_node_t*,vector<bool> > > &input_nodes){
    map<adder_graph_base_node_t*,vector<pair<vector<vector<int64_t> >,int > > >::iterator node_inputs_it;
    node_inputs_it = node_inputs_map.find(node);
    if( node_inputs_it == node_inputs_map.end() )
        return;

    vector<pair<vector<vector<int64_t> >,int > > node_inputs = node_inputs_it->second;
    for( vector<pair<vector<vector<int64_t> >,int > >::iterator node_input_it=node_inputs.begin();
         node_input_it!=node_inputs.end();++node_input_it ){


        struct current_find_struc{
            adder_graph_base_node_t* node;
            vector<bool> signs;
            short has_signs;
        } current_find,saved_find;

        saved_find.node = NULL;
        saved_find.has_signs = 0;
        saved_find.signs.clear();

        for( multimap<pair<vector<vector<int64_t> >,int >,adder_graph_base_node_t* >::iterator realized_node = realized_nodes_map.begin();
                realized_node!=realized_nodes_map.end(); ++ realized_node){

            int cur_stage = realized_node->first.second;

            if ( cur_stage == node_input_it->second && realized_node->second != node ){
                current_find.signs.clear();
                current_find.node = NULL;
                current_find.has_signs = 0;
                for(int i=0;i<(int)node_input_it->first.size();i++){
                    int res = vecEqual( node_input_it->first[i],realized_node->first.first[i] );
                    if( res == 1 ){
                        current_find.signs.push_back(false);
                        current_find.has_signs |= 0x1;
                    }
                    else if(res == -1){
                        current_find.signs.push_back(true);
                        current_find.has_signs |= 0x2;
                    }
                    else{
                        current_find.signs.clear();
                        break;
                    }
                }
                if(!current_find.signs.empty()){
                    current_find.node = realized_node->second;
                    if( saved_find.node != NULL ){
                        int n_a=0,n_b=0;
                        for(int i=0;i<(int)saved_find.signs.size();i++){
                            if(saved_find.signs[i]) n_a++;
                            if(current_find.signs[i]) n_b++;
                        }

                        if( n_b < n_a )
                            saved_find = current_find;
                    }
                    else{
                        saved_find = current_find;
                    }
                }
            }
        }


        if( saved_find.node == NULL && node_input_it->second != -1 )
            throw "[pagLib] Cannot find input node";
        else{
            if( saved_find.has_signs!=0x3 ){
                saved_find.signs.clear();
                if( saved_find.has_signs&0x1 ) saved_find.signs.push_back(false);
                else saved_find.signs.push_back(true);
            }
            input_nodes.push_back( make_pair<adder_graph_base_node_t*,vector<bool> >( saved_find.node,saved_find.signs )  );
        }
    }
}


int adder_tree_scope::vecEqual(vector<int64_t> v1, vector<int64_t> v2){
    if( v1 == v2 ){
        return 1;
    }
    else {
        vector<int64_t> v2_neg;
        for(vector<int64_t>::iterator item=v2.begin();item!=v2.end();++item){
            v2_neg.push_back( (*item)*-1 );
        }
        if( v1 == v2_neg ){
            return -1;
        }
        else
        {
            return 0;
        }
    }

}
