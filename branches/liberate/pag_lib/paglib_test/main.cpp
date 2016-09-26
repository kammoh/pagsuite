#include <iostream>
#include "adder_graph.h"

using namespace std;

int main(int argc,char *argv[])
{
    string test( argv[1] );
    adder_graph_t graph;
    if ( graph.parse_to_graph(test) )
    {
        graph.check_and_correct(test);
        graph.print_graph();
        graph.drawdot();
        graph.writesyn();
        graph.clear();
    }
    return 0;
}


