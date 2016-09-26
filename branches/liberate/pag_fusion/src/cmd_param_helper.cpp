#include "cmd_param_helper.h"

cmd_param_helper::cmd_param_helper()
{
    max_token_size=0;
}

cmd_param_helper::~cmd_param_helper()
{
    for(map<string,cmd_param_helper_entry*>::iterator iter = entries.begin(), iter_end = entries.end();iter!=iter_end;++iter)
    {
        if( strcmp((*iter).second->type_id ,typeid(string).name())==0 )
        {
            delete  ((string*)(*iter).second->data);
        }
        else if( strcmp((*iter).second->type_id ,typeid(int).name())==0 )
        {
            delete  ((int*)(*iter).second->data);
        }
        else if( strcmp((*iter).second->type_id ,typeid(float).name())==0 )
        {
            delete  ((float*)(*iter).second->data);
        }
        else if( strcmp((*iter).second->type_id ,typeid(double).name())==0 )
        {
            delete  ((double*)(*iter).second->data);
        }
        else if( strcmp((*iter).second->type_id ,typeid(bool).name())==0 )
        {
            delete  ((bool*)(*iter).second->data);
        }

        delete (*iter).second;
    }
    entries.clear();
}

void cmd_param_helper::print_space(int x)
{
    for(int i=0;i<x;++i) cout<<" ";
}

void cmd_param_helper::read_cmd(int argc,char* argv[])
{
    for(int i=1;i<argc;++i)
    {
        string current(argv[i]);
        for(map<string,cmd_param_helper_entry*>::iterator iter = entries.begin(), iter_end = entries.end();iter!=iter_end;++iter)
        {
            if( current.find( (*iter).second->token )==0 )
            {
                cmd_param_helper_entry* t = (*iter).second;
                t->is_set = true;
                if( strcmp(t->type_id ,typeid(bool).name())==0 )
                {
                    (*(bool*)t->data) = true;
                }
                else
                {
                    string value;
                    if(t->sp_or_eq) value = string(argv[++i]);
                    else value = current.substr( current.find("=")+1 );

                    if( strcmp(t->type_id ,typeid(string).name())==0 )
                    {
                        (*(string*)t->data) = value;
                    }
                    else if( strcmp(t->type_id ,typeid(int).name())==0 )
                    {
                        (*(int*)t->data) = atoi( value.c_str() );
                    }
                    else if( strcmp(t->type_id ,typeid(float).name())==0 )
                    {
                        (*(float*)t->data) = atof( value.c_str() );
                    }
                    else if( strcmp(t->type_id ,typeid(double).name())==0 )
                    {
                        (*(double*)t->data) = atof( value.c_str() );
                    }
                }
                break;
            }
        }
    }

}

void cmd_param_helper::print_help(int wide)
{
    unsigned int help_begin = max_token_size + 20;
    unsigned int help_length = wide - help_begin;

    if(name.size()>0) cout<< name << endl;
    if(description.size()>0) cout<< description << endl;
    if(version.size()>0) cout<<"version: " << version << endl << endl;
    cout<<"COMMAND LINE OPTIONS"<<endl<<endl;

    for(map<string,cmd_param_helper_entry*>::iterator iter = entries.begin(), iter_end = entries.end();iter!=iter_end;++iter)
    {
        cmd_param_helper_entry* t = (*iter).second;

        print_space(4);
        int datatype_length=0;
        cout<<t->token;
        if( strcmp(t->type_id ,typeid(bool).name())!=0 )
        {
            if(t->sp_or_eq) cout<<" ";
            else cout<<"=";

            cout<<"<";
            if( strcmp(t->type_id ,typeid(string).name())==0 ){ cout<<"string>";datatype_length=9;}
            else if( strcmp(t->type_id ,typeid(int).name())==0 ){ cout<<"int>";datatype_length=6;}
            else if( strcmp(t->type_id ,typeid(float).name())==0 ){ cout<<"float>";datatype_length=8;}
            else if( strcmp(t->type_id ,typeid(double).name())==0 ){ cout<<"double>";datatype_length=9;}
        }
        print_space(help_begin-(t->token.size()+4+datatype_length));
        string t_help = t->help;
        unsigned int pos=0;
        while(pos+help_length < t_help.size() || t_help.find("\n")!=string::npos)
        {
            pos = t_help.find_last_of(" ",pos+help_length);
            if( t_help.find_first_of("\n") < pos ) pos = t_help.find_first_of("\n");
            cout << t_help.substr(0,pos)<<endl;
            t_help = t_help.substr(++pos);
            print_space(help_begin);
        }
        cout<<t_help<<endl<<endl;
    }
}
