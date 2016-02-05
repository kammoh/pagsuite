#include "mt_merger.h"

merger::merger()
{
    best_cost_all = INT_MAX;

    path_ghost = new mt_path(true);
    path_nop = new mt_path(false);
}

merger::~merger()
{
    while(configurations.size() > 0){
        delete configurations.back();
        configurations.pop_back();
    }
    configurations.clear();

    merge_tree_n.clear();
    best_merge_tree_n.clear();

    while(matree_tree_n.size() > 0){
        delete matree_tree_n.back();
        matree_tree_n.pop_back();
    }
    delete path_ghost;
    delete path_nop;
}

void merger::init()
{
    path_ghost = new mt_path(true);
    path_nop = new mt_path(false);
    inp_count = 0;

    best_cost_all = INT_MAX;
    cut_count=0;
    scut_count=0;
    tcut_count=0;
    ign_count=0;
    new_count=0;
    rec_count=0;
	exit_code=0;
}




