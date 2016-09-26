#include "mt_graph_types.h"

mt_path::mt_path()
{
    is_ghost = false;
    is_nop = false;
    is_neg = false;
    parent = NULL;
    target = NULL;
}

mt_path::mt_path(index_type t, short s, bool reg)
{
    isreg = reg;
    shift = s;
    is_neg = false;
    is_ghost = false;
    is_nop = false;
    parent = NULL;
    target = NULL;
}

mt_path::mt_path(bool ghost)
{
    isreg = false;
    shift = 0;
    is_neg = false;
    is_ghost = ghost;
    is_nop = !ghost;
    parent = NULL;
    target = NULL;
}

void mt_path::print(ostream& dest, std::string pre)
{
    if( target!=NULL ){
        dest<< pre <<"--<"<<shift<<">->"<<target->id;
        dest<<" ("<<target->id_merged<<")";
        if(isreg) dest <<"R";
        if(is_neg) dest <<"-";
    }
    else{
        dest<<pre<<"ghostpath";
    }
    dest<<endl;
}

mt_node::mt_node(index_type index)
{
    id = index;
    is_ghost = false;
    id_merged = index;
    is_ternary = false;
    is_fixed = false;
    is_fully_fixed = false;
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

void mt_node::print(ostream& dest, std::string pre)
{
    dest<< "--* node#"<<id<<"|"<<id_merged<<" ("<<print_values()<<" ws:"<<wordsize<<") " << (is_ternary?"ternary ":"")<<(is_ghost?"ghost":"")<<(is_fully_fixed?"fullyfixed":(is_fixed?"fixed":""))<<endl;
    if( !fixed_to.empty() ){

        dest<< pre << (paths_down.empty()?"\\":"|") <<"--* fixed to:"<<endl;
        for(int i=0;i<fixed_to.size();++i){
            if(i==(fixed_to.size()-1))
                dest<< pre << (paths_down.empty()?" ":"|") <<"  \\--> node#"<<fixed_to[i]->id << " in cfg#" <<fixed_to[i]->cfg_id <<endl;
            else
                dest<< pre << (paths_down.empty()?" ":"|") <<"  |--> node#"<<fixed_to[i]->id << " in cfg#" <<fixed_to[i]->cfg_id <<endl;
        }
    }
    if( !paths_down.empty() ){
        dest<< pre <<"\\--* paths:"<<endl;
        for(vector<mt_path*>::iterator iter=paths_down.begin(), iter_end = paths_down.end();iter!=iter_end;++iter)
        {
            if( (*iter) != paths_down.back() ){
                dest << pre << "   |";
                (*iter)->print(dest,"");
            }
            else{
                dest << pre << "   \\";
                (*iter)->print(dest,"");
            }
        }
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

void mt_stage::print(ostream& dest, std::string pre)
{
    dest<<"--* stage#"<<id<<endl;
    for(vector<mt_node*>::iterator iter=nodes.begin(), iter_end = nodes.end();iter!=iter_end;++iter)
    {
        if( (*iter)!=nodes.back() ){
            dest << pre << "|";
            (*iter)->print(dest,pre+"|  ");
        }else{
            dest << pre << "\\";
            (*iter)->print(dest,pre+"   ");
        }

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
        if( (*iter)!=stages.back() ){
            dest << "|";
            (*iter)->print(dest,"|  ");
        }else{
            dest << "\\";
            (*iter)->print(dest,"   ");
        }
    }
}
