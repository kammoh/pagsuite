#include "logging.h"
#include "mt_debug.h"
#include "mt_options.h"
#include "mt_merger.h"
#include "cmd_param_helper.h"
#include "parsers/inputparser.h"


#include <iostream>
#include <signal.h>

typedef long long int_t;

cmd_options global_options;
ofstream LOG_FILE;

bool EXIT_REQUEST=false;
merger* pMerger=NULL;

void signal_handler(int signum)
{
    (void)signum;
    if( !EXIT_REQUEST ){
        EXIT_REQUEST = true;
    }
}

void get_options(int argc, char* argv[]);
void print_help();

int main(int argc, char* argv[])
{
#ifndef RELEASE_BUILD
    cerr << "CAUTION: THIS IS NO RELEASE BUILD!" << endl << "Please consider using a release build for performance reasons." << endl << endl;
#endif

	int exit_code = 0;
    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);

    get_options(argc,argv);
    if(global_options.printing_options.file)
    {
        stringstream filename;
        if(global_options.outfileprefix.length()>1)filename<<global_options.outfileprefix;
        filename<<"output";
        if(global_options.outfilesuffix.length()>1)filename<<global_options.outfilesuffix;
        filename<<".txt";
        LOG_FILE.open(filename.str().c_str());
    }

    if(!global_options.start)
    {
        return -1;
    }

    stringstream filename;
    if(global_options.inputfile.length()>0) filename<<global_options.inputfile;
    else filename<<"input.txt";

    merger merging_obj(filename.str());
    pMerger = &merging_obj;
    merging_obj.solve();
    exit_code = merging_obj.exit_code;

    if(global_options.printing_options.file) LOG_FILE.close();
    return exit_code;
}

void get_options(int argc, char *argv[])
{
    cmd_param_helper helper;
    helper.name = "PAG_FUSION";
    helper.description = "commandline tool for merging adder graphs created by RPAG";
    helper.version = "2.2.0";

    helper.add_item<string>("if","--if","reads input from file 'string'\ndefault: input.txt");
    helper.add_item<string>("ofpre","--ofpre","sets filename prefix for output files");
    helper.add_item<string>("ofsuf","--ofsuf","sets filename suffix for output files");
    helper.add_item<bool>("show_stats","--show_stats","shows statistics at end");
    helper.add_item<bool>("complete","--complete","does a complete search; caution very slow!");
    helper.add_item<bool>("fix_by_value","--fix_by_value","fixes nodes with equal values; caution very fast!");
    helper.add_item<string>("costmodel","--costmodel","sets type of cost computation; combinations allowed\nl - lowcost model (count mux paths)\ns - strong cost model\nz - ignore costs of zero paths\np - primitive cost model");
    helper.add_item<string>("print","--print","sets printing options; combinations allowed\nf - print outputs to file\ng - print converted graphs\np - print placement matrices\nm - print merge matrices\nc - print cost matrices\ns - print construction swaps\no - print operations\nw - print NEW occurence\nx - output to performance file");
    helper.add_item<int>("greedy_depth","--greedy_search_depth","sets the greedy search depth to given value\ndefault: -1");
    helper.add_item<string>("timeout","--timeout","set timout to pag_fusion; allowed units s,m,h");
    helper.add_item<string>("limit","--limit","set memory limit in bytes to pag_fusion; allowed units g,m,k\ndefault: 3 GB");
    helper.add_item<bool>("help","--help","print this help");

    global_options.inputfile = "input.txt";
    global_options.outfileprefix = "";
    global_options.outfilesuffix = "";
    global_options.ternary_mode = false;
    global_options.start = true;
    global_options.stats = false;
    global_options.greedy_depth = -1;
    global_options.timeout = 0;
    global_options.fix_by_value = false;
    global_options.mem_limit = 3.0 * 1024.0 * 1024.0 * 1024.0;

    helper.read_cmd(argc,argv);

    helper.get_item("if",&global_options.inputfile);
    if(helper.get_item("ofpre",&global_options.outfileprefix)) global_options.outfileprefix += (string)"_";
    if(helper.get_item("ofsuf",&global_options.outfilesuffix)) global_options.outfilesuffix = (string)"_" + global_options.outfilesuffix;
    helper.get_item("show_stats",&global_options.stats);
    helper.get_item("complete",&global_options.complete);
    helper.get_item("greedy_depth",&global_options.greedy_depth);
    helper.get_item("fix_by_value",&global_options.fix_by_value);


    string tmp;
    helper.get_item("costmodel",&tmp);
    for(unsigned int i=0, i_end = tmp.size();i<i_end;++i)
    {
        switch(tmp[i])
        {
        case 'l': global_options.costmodel.lowcost = true;break;
        case 's': global_options.costmodel.strongcost = true;break;
        case 'z': global_options.costmodel.ignorezero = true;break;
        case 'p': global_options.costmodel.primitive = true;break;
        }
    }
    tmp = "";
    helper.get_item("print",&tmp);
    for(unsigned int i=0, i_end = tmp.size();i<i_end;++i)
    {
        switch(tmp[i])
        {
        case 'g': global_options.printing_options.graphs = true;break;
        case 'p': global_options.printing_options.placement = true;break;
        case 'm': global_options.printing_options.merge = true;break;
        case 'c': global_options.printing_options.cost = true;break;
        case 's': global_options.printing_options.construction = true;break;
        case 'o': global_options.printing_options.operations = true;break;
        case 'f': global_options.printing_options.file = true;break;
        case 'w': global_options.printing_options.newOcc = true;break;
        case 'x': global_options.printing_options.perfFile = true;break;
        }
    }

    tmp = "";
    helper.get_item("timeout",&tmp);
    if(tmp.size()>0){
	
        if( (*tmp.rbegin()) == 'm' )
        {
            global_options.timeout = (double)atoi(tmp.substr(0,tmp.size()-1).c_str()) * 1000000.0 * 60.0;
        }
        else if( (*tmp.rbegin()) == 'h' )
        {
            global_options.timeout = (double)atoi(tmp.substr(0,tmp.size()-1).c_str()) * 1000000.0 * 3600.0;
        }
        else
        {
            global_options.timeout = (double)atoi(tmp.substr(0,tmp.size()-1).c_str()) * 1000000.0;
        }
	
    }

    helper.get_item("limit",&tmp);
    if(tmp.size()>0){
        if( (*tmp.rbegin()) == 'g' )
        {
            global_options.mem_limit = (float)atof(tmp.substr(0,tmp.size()-1).c_str()) * 1024 * 1024 * 1024;
        }
        else if( (*tmp.rbegin()) == 'm' )
        {
            global_options.mem_limit = (float)atof(tmp.substr(0,tmp.size()-1).c_str()) * 1024 * 1024;
        }
        else if( (*tmp.rbegin()) == 'k' )
        {
            global_options.mem_limit = (float)atof(tmp.substr(0,tmp.size()-1).c_str()) * 1024;
        }
        else
        {
            global_options.mem_limit = (float)atof(tmp.substr(0,tmp.size()-1).c_str());
        }

    }
	

    bool help=false;
    helper.get_item("help",&help);
    if(help || argc < 2){
        helper.print_help(80);
        global_options.start = false;
    }
}
