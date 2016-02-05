#include "logging.h"
using namespace std;

extern cmd_options global_options;
extern ofstream LOG_FILE;

string LOG_OP::last_instr = "";
int LOG_OP::last_count = 0;

ostream& LOG()
{

    LOG_OP::clear();

    if(global_options.printing_options.file) return LOG_FILE;
    else return cout;
}

void LOG_OP::clear()
{
    if(LOG_OP::last_count>1)
        cout << " x" << LOG_OP::last_count << endl;
    else if(LOG_OP::last_count==1)
        cout << endl;

    LOG_OP::last_instr="";
    LOG_OP::last_count=0;
}

void LOG_OP::log(const char *_instr)
{
     if(global_options.printing_options.operations)
     {
        string instr(_instr);
        if(instr == LOG_OP::last_instr)
        {
            LOG_OP::last_count++;
        }
        else
        {
            if(LOG_OP::last_count>1)
                cout << " x" << LOG_OP::last_count+1 << endl;
            else if(LOG_OP::last_count==1)
                cout << endl;

            LOG_OP::last_count = 1;
            LOG_OP::last_instr = instr;
            cout << instr;
        }


     }
}
