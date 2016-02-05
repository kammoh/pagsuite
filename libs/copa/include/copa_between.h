#ifndef CoPa_BETWEEN_H
#define CoPa_BETWEEN_H

#include "copa_container_base.h"

namespace CoPa_clabana{
class CoPa_Between : public CoPa_ContainerBase{
private:
    const std::string left,right;
    bool parse(string &pstr,int level=0);
public:
    CoPa_Between(const std::string& lhs,const std::string& rhs);
    ~CoPa_Between();

    void print(string pre="",int level=0);
};
}

#endif
