#include <iostream>
#include <string>
#include "pag_muxilp.h"

using namespace std;


int main(int argc,const char* argv[])
{
    string instance_name;
    int seed = 0;
    bool quiet = true;

    if(argc < 2) {
        cout << "please provide at least an input string in the form"<<endl;
        cout << "{{'type',[output],stage,[pred_output],shift,pred_stage,...next_pred},{other_node}}"<< endl;
        return 0;
    }


    for(int i=2;i<argc;i++)
    {
        string current(argv[i]);
        if(current.find("--instance=") != string::npos)
        {
            instance_name = current.substr(11);
            continue;
        }
        if(current.find("--seed=") != string::npos)
        {
            seed = atoi(current.substr(7).c_str());
            continue;
        }
        if(current.find("--loud") != string::npos)
        {
            quiet = false;
            continue;
        }
    }
    srand (seed);

    string graphstring(argv[1]);
    pag_muxilp mov_mux_object(graphstring,instance_name,quiet);

    int global_best = INT_MAX;
    global_best = mov_mux_object.getMuxIn();
    // cout << "Cost: " << global_best << " with " << mov_mux_object.getMuxIn() <<" 2:1 Multiplexers before optimization" << endl;
    cout << mov_mux_object.getMuxIn() <<  ";" << mov_mux_object.getCost(COST_MODEL) << ";";
    mov_mux_object.addNonMux();
    mov_mux_object.source_graph->drawdot("_after_mux.dot",quiet);
    mov_mux_object.source_graph->writesyn("deb_after_mux_insert.txt",quiet);
    mov_mux_object.fill_successors_map();
    mov_mux_object.generateOptimalMUXDistribution();
    mov_mux_object.applySolution();
    mov_mux_object.source_graph->drawdot("_after_mux_mov.dot",quiet);

    //Check the resuling solution

    mov_mux_object.removeNonMux();
    mov_mux_object.source_graph->writesyn("deb_after_move.txt",quiet);
    mov_mux_object.source_graph->drawdot(instance_name+"final.dot",quiet);
    /*
    std::ifstream ti("deb_after_move.txt");
    std::stringstream old_grap;
    old_grap << ti.rdbuf();
    mov_mux_object.source_graph->clear();
    if(mov_mux_object.source_graph->parse_to_graph(old_grap.str()))
    {
        mov_mux_object.source_graph->check_and_correct(old_grap.str());
    }
    else throw runtime_error("parse failed");
    */

    //cout << "Cost: " << global_best << " with " << mov_mux_object.getMuxIn() <<" 2:1 Multiplexers after optimization" << endl;
    cout << mov_mux_object.getMuxIn() <<  ";" << mov_mux_object.getCost(COST_MODEL) << ";" << instance_name;
    if (global_best < mov_mux_object.getMuxIn())
        cout << "; <--------";

    cout << endl;
    return 0;
}
