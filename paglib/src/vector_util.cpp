#include "adder_graph.h"
#include <stdlib.h>
#include <math.h>

int computeWordSize(std::vector<std::vector<int64_t> > &output_factor, int wIn)
{
    int max_sum = 0;

    for(unsigned i=0; i < output_factor.size(); ++i)
	{
        int sumOfFactors = 0;
        for(unsigned j=0; j < output_factor[i].size(); ++j)
        {
            if (output_factor[i][j] != DONT_CARE)
            sumOfFactors += abs(output_factor[i][j]);
        }
        if (sumOfFactors > max_sum)
            max_sum = sumOfFactors;
    }
    return ceil(log2(max_sum))+wIn;
}

int computeWordSize(adder_graph_base_node_t* node, int wIn)
{
    int wOut = computeWordSize(node->output_factor,wIn);

    if (is_a<adder_subtractor_node_t>(*node))
    {
        adder_subtractor_node_t* t = (adder_subtractor_node_t*)(node);
        if ( t->input_shifts.front() < 0) wOut += abs(t->input_shifts.front());
    }
    if (is_a<conf_adder_subtractor_node_t>(*node)){
        conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)(node);
        if ( t->input_shifts.front() < 0) wOut += abs(t->input_shifts.front());
    }
    return wOut;
}


