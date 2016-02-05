#include "parsers/inputparser.h"
#include "copa.h"
#include <fstream>

using namespace CoPa_clabana;

InputParser::InputParser(const string &filename){
    ifstream file(filename.c_str());
    std::string line;
    while( getline(file,line) ){
        if( line.find('#')!=string::npos ){
            input_file_content += line.substr(0,line.find('#'));
        }else{
            input_file_content += line;
        }
    }
    file.close();
}

bool InputParser::parse(){
    CoPa graphs = between("<graph>",(CoPa(PARSE_STR,0)),"</graph>") + event(0);

    CoPa merge_single = CoPa(PARSE_INT,1).repeat_separated(",",'*') + event(1);
    CoPa merge_cfg = merge_single.repeat_separated(";",'*');
    CoPa merges = merge_cfg.between("<merge>","</merge>") + event(5);

    CoPa fixed_single = (("DONT_CARE" + event(4)) | (CoPa(PARSE_INT,2).repeat_separated(",",'*') + event(2)));
    CoPa fixed_cfg = fixed_single.repeat_separated(";",'*');
    CoPa fixed_node = fixed_cfg.between("[" ,"]") + "," + CoPa(PARSE_UINT,3) + event(3);
    CoPa fixed_nodes = fixed_node.between("{","}").repeat_separated(",",'*');
    CoPa fixes = fixed_nodes.between( "<fixed>","</fixed>");

    CoPa toplevel = ignore(" \t\r\n") + repeat(graphs,'+') + repeat(merges,'+') + (fixes|"");
    InputParser_Scope *scope = new InputParser_Scope;
    scope->parser = this;
    toplevel.setScope( scope );

    //toplevel.print();

    return toplevel.parse( input_file_content );
}


void InputParser_Scope::onEvent(int id)
{
    switch(id){
    case 0: //graphs finished
    {
        CoPa_Value val = getValueSingle(0);
        parser->graphs.push_back(val.toString());
        clearValueGroup(0);
    }
        break;
    case 1: //merge_single finished
    {
        std::vector<CoPa_Value> val_gr = getValueGroup(1);
        std::vector<int64_t> outputs;
        for(int i=0;i<val_gr.size();++i){
            outputs.push_back( val_gr.at(i).toInt() );
        }
        cur_merge_node.push_back(outputs);
        clearValueGroup(1);
    }
        break;
    case 2: //fixed_single finished
    {
        std::vector<CoPa_Value> val_gr = getValueGroup(2);
        std::vector<int64_t> outputs;
        for(int i=0;i<val_gr.size();++i){
            outputs.push_back( val_gr.at(i).toInt() );
        }
        cur_fix_node.outputs.push_back(outputs);
        clearValueGroup(2);
    }
        break;
    case 3: //merge_single finished
    {
        cur_fix_node.stage = getValueSingle(3).toInt();

        parser->fixed_nodes.push_back( cur_fix_node );
        cur_fix_node.stage = 0;
        cur_fix_node.outputs.clear();
        clearValueGroup(3);
    }
        break;

    case 4: //merge_single finished
    {
        cur_fix_node.outputs.push_back(std::vector<int64_t>());
    }
        break;
    case 5: //merge_single finished
    {
        parser->start_merging.push_back( cur_merge_node );
    }
        break;
    }
}

void InputParser_Scope::onExit(){
}
