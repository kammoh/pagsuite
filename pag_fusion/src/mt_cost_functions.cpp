#include "logging.h"
#include "mt_options.h"
extern cmd_options global_options;

#include "mt_graph_types.h"
#include "mt_cost_functions.h"

float cost_function::costmodel_std(mt_decision_node *output)
{
    int cfg_count = output->decision_types.size();
    float current_cost = 0.0;
    int no_of_zero,no_of_pm,max_ws,no_of_dc;
    int c_add=0,c_sub=0,c_mix=0;
    //set<int> unique_shifts;
    unsigned int unique_shifts;
    unsigned short signs;
    for(int i=0, i_end = output->noLines;i<i_end;++i)
    {
        signs = 0;
        max_ws = 0;
        no_of_zero = 0;
        no_of_pm = 0;
        no_of_dc = 0;
        unique_shifts = 0;

        for(int j=0;j<cfg_count;++j)
        {
            mt_path* t = output->decision_paths[j][output->paths[j][i]];;
            if( t != NULL )
            {
                if(max_ws < (t->shift-output->neg_shift+t->parent->wordsize))
                    max_ws = t->shift-output->neg_shift+t->parent->wordsize;

                //unique_shifts.insert(t->shift);
                unique_shifts |= (1<<(t->shift-output->neg_shift));

                if(t->is_neg) signs |= 0x2;
                else signs |= 0x1;
            }
            else if( output->decision_types[j] == DECISION_REG || output->decision_types[j] == DECISION_ADD ) no_of_zero++;
            else if( output->decision_types[j] == DECISION_DONT_CARE ) no_of_dc++;
            else
            {
                no_of_pm++;
                signs |= 0x4;
            }
        }
        //int c_us = (int)unique_shifts.size();
        int c_us = __builtin_popcount( unique_shifts );

        int c_z = no_of_zero;
        int dc = no_of_dc;
        int c_zp = ( no_of_zero > 0?1.0:0 );
        int c_usp = ( c_us == 1 && c_zp==0 && no_of_pm==0? 1.0:0 );



        current_cost += float(( c_us - c_usp + c_zp )* (cfg_count-dc-c_z+c_zp-1) * max_ws * COST_MUX) / float(cfg_count-dc-c_z+c_zp);

        if(signs == 1) c_add++;
        else if(signs == 2) c_sub++;
        else if(signs == 3) c_mix++;
    }

    if(c_mix > 0) output->cur_type = 'C';
    else if(c_add > 0 )
    {
        if( c_sub > 0 )
        {
           output->cur_type = 'S';
        }
        else if(c_add == 1) output->cur_type = 'R';
        else
        {
            output->cur_type = 'A';
        }
    }
    else
    {
        // kein sinn
    }
    return current_cost;
}

float cost_function::costmodel_lowlevel(mt_decision_node *output)
{
    int cfg_count = output->decision_types.size();
    float current_cost = 0.0;
    int no_of_zero,no_of_pm,no_of_dc;
    unsigned int unique_shifts;
    //set<int> unique_shifts;
    for(int i=0, i_end = output->noLines;i<i_end;++i)
    {
        no_of_zero = 0;
        no_of_pm = 0;
        no_of_dc = 0;
        unique_shifts = 0;
        //unique_shifts.clear();
        for(int j=0;j<cfg_count;++j)
        {
            mt_path* t = output->decision_paths[j][output->paths[j][i]];
            if( t != NULL )
            {
                //unique_shifts.insert(t->shift);
                unique_shifts |= (1<<(t->shift-output->neg_shift));
            }
            else if( output->decision_types[j] == DECISION_REG && i==0 || output->decision_types[j] == DECISION_ADD ) no_of_zero++;
            //Konrad: ich bin davon noch nicht 100 % ueberzeugt würde ADD weglassen
            else if( output->decision_types[j] == DECISION_DONT_CARE ) no_of_dc++;
            else
            {
                no_of_pm++;
            }
        }
        //int c_us = (int)unique_shifts.size();
        float c_us = (float)__builtin_popcount( unique_shifts );
        float c_z = no_of_zero;
        float dc = no_of_dc;
        float c_zp = ( no_of_zero > 0?1.0:0 );
        float c_usp = ( c_us == 1 && c_zp==0 && no_of_pm==0? 1.0:0 );
        current_cost += ( c_us - c_usp + c_zp )* (cfg_count-dc-c_z+c_zp-1)/(cfg_count-dc-c_z+c_zp);
    }
    return current_cost;
}

float cost_function::costmodel_primitive(mt_decision_node *output)
{
    int cfg_count = output->decision_types.size();
    float current_cost = 0.0;
    int no_of_zero=0,no_of_pm=0,max_ws=0,no_of_dc=0;
    //set< int > unique_shifts;
    unsigned int unique_shifts;
    for(int i=0, i_end = output->noLines;i<i_end;++i)
    {
        for(int j=0;j<cfg_count;++j)
        {
            mt_path* t = output->decision_paths[j][output->paths[j][i]];;
            if( t != NULL )
            {
                if(max_ws < t->shift-output->neg_shift+t->parent->wordsize)
                    max_ws = t->shift-output->neg_shift+t->parent->wordsize;

                unique_shifts |= (1<<(t->shift-output->neg_shift));
            }
            else if( output->decision_types[j] == DECISION_REG || output->decision_types[j] == DECISION_ADD ) no_of_zero++;
            else if( output->decision_types[j] == DECISION_DONT_CARE ) no_of_dc++;
            else
            {
                no_of_pm++;
            }
        }
    }
    int c_us = (int)__builtin_popcount( unique_shifts );

    int c_z = no_of_zero;
    int dc = no_of_dc;
    int c_zp = ( no_of_zero > 0?1.0:0 );
    int c_usp = ( c_us == 1 && c_zp==0 && no_of_pm==0? 1.0:0 );
    cfg_count *= output->noLines;
    current_cost += float(( c_us - c_usp + c_zp )* (cfg_count-dc-c_z+c_zp-1) * max_ws * COST_MUX) / float(cfg_count-dc-c_z+c_zp);
    return current_cost;
}

float cost_function::costmodel_lowlevel_primitive(mt_decision_node *output)
{
    int cfg_count = output->decision_types.size();
    float current_cost = 0.0;
    int no_of_zero,no_of_pm,no_of_dc;
    unsigned int unique_shifts;
    //set<int> unique_shifts;
    for(int i=0, i_end = output->noLines;i<i_end;++i)
    {
        for(int j=0;j<cfg_count;++j)
        {
            mt_path* t = output->decision_paths[j][output->paths[j][i]];;
            if( t != NULL )
            {
                unique_shifts |= (1<<(t->shift-output->neg_shift));
            }
            else if( output->decision_types[j] == DECISION_REG || output->decision_types[j] == DECISION_ADD ) no_of_zero++;
            else if( output->decision_types[j] == DECISION_DONT_CARE ) no_of_dc++;
            else
            {
                no_of_pm++;
            }
        }
        //int c_us = (int)unique_shifts.size();

    }
    float c_us = (float)__builtin_popcount( unique_shifts );
    float c_z = no_of_zero;
    float dc = no_of_dc;
    float c_zp = ( no_of_zero > 0?1.0:0 );
    float c_usp = ( c_us == 1 && c_zp==0 && no_of_pm==0? 1.0:0 );

    current_cost += ( c_us - c_usp + c_zp )* (cfg_count-dc-c_z+c_zp-1)/(cfg_count-dc-c_z+c_zp);
    return current_cost;
}
