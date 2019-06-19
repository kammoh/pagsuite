#include "pagsuite/adder_graph.h"
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_GMPXX
#include <gmp.h>
#include <gmpxx.h>
#endif

namespace PAGSuite
{
    int computeWordSize(std::vector<std::vector<int64_t> > &output_factor, int wIn)
    {
#ifdef HAVE_GMPXX
        mpz_class max_sum{0};
#else
        int64_t max_sum = 0;
#endif

        for (unsigned i = 0; i < output_factor.size(); ++i)
        {
#ifdef HAVE_GMPXX
            mpz_class sumOfFactors{0};
#else
            int64_t sumOfFactors = 0;
#endif
            for (unsigned j = 0; j < output_factor[i].size(); ++j)
            {
                if (output_factor[i][j] != DONT_CARE)
                {
#ifdef HAVE_GMPXX
                    mpz_class mp_output_factor(std::to_string(abs(output_factor[i][j])).c_str());
                    sumOfFactors += mp_output_factor;
#else
                    sumOfFactors += output_factor[i][j];
#endif
                }
            }
            if (sumOfFactors > max_sum)
                max_sum = sumOfFactors;
        }
		if (abs(max_sum) <= 1) {
			return wIn;
		}
		max_sum -= 1;
        return mpz_sizeinbase(max_sum.get_mpz_t(), 2) + wIn;
    }

    int computeWordSize(adder_graph_base_node_t *node, int wIn)
    {
        int wOut = computeWordSize(node->output_factor, wIn);

        if (is_a<adder_subtractor_node_t>(*node))
        {
            adder_subtractor_node_t *t = (adder_subtractor_node_t *) (node);
            if (t->input_shifts.front() < 0) wOut += abs(t->input_shifts.front());
        }
        if (is_a<conf_adder_subtractor_node_t>(*node))
        {
            conf_adder_subtractor_node_t *t = (conf_adder_subtractor_node_t *) (node);
            if (t->input_shifts.front() < 0) wOut += abs(t->input_shifts.front());
        }
        return wOut;
    }

}
