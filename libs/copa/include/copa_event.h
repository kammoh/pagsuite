#ifndef CoPa_EVENT_H
#define CoPa_EVENT_H

#include "copa_base.h"

namespace CoPa_clabana{
class CoPa_Event : public CoPa_Base{
private:
    int event_id;
    bool parse(string &pstr,int level);
public:
    CoPa_Event(int id);
    ~CoPa_Event();

    void print(string pre="",int level=0);
};
}
#endif // CoPa_EVENT_H
