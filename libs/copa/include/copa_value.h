#ifndef CoPa_VALUE_H
#define CoPa_VALUE_H

#include <stdlib.h>
#include <stdexcept>
#include "copa_types.h"

namespace CoPa_clabana{
    class CoPa_Value{
    private:
        CoPa_ATOM_TYPE type;

    public:
        int group_id;
        std::string text;
        CoPa_Value(CoPa_ATOM_TYPE type_);

        bool isInt();
        bool isChar();
        bool isUInt();
        bool isString();
        int toInt();
        long toLong();
        long long toLLong();
        unsigned int toUInt();
        char toChar();
        std::string toString();
    };
}
#endif // CoPa_VALUE_H
