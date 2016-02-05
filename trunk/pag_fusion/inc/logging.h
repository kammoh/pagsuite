#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <fstream>
#include <typeinfo>
#include "mt_options.h"

std::ostream& LOG();



class LOG_OP{
public:
    static string last_instr;
    static int last_count;

    static void log(const char *_instr);
    static void clear();
};

#endif // LOGGING_H
