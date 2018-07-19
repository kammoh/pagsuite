#include "rpag_matrix_decomp.h"
#include <algorithm>
#include "rpagvm.h"
#include "rpagtvm.h"


int rpag_matrix_decomp::optimize()
{

    std:: cout << " rpag_matrix_decomp::optimize " << std::endl;
    if(operation_type == typ_rpagtvm)
        std:: cout << "opperation type: typ_rpagtvm" << std::endl;
    if(operation_type == typ_rpagvm)
        std:: cout << "opperation type: typ_rpagvm" << std::endl;

    parse_target_set();

    set<vec_t> target_fun_set;
    //set<vec_t> pipeline_set;

    int no_of_pipeline_stages = initialize(target_set,&target_fun_set);

    decomposision_recursion(&target_fun_set);

    return 0;
}

rpag_type rpag_matrix_decomp::get_rpag_type()
{
    if(is_this_a_two_input_system())
        return typ_rpagvm;
    else
        return typ_rpagtvm;
}

bool rpag_matrix_decomp::is_this_a_two_input_system(void)
{
    return !ternary_adders;
}

int rpag_matrix_decomp::make_all_solutions_same_depth(vector<vector<set<vec_t> > > &solutions)
{
    unsigned int max_pipeline_stages=0;
    for(vector<set<vec_t> > s: solutions)
    {
      if(s.size() > max_pipeline_stages)
      {
          max_pipeline_stages = s.size();
      }
    }

    cout << "!!!max_pipeline_stages=" << max_pipeline_stages << endl;

    // Make all solutens the same pipeline depth
    for(unsigned int j=0; j< solutions.size(); ++j)
    {
        //
        if(solutions[j].size() < max_pipeline_stages)
        {
            // find pipeline stage with the lowes nuber of
            unsigned int min_stage = max_pipeline_stages+10;
            int stage = 0;
            set<vec_t> statge_to_duplicte;

            for(unsigned int i=0; i < solutions[j].size(); ++i)
            {
               IF_VERBOSE(6) std::cout << "solution.size() = " << (solutions[j][i]).size() << std::endl;
                if((solutions[j][i]).size() < min_stage)
                {
                    IF_VERBOSE(7) std::cout << "found stage:" << i << std::endl;
                    stage=i;
                    min_stage = solutions[j][i].size();
                }
            }
            statge_to_duplicte=solutions[j][stage];

            //generate the necesseary iterater...
            vector<set<vec_t> >::iterator it = solutions[j].begin();
            for(int i=0; i <= stage;i++){it++;}

            while(solutions[j].size() != max_pipeline_stages)
            {
                it = solutions[j].insert(it,statge_to_duplicte);
            }
        }
    }
    return max_pipeline_stages;
}

void rpag_matrix_decomp::insert_problems_into_pipeline_set_rev(vector<set<vec_t> > &pipeline_set_rev, vector<pair<set<vec_t>, vector<bool> > > &problems)
{
    IF_VERBOSE(9) std::cout << "Insert_problems_into_pipeline_set_rev currently do nothing..." << std::endl;
    set<vec_t> next_pipeline_stage;

    for(pair<set<vec_t>, vector<bool> > p : problems)
    {
        for(vec_t t : p.first)
        {
         next_pipeline_stage.insert(norm(fundamental(t)));
        }
    }

    pipeline_set_rev.push_back(next_pipeline_stage);
}

void rpag_matrix_decomp::split_target_into_2(set<vec_t> target_split_set, vector<bool> partion_helper, vector<pair<set<vec_t>, vector<bool> > > &result)
{
    IF_VERBOSE(6) std::cout << "start with: split_target_into_2" << std::endl;
    bool var= true;
    int count_active_elemts = std::count(partion_helper.begin(),partion_helper.end(),var);
    int elemts_per_split = count_active_elemts/2;

    IF_VERBOSE(6) std::cout << "count_active_elemts = " << count_active_elemts << std::endl;

    pair<set<vec_t>, vector<bool> > new_split_set_1;
    pair<set<vec_t>, vector<bool> > new_split_set_2;

    new_split_set_1.second.clear(); new_split_set_1.first.clear(); new_split_set_1.second.resize(partion_helper.size());
    new_split_set_2.second.clear(); new_split_set_2.first.clear(); new_split_set_2.second.resize(partion_helper.size());

    vec_t new_target_1;
    vec_t new_target_2;

    for(vec_t t : target_split_set)
    {
        new_target_1.clear(); new_target_1.resize(partion_helper.size());
        new_target_2.clear(); new_target_2.resize(partion_helper.size());

        int counter = 0;// conts already attatched elemts
        IF_VERBOSE(6) std::cout << "partion_helper:" << partion_helper << std::endl;
        for(int i=0; i < partion_helper.size(); ++i)
        {
            if(partion_helper[i] == true) // element is part of this problem
            {
                if(counter < elemts_per_split)  // new_target_1 not filled yet
                {
                    IF_VERBOSE(6) std::cout << "counter=" << counter << " put into target 1" << std::endl;
                    new_target_1[i] = t[i];
                    new_split_set_1.second[i] = true;
                }
                else    // new target 1 is filled now fill new_target_2
                {
                    IF_VERBOSE(6) std::cout << "counter=" << counter << " put into target 2" << std::endl;
                    new_target_2[i] = t[i];
                    new_split_set_2.second[i] = true;
                }
                ++counter;
            }
        }

        //add new targets to subsets, if they are not zero/empty
        if(new_target_1 != 0 ){new_split_set_1.first.insert(new_target_1);}
        if(new_target_2 != 0 ){new_split_set_2.first.insert(new_target_2);}

    }

    // add the new generatet Problems to the result set if it is not empty
    if(new_split_set_1.first.size() != 0 ){result.push_back(new_split_set_1);}
    if(new_split_set_2.first.size() != 0 ){result.push_back(new_split_set_2);}
}

void rpag_matrix_decomp::split_target_into_3(set<vec_t> target_split_set, vector<bool> partion_helper, vector<pair<set<vec_t>, vector<bool> > > &result)
{
    IF_VERBOSE(6) std::cout << "start with: split_target_into_2" << std::endl;
    bool var= true;
    int count_active_elemts = std::count(partion_helper.begin(),partion_helper.end(),var);
    int elemts_per_split = ceil(((double)count_active_elemts)/3);

    IF_VERBOSE(6) std::cout << "count_active_elemts = " << count_active_elemts << std::endl;

    pair<set<vec_t>, vector<bool> > new_split_set_1;
    pair<set<vec_t>, vector<bool> > new_split_set_2;
    pair<set<vec_t>, vector<bool> > new_split_set_3;

    new_split_set_1.second.clear(); new_split_set_1.first.clear(); new_split_set_1.second.resize(partion_helper.size());
    new_split_set_2.second.clear(); new_split_set_2.first.clear(); new_split_set_2.second.resize(partion_helper.size());
    new_split_set_3.second.clear(); new_split_set_3.first.clear(); new_split_set_3.second.resize(partion_helper.size());

    vec_t new_target_1;
    vec_t new_target_2;
    vec_t new_target_3;

    for(vec_t t : target_split_set)
    {
        new_target_1.clear(); new_target_1.resize(partion_helper.size());
        new_target_2.clear(); new_target_2.resize(partion_helper.size());
        new_target_3.clear(); new_target_3.resize(partion_helper.size());

        int counter = 0;// conts already attatched elemts
        IF_VERBOSE(6) std::cout << "partion_helper:" << partion_helper << std::endl;
        for(int i=0; i < partion_helper.size(); ++i)
        {
            if(partion_helper[i] == true) // element is part of this problem
            {
                if(counter < elemts_per_split)  // new_target_1 not filled yet
                {
                    IF_VERBOSE(6) std::cout << "counter=" << counter << " put into target 1" << std::endl;
                    new_target_1[i] = t[i];
                    new_split_set_1.second[i] = true;
                }
                else if (counter < (2*elemts_per_split))  // new_target_1 is filled now new_target_2
                {
                    IF_VERBOSE(6) std::cout << "counter=" << counter << " put into target 1" << std::endl;
                    new_target_2[i] = t[i];
                    new_split_set_2.second[i] = true;
                }
                else // new target 2 is filled now fill new_target_3
                {
                    IF_VERBOSE(6) std::cout << "counter=" << counter << " put into target 2" << std::endl;
                    new_target_3[i] = t[i];
                    new_split_set_3.second[i] = true;
                }
                ++counter;
            }
        }

        //add new targets to subsets, if they are not zero/empty
        if(new_target_1 != 0 ){new_split_set_1.first.insert(new_target_1);}
        if(new_target_2 != 0 ){new_split_set_2.first.insert(new_target_2);}
        if(new_target_3 != 0 ){new_split_set_3.first.insert(new_target_3);}

    }

    // add the new generatet Problems to the result set if it is not empty
    if(new_split_set_1.first.size() != 0 ){result.push_back(new_split_set_1);}
    if(new_split_set_2.first.size() != 0 ){result.push_back(new_split_set_2);}
    if(new_split_set_3.first.size() != 0 ){result.push_back(new_split_set_3);}
}

int rpag_matrix_decomp::adder_depth(vec_t x)
{
  if(is_this_a_two_input_system())
  {
    return log2c_64(nonzeros(abs(x)));
  }
  else
  {
      if(ternary_sign_filter)
      {
          return log3c_64(nonzeros_for_sign_filter(x));
      }
      else
      {
          return log3c_64(nonzeros(abs(x)));
      }
  }
}

void rpag_matrix_decomp::decomposision_recursion(set<vec_t> *target_fun_set)
{
    //init the partionion_helper for target_fun_set each "true" in the vector corresponds to an element in each target, which belongs to the problem.
    //this is necessary to know after spliting which elemts are still part of the probelm
    vector<bool> partition_helper;
    partition_helper.resize(target_fun_set->begin()->size()); // set partition_helper to the same size as all targets
    for(int i=0; i< partition_helper.size();++i){partition_helper[i]=true;} //in the beginning all alements in all targets belong to the Problem

    vector<set<vec_t> >pipeline_set_rev; //Pipeline set which is reversed cause the nomber of resulting stages is not known jet
    vector<vector<set<vec_t> > >solutions; // later filled with the RPAG solutions for all Problems

    vector<pair<set<vec_t>, vector<bool> > > problems; // a vector, which store all problems, with the corrsponding partionion_helper, to know which elemts belong to the problem
    vector<pair<set<vec_t>, vector<bool> > > problems_next_step;

    // set the init Problem
    pair<set<vec_t>, vector<bool> > init;
    init.first = *target_fun_set;
    init.second = partition_helper;

    problems.push_back(init);

    pipeline_set_rev.reserve(No_of_decompose_stages);

    // set the output of the Pipeline set
    insert_problems_into_pipeline_set_rev(pipeline_set_rev, problems);

    for(int i=0; i < No_of_decompose_stages; ++i)
    {    
        for(pair<set<vec_t>, vector<bool> > t_and_p: problems)
        {
            if(is_this_a_two_input_system())
            {
                split_target_into_2(t_and_p.first,t_and_p.second,problems_next_step);
            }
            else
            {
                split_target_into_3(t_and_p.first,t_and_p.second,problems_next_step);
            }
        }

        problems.clear();
        problems = problems_next_step;
        problems_next_step.clear();

        if(i < No_of_decompose_stages-1)
            insert_problems_into_pipeline_set_rev(pipeline_set_rev, problems);
    }

    for(pair<set<vec_t>, vector<bool> > p: problems)
    {

        IF_VERBOSE(5)
        {
            std::cout << std::endl << "################  new_problem  ####################" << std::endl;

            for(vec_t t : p.first)
            {
                std::cout << "# partition: " << p.second << std::endl;
                std::cout << "# target:    " << t << std::endl;
                std::cout << "#" << std::endl;
            }
        }

        IF_VERBOSE(1)
        {
            std::cout << "************************************************************************************" << std::endl;
            std::cout << "start RPAG instance:" << std::endl;
            std::cout << "************************************************************************************" << std::endl;
        }
        rpag_base<vec_t> *rpagp;
        if(is_this_a_two_input_system())
        {
           rpagp = new rpagvm(this);
        }
        else
        {
            rpagp = new rpagtvm(this);
        }
        rpagp->show_adder_graph = false;
        rpagp->adder_depth_constraints.clear();
        rpagp->wordsize_constraints.clear();
        rpagp->target_vec.clear();
        rpagp->set_target_set(p.first);
        rpagp->cost_pointer =  this->cost_pointer;
        rpagp->optimize();
        solutions.push_back(rpagp->get_best_pipeline_set());
        IF_VERBOSE(1) std::cout << "************************************************************************************" << std::endl;
    }


    int solution_pipeline_stages = 0;
    IF_VERBOSE(3)
    {
        std::cout << std::endl << "################  Solutions  ####################" << std::endl;
        for(vector<set<vec_t> >s : solutions)
        {
             std::cout << "RPAG result: " << s <<std::endl<<std::endl<<std::endl;
        }
    }

    solution_pipeline_stages = make_all_solutions_same_depth(solutions);

    IF_VERBOSE(3)
    {
        std::cout << std::endl << "################  Solutions after same depth  ####################" << std::endl;
        for(vector<set<vec_t> >s : solutions)
        {
             std::cout << "RPAG result (depth corrected): " << s <<std::endl<<std::endl<<std::endl;
        }
    }

    //std::cout << "pipelineset_rev: " << pipeline_set_rev << std::endl;


    IF_VERBOSE(5) std::cout << "reverse the reversed pipeline set" << std::endl;
     pipeline_set_rev.resize(pipeline_set_rev.size()+solution_pipeline_stages);
    //reverse the reversed pipeline set...
    {
        unsigned int stages = pipeline_set_rev.size();
        best_pipeline_set_result.resize(pipeline_set_rev.size());
        for(int i=0; i < stages;++i)
        {
            best_pipeline_set_result[stages-i-1] = pipeline_set_rev[i];
        }
    }

    IF_VERBOSE(5) std::cout << "insert all solutions to result" << std::endl;
    //insert all solutions to result
    for(unsigned int i=0; i < solutions.size();++i)
    {
        for(int j=0; j< solutions[i].size();++j)
        {
            for(vec_t target : solutions[i][j])
            {
                best_pipeline_set_result[j].insert(target);
            }
        }

    }

    std::cout << "pipeline_set_best: " << best_pipeline_set_result << std::endl<< std::endl;


    IF_VERBOSE(5) std::cout << "calculating rpag pag cost" << std::endl;

    cost_model_base<vec_t> *cost = ( cost_model_base<vec_t>* )this->cost_pointer;
    cost_pointer = NULL;

    double pag_cost_best = cost->cost_pag(&best_pipeline_set_result);

    IF_VERBOSE(5)std::cout << "calculating rpag output" << std::endl;
    std::cout << std::endl;
    this->create_rpag_output(best_pipeline_set_result, pag_cost_best, target_set);

}
