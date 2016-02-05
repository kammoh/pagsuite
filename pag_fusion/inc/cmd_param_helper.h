#ifndef CMD_PARAM_HELPER_H
#define CMD_PARAM_HELPER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <typeinfo>
#include <iostream>
#include <map>
using namespace std;

class cmd_param_helper{
    struct cmd_param_helper_entry
    {
        string token;
        string help;
        bool sp_or_eq;
        void* data;
        const char* type_id;
        bool is_set;
    };

public:
    string name;
    string version;
    string description;
    cmd_param_helper();
    ~cmd_param_helper();

    template<typename T>
    void add_item(string name,string token,string help,bool space_or_equal=true)
    {
        if((int)token.size()>max_token_size) max_token_size = token.size();

        cmd_param_helper_entry* new_entry = new cmd_param_helper_entry;
        new_entry->help = help;
        new_entry->token = token;
        new_entry->sp_or_eq = space_or_equal;
        new_entry->type_id = typeid(T).name();
        new_entry->data = new T;
        new_entry->is_set = false;
        //entries.insert( make_pair<string,cmd_param_helper_entry*>(name,new_entry) );
        entries.insert( make_pair(name,new_entry) );
    }

    template<typename T>
    bool get_item(string name,T* data)
    {
        cmd_param_helper_entry* t = (*entries.find(name)).second;
        if( typeid(T).name() == t->type_id && t->is_set)
        {
            (*data) = (*(T*)t->data);
            return true;
        }
        return false;
    }

    void read_cmd(int argc,char* argv[]);

    void print_help(int wide);

private:
    map<string,cmd_param_helper_entry*> entries;
    int max_token_size;


    void print_space(int x);

};

#endif // CMD_PARAM_HELPER_H
