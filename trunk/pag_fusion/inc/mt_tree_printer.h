#ifndef MT_TREE_PRINTER_H
#define MT_TREE_PRINTER_H

#include <map>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

//#define PRINT_COMPLETE_TREE

class low_tree{
public:
    low_tree* root;
    double edge_value;
    string label;
    vector<low_tree*> leaves;

    low_tree()
    {
        root = NULL;
    }

    low_tree(string tlabel)
    {
        root = NULL;
        label = tlabel;
        edge_value = -1;
    }
};

class mt_tree_printer
{
private:
    static low_tree* current_leave;

public:
    static low_tree* root_leave;
    mt_tree_printer();
    static void init(string tlabel);
    static void save_to_dot(string filename);
    static void make_rec(low_tree* cur_leave,
                         std::vector<std::vector<low_tree*>*>* stage_vec,
                         std::vector<std::pair<low_tree*,low_tree*> >* connections,
                         int cur_depth=0);
    static bool up();
    static void down_break(string tlabel,double value =-1);
    static void down(string tlabel, double value=-1);
};

#ifdef PRINT_COMPLETE_TREE
    #define TREE_PRINT_UP(); mt_tree_printer::up();
    #define TREE_PRINT_INIT(name); mt_tree_printer::init(name);
    #define TREE_PRINT_DOWN(name,value); mt_tree_printer::down(name,value);
    #define TREE_PRINT_BREAK(name,value); mt_tree_printer::down_break(name,value);
    #define TREE_PRINT_SAVE(file); mt_tree_printer::save_to_dot(file);
#else
    #define TREE_PRINT_UP(); ((void)0);
    #define TREE_PRINT_INIT(name); ((void)0);
    #define TREE_PRINT_DOWN(name,value); ((void)0);
    #define TREE_PRINT_BREAK(name,value); ((void)0);
    #define TREE_PRINT_SAVE(file); ((void)0);
#endif

#endif // MT_TREE_PRINTER_H
