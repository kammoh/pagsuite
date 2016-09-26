#ifndef CoPa_USR_H
#define CoPa_USR_H

#include "copa_container_base.h"

namespace CoPa_clabana{
class CoPa_UserRule : public CoPa_ContainerBase{
    friend class CoPa_Scope;
private:
    bool parse(string &pstr,int level=0);
public:
    CoPa_UserRule();
    ~CoPa_UserRule();
    void print(string pre="",int level=0);
    void setScope(CoPa_Scope* ext,bool clear);
};
}
#endif // CoPa_USR_H
