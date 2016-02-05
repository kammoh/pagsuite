#ifndef CoPa_REP_H
#define CoPa_REP_H

#include "copa_container_base.h"

namespace CoPa_clabana{
class CoPa_Repeat : public CoPa_ContainerBase{
private:
    bool acceptZero;
    bool parse(string &pstr,int level);
public:
    CoPa_Repeat(bool acceptZero_=true);
    ~CoPa_Repeat();
    void print(string pre="",int level=0);
};
}
#endif // CoPa_REP_H
