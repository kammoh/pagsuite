#ifndef PAGLIB_COPA_H
#define PAGLIB_COPA_H

#include "adder_graph.h"

class Paglib_copa
{
public:
    Paglib_copa();
    static adder_graph_t* parse(string text);
};

#endif // PAGLIB_COPA_H
