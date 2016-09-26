#include "adder_graph.h"
#include "mt_merger.h"
#include "sstream"

struct dot_mem{
    stringstream dot_connect;
    stringstream dot_muxes;
    stringstream dot_zero;
};


void merger::create_muxed_graph()
{
    for(int i=1;i<=stage_count;++i)
    {
            temp_merge(i-1,best_merge_tree_n[i]);
    }

    new_graph = new mt_graph_construct(stage_count);
    new_graph->inp_count = inp_count;

    for(uint s=0;s<stage_count;++s)
    {
        for(uint o=0, o_end = stage_node_count[s+1];o<o_end;++o)
        {
             mt_node_construct* new_node = new mt_node_construct();

             mt_decision_node dec_node(configurations.size() );

             for(uint c=0, c_end = configurations.size();c<c_end;++c)
             {
                 mt_stage* cur_stage = configurations[c]->stages[s+1];
                 //find output node
                 for(vector<mt_node*>::iterator iter_n=cur_stage->nodes.begin(), iter_n_end = cur_stage->nodes.end();
                     iter_n!=iter_n_end;
                     ++iter_n)
                 {
                    if( (*iter_n)->id_merged == (int)o )
                    {
                        //new_node->nodes.insert( make_pair<l_int,mt_node*>(c,(*iter_n)));
                        new_node->nodes.insert( make_pair(c,(*iter_n)));
                        if( (*iter_n)->is_ghost ) dec_node.decision_types[c] = DECISION_DONT_CARE;
                        else
                        {
                            if( (*iter_n)->is_ternary ) dec_node.has_ternary = true;
                            for(vector<mt_path*>::iterator path_it=(*iter_n)->paths_up.begin(), path_it_end = (*iter_n)->paths_up.end();path_it!=path_it_end;++path_it )
                            {
                                dec_node.decision_paths[c].push_back( (*path_it) );
                                if( (*path_it)->isreg ) dec_node.decision_types[c] = DECISION_REG;
                                else dec_node.decision_types[c] = DECISION_ADD;
                            }
                        }
                        break;
                    }
                 }
             }
             bool has_mux = dec_node.construct_best(new_node);
             if(has_mux) new_graph->stage_has_mux->at(s) = true;
             new_graph->stages->at(s)->push_back(new_node);
        }
    }

    new_graph->conf_count = configurations.size();
    adder_graph_t new_adder_graph;

    new_graph->convert(&new_adder_graph,stage_node_count[0]);

    delete new_graph;

    stringstream filename;
    if(global_options.outfileprefix.length()>1)filename<<global_options.outfileprefix;
    filename<<"output_graph";
    if(global_options.outfilesuffix.length()>1)filename<<global_options.outfilesuffix;
    filename<<".txt";

    new_adder_graph.writesyn(filename.str().c_str());
    filename.str("");
    if(global_options.outfileprefix.length()>1)filename<<global_options.outfileprefix;
    filename<<"output_graph";
    if(global_options.outfilesuffix.length()>1)filename<<global_options.outfilesuffix;
    filename<<".dot";

    new_adder_graph.drawdot(filename.str().c_str());
    try{
    new_adder_graph.check_and_correct();
	}
	catch(...)
	{
		exit_code = -1;
	}
    get_pag_info(&new_adder_graph);
	
#ifdef PRINT_COMPLETE_TREE
    walk_complete_tree(mt_tree_printer::root_leave, stage_count );
    TREE_PRINT_SAVE("complete_tree.dot");
#endif
}
