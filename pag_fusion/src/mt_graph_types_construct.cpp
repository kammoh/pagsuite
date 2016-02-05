#include "mt_graph_types_construct.h"
#include <algorithm>

mt_graph_construct::mt_graph_construct(int stages_count){
    no_of_stages = stages_count;
    stage_has_mux = new vector<bool>(stages_count);
    stages = new vector<vector<mt_node_construct*>*>(stages_count);
    stage_mux_register = new vector<vector<register_node_t*>*>(stages_count);
    stage_mux = new vector<vector<mux_node_t*>*>(stages_count);
    for(int i=0;i<stages_count;++i)
    {
        stages->at(i) = new vector<mt_node_construct*>();
        stage_mux_register->at(i) = new vector<register_node_t*>();
        stage_mux->at(i) = new vector<mux_node_t*>();
    }
}

mt_graph_construct::~mt_graph_construct()
{
    for(uint i=0, i_end = stages->size();i<i_end;++i)
    {
        for(uint j=0, j_end = stages->at(i)->size();j<j_end;++j)
        {
            delete stages->at(i)->at(j);
        }
        stages->at(i)->clear();
        delete stages->at(i);
        delete stage_mux_register->at(i);
        delete stage_mux->at(i);
    }

    delete stages;
    delete stage_mux;
    delete stage_has_mux;
    delete stage_mux_register;
}

void mt_graph_construct::convert(adder_graph_t* new_graph,int no_of_inputs)
{
    map<int,map<int,adder_graph_base_node_t*> > new_stages;

    {
        map<int,adder_graph_base_node_t*> new_stage;

        for(int in=0;in<no_of_inputs;++in)
        {
            input_node_t* new_in_node = new input_node_t();
            new_in_node->stage = 0;

            for(int i=0;i<conf_count;++i)
            {
                  vector<int64_t> new_outputs;
                  for(int j=0;j<no_of_inputs;++j)
                  {
                        if(in==j) new_outputs.push_back(1);
                        else new_outputs.push_back(0);
                  }
                  new_in_node->output_factor.push_back(new_outputs);
            }
            new_stage.insert(make_pair(in,new_in_node));
            new_graph->nodes_list.push_back(new_in_node);
        }
        new_stages.insert( make_pair(-1,new_stage) );
    }

    int node_in_stage=0,mux_in_stage=0;
    for(uint cur_stage_id=0, cur_stage_id_end = stages->size();cur_stage_id < cur_stage_id_end;++cur_stage_id)
    {
        mux_in_stage = node_in_stage + 1;
        node_in_stage++;
        if( stage_has_mux->at(cur_stage_id) )
        {
            node_in_stage++;
        }

        map<int,adder_graph_base_node_t*> new_stage;

        vector<mt_node_construct*>* cur_stage = stages->at(cur_stage_id);

        for(vector<mt_node_construct*>::iterator mt_node_it = cur_stage->begin(), mt_node_it_end = cur_stage->end();
            mt_node_it !=mt_node_it_end;
            ++mt_node_it)
        {
            mt_node_construct* cur_node = *mt_node_it;
            vector<bool> is_negative;
            adder_graph_node_type cur_type = get_adder_type(cur_node,is_negative);
            int cur_neg_shift=0;

            for(int i=0;i<cur_node->nodes.size();i++)
                if(cur_node->nodes[i]->neg_shift<0 && cur_node->nodes[i]->neg_shift<cur_neg_shift)
                    cur_neg_shift = cur_node->nodes[i]->neg_shift;

            adder_graph_base_node_t* new_node;

            switch(cur_type)
            {
            case adder_graph_node_type_add:
                new_node = new adder_subtractor_node_t();
                ((adder_subtractor_node_t*)new_node)->input_is_negative = is_negative;
                break;
            case adder_graph_node_type_sub:
                new_node = new adder_subtractor_node_t();
                ((adder_subtractor_node_t*)new_node)->input_is_negative = is_negative;
                break;
            case adder_graph_node_type_conf_addsub:
                new_node = new conf_adder_subtractor_node_t();
                set_conf_adder(((conf_adder_subtractor_node_t*)new_node),cur_node);
                break;
            case adder_graph_node_type_reg:
                new_node = new register_node_t();
                break;
            }

            vector<mt_path_construct*>::iterator path_iter;
            for(path_iter=cur_node->inputs.begin();
                path_iter!=cur_node->inputs.end();
                ++path_iter)
            {
                mt_path_construct* cur_path=*path_iter;
                switch(cur_path->paths_type)
                {
                case path_construct_type_path:
                    if( stage_has_mux->at(cur_stage_id) )
                    {
                        int64_t t_input_shift=0;
                        register_node_t* new_reg = get_mux_reg(cur_path,(*new_stages.find(cur_stage_id-1)).second,t_input_shift);
                        new_reg->stage = mux_in_stage;
                        bool reg_combined = combine_reg(&new_reg,cur_stage_id);

                        if( cur_type == adder_graph_node_type_reg )
                        {
                             ((register_node_t*)new_node)->input = new_reg;
                        }
                        else
                        {
                            ((adder_subtractor_node_t*)new_node)->inputs.push_back( new_reg );
                            ((adder_subtractor_node_t*)new_node)->input_shifts.push_back( t_input_shift );
                        }

                        if(!reg_combined) new_graph->nodes_list.push_back(new_reg);
                    }
                    else
                    {
                        int source_id=-1,shift=0;
                        map<l_int,mt_path*>::iterator tmp_iter;
                        for(tmp_iter=cur_path->paths.begin();
                            tmp_iter!=cur_path->paths.end();
                            ++tmp_iter)
                        {
                            if( (*tmp_iter).second != NULL )
                            {
                                mt_path* tmp_path=(*tmp_iter).second;
                                source_id = tmp_path->parent->id_merged;
                                shift = tmp_path->shift;
                                break;
                            }
                        }
                        if(source_id==-1) throw runtime_error("Converting: PATH SOURCE ID not found");
                        map<int,adder_graph_base_node_t*>* pre_stage = &(*new_stages.find(cur_stage_id-1)).second;
                        map<int,adder_graph_base_node_t*>::iterator found_node = pre_stage->find(source_id);
                        if(found_node == pre_stage->end())
                            throw runtime_error("Converting: PreNode not found");


                        if( cur_type == adder_graph_node_type_reg )
                        {
                            ((register_node_t*)new_node)->input = (*found_node).second;
                        }
                        else
                        {
                            ((adder_subtractor_node_t*)new_node)->inputs.push_back( (*found_node).second );
                            ((adder_subtractor_node_t*)new_node)->input_shifts.push_back(shift);
                        }
                    }
                    break;
                case path_construct_type_none: break;
                case path_construct_type_mux:
                    mux_node_t* new_mux = get_mux(cur_path,&(*new_stages.find(cur_stage_id-1)).second);
                    new_mux->stage = mux_in_stage;

                    if( cur_type != adder_graph_node_type_reg )
                    {
                        int input_id = ((adder_subtractor_node_t*)new_node)->inputs.size();
                        ((adder_subtractor_node_t*)new_node)->inputs.push_back( new_mux );
                        normalize_mux_shift(((adder_subtractor_node_t*)new_node),new_mux);

                        if( !combine_mux(&new_mux,cur_stage_id ) )
                            new_graph->nodes_list.push_back(new_mux);
                        else
                            ((adder_subtractor_node_t*)new_node)->inputs[input_id] = new_mux;
                    }
                    else
                    {
                        ((register_node_t*)new_node)->input = new_mux;

                        if( !combine_mux(&new_mux,cur_stage_id ) )
                            new_graph->nodes_list.push_back(new_mux);
                        else
                            ((register_node_t*)new_node)->input= new_mux;

                    }

                        //throw runtime_error("Converting: REG with MUX forbidden");

                    break;
                }
            }
            new_node->stage = node_in_stage;
            int new_id = set_node_outputs(new_node,cur_node);
            new_graph->nodes_list.push_back(new_node);
            new_stage.insert( make_pair(new_id,new_node) );
        }

        new_stages.insert( make_pair(cur_stage_id,new_stage) );
    }
    normalize_all( new_graph );
}

bool mt_graph_construct::is_node_reg(mt_node_construct* node)
{
    int count=0;
    for(vector<mt_path_construct*>::iterator iter=node->inputs.begin(), iter_end = node->inputs.end();
        iter!=iter_end;
        ++iter)
    {
        if( (*iter)->paths_type != path_construct_type_none ) count++;
    }
    if(count>1) return false;
    else    return true;
}

int mt_graph_construct::set_node_outputs( adder_graph_base_node_t* new_node,mt_node_construct* node )
{
    int id = -1;
    map<l_int,mt_node*>::iterator iter;
    for(int i=0;i<conf_count;++i)
    {
        mt_node* finding = node->nodes[i];
        id = finding->id_merged;
        new_node->output_factor.push_back(finding->values);
    }
    return id;
}

void mt_graph_construct::set_conf_adder( conf_adder_subtractor_node_t* new_node,mt_node_construct* node )
{
    for(int c=0;c<conf_count;++c)
    {
        vector<bool> new_tmp;
        for(uint i=0, i_end = node->inputs.size();i<i_end;++i)
        {
            mt_path_construct* cur_path = node->inputs.at(i);
            map<l_int,mt_path*>::iterator finding = cur_path->paths.find(c);
            if( finding != cur_path->paths.end() )
            {
                mt_path* source = (*finding).second;
                if(source == NULL)
                {
                    new_tmp.push_back(false);
                }
                else
                {
                    if( source->is_neg )
                        new_tmp.push_back(true);
                    else
                        new_tmp.push_back(false);
                }
            }
            else
            {
                new_tmp.push_back(false);
            }
        }
        new_node->input_is_negative.push_back(new_tmp);
    }
}

adder_graph_node_type mt_graph_construct::get_adder_type(mt_node_construct* node,vector<bool>& is_negative)
{
    vector<bool> path_has_sub(node->inputs.size());
    vector<bool> path_has_add(node->inputs.size());
    int count=0;
    for(uint i=0, i_end = node->inputs.size();i<i_end;++i)
    {
        mt_path_construct* tmp_path_c = node->inputs.at(i);

        if(tmp_path_c->paths_type != path_construct_type_none)
        {
            count++;
            for(map<l_int,mt_path*>::iterator iter=tmp_path_c->paths.begin(), iter_end = tmp_path_c->paths.end();
                iter!=iter_end;
                ++iter)
            {
                mt_path* tmp_path = (*iter).second;
                if(tmp_path!=NULL)
                {
                    if(tmp_path->is_neg) path_has_sub[i] = true;
                    else path_has_add[i] = true;
                }
            }
        }
    }
    if(count>1)
    {
        int add=0,sub=0;
        for(uint i=0, i_end = node->inputs.size();i<i_end;++i)
        {
            if( path_has_add[i] ) add++;
            if( path_has_sub[i] ) sub++;
        }
        if(sub==0){
            //ADDER
            is_negative = path_has_sub;
            return adder_graph_node_type_add;
        }
        else if( (sub+add)==(int)node->inputs.size() ){
            is_negative = path_has_sub;
            return adder_graph_node_type_sub;
        }
        else
        {
            //MIXED
            return adder_graph_node_type_conf_addsub;
        }
    }
    else
    {
        return adder_graph_node_type_reg;
    }
}

register_node_t* mt_graph_construct::get_mux_reg( mt_path_construct* cur_path, map<int,adder_graph_base_node_t*> &pre_stage,int64_t& input_shift )
{
    register_node_t* new_reg = new register_node_t();
    vector<int64_t> new_outputs;
    for(int i=0;i<conf_count;++i)
    {
        new_outputs.clear();
        map<l_int,mt_path*>::iterator finding = cur_path->paths.find(i);
        if( finding != cur_path->paths.end() )
        {
            mt_path* source = (*finding).second;
            if(source == NULL)
            {
                for(int j=0;j<inp_count;++j )
                    new_outputs.push_back( 0 );
            }
            else
            {
                int source_id = source->parent->id_merged;
                map<int,adder_graph_base_node_t*>::iterator found_node = pre_stage.find(source_id);
                if(found_node == pre_stage.end())
                    throw runtime_error("Converting: PreNode not found");

                new_reg->input = (*found_node).second;
                input_shift = source->shift;
                for(vector<int64_t>::iterator iter=source->parent->values.begin(), iter_end = source->parent->values.end();
                    iter!=iter_end;
                    ++iter)
                {
                    new_outputs.push_back( (*iter) );
                }
            }
        }
        else
        {
            for(int j=0;j<inp_count;++j )
                new_outputs.push_back( DONT_CARE );
        }
        new_reg->output_factor.push_back(new_outputs);
    }

    return new_reg;
}

mux_node_t* mt_graph_construct::get_mux( mt_path_construct* cur_path, map<int,adder_graph_base_node_t*>* pre_stage )
{
    mux_node_t* new_mux = new mux_node_t();
    vector<int64_t> new_outputs;
    for(int i=0;i<conf_count;++i)
    {
        new_outputs.clear();
        map<l_int,mt_path*>::iterator finding = cur_path->paths.find(i);
        if( finding != cur_path->paths.end() )
        {
            //FOUND PATH IN MAP MEANS ITS SET OR ZERO
            mt_path* source = (*finding).second;
            if(source == NULL)
            {
                //PATH HAS NO SOURCE MEANS ITS ZERO
                new_mux->inputs.push_back( NULL );
                new_mux->input_shifts.push_back( 0 );
                for(int j=0;j<inp_count;++j)
                    new_outputs.push_back(0);
            }
            else
            {
                //PATH HAS SOURCE MEANS IT HAS VALUE
                int source_id = source->parent->id_merged;
                map<int,adder_graph_base_node_t*>::iterator found_node = pre_stage->find(source_id);
                if(found_node == pre_stage->end())
                    throw runtime_error("Converting: PreNode not found");

                new_mux->inputs.push_back( (*found_node).second );
                new_mux->input_shifts.push_back( source->shift );

                for(vector<int64_t>::iterator iter=source->parent->values.begin(), iter_end = source->parent->values.end();
                    iter!=iter_end;
                    ++iter)
                {
                    if( source->shift < 0 )
                        new_outputs.push_back( (*iter) );
                    else
                        new_outputs.push_back( ((*iter)<<source->shift) );
                }
            }
        }
        else
        {
            //FOUND NO PATH IN MAP MEANS ITS SET TO DONT CARE
            new_mux->inputs.push_back( NULL );
            new_mux->input_shifts.push_back( DONT_CARE );
            for(uint j=0;j<inp_count;++j)
            {
                new_outputs.push_back( DONT_CARE );
            }
        }
        new_mux->output_factor.push_back(new_outputs);
    }

    return new_mux;
}

void mt_graph_construct::normalize_mux_shift( adder_subtractor_node_t* node,mux_node_t* mux)
{
    int min_shift=INT_MAX;

    for(int i=0;i<(int)mux->inputs.size();++i)
    {
        if( mux->inputs[i] != NULL)
        {
            int shift = mux->input_shifts[i];
            if( shift < min_shift ) min_shift = shift;
        }
    }
    if( min_shift < 0)
        locked_mux.push_back(mux);

    for(int i=0;i<(int)mux->inputs.size();++i)
    {
        if( mux->inputs[i] != NULL)
        {
            int shift_diff=0;
            if( mux->input_shifts[i] < 0){
                if( mux->input_shifts[i] == min_shift ){
                    mux->input_shifts[i] = 0;
                    shift_diff = 0;
                }
                else{
                    mux->input_shifts[i] = mux->input_shifts[i] - min_shift;
                    shift_diff = -mux->input_shifts[i];
                }
            }
            else{
                mux->input_shifts[i] -= min_shift;
                shift_diff = min_shift;
            }
            for(int j = 0, j_end = (int)mux->output_factor[i].size(); j < j_end; ++j){
                if(shift_diff < 0)
                    mux->output_factor[i][j] = (mux->output_factor[i][j] << abs(shift_diff));
                else
                    mux->output_factor[i][j] = (mux->output_factor[i][j] >> abs(shift_diff));
            }
        }
    }
    node->input_shifts.push_back(min_shift);
}

bool mt_graph_construct::combine_reg(register_node_t** reg, int stage)
{
    vector<register_node_t*>* vec = stage_mux_register->at(stage);
    vector<vector<int64_t> > values ( (*reg)->output_factor.size() , vector<int64_t>( (*reg)->output_factor.front().size() ) );

    vector<vector<int64_t > > new_values;
    vector<int64_t> inner_values;

    bool combined = false;
    for(vector<register_node_t*>::iterator cur_reg = vec->begin(), cur_reg_end = vec->end();
        cur_reg !=cur_reg_end;
        ++cur_reg)
    {
        new_values.clear();
        register_node_t* t = *cur_reg;
        bool same = true;
        for(uint c=0, c_end = t->output_factor.size();c<c_end;++c)
        {
            inner_values.clear();
            for(uint e=0, e_end = t->output_factor.front().size();e<e_end;++e)
            {
                if( t->output_factor[c][e]!=DONT_CARE
                        && (*reg)->output_factor[c][e]!=DONT_CARE
                        && t->output_factor[c][e]!=(*reg)->output_factor[c][e] )
                {
                    same = false;
                    break;
                }
                else
                {
                    if(t->output_factor[c][e]!=DONT_CARE) inner_values.push_back(t->output_factor[c][e]);
                    else if((*reg)->output_factor[c][e]!=DONT_CARE) inner_values.push_back((*reg)->output_factor[c][e]);
                    else inner_values.push_back(DONT_CARE);
                }
            }
            if(!same) break;
            else new_values.push_back(inner_values);
        }
        if(same)
        {
            t->output_factor.clear();
            t->output_factor = new_values;
            delete (*reg);
            (*reg) = t;
            combined = true;
            break;
        }
    }
    if(!combined)
    {
        vec->push_back( (*reg) );
        return false;
    }
    else return true;
}

bool mt_graph_construct::combine_mux(mux_node_t** mux, int stage)
{
    bool combined = false;

    vector<mux_node_t*>* vec = stage_mux->at(stage);
    for(vector<mux_node_t*>::iterator cur_mux = vec->begin(), cur_mux_end = vec->end();
        cur_mux !=cur_mux_end;
        ++cur_mux)
    {
        mux_node_t* t = *cur_mux;
        if(t->output_factor == (*mux)->output_factor)
        {
            delete (*mux);
            (*mux) = t;
            combined = true;
            break;
        }
    }

    if(!combined)
    {
        vec->push_back( (*mux) );
        return false;
    }
    else return true;
}

void mt_graph_construct::normalize_all(adder_graph_t *new_graph)
{
    int stage_count = 0;
    for(list<adder_graph_base_node_t*>::iterator node=new_graph->nodes_list.begin(), node_end = new_graph->nodes_list.end();node!=node_end ;++node )
    {
        if( (*node)->stage > stage_count  ) stage_count = (*node)->stage;
    }

    map<adder_graph_base_node_t*,int> normalizing_map;
    for(list<adder_graph_base_node_t*>::iterator node=new_graph->nodes_list.begin(), node_end = new_graph->nodes_list.end();node!=node_end ;++node )
    {
        if((*node)->stage < stage_count)
        {
            if( is_a<register_node_t>(**node) )
            {
                register_node_t* t = (register_node_t*)*node;
                if( t->input_shift > 0)
                {
                    normalizing_map[t] = t->input_shift;
                    int min_shift = t->input_shift;
                    t->input_shift = 0;
                    for(uint i=0, i_end = t->output_factor.size();i<i_end;++i)
                    {
                        for(uint j=0, j_end = t->output_factor[i].size();j<j_end;++j)
                        {
                            if(t->output_factor[i][j] != DONT_CARE) t->output_factor[i][j] = (t->output_factor[i][j]>>min_shift);
                        }
                    }
                }
            }
            else if( is_a<mux_node_t>(**node) )
            {
                mux_node_t* t = (mux_node_t*)*node;
                if ( std::find(locked_mux.begin(),locked_mux.end(),t)==locked_mux.end() ){
                    int min_shift=INT_MAX;
                    for(uint i=0, i_end = t->input_shifts.size();i<i_end;++i)
                    {
                        if( t->input_shifts[i] < min_shift && t->inputs[i]!=NULL ) min_shift = t->input_shifts[i];
                    }
                    if( min_shift > 0 )
                    {
                        for(uint i=0, i_end = t->input_shifts.size();i<i_end;++i)
                        {
                            if(t->inputs[i]!=NULL)  t->input_shifts[i] -= min_shift;
                        }
                        normalizing_map[t] = min_shift;
                        for(uint i=0, i_end = t->output_factor.size();i<i_end;++i)
                        {
                            for(uint j=0, j_end = t->output_factor[i].size();j<j_end;++j)
                            {
                                if(t->output_factor[i][j] != DONT_CARE) t->output_factor[i][j] = (t->output_factor[i][j]>>min_shift);
                            }
                        }
                    }
                }
            }
        }
    }
    if( normalizing_map.size() > 0 )
    {
        map<adder_graph_base_node_t*,int>::iterator found;
        for(list<adder_graph_base_node_t*>::iterator node=new_graph->nodes_list.begin(), node_end = new_graph->nodes_list.end();node!=node_end ;++node )
        {
            if( is_a<register_node_t>(**node) )
            {
                register_node_t* t = (register_node_t*)*node;
                if( (found = normalizing_map.find(t->input)) != normalizing_map.end() )
                {
                    t->input_shift += (*found).second;
                }
            }
            else if( is_a<mux_node_t>(**node) )
            {
                mux_node_t* t = (mux_node_t*)*node;
                for(uint i=0, i_end = t->inputs.size(); i<i_end; ++i)
                {
                    if( (found = normalizing_map.find(t->inputs[i])) != normalizing_map.end() )
                    {
                        t->input_shifts[i] += (*found).second;
                    }
                }
            }
            else if( is_a<adder_subtractor_node_t>(**node) )
            {
                adder_subtractor_node_t* t = (adder_subtractor_node_t*)*node;
                for(uint i=0, i_end = t->inputs.size(); i<i_end; ++i)
                {
                    if( (found = normalizing_map.find(t->inputs[i])) != normalizing_map.end() )
                    {
                        t->input_shifts[i] += (*found).second;
                    }
                }
            }
            else if( is_a<conf_adder_subtractor_node_t>(**node) )
            {
                conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)*node;
                for(uint i=0, i_end = t->inputs.size(); i<i_end; ++i)
                {
                    if( (found = normalizing_map.find(t->inputs[i])) != normalizing_map.end() )
                    {
                        t->input_shifts[i] += (*found).second;
                    }
                }
            }
        }
        normalize_all(new_graph);
    }
}
