#ifndef COPA_H
#define COPA_H

#include "copa_func.h"
#include "copa_types.h"

#include "copa_base.h"
#include "copa_container_base.h"

#include "copa_atom.h"
#include "copa_event.h"
#include "copa_filter.h"
#include "copa_or.h"
#include "copa_rep.h"
#include "copa_str.h"
#include "copa_usr.h"
#include "copa_between.h"
#include "copa_print.h"

namespace CoPa_clabana{
class CoPa{
    friend class CoPa_ContainerBase;
private:
    CoPa_TYPE type;
    counting_ptr<CoPa_Base*> base;

    void reset_base(CoPa_Base* ptr);
    void pushOp(CoPa &op, bool back=true);
    void takeOps(CoPa &op, bool back=true);

public:
    CoPa();
    CoPa(const string &str);
    CoPa(CoPa_ATOM_TYPE t,int group_id=-1);
    CoPa(CoPa_ATOM_TYPE t,int group_id,string delimiters);
    CoPa(const CoPa &c);
    ~CoPa();

    CoPa_Base *&getBase();
    CoPa_Scope* getScope();

    void setScope(CoPa_Scope* scope,bool clear=true);

    bool parse(string text);
    void print(string pre="", int level=0);

    CoPa &operator =(CoPa right);

    CoPa between(const string &lhs, const string &rhs);

    friend CoPa (::operator+)(CoPa left,CoPa right);
    friend CoPa (::operator+)(const char* left,CoPa right);
    friend CoPa (::operator+)(CoPa left,const char* right);
    friend CoPa (::operator+)(const std::string& left,CoPa right);
    friend CoPa (::operator+)(CoPa left,const std::string& right);

    friend CoPa (::operator|)(CoPa left,CoPa right);
    friend CoPa (::operator|)(const char* left,CoPa right);
    friend CoPa (::operator|)(CoPa left,const char* right);
    friend CoPa (::operator|)(const std::string& left,CoPa right);
    friend CoPa (::operator|)(CoPa left,const std::string& right);

    friend CoPa repeat(CoPa c,const char rep_type);
    friend CoPa event (int id);
    friend CoPa print ();
    friend CoPa ignore(const char* filter);
    friend CoPa between(const std::string& lhs, CoPa c, const std::string& rhs);
    friend CoPa splitBy(const std::string& lhs, CoPa c);
};
}
#endif // COPA_H
