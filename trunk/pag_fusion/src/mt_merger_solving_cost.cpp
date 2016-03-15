#include "mt_merger.h"

float merger::path_combine(vector<index_type> &cur_nodes, index_type cur_stage)
{
    collect_paths(cur_nodes,cur_stage);

    float cost_complete = 0;

    for(vector<mt_decision_node*>::iterator item = stage_decision_nodes.begin();item!=stage_decision_nodes.end();++item)
    {
        mt_decision_node* t = *item;
        t->expand();
        t->get_best();
        float cost = t->result.cost;
        if(cost != FLT_MAX)
            cost_complete += cost;

        t->clear();
    }

    if( !global_options.costmodel.lowcost && cur_stage > 0)
    {
        int complete_ws = 0;
        for(uint c=0, c_end = cur_nodes.size();c<c_end;++c)
        {
            int ws = configurations[c]->stages[cur_stage]->nodes[cur_nodes[c]]->wordsize;
            if( ws > complete_ws ) complete_ws = ws;
        }
        cost_complete += complete_ws * COST_ADD;
    }

    return cost_complete;
}

void merger::prepare_decision_nodes(index_type cur_stage)
{
    stage_decision_nodes.resize(stage_node_count[cur_stage+1]);  // FFNODE: muss das angepasst werden?

    for(uint i = 0, i_end = stage_decision_nodes.size();i<i_end;++i)
        stage_decision_nodes[i] = new mt_decision_node(configurations.size());

    map<int,vector<mt_node*> >::iterator ghost_it;
    if( (ghost_it = ghost_nodes_map.find(cur_stage+1)) != ghost_nodes_map.end() )
    {
        for( vector<mt_node*>::iterator iter = (*ghost_it).second.begin(), iter_end = (*ghost_it).second.end();iter!=iter_end;++iter  )
        {
            stage_decision_nodes[(*iter)->id_merged]->no_of_dont_cares++;
            stage_decision_nodes[(*iter)->id_merged]->decision_types[ (*iter)->cfg_id ] = DECISION_DONT_CARE;
        }
    }
}

void merger::clear_decision_nodes()
{
    while(stage_decision_nodes.size() > 0){
        delete stage_decision_nodes.back();
        stage_decision_nodes.pop_back();
    }
}

void merger::collect_paths(vector<index_type> &cur_nodes, int cur_stage)
{
    for(uint i=0, i_end = configurations.size();i<i_end;++i)
    {
        mt_node* node = configurations[i]->stages[cur_stage]->nodes[cur_nodes[i]];
        if(!node->is_ghost)
        {
            for(vector<mt_path*>::iterator iter=node->paths_down.begin(), iter_end = node->paths_down.end();iter!=iter_end;++iter)
            {
                mt_path* t = (*iter);
                mt_decision_node* tt =  stage_decision_nodes[t->target->id_merged];
                if( t->target->neg_shift<0 && t->target->neg_shift<tt->neg_shift )
                    tt->neg_shift = t->target->neg_shift;

                tt->decision_paths[i].push_back( t );
                if(t->target->is_ternary){
                    tt->has_ternary = true;
                    tt->decision_types[i] = DECISION_ADD3;
                }
                else if(t->isreg) tt->decision_types[i] = DECISION_REG;
                else tt->decision_types[i] = DECISION_ADD;
            }
        }
    }
}
