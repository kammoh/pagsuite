#ifndef CoPa_PRINT_H
#define CoPa_PRINT_H

#include "copa_base.h"

namespace CoPa_clabana{
class CoPa_Print : public CoPa_Base{
private:
    int event_id;
    bool parse(string &pstr,int level);
public:
    CoPa_Print();
    ~CoPa_Print();

    void print(string pre="",int level=0);
};
}
#endif // CoPa_EVENT_H
