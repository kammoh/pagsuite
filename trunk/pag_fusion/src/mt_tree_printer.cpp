#include "mt_tree_printer.h"

low_tree* mt_tree_printer::current_leave = NULL;
low_tree* mt_tree_printer::root_leave = NULL;

mt_tree_printer::mt_tree_printer()
{
}

void mt_tree_printer::init(string tlabel)
{
    low_tree* leave = new low_tree(tlabel);
    current_leave = leave;
    root_leave = leave;
}

void mt_tree_printer::down(string tlabel,double value)
{
    low_tree* leave = new low_tree(tlabel);
    leave->edge_value = value;
    leave->root = current_leave;
    current_leave->leaves.push_back(leave);
    current_leave = leave;
}

void mt_tree_printer::down_break(string tlabel, double value)
{
    low_tree* leave = new low_tree();
    leave->edge_value = value;
    leave->root = current_leave;
    leave->label = tlabel;
    current_leave->leaves.push_back(leave);
}

bool mt_tree_printer::up()
{
    if(current_leave->root != NULL){
        current_leave = current_leave->root;
        return true;
    }
    else return false;
}

void mt_tree_printer::make_rec(low_tree* cur_leave,
                               std::vector<std::vector<low_tree*>*>* stage_vec,
                               std::vector<std::pair<low_tree*,low_tree*> >* connections,
                               int cur_depth)
{
    if(stage_vec->size() <= (unsigned int)cur_depth) stage_vec->push_back( new std::vector<low_tree*>() );

    stage_vec->at(cur_depth)->push_back(cur_leave);
    for(unsigned int i=0, i_end = cur_leave->leaves.size();i<i_end;++i)
    {
        connections->push_back( make_pair(cur_leave,cur_leave->leaves[i]) );
        make_rec( cur_leave->leaves[i],stage_vec,connections,cur_depth+1);
    }
}

void mt_tree_printer::save_to_dot(string filename)
{
    std::vector<std::vector<low_tree*>*> stage_vec;
    std::vector<std::pair<low_tree*,low_tree*> > connections;
    make_rec(root_leave,&stage_vec,&connections);

    stringstream out;

    out<<"digraph TREE{"<<endl;
    for(unsigned int st=0, st_end = stage_vec.size();st<st_end;++st )
    {
        out<<"{"<<endl;
        for(std::vector<low_tree*>::iterator iter_in = stage_vec[st]->begin(), iter_in_end = stage_vec[st]->end();
            iter_in != iter_in_end;
            ++iter_in)
        {
            if( (*iter_in)->label == "new") out<<"node_"<<*iter_in<<" [style=filled,fillcolor=green,label=\""<<(*iter_in)->label<<"\"];"<<endl;
            else if( (*iter_in)->label == "ign" || (*iter_in)->label.find("cut")!=string::npos)  out<<"node_"<<*iter_in<<" [style=filled,fillcolor=red,label=\""<<(*iter_in)->label<<"\"];"<<endl;
            else if( (*iter_in)->label.find("stage")!=string::npos )  out<<"node_"<<*iter_in<<" [style=filled,fillcolor=yellow,label=\""<<(*iter_in)->label<<"\"];"<<endl;
            else out<<"node_"<<*iter_in<<" [label=\""<<(*iter_in)->label<<"\"];"<<endl;
        }
        out<<"}\n";
    }
    out<<endl<<endl;
    for(std::vector<std::pair<low_tree*,low_tree*> >::iterator iter = connections.begin(), iter_end = connections.end();
        iter!= iter_end;
        ++iter)
    {
        out<<"node_"<<(*iter).first<<" -> node_"<<(*iter).second;
        if( (*iter).second->edge_value != -1 )
        {
            out<<"[label=\"" << (*iter).second->edge_value << "\",fontcolor=\"";
            if ( (*iter).second->label == "cut" || (*iter).second->label == "ign"|| (*iter).second->label == "tcut") out << "red";
            else if ( (*iter).second->label == "new" ) out << "darkgreen";
            else if ( (*iter).second->label == "next stage" || (*iter).second->label == "last stage") out << "blue";
            else out << "black";
            out << "\"];";
        }

        out << endl;
    }

    out<<"}"<<endl;

    ofstream outfile;
    outfile.open(filename.c_str());
    if(outfile.is_open())
    {
        outfile << out.str();
        outfile.close();
    }
}
