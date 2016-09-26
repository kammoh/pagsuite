#ifndef CoPa_FILTER_H
#define CoPa_FILTER_H

#include "copa_base.h"

namespace CoPa_clabana{
class CoPa_Filter : public CoPa_Base{
private:
    string str_val;
    bool parse(string &pstr,int level=0);
public:
    CoPa_Filter(string str);
    ~CoPa_Filter();

    void print(string pre="",int level=0);
};
}
#endif // CoPa_FILTER_H
