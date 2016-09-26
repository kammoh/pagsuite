#ifndef PAG_FUSION_INPUTPARSER_H
#define PAG_FUSION_INPUTPARSER_H

#include <string>
#include "mt_types.h"
#include "copa_scope.h"

class InputParser
{
private:
    std::string input_file_content;
public:
    std::vector< std::string > graphs;
    std::vector<std::vector<std::vector<int64_t> > > start_merging;
    struct fix_node{
        std::vector<std::vector<int64_t> > outputs;
        int stage;
    };
    std::vector< fix_node > fixed_nodes;
    InputParser(const std::string& filename);

    bool parse();
};


class InputParser_Scope : public CoPa_clabana::CoPa_Scope{
public:
    InputParser* parser;

    std::vector<std::vector<int64_t> > cur_merge_node;

    InputParser::fix_node cur_fix_node;

    void onEntry()
    {
    }
    void onEvent(int id);
    void onExit();
};

#endif // PAG_FUSION_INPUTPARSER_H
