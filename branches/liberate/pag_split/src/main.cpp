#include <iostream>
#include "pag_splitter.h"

using namespace std;

int main(int argc,const char* argv[])
{
    if(argc < 2) return 0;

    bool generate_input_file=false;
    bool detox_graph = false;
    string instance_name;
    string graphstring(argv[1]);

    vector<vector<vector<int64_t> > > groups;
    for(int i=2;i<argc;i++)
    {
        string current(argv[i]);
        if(current=="--pag_fusion_input")
        {
            generate_input_file = true;
            continue;
        }
        else if(current.find("--instance=") != string::npos)
        {
            instance_name = current.substr(11) + "_";
            continue;
        }
        else if(current=="--graph_detox")
        {
            detox_graph = true;
            continue;
        }
        else{
            vector<vector<int64_t> > group;
            vector<int64_t> inner_group;
            string cur_num;
            while(current.size()>0){
                  if( current[0]==' ' ){
                      inner_group.push_back(atoi(cur_num.c_str()));
                      group.push_back(inner_group);
                      inner_group.clear();
                      groups.push_back(group);
                      group.clear();
                      current.erase(current.begin());
                  }
                  else if( current[0]==';' ){
                      inner_group.push_back(atoi(cur_num.c_str()));
                      group.push_back(inner_group);
                      inner_group.clear();
                      current.erase(current.begin());
                  }
                  else if( current[0]==',' ){
                      inner_group.push_back(atoi(cur_num.c_str()));
                      current.erase(current.begin());
                  }
                  else{
                      size_t pos = current.find_first_of(" ,;");
                      if( pos != string::npos ){
                      cur_num = current.substr(0,pos);
                      current = current.substr(pos);
                      }
                      else{
                          cur_num = current;
                          current = "";
                      }
                  }
            }
            inner_group.push_back(atoi(cur_num.c_str()));
            group.push_back(inner_group);
            groups.push_back(group);
        }
    }

    pag_splitter splitter(graphstring,instance_name);


    if( detox_graph ){
        cout << "detoxing graph..." << endl;
        splitter.detox_graph();
        return 0;
    }

    cout << "inflating graph..." << endl;
    splitter.inflate_graph(groups);

    cout << "adding groups..." << endl;
    for(unsigned int conf=0;conf<groups.front().size();conf++)
    {
        vector<vector<int64_t > > new_group;
        for(unsigned int graph=0;graph<groups.size();graph++)
        {
            new_group.push_back( groups[graph][conf] );
        }
        splitter.add_group(new_group);
    }

    splitter.add_no_group();

    cout << "splitting groups..." << endl;
    splitter.split();

    //cout << "printing groups..." << endl;
    splitter.print_all_splitted(instance_name);

    cout << "generating input file..." << endl;
    if(generate_input_file)
        splitter.generate_input_file(instance_name,groups);

    return 0;
}
