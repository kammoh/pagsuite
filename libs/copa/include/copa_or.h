#ifndef CoPa_OR_H
#define CoPa_OR_H

#include "copa_container_base.h"

namespace CoPa_clabana{
class CoPa_Or : public CoPa_ContainerBase{
private:
    bool parse(string &pstr,int level);
public:
    CoPa_Or();
    ~CoPa_Or();
    void print(string pre="",int level=0);
};
}
#endif // CoPa_OR_H
