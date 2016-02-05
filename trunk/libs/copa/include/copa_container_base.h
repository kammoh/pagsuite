#ifndef CoPa_CONTAINER_BASE_H
#define CoPa_CONTAINER_BASE_H

#include "copa_base.h"

namespace CoPa_clabana{
class CoPa;

class CoPa_ContainerBase : public CoPa_Base{
    friend class CoPa;
    friend class CoPa_Scope;
protected:
    bool scopeOwner;
    bool autoclear;
    bool *isVisited;
    void enter();
    void leave();

    vector< counting_ptr<CoPa_Base*> > children;
    CoPa_ContainerBase();
    ~CoPa_ContainerBase();

    void printChildren(string pre="", int level=0);
    void pushScope(CoPa_Scope *scope_);
    void pushOp(CoPa &op, bool back=true);
    void takeOps(CoPa &op, bool back=true);
};
}
#endif // CoPa_CONTAINER_BASE_H
