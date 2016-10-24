#include "paglib_copa.h"
#include "copa/copa.h"
#include "paglib_scopes.h"

using namespace CoPa_clabana;

Paglib_copa::Paglib_copa()
{
}

adder_graph_t* Paglib_copa::parse(string text)
{
    CoPa outfact_single = ( "NaN" | CoPa(PARSE_INT,0) );
    outfact_single.setScope(new outfact_single_scope);
    CoPa outfact_cfg =  outfact_single + repeat( "," + outfact_single,'*');
    CoPa outfact =      ( ("[" + outfact_cfg + repeat(";" + event(0) + outfact_cfg,'*') + "]")
                        | outfact_single
                        );
    CoPa shift_single = ( "NaN" | CoPa(PARSE_INT,1) );
    shift_single.setScope(new shift_single_scope);
    CoPa shifts =       ( ("[" + shift_single + repeat(";" + shift_single,'*') + "]")
                        | shift_single
                        );


    CoPa node_param = splitBy(",",(CoPa(PARSE_STR,5,"=") + "=" + CoPa(PARSE_STR,5,",") + event(10) )).between("{", "}");

    CoPa node_core =    outfact + "," + CoPa(PARSE_UINT,2) ;
    node_core_scope *node_core_scp = new node_core_scope;
    node_core.setScope(node_core_scp);
    CoPa node_shft =    node_core + "," + shifts ;
    CoPa node_add =     "{\'A\'," + event(1) + node_core + "," + node_shft + repeat( "," + node_shft ,'+') + "}" + ((":" + node_param) |"");
    CoPa node_reg =     "{\'R\'," + event(2) + node_core + "," + node_core + (( ","+shifts)|"") + "}" + ((":" + node_param) |"");
    CoPa node_mux =     "{\'M\'," + event(3) + node_core + repeat( "," + node_shft ,'+') + "}" + ((":" + node_param) |"");
    CoPa node_out =     "{\'O\'," + event(4) + node_core + "," + node_core+ (( ","+shifts)|"") + "}" + ((":" + node_param) |"");

    CoPa node =         ( node_add | node_reg | node_mux | node_out );
    node.setScope( new node_scope);

    CoPa adder_tree =  ignore(" \n\t\r") + "{" + node + repeat( "," + node,'*') + "}"  + ((":" + node_param) |"");

    adder_tree_scope *master = new adder_tree_scope;
    adder_tree.setScope(master);

    if ( adder_tree.parse(text) ){
        master->graph->check_and_correct();
        return master->graph;
    }
    else{
        if(master->graph != NULL)
            delete master->graph;
        return NULL;
    }
}
