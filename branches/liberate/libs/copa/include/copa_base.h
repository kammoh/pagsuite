#ifndef INLINEPARSELIB_H
#define INLINEPARSELIB_H

#include <vector>
#include <string>
#include <iostream>
#include <cassert>

#include "copa_scope.h"

using namespace std;

namespace CoPa_clabana{

class CoPa_Base
{
    friend class CoPa;
    friend class CoPa_Scope;
    friend class CoPa_ContainerBase;

protected:
    virtual void pushScope(CoPa_Scope *scope_);

    void makeLogEntry(int level, int rpos, string msg);
    void recoverLogEntry(int level);
    CoPa_Scope* scope;

public:
    CoPa_Base();
    virtual ~CoPa_Base(){}

    virtual bool parse(string &pstr,int level);
    virtual void print(string pre="",int level=0);
    void clear();
};


}
#endif // INLINEPARSELIB_H
