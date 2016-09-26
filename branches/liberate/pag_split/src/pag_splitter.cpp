#include "pag_splitter.h"
#include <algorithm>

bool comparer_stage(const adder_graph_base_node_t* first, const adder_graph_base_node_t* second )
{
    return (first->stage > second->stage);
}

bool output_reg_selector(adder_graph_base_node_t*& t){
    return ( is_a<register_node_t>(*t) && static_cast<register_node_t*>(t)->stage == static_cast<register_node_t*>(t)->input->stage );
}

pag_splitter::pag_splitter(string complete_graph,string instance_name)
{
    instance = instance_name;
    source_graph = new adder_graph_t();
    if(source_graph->parse_to_graph(complete_graph))
    {
        source_graph->check_and_correct(complete_graph);
        source_graph->nodes_list.remove_if(&output_reg_selector);

        if( graph_has_negshift() ){
            std::cerr << "[PAG_SPLIT] Source graph is incompatible because it contains negative shifts." << std::endl;
            exit(-1);
            //throw std::runtime_error("[PAG_SPLIT] Incompatible graph input.");
        }
        //source_graph->normalize_graph();
        //source_graph->check_and_correct(complete_graph);
        source_graph->drawdot(instance_name+"split_input.dot");
        get_output_nodes();
    }
    else throw runtime_error("parse failed");
}

pag_splitter::~pag_splitter()
{
    delete source_graph;
}

void pag_splitter::inflate_graph(vector<vector<vector<int64_t> > > &groups)
{
    map<adder_graph_base_node_t*,vector<vector<int64_t> > > visited_nodes;
    for( vector<vector<vector<int64_t> > >::iterator outer = groups.begin();outer!=groups.end();++outer )
    {
        for( vector<vector<int64_t> >::iterator inner = (*outer).begin();inner!=(*outer).end();++inner )
        {
            vector<int64_t> node_output(*inner);

            bool normalize=true;
            do
            {
                normalize = true;
                for(unsigned int i=0;i<node_output.size();i++)
                {
                     if( abs(node_output[i]) % 2 == 1) normalize=false;
                }
                if(normalize)
                {
                    for(unsigned int i=0;i<node_output.size();i++)
                    {
                        node_output[i] /= 2;
                    }
                }
            }while( normalize );

            for(  list<adder_graph_base_node_t*>::iterator node = source_graph_output_nodes.begin();
                  node != source_graph_output_nodes.end(); ++node)
            {
                if( (*node)->output_factor[0] == node_output )
                {
                     visited_nodes[*node].push_back(*inner);
                }
            }
        }
    }

    // Copy Nodes
    for( map<adder_graph_base_node_t*,vector<vector<int64_t> > >::iterator duplicate = visited_nodes.begin();duplicate!=visited_nodes.end();++duplicate )
    {
        for(unsigned int i=0;i<(*duplicate).second.size();i++)
        {
            adder_graph_base_node_t* node = (*duplicate).first;
            if(i+1 < (*duplicate).second.size())
            {
                if( is_a<register_node_t>(*(*duplicate).first) )
                {
                    register_node_t* new_node = new register_node_t;
                    new_node->stage = ((register_node_t*)node)->stage;
                    new_node->input = ((register_node_t*)node)->input;
                    new_node->output_factor = ((register_node_t*)node)->output_factor;
                    new_node->input_shift = ((register_node_t*)node)->input_shift;
                    node = new_node;
                }
                else if( is_a<adder_subtractor_node_t>(*(*duplicate).first) )
                {
                    adder_subtractor_node_t* new_node = new adder_subtractor_node_t;
                    new_node->stage = ((adder_subtractor_node_t*)node)->stage;
                    new_node->inputs = ((adder_subtractor_node_t*)node)->inputs;
                    new_node->output_factor = ((adder_subtractor_node_t*)node)->output_factor;
                    new_node->input_is_negative = ((adder_subtractor_node_t*)node)->input_is_negative;
                    new_node->input_shifts = ((adder_subtractor_node_t*)node)->input_shifts;
                    node = new_node;
                }
                source_graph->nodes_list.push_back(node);
                source_graph_output_nodes.push_back(node);
            }
            while( node->output_factor[0] != (*duplicate).second[i])
            {
                for(unsigned int j=0;j<node->output_factor[0].size();j++)
                    node->output_factor[0][j] = node->output_factor[0][j]<<1;

                if( is_a<register_node_t>(*(*duplicate).first) )
                {
                    ((register_node_t*)node)->input_shift++;
                }
                else if( is_a<adder_subtractor_node_t>(*(*duplicate).first) )
                {
                    for(unsigned int f=0;f<((adder_subtractor_node_t*)node)->input_shifts.size();f++)
                        ((adder_subtractor_node_t*)node)->input_shifts[f]++;
                }
            }
        }
    }
}

void pag_splitter::get_output_nodes()
{
    int max_stage=0;
    for( list<adder_graph_base_node_t*>::iterator iter= source_graph->nodes_list.begin();iter!=source_graph->nodes_list.end();++iter)
    {
        if( max_stage < (*iter)->stage ){
            max_stage = (*iter)->stage;
        }
    }


    for( list<adder_graph_base_node_t*>::iterator iter= source_graph->nodes_list.begin();iter!=source_graph->nodes_list.end();++iter)
    {
        if( (*iter)->stage == max_stage ){
             source_graph_output_nodes.push_back((*iter));
        }
    }
}

void pag_splitter::add_no_group()
{
    list<adder_graph_base_node_t*>::iterator iter;
    for(iter=source_graph_output_nodes.begin();
        iter!=source_graph_output_nodes.end();
        ++iter)
    {
        adder_graph_base_node_t* t = *iter;
        list<adder_graph_base_node_t*> tg;
        tg.push_back(t);
        groups.push_back(tg);
    }
}

void pag_splitter::add_group(vector<vector<int64_t> > &group_nodes)
{
    list<adder_graph_base_node_t*> tg;
    unsigned int found=0;
    list<adder_graph_base_node_t*>::iterator iter;
    for(iter=source_graph_output_nodes.begin();
        iter!=source_graph_output_nodes.end();
        ++iter)
    {
        adder_graph_base_node_t* t = *iter;

        vector<vector<int64_t> >::iterator group_outs_del = group_nodes.end();
        vector<vector<int64_t> >::iterator group_outs;
        for(group_outs= group_nodes.begin();
            group_outs!=group_nodes.end();
            ++group_outs)
        {
            if(t->output_factor[0] == (*group_outs))
            {
                group_outs_del = group_outs;
                tg.push_back(t);
                found++;
                break;
            }
        }
        if(group_outs_del!=group_nodes.end())
        {
            group_nodes.erase(group_outs_del);
        }
    }
    for(iter=tg.begin();
        iter!=tg.end();
        ++iter)
    {
        source_graph_output_nodes.remove( (*iter) );
    }
    if(tg.size() != found)
    {
        throw runtime_error("Add group failed");
    }
    groups.push_back(tg);
}

void pag_splitter::split()
{
    list<list<adder_graph_base_node_t*> >::iterator group_iter;
    for(group_iter = groups.begin();
        group_iter!= groups.end();
        ++group_iter)
    {
        adder_graph_t* t_graph = new adder_graph_t();
        list<adder_graph_base_node_t*>::iterator group_node_iter;
        for(group_node_iter = (*group_iter).begin();
            group_node_iter!=(*group_iter).end();
            ++group_node_iter)
        {
            walk_graph((*group_node_iter),t_graph);
        }
        t_graph->nodes_list.sort();
        t_graph->nodes_list.unique();
        split_graphs.push_back(t_graph);
    }
}

void pag_splitter::walk_graph(adder_graph_base_node_t *node, adder_graph_t *graph)
{
    if(node != NULL)
    {
        graph->nodes_list.push_back(node);
        if( is_a<adder_subtractor_node_t>(*node) || is_a<conf_adder_subtractor_node_t*>(*node) )
        {
            adder_subtractor_node_t* t = (adder_subtractor_node_t*)node;
            vector<adder_graph_base_node_t*>::iterator iter;
            for(iter=t->inputs.begin();
                iter!=t->inputs.end();
                ++iter)
            {
                if( std::find(graph->nodes_list.begin(),graph->nodes_list.end(),*iter)==graph->nodes_list.end() )
                    walk_graph(*iter,graph);
            }

        }
        else if( is_a<mux_node_t>(*node) )
        {
            mux_node_t* t = (mux_node_t*)node;
            vector<adder_graph_base_node_t*>::iterator iter;
            for(iter=t->inputs.begin();
                iter!=t->inputs.end();
                ++iter)
            {
                if( std::find(graph->nodes_list.begin(),graph->nodes_list.end(),*iter)==graph->nodes_list.end() )
                    walk_graph(*iter,graph);
            }
        }
        else if( is_a<register_node_t>(*node) )
        {
            register_node_t* t = (register_node_t*)node;
            if( std::find(graph->nodes_list.begin(),graph->nodes_list.end(),t->input)==graph->nodes_list.end() )
                walk_graph(t->input,graph);
        }
    }
}

void pag_splitter::print_all_splitted(string instance_name)
{
    int count = 1;
    list<adder_graph_t*>::iterator iter;
    for(iter=split_graphs.begin();
        iter!=split_graphs.end();
        ++iter)
    {
        stringstream ss;
        ss<<instance_name << "split_graph_"<<count<<".txt";
        ofstream file;
        file.open(ss.str().c_str());
        adder_graph_t* t = (*iter);
        t->nodes_list.sort(comparer_stage);
        stringstream dot_name;
        dot_name<<instance_name << "split_graph_"<<count << ".dot";
        t->drawdot(dot_name.str());
        (*iter)->writesyn(file,true);
        file.close();
        count++;
    }
}

void pag_splitter::generate_input_file(string instance_name,vector<vector<vector<int64_t > > >& groups )
{
    stringstream outstream;

    list<adder_graph_t*>::iterator iter;
    for(iter=split_graphs.begin();
        iter!=split_graphs.end();
        ++iter)
    {
        outstream << "<graph>\n";
        (*iter)->nodes_list.sort(comparer_stage);
        (*iter)->writesyn(outstream,true);
        outstream << "</graph>\n";
    }

    for( unsigned int i=0;i<groups.size();i++ )
    {
        outstream << "<merge>\n";

        for(unsigned int j=0;j<groups[i].size();j++)
        {
            if(j>0) outstream << ";";
            for(unsigned int k=0;k<groups[i][j].size();k++)
            {
                if(k>0) outstream << ",";
                outstream << groups[i][j][k];
            }
        }
        outstream << "\n</merge>\n";
    }

    ofstream file;
    string filename = instance_name + "pag_fusion_input.txt";

    file.open(filename.c_str());
    file << outstream.str();
    file.close();
}

bool pag_splitter::graph_has_negshift(){
    std::vector<int> nn;
    int max_stage=0;
    for(list<adder_graph_base_node_t*>::iterator it= source_graph->nodes_list.begin(), it_end = source_graph->nodes_list.end();
        it!=it_end;
        ++it)
    {
        if (is_a<adder_subtractor_node_t>(*(*it))){
            adder_subtractor_node_t* t = (adder_subtractor_node_t*)*it;
            if(t->stage>max_stage)
                max_stage = t->stage;
            for(int i=0;i<t->input_shifts.size();++i)
                if( t->input_shifts[i] < 0 )
                    nn.push_back(t->stage);
        }
    }
    for(int i=0;i<nn.size();++i)
        if(nn[i]<max_stage)
            return true;

    return false;
}

void pag_splitter::detox_graph()
{
    bool stage_has_nodes;
    int cur_stage =0;
    vector<adder_graph_base_node_t*> erase_list;
    map<adder_graph_base_node_t*,adder_graph_base_node_t*> replace_list;
    map<adder_graph_base_node_t*,int64_t> replace_shift;
    // delete first input factor
    do{
        stage_has_nodes=false;
        for(  list<adder_graph_base_node_t*>::iterator node = source_graph->nodes_list.begin();
              node != source_graph->nodes_list.end(); ++node)
        {
            adder_graph_base_node_t* t = *node;
            if(cur_stage==t->stage){
                stage_has_nodes=true;
                t->output_factor[0].erase(t->output_factor[0].begin());
                if( t->output_factor[0][0] == 0 )
                    erase_list.push_back(t);
                else{
                    if ( is_a<adder_subtractor_node_t>(*t) ){
                        adder_subtractor_node_t* tt = (adder_subtractor_node_t*)t;
                        vector<adder_graph_base_node_t*> new_inputs;
                        vector<bool> new_inputs_is_negative;
                        vector<int64_t> new_input_shifts;
                        for( int k=0;k<(int)tt->inputs.size();k++ ){
                            if( std::find( erase_list.begin(),erase_list.end(),tt->inputs[k]) == erase_list.end() ){
                                new_inputs.push_back(tt->inputs[k]);
                                new_inputs_is_negative.push_back(tt->input_is_negative[k]);
                                new_input_shifts.push_back(tt->input_shifts[k]);
                            }
                            else if( replace_list.find(tt->inputs[k]) != replace_list.end() ){
                                adder_graph_base_node_t* rep_t = replace_list[tt->inputs[k]];
                                new_inputs.push_back( rep_t  );
                                new_inputs_is_negative.push_back(tt->input_is_negative[k]);
                                new_input_shifts.push_back( tt->input_shifts[k] + replace_shift[rep_t] );
                            }
                        }
                        tt->inputs = new_inputs;
                        tt->input_is_negative = new_inputs_is_negative;
                        tt->input_shifts = new_input_shifts;


                        if(  tt->inputs.size() == 0 ){
                            erase_list.push_back( tt );
                        }
                        else if( tt->inputs.size() == 1  ){
                            register_node_t* new_reg = new register_node_t;
                            new_reg->stage = t->stage;
                            new_reg->output_factor = t->output_factor;
                            for(int i=0;i<(int)new_reg->output_factor[0].size();++i){
                                new_reg->output_factor[0][i] = (new_reg->output_factor[0][i] >> tt->input_shifts.front());
                            }
                            new_reg->input = tt->inputs[0];
                            new_reg->input_shift = 0;
                            replace_shift.insert( make_pair<adder_graph_base_node_t*,int64_t>(new_reg,tt->input_shifts.front()) );
                            replace_list.insert( make_pair<adder_graph_base_node_t*,adder_graph_base_node_t*>(tt,new_reg) );
                            erase_list.push_back( tt );
                        }
                    }
                    else if ( is_a<register_node_t>(*t) ){
                        register_node_t* tt = (register_node_t*)t;

                        if( std::find( erase_list.begin(),erase_list.end(),tt->input) != erase_list.end() ){
                            erase_list.push_back( tt );
                        }
                        else if( replace_list.find(tt->input) != replace_list.end() ){
                            adder_graph_base_node_t* rep_t = replace_list[tt->input];
                            tt->input = rep_t;
                            tt->input_shift = tt->input_shift + replace_shift[rep_t];
                        }
                    }
                }
            }
        }
        cur_stage++;
    }while(stage_has_nodes);

    for( int i=0;i<(int)erase_list.size();i++){
        list<adder_graph_base_node_t*>::iterator it = std::find( source_graph->nodes_list.begin(),source_graph->nodes_list.end(),erase_list[i] );
        source_graph->nodes_list.erase(it);
    }

    for( map<adder_graph_base_node_t*,adder_graph_base_node_t*>::iterator iter = replace_list.begin(); iter!= replace_list.end();++iter ){
        source_graph->nodes_list.push_back( (*iter).second );
    }

    erase_list.clear();
    replace_list.clear();
    replace_shift.clear();
    cur_stage = 0;
    // delete duplicates
    do{
        stage_has_nodes=false;
        for(  list<adder_graph_base_node_t*>::iterator node = source_graph->nodes_list.begin();
              node != source_graph->nodes_list.end(); ++node)
        {
            if( (*node)->stage == cur_stage && replace_list.find(*node) == replace_list.end()){
                stage_has_nodes = true;
                for(  list<adder_graph_base_node_t*>::iterator node2 = source_graph->nodes_list.begin();
                      node2 != source_graph->nodes_list.end(); ++node2)
                {
                    if( (*node)!=(*node2) && (*node)->stage == (*node2)->stage && (*node)->output_factor == (*node2)->output_factor ){
                        replace_list.insert( make_pair<adder_graph_base_node_t*,adder_graph_base_node_t*>(*node2,*node) );
                    }
                }

                if ( is_a<adder_subtractor_node_t>(**node) ){
                    adder_subtractor_node_t* tt = (adder_subtractor_node_t*)(*node);
                    for( int k=0;k<(int)tt->inputs.size();k++ ){
                        if( replace_list.find(tt->inputs[k]) != replace_list.end() ){
                            adder_graph_base_node_t* rep_t = replace_list[tt->inputs[k]];
                            tt->inputs[k] = rep_t;
                        }
                    }
                }
                else if ( is_a<register_node_t>(**node) ){
                    register_node_t* tt = (register_node_t*)(*node);

                    if( replace_list.find(tt->input) != replace_list.end() ){
                        adder_graph_base_node_t* rep_t = replace_list[tt->input];
                        tt->input = rep_t;
                        tt->input_shift = tt->input_shift + replace_shift[rep_t];
                    }
                }
            }
        }
        cur_stage++;
    }while(stage_has_nodes);

    for( map<adder_graph_base_node_t*,adder_graph_base_node_t*>::iterator iter = replace_list.begin(); iter!= replace_list.end();++iter ){
        list<adder_graph_base_node_t*>::iterator it = std::find( source_graph->nodes_list.begin(),source_graph->nodes_list.end(),(*iter).first );
        source_graph->nodes_list.erase(it);
    }

    source_graph->nodes_list.sort(comparer_stage);

    source_graph->drawdot( instance+"detox.dot" );
    source_graph->writesyn(instance+"detox.txt",true);
}
