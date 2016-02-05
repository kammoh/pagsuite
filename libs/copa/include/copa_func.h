#ifndef INLINEPARSE_FUNC_H
#define INLINEPARSE_FUNC_H

#include <string>

namespace CoPa_clabana{
    class CoPa;

    CoPa repeat(CoPa c,const char rep_type='*');
    CoPa between(const std::string& lhs, CoPa c, const std::string& rhs);

    CoPa event (int id);
    CoPa ignore(const char* filter);
    CoPa print();
}

CoPa_clabana::CoPa operator+(CoPa_clabana::CoPa left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator+(const char* left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator+(CoPa_clabana::CoPa left,const char* right);
CoPa_clabana::CoPa operator+(const std::string& left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator+(CoPa_clabana::CoPa left,const std::string& right);

CoPa_clabana::CoPa operator|(CoPa_clabana::CoPa left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator|(const char* left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator|(CoPa_clabana::CoPa left,const char* right);
CoPa_clabana::CoPa operator|(const std::string& left,CoPa_clabana::CoPa right);
CoPa_clabana::CoPa operator|(CoPa_clabana::CoPa left,const std::string& right);

#endif // INLINEPARSE_FUNC_H
