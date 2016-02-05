#ifndef CoPa_STR_H
#define CoPa_STR_H

#include "copa_base.h"

namespace CoPa_clabana{
class CoPa_StringConstant : public CoPa_Base{
private:
    string str_val;
    bool parse(string &pstr,int level=0);
public:
    CoPa_StringConstant(const string &str);
    ~CoPa_StringConstant();

    void print(string pre="",int level=0);
};
}

#endif // CoPa_STR_H
