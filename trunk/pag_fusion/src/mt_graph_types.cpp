#include "mt_graph_types.h"

mt_path::mt_path()
{
    is_ghost = false;
    is_nop = false;
    is_neg = false;
    parent = NULL;
}

mt_path::mt_path(index_type t, short s, bool reg)
{
    isreg = reg;
    to = t;
    shift = s;
    is_neg = false;
    is_ghost = false;
    is_nop = false;
}

mt_path::mt_path(bool ghost)
{
    isreg = false;
    to = 0;
    shift = 0;
    is_neg = false;
    is_ghost = ghost;
    is_nop = !ghost;
}

void mt_path::print(ostream& dest)
{
    dest<<"\t\t"<<"-<"<<shift<<">->"<<to;
    dest<<" ("<<to_merged<<")";
    if(isreg) dest <<"R";
    if(is_neg) dest <<"-";
    dest<<endl;
}

mt_node::mt_node(index_type index)
{
    id = index;
    is_ghost = false;
    id_merged = index;
    is_ternary = false;
    is_fixed = false;
    neg_shift = 0;
}

mt_node::~mt_node()
{
    while(paths_down.size() > 0){
        delete paths_down.back();
        paths_down.pop_back();
    }
}

void mt_node::add(index_type t,ushort s,bool isreg)
{
    paths_down.push_back(new mt_path(t,s,isreg));
}

string mt_node::print_values()
{
    stringstream ss;
    //if( is_ghost ) ss << "-";
    //else
    {
        for(uint i=0, i_end = values.size();i<i_end;++i)
        {
            if(values.at(i) == DONT_CARE)
                ss << "-";
            else
                ss<<values.at(i);
            if(i<values.size()-1) ss<<",";
        }
    }
    return ss.str();
}

void mt_node::print(ostream& dest)
{
    dest<<"\tnode#"<<id<<"|"<<id_merged<<" ("<<print_values()<<" ws:"<<wordsize<<")"<<endl;
    for(vector<mt_path*>::iterator iter=paths_down.begin(), iter_end = paths_down.end();iter!=iter_end;++iter)
    {
        (*iter)->print(dest);
    }
}

mt_stage::mt_stage(index_type index)
{
    id = index;
}

mt_stage::~mt_stage()
{
    while( nodes.size() > 0 )
    {
        delete nodes.back();
        nodes.pop_back();
    }
}

void mt_stage::add(mt_node* node)
{
    nodes.push_back(node);
}

void mt_stage::print(ostream& dest)
{
    dest<<"stage#"<<id<<endl;
    for(vector<mt_node*>::iterator iter=nodes.begin(), iter_end = nodes.end();iter!=iter_end;++iter)
    {
        (*iter)->print(dest);
    }
}

mt_graph::mt_graph()
{
}

mt_graph::~mt_graph()
{
    while(stages.size() > 0){
        delete stages.back();
        stages.pop_back();
    }
}

void mt_graph::add(mt_stage* stage)
{
    stages.push_back(stage);
}

void mt_graph::print(index_type id,ostream& dest)
{
    dest<<"____________graph#"<<id<<"____________"<<endl;
    for(vector<mt_stage*>::iterator iter=stages.begin(), iter_end = stages.end();iter!=iter_end;++iter)
    {
        (*iter)->print(dest);
    }
}
