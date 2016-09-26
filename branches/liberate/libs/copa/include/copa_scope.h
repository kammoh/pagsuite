#ifndef CoPa_SCOPE_H
#define CoPa_SCOPE_H

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "copa_value.h"

namespace CoPa_clabana{

class CoPa_Scope{
    friend class CoPa_Base;
    friend class CoPa_UserRule;
    friend class CoPa_Atom;
    friend class CoPa_StringConstant;
    friend class CoPa_ContainerBase;
    friend class CoPa_Or;
    friend class CoPa_Repeat;
    friend class CoPa_Event;
    friend class CoPa_Filter;
    friend class CoPa_Between;
    friend class CoPa_Print;
private:
    bool autoclear;
    std::vector<log_entry> log;

    CoPa_Scope* parent;
    std::map<int,std::vector<CoPa_Value> > valueGroups;
    void add_value(int group_id, CoPa_Value val);
    void clear();

public:
    CoPa_Scope();

    std::vector<CoPa_Value> getValueGroup(int group_id);
    CoPa_Value getValueSingle(int group_id);
    void clearValueGroup(int group_id);
    bool hasValue(int group_id);

    template<typename T>
    T* getParentScope(){
        if(parent!=NULL)
            return (T*)parent;
        else
            throw std::runtime_error("[CoPa] Error: parent scope not set");
    }

    virtual void onEntry(){};
    virtual void onEvent(__attribute__((unused)) int id){};
    virtual void onExit(){};
    virtual ~CoPa_Scope(){};
};

}

#endif // CoPa_SCOPE_H
