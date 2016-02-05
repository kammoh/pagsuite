#ifndef MT_COST_FUNCTIONS_H
#define MT_COST_FUNCTIONS_H

#include "mt_decision_node.h"

#define COST_MUX 28
#define COST_ADDSUB 98
#define COST_ADD 67
#define COST_SUB 75
#define COST_TERNARY_XCONF 28
#define COST_ADDSUB_TERNARY 98
#define COST_ADD_TERNARY 67
#define COST_SUB_TERNARY 75
#define COST_REG 53

class mt_decision_node;

namespace cost_function {
    float costmodel_std(mt_decision_node *output);
    float costmodel_lowlevel(mt_decision_node *output);
    float costmodel_primitive(mt_decision_node *output);
    float costmodel_lowlevel_primitive(mt_decision_node *output);
}


#endif // MT_COST_FUNCTIONS_H
