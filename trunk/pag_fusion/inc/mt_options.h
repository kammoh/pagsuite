#ifndef MT_OPTIONS_H
#define MT_OPTIONS_H

#include <string>
using namespace std;

struct cmd_options
{
    bool start;
    bool ternary_mode;
    bool stats;
    bool complete;
    bool fix_by_value;
    int greedy_depth;
    string outfileprefix;
    string outfilesuffix;
    string inputfile;
    float mem_limit;

    double timeout;

    struct costmodel_struc{
        bool lowcost;
        bool strongcost;
        bool ignorezero;
        bool primitive;
    } costmodel;

    struct printing_struc{
        bool operations;
        bool graphs;
        bool placement;
        bool merge;
        bool cost;
        bool construction;
        bool file;
        bool newOcc;
        bool perfFile;
    } printing_options;

    cmd_options()
    {
                 start=true;
                 ternary_mode=false;
                 stats=false;
                 complete=false;
                 outfileprefix="";
                 outfilesuffix="";
                 inputfile="";
                 costmodel.lowcost = false;
                 costmodel.strongcost = false;
                 costmodel.ignorezero = false;
                 printing_options.construction = false;
                 printing_options.cost = false;
                 printing_options.graphs = false;
                 printing_options.merge = false;
                 printing_options.operations = false;
                 printing_options.placement = false;
                 printing_options.newOcc = false;
                 printing_options.perfFile = false;
    }
};


#endif // MT_OPTIONS_H
