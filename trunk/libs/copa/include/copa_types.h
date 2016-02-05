#ifndef CoPa_TYPES_H
#define CoPa_TYPES_H

#include <string>
#include <sstream>

namespace CoPa_clabana{

enum CoPa_TYPE{
    CoPa_TYPE_REP_ZM,
    CoPa_TYPE_REP_OM,
    CoPa_TYPE_OPT,
    CoPa_TYPE_STR,
    CoPa_TYPE_OR,
    CoPa_TYPE_USR,
    CoPa_TYPE_ATOM,
    CoPa_TYPE_EVENT,
    CoPa_TYPE_FILTER,
    CoPa_TYPE_BETWEEN,
    CoPa_TYPE_PRINT,
    CoPa_TYPE_NONE
};

enum CoPa_ATOM_TYPE{
    PARSE_INT,
    PARSE_UINT,
    PARSE_STR,
    PARSE_CHAR,
    PARSE_NONE
};

struct log_entry{
    int level;
    int rpos;
    std::string msg;
    bool recovered;
};

}

template <typename T>
class counting_ptr{
private:
    void inc(){++(*c);}
    void dec(){--(*c);}
    int* c;
    T* p;
public:
    counting_ptr(){c = new int;*c = 1;p = new T;}
    counting_ptr(T* _p){c = new int;*c = 1;p=_p;}
    counting_ptr(const counting_ptr& c){this->p = c.p;this->c = c.c;inc();}
    ~counting_ptr(){dec();if((*c) == 0){delete c;delete p;}}
    int getCount(){return *c;}
    T& getObj(){return *p;}
    T* getPtr(){return p;}
    void operator =(const counting_ptr& c){this->dec();this->p = c.p;this->c = c.c;this->inc();}
    T& operator *(){return getObj();}
};
#endif // CoPa_TYPES_H
