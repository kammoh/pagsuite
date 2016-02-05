#ifndef CoPa_ATOM_H
#define CoPa_ATOM_H

#include <string>
#include "copa_base.h"

namespace CoPa_clabana{
class CoPa_Atom : public CoPa_Base{
private:
    CoPa_ATOM_TYPE atom_type;
    int group_id;
    string delimiters;

    bool parse(string &pstr,int level);
public:
    CoPa_Atom(CoPa_ATOM_TYPE t, int group);
    CoPa_Atom(CoPa_ATOM_TYPE t, int group,string _delimiters);
    ~CoPa_Atom();

    void print(string pre="",int level=0);
};
}
#endif // CoPa_ATOM_H
