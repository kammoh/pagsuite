#include "mt_decision_node.h"

mt_decision_node::mt_decision_node(int size)
{
    has_ternary = false;
    no_of_dont_cares = 0;
    outputs_ws = 0;
    neg_shift = 0;
    result.cost = FLT_MAX;
    if(global_options.costmodel.primitive){
        if(global_options.costmodel.lowcost)
            costfunction = cost_function::costmodel_lowlevel_primitive;
        else
            costfunction = cost_function::costmodel_primitive;
    }
    else if(global_options.costmodel.lowcost)
        costfunction = cost_function::costmodel_lowlevel;
    else
        costfunction = cost_function::costmodel_std;

    construct = false;
    decision_paths.resize(size,vector<mt_path*>());
    paths.resize(size,vector<int>());

    decision_types.resize(size,DECISION_NONE);
    neg_shift = 0;
}

void mt_decision_node::clear()
{
    outputs_ws = 0;
    neg_shift = 0;
    int size = decision_types.size();
    result.cost = FLT_MAX;
    for(int i=0;i<size;++i)
    {
        std::sort(paths[i].begin(),paths[i].end());
        decision_paths[i].clear();
        if(decision_types[i] != DECISION_DONT_CARE)
            decision_types[i] = DECISION_NONE;
    }
}

bool mt_decision_node::swap_cfg(int cfg)
{
    return next_permutation(decision_paths[cfg].begin(),decision_paths[cfg].end());
}

void mt_decision_node::expand()
{
    bool isReg = true;
    for(int i=0;i<(int)decision_types.size();i++){
        if(decision_types[i]!=DECISION_REG && decision_types[i]!=DECISION_DONT_CARE)
        {
            isReg = false;
            break;
        }
    }
    if (isReg)
        noLines = 1;
    else
        noLines = (has_ternary?3:2);

    for(vector<vector<mt_path*> >::iterator iter = decision_paths.begin(), iter_end = decision_paths.end();iter!=iter_end;++iter)
    {
        (*iter).resize( noLines );
    }

    for(int i=0;i<decision_types.size();++i)
    {
        paths[i].resize(noLines,0);
        for(int j=0;j<noLines;++j){
            paths[i][j] = j;
        }
    }
}

void mt_decision_node::get_best()
{
    if(global_options.costmodel.primitive)
    {
        if(construct){
            if(global_options.costmodel.lowcost) costfunction = cost_function::costmodel_lowlevel;
            else costfunction = cost_function::costmodel_std;
            get_best(0);
        }
        else
        {
            result.cost = costfunction(this);
            if ( result.cost < 0.0 )
            {
                costfunction(this);
            }
            result.type = 'C';
        }
    }
    else
    {
        get_best(0);
    }
}

void mt_decision_node::get_best( unsigned int cur_cfg )
{   
    result.type = 'F';
    cur_type = 'F';

    if( cur_cfg < decision_types.size() )
    {
        do{
            get_best( cur_cfg+1 );
        }while ( next_permutation(paths[cur_cfg].begin(),paths[cur_cfg].end()) );
    }
    else
    {
        float cost = costfunction(this);
        //print_state(cost);
        if(cost < result.cost)
        {
            result.cost = cost;
            result.type = cur_type;
            if(construct){
                result.paths_copy = paths;
            }
        }
    }
}

void mt_decision_node::swap_to_best_state()
{
    paths = result.paths_copy;
}

void mt_decision_node::print_state(float cost)
{
    LOG() << "DECISION_NODE: State(";
    LOG() << ") result: type <" << (cur_type) << "> @cost "<< cost << (cost < result.cost?" >NEW BEST":" >IGNORED") << endl;

    for(int j=0, j_end = noLines;j<j_end;++j)
    {
        LOG() << "\tSIDE (" << j << ")" << endl;
        for(uint c=0, c_end = decision_paths.size();c<c_end;++c)
        {
            LOG() << "\t\tcfg "<<c << ": ";
            if( decision_types[c] == DECISION_DONT_CARE ) LOG() << "DON'T CARE";
            else if ( decision_types[c] == DECISION_REG && decision_paths[c][paths[c][j]]==NULL ) LOG() << "REG ZERO";
            else if ( decision_types[c] == DECISION_NONE ) LOG() << "TYPE NOT SET";
            else
            {
                if( decision_types[c] == DECISION_ADD && decision_paths[c][paths[c][j]]==NULL )
                    LOG() << "PATH UNKNOWN";
                else if( decision_paths[c][paths[c][j]]==NULL )
                    LOG() << "PATH NOT SET";
                else
                {
                    LOG() << (decision_paths[c][paths[c][j]]->is_neg?"-":"") << "(" << decision_paths[c][paths[c][j]]->parent->print_values() << ") shift(" << decision_paths[c][paths[c][j]]->shift << ")";
                }
            }
            LOG() << endl;
        }
    }
}

bool mt_decision_node::construct_best(mt_node_construct *new_output_node)
{
    bool has_mux = false;
    construct = true;
    expand();
    get_best();
    swap_to_best_state();

    pair<int,int> node_and_shift_pair;
    set<pair<int,int> > node_and_shift_set;

    for(uint i=0, i_end = noLines;i<i_end;++i)
    {
        int c_zero=0;
        node_and_shift_set.clear();
        mt_path_construct* new_path = new mt_path_construct();
        for( uint c=0, c_end = decision_paths.size();c<c_end;++c )
        {
            mt_path* t = decision_paths[c][paths[c][i]];
            if(t != NULL)
            {
                new_path->paths.insert(make_pair(c,t));
                node_and_shift_pair.first = t->parent->id_merged;
                node_and_shift_pair.second = t->shift-neg_shift;
                node_and_shift_set.insert(node_and_shift_pair);

            }
            else if( decision_types[c] == DECISION_REG || (has_ternary && decision_types[c]==DECISION_ADD) )
            {
                c_zero++;
                new_path->paths.insert(make_pair(c,(mt_path*)NULL));
            }
        }

        if( node_and_shift_set.size() == 0 && c_zero == 0)
            new_path->paths_type = path_construct_type_none;
        else if( node_and_shift_set.size() == 1 && c_zero == 0  )
            new_path->paths_type = path_construct_type_path;
        else
        {
            has_mux = true;
            new_path->paths_type = path_construct_type_mux;
        }
        new_output_node->inputs.push_back( new_path );
    }
    return has_mux;
}
