/*
  This file is part of the PAGSuite project, a suite of optimization tools to optimize pipelined adder graphs.
  It is developed at the University of Kassel and maintained by Martin Kumm (kumm|at|uni-kassel.de).
  You can use and/or modify it for research purposes, for commercial use, please ask for a license.

  For more information please visit http://www.uni-kassel.de/go/pagsuite.
*/

#include "opag.h"
#include "adder_depth.h"
#include "compute_successor_set.h"
#include "log2_64.h"

opag::opag()
{
    solver=NULL;
    no_of_extra_stages=0;
    input_wordsize=-1;

    costModel=HIGH_LEVEL;
    solverType=SCIP;
    ilp_formulation=ILP3;

    cost_add=1.0;
    cost_reg=1.0;
    cost_fa=1.0;
    cost_ff=1.0;

    timeout=-1;

    matlab_output=false;
    optimization_problem_type = MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING;
    lp_relaxation = false;

    optimize_single_stage = false;

    problemType = PMCM;
}

void opag::prepare_problem(int_set_t *target_set)
{
    this->target_set = target_set;
    no_of_pipeline_stages = adder_depth(*target_set)+no_of_extra_stages; //maximum pipeline depth

    if(target_set->empty())
    {
        IF_VERBOSE(1) cout << "found empty target set" << endl;
        no_of_pipeline_stages=1;
        return;
    }


    switch(problemType)
    {
    case PMCM:
        IF_VERBOSE(1) cout << "Solving the PMCM problem" << endl;
        break;
    case MCM_MAD:
        IF_VERBOSE(1) cout << "Solving the MCM problem with minimal adder depth constraint, setting cost_fa and cost_reg to zero..." << endl;
        cost_fa = 0;
        cost_reg = 0;
        break;
    case MCM_MGPC:
    case MCM_RGPC:
        IF_VERBOSE(1) cout << "Solving the MCM problem with reduced/minimal GPC constraint, setting cost_fa and cost_reg to zero..." << endl;
        cost_fa = 0;
        cost_reg = 0;
        break;
    }
    IF_VERBOSE(1) cout << "No of (pipeline) stages=" << no_of_pipeline_stages << endl;

    switch(costModel)
    {
    case HIGH_LEVEL:
        IF_VERBOSE(1) cout << "Using high level cost model with cost_add=" << cost_add << " and cost_reg=" << cost_reg << endl;
        break;
    case LOW_LEVEL:
        IF_VERBOSE(1) cout << "using low level cost model with cost_fa=" << cost_fa << " and cost_ff=" << cost_ff << endl;
        IF_VERBOSE(1) cout << "input word size=" << input_wordsize << endl;
        if(input_wordsize < 0)
        {
            cerr << "Error: No intput word size specified (--input_wordsize=) which is necessary in low level cost mode" << endl;
            exit(-1);
        }
        break;
    }

    int bmax=0;     //max. word size
    for(int_set_t::iterator it=target_set->begin(); it != target_set->end(); ++it)
    {
        int b=log2c_64(*it);
        if(b > bmax) bmax = b;
    }
    c_max = (1LL << (bmax+1));
    IF_VERBOSE(1) cout << "max. coefficient word size=" << bmax << endl;
    IF_VERBOSE(1) cout << "c_max=" << c_max << endl;

    single_set_vec.resize(no_of_pipeline_stages+1);
    pair_map.resize(no_of_pipeline_stages+1);

    IF_VERBOSE(3) cout << "computing pipeline set..." << endl;

    compute_complete_pipeline_set(*target_set);

    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        IF_VERBOSE(3) cout << "stage " << s << ", single_set=" << single_set_vec[s] << endl;
    }

    IF_VERBOSE(3) cout << "computing pairs..." << endl;
    compute_pair_map();

    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        IF_VERBOSE(3) cout << "stage " << s << ", reduced single_set=" << single_set_vec[s] << endl;
    }

    IF_VERBOSE(3) print_pair_map();

    if(problemType == MCM_RGPC)
    {
        filter_pair_map_min_gpc();
        IF_VERBOSE(3)
        {
            cout << "filtered tripplets with minimal GPC:" << endl;
            print_pair_map();
        }
    }

    if(solver) delete solver; //delete solver if anyone was created before

    switch(solverType)
    {
#ifdef USE_SCIP
    case SCIP:
        optimization_problem_type = MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING;
        IF_VERBOSE(1) cout << "Using solver SCIP" << endl;
        break;
#endif
#ifdef USE_CPLEX
    case CPLEX:
        optimization_problem_type = MPSolver::CPLEX_MIXED_INTEGER_PROGRAMMING;
        IF_VERBOSE(1) cout << "Using solver CPLEX" << endl;
        break;
#endif
#ifdef USE_GUROBI
    case GUROBI:
        optimization_problem_type = MPSolver::GUROBI_MIXED_INTEGER_PROGRAMMING;
        IF_VERBOSE(1) cout << "Using solver Gurobi" << endl;
        break;
#endif
#ifdef USE_GLPK
    case GLPK:
        optimization_problem_type = MPSolver::GLPK_MIXED_INTEGER_PROGRAMMING;
        IF_VERBOSE(1) cout << "Using solver GLPK" << endl;
        break;
#endif
#ifdef USE_CBC
    case CBC:
        optimization_problem_type = MPSolver::CBC_MIXED_INTEGER_PROGRAMMING;
        IF_VERBOSE(1) cout << "Using solver CBC" << endl;
        break;
#endif
    default:
        cerr << "Unknown solver type " << solverType << endl;
    }

    adder_variable_map_vec.resize(no_of_pipeline_stages+1);

    if(problemType == MCM_MGPC)
    {
        gpc_variable_map_vec.resize(no_of_pipeline_stages+1);
    }

    switch(ilp_formulation)
    {
    case ILP1:
    case ILP1BIN:
        IF_VERBOSE(1) cout << "Using PMCM formulation 1 (binary=" << (ilp_formulation == ILP1BIN) << ")" << endl;

        if(optimize_single_stage)
        {
            cerr << "Error: optimize_single_stage currently not supported for this ILP formulation" << endl;
            exit(-1);
        }
        if(problemType == MCM_MGPC)
        {
            cerr << "Error: minimum GPC not supported for this ILP formulation." << endl;
            exit(-1);
        }

        solver = new MPSolver("PMCM1", optimization_problem_type);
        register_variable_map_vec.resize(no_of_pipeline_stages+1);
        uv_pair_variable_map_vec.resize(no_of_pipeline_stages+1);
        insert_pmcm_ilp_formulation1(ilp_formulation == ILP1);

        break;
    case ILP2:
    case ILP2BIN:
        IF_VERBOSE(1) cout << "Using PMCM formulation 2 (binary=" << (ilp_formulation == ILP2BIN) << ")" << endl;

        if(optimize_single_stage)
        {
            cerr << "Error: optimize_single_stage currently not supported for this ILP formulation" << endl;
            exit(-1);
        }

        solver = new MPSolver("PMCM2", optimization_problem_type);
        uvw_triplet_variable_map_vec.resize(no_of_pipeline_stages+1);
        insert_pmcm_ilp_formulation2(ilp_formulation == ILP2);

        break;
    case ILP3:
        IF_VERBOSE(1) cout << "Using PMCM formulation 3" << endl;

        if(problemType == MCM_MGPC)
        {
            cerr << "Error: minimum GPC not supported for this ILP formulation." << endl;
            exit(-1);
        }
        solver = new MPSolver("PMCM3", optimization_problem_type);
        uvw_triplet_variable_map_vec.resize(no_of_pipeline_stages+1);
        insert_pmcm_ilp_formulation3();

        break;
    default:
        cerr << "Unknown optimization method " << ilp_formulation << endl;
        exit(-1);
    }

    IF_VERBOSE(1) solver->EnableOutput();

    if(timeout > 0) solver->set_time_limit(timeout);
}

opag::~opag()
{
    if(solver) delete solver;
}

void opag::print_problem_info()
{
    if(solver)
    {
        cout << "Number of variables = " << solver->NumVariables() << endl;
        cout << "Number of constraints = " << solver->NumConstraints() << endl;
        string lpstr;
        IF_VERBOSE(3) solver->ExportModelAsLpFormat(false,&lpstr);

        cout << "ILP model:" << endl << lpstr << endl;
    }
}

void opag::optimize()
{
    vector< set<int_t> > pipeline_set_opt(no_of_pipeline_stages);
    list< realization_row<int_t> > pipelined_adder_graph;
    double solver_time;
    double obj_value;

    if(!target_set->empty())
    {
        if(!solver)
        {
            cerr << "Error: call of optimize() before prepare_problem()" << endl;
            exit(-1);
        }

        IF_VERBOSE(1) cout << "Starting optimization using " << solver->SolverVersion();
        const MPSolver::ResultStatus result_status = solver->Solve();

        // Check that the problem has a feasable solution.
        if (result_status == MPSolver::NOT_SOLVED)
        {
          cerr << "No solution found!" << endl;
          exit(-1);
        }

        if (result_status != MPSolver::OPTIMAL)
        {
          cerr << "WARNING: The solution is not proven to be optimal!" << endl;
        }
        IF_VERBOSE(3)
        {
            cout << "solution:" << endl;
            for(int s=1; s <= no_of_pipeline_stages; s++)
            {
                for(map<int_t,MPVariable*>::iterator iter = adder_variable_map_vec[s].begin(); iter != adder_variable_map_vec[s].end(); ++iter)
                {
                    if((*iter).second->solution_value() > 0)
                    {
                        IF_VERBOSE(3) cout << ((*iter).second)->name() << "=" << ((*iter).second)->solution_value() << endl;
                    }
                }
                if((ilp_formulation == ILP1) || (ilp_formulation == ILP1BIN))
                {
                    for(map<int_t,MPVariable*>::iterator iter = register_variable_map_vec[s].begin(); iter != register_variable_map_vec[s].end(); ++iter)
                    {
                        if((*iter).second->solution_value() > 0)
                        {
                            IF_VERBOSE(3) cout << ((*iter).second)->name() << "=" << ((*iter).second)->solution_value() << endl;
                        }
                    }
                }
                if((ilp_formulation == ILP2) || (ilp_formulation == ILP2BIN) || (ilp_formulation == ILP3))
                {
                    for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
                    {
                        if((*triplet_map_it).second->solution_value() > 0)
                        {
                            IF_VERBOSE(3) cout << ((*triplet_map_it).second)->name() << "=" << ((*triplet_map_it).second)->solution_value() << endl;
                        }
                    }
                }
                if(problemType == MCM_MGPC)
                {
                    for(map<int_t,MPVariable*>::iterator iter = gpc_variable_map_vec[s].begin(); iter != gpc_variable_map_vec[s].end(); ++iter)
                    {
                        if((*iter).second->solution_value() > 0)
                        {
                            IF_VERBOSE(3) cout << ((*iter).second)->name() << "=" << ((*iter).second)->solution_value() << endl;
                        }
                    }
                }
            }
        }

        if(!lp_relaxation)
        {
            create_pipeline_set_from_solution(&pipeline_set_opt);

            create_pipelined_adder_graph_from_solution(&pipelined_adder_graph);
        }
        solver_time = solver->wall_time()*1E-3;
        MPObjective* const objective = solver->MutableObjective();
        obj_value = objective->Value();
        if(!lp_relaxation)
        {
            IF_VERBOSE(1) cout << "Optimal objective value = " << obj_value << endl;
        }
        else
        {
            IF_VERBOSE(1) cout << "LP Relaxation objective value = " << obj_value << endl;
        }
        IF_VERBOSE(1) cout << "Problem solved in " << solver_time << " seconds" << endl;

    }
    else
    {
        //trivial solution:
        solver_time=0.0;
        obj_value=0.0;
//        pipeline_set_opt[0].insert(*(target_set->begin()));
    }

    if(!lp_relaxation)
    {
        //count adders and registers:
        int no_of_adders=0;
        int no_of_registers=0;
        for(unsigned int s = 0; s < pipeline_set_opt.size(); ++s)
        {
          for(set<int_t>::iterator set_iter = pipeline_set_opt[s].begin(); set_iter != pipeline_set_opt[s].end(); ++set_iter)
          {
            if(s == 0)
            {
              if(*set_iter == 1)
                no_of_registers++;
              else
                no_of_adders++;
            }
            else
            {
              //seach element in previous stage
              if(pipeline_set_opt[s-1].find(*set_iter) != pipeline_set_opt[s-1].end())
              {
                //element found -> count as register
                no_of_registers++;
              }
              else
              {
                no_of_adders++;
              }
            }
          }
        }

        IF_VERBOSE(0) cout << "pipline_set_opt=" << pipeline_set_opt << endl;
        IF_VERBOSE(0) cout << output_adder_graph(pipelined_adder_graph,false);
        IF_VERBOSE(0) cout << "pag_cost=" << obj_value << endl;
        IF_VERBOSE(1) cout << "no_of_adders=" << no_of_adders << endl;
        IF_VERBOSE(1) cout << "no_of_registers=" << no_of_registers << endl;
        if((problemType == MCM_MGPC) || (problemType == MCM_RGPC))
        {
            int gpc = gpc_fom_pipeline_set(pipeline_set_opt);
            IF_VERBOSE(0) cout << "GPC=" << gpc << endl;
        }

        if(matlab_output)
        {
          cout << "objective{" << matlab_out_adress_string << "}=" << obj_value <<"; ";
          cout << "no_of_adders{" << matlab_out_adress_string << "}=" << no_of_adders <<"; ";
          cout << "no_of_registers{" << matlab_out_adress_string << "}=" << no_of_registers <<"; ";
          cout << "already_computed(" << matlab_out_adress_string << ")=1; ";
          cout << "pipeline_set_best{" << matlab_out_adress_string << "}=" << pipeline_set_opt <<"; " ;
          cout << "needed_time_in_s(" << matlab_out_adress_string << ")=" << std::fixed << std::setprecision(3) << solver_time <<"; " ;
    //      cout << "rpag_info{" << matlab_out_adress_string << "}='" << rpag_info(start_arguments,false) <<"';" ;
          cout << std::endl;
        }
    }
    else
    {
        if(matlab_output)
        {
            cout << "objective{" << matlab_out_adress_string << "}=" << obj_value <<"; ";
            cout << "needed_time_in_s(" << matlab_out_adress_string << ")=" << std::fixed << std::setprecision(3) << solver_time <<"; " ;
            cout << std::endl;
        }
    }


}

void opag::create_pipeline_set_from_solution(vector< set<int_t> > *pipeline_set)
{
    //fill pipeline_set_opt structure:
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        if((ilp_formulation == ILP1) || (ilp_formulation == ILP1BIN))
        {
            for(map<int_t,MPVariable*>::iterator iter = register_variable_map_vec[s].begin(); iter != register_variable_map_vec[s].end(); ++iter)
            {
                if((*iter).second->solution_value() > 0)
                {
                    (*pipeline_set)[s-1].insert((*iter).first);
                }
            }
        }
        if((ilp_formulation == ILP1) || (ilp_formulation == ILP1BIN) || (ilp_formulation == ILP2) || (ilp_formulation == ILP2BIN))
        {
            for(map<int_t,MPVariable*>::iterator iter = adder_variable_map_vec[s].begin(); iter != adder_variable_map_vec[s].end(); ++iter)
            {
                if((*iter).second->solution_value() > 0)
                {
                    (*pipeline_set)[s-1].insert((*iter).first);
                }
            }
        }
        if(ilp_formulation == ILP3)
        {
            for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
            {
                if((*triplet_map_it).second->solution_value() > 0)
                {
                    (*pipeline_set)[s-1].insert((*triplet_map_it).first.third);
                }
            }
        }
    }
}

void opag::create_pipelined_adder_graph_from_solution(list<realization_row<int_t> > *pipelined_adder_graph)
{
    if(ilp_formulation == opag::ILP3)
    {
        for(int s=1; s <= no_of_pipeline_stages; s++)
        {
            for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
            {
                if((*triplet_map_it).second->solution_value() > 0)
                {
                    realization_row<int_t> row;
                    row.A = (*triplet_map_it).first.first;
                    row.B = (*triplet_map_it).first.second;
                    row.W = (*triplet_map_it).first.third;
                    row.stageA = s-1;
                    row.stageB = s-1;
                    row.stageW = s;

                    int signA, signB;
                    getExponents(row.A,row.B, row.W, &row.eA, &row.eB, &signA, &signB);
                    row.A *= signA;
                    row.B *= signB;

                    //normalize inputs such that a is positive
                    if(row.A < 0)
                    {
                        int_t t = row.A;
                        row.A = row.B;
                        row.B = t;

                        int e = row.eA;
                        row.eA = row.eB;
                        row.eB = e;
                    }
                    pipelined_adder_graph->push_back(row);
                }
            }
        }
    }

}

void opag::compute_complete_pipeline_set(int_set_t &target_set)
{
    int_vec_t single_count_vec(no_of_pipeline_stages+1);
    single_count_vec[0] = 1;
    single_set_vec[0].insert(1);

    for(int_set_t::iterator iter = target_set.begin(); iter != target_set.end(); ++iter)
    {
        int s;
        switch(problemType)
        {
        case PMCM:
            s = no_of_pipeline_stages;
            break;
        case MCM_MAD:
        case MCM_RGPC:
        case MCM_MGPC:
            s = adder_depth(*iter);
            break;
        }

        single_set_vec[s].insert(*iter);
    }

    for(int s=1; s <= no_of_pipeline_stages-1; s++)
    {
        if(s < no_of_pipeline_stages)
        {
            compute_successor_set(&(single_set_vec[s-1]), c_max, &(single_set_vec[s]),false); //fill new current set
        }
        single_count_vec[s] = single_set_vec[s].size();
    }
    IF_VERBOSE(1)
    {
        cout << "no of singles in for c_max=" << c_max << " in stages 0..." << no_of_pipeline_stages << " : ";
        for(int s=0; s <= no_of_pipeline_stages; s++)
        {
            cout << single_count_vec[s] << "   ";
        }
        cout << endl;
    }
}


void opag::compute_pair_map(bool filter_pipeline_set)
{
    int no_of_pipeline_stages = pair_map.size()-1;
    int_vec_t pair_count_vec(no_of_pipeline_stages+1,0);

    pair_count_vec[0] = 1;

    for(int s=no_of_pipeline_stages-1; s >= 0; --s)
    {
        int_set_t predecessor_set;
        for(int_set_t::iterator int_set_u_iter = single_set_vec[s].begin(); int_set_u_iter != single_set_vec[s].end(); ++int_set_u_iter)
        {
            for(int_set_t::iterator int_set_w_iter = single_set_vec[s+1].begin(); int_set_w_iter != single_set_vec[s+1].end(); ++int_set_w_iter)
            {
                int_set_t v_set;
                compute_successor_set(*int_set_u_iter,*int_set_w_iter, c_max, &v_set,false);

                for(int_set_t::iterator int_set_v_iter=v_set.begin(); int_set_v_iter != v_set.end(); ++int_set_v_iter)
                {
                    if(*int_set_v_iter >= *int_set_u_iter)
                    {
                        if(adder_depth(*int_set_v_iter) <= s)
                        {
                            pair_count_vec[s+1]++;
                            pair_map[s+1][*int_set_w_iter].insert(make_pair(*int_set_u_iter,*int_set_v_iter));
                            if(filter_pipeline_set)
                            {
                                predecessor_set.insert(*int_set_u_iter);
                                predecessor_set.insert(*int_set_v_iter);
                            }
                        }
                    }
                }
            }
        }
        if(filter_pipeline_set && (s > 0))
        {
            single_set_vec[s].clear();
            single_set_vec[s].insert(predecessor_set.begin(), predecessor_set.end());
        }
    }

  IF_VERBOSE(1)
    {
        cout << "no of pairs in for c_max=" << c_max << " in stages 0..." << no_of_pipeline_stages << " : ";
        for(int s=0; s <= no_of_pipeline_stages; s++)
        {
            cout << pair_count_vec[s] << "   ";
        }
        cout << endl;
    }
}

void opag::compute_min_gpc_from_pair_map()
{
    gpc_min_map[(int_t) 1] = 0;
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        cout << endl << "GPC in stage " << s << " :" << endl << endl;
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w=(*pair_map_iter).first;
            int gpc_min = INT32_MAX;

            //determine the minimal GPC for the current w:
            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;
                int gpc;

                if((w == u) && (w == v))
                {
                    gpc = gpc_min_map[u];
                }
                else
                {
                    gpc = gpc_min_map[u] + gpc_min_map[v] + 1;
                }

                if(gpc < gpc_min) gpc_min = gpc;
                IF_VERBOSE(4) cout << "GPC(" << w << "," << u << "," << v << ") = " << gpc << endl;
            }
            IF_VERBOSE(3) cout << "GPCmin(" << w << ") = " << gpc_min << endl;
            gpc_min_map[w] = gpc_min; //store minimal GPC for later use
        }
    }
}

void opag::filter_pair_map_min_gpc()
{
    if(gpc_min_map.empty()) compute_min_gpc_from_pair_map();

    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        cout << endl << "GPC in stage " << s << " :" << endl << endl;
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w=(*pair_map_iter).first;
            int gpc_min = gpc_min_map[w];

            //remove all pairs which does not lead to the minimal GPC:
            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); )
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;
                int gpc;

                if((w == u) && (w == v))
                {
                    gpc = gpc_min_map[u];
                }
                else
                {
                    gpc = gpc_min_map[u] + gpc_min_map[v] + 1;
                }

                if(gpc > gpc_min)
                {
//                    cout << "erasing (" << (*pair_set_iter).first << "," << (*pair_set_iter).second << ")" << endl;
                    (*pair_map_iter).second.erase(pair_set_iter++);
                }
                else
                {
                    ++pair_set_iter;
                }
            }

        }
    }
}

void opag::print_pair_map()
{
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        cout << endl << "tripplets in stage " << s << " :" << endl << endl;
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            cout << "{" << (*pair_map_iter).first << ",(" << (*pair_map_iter).second << ")} " << endl;
        }
    }
}

int opag::gpc_fom_pipeline_set(vector< set<int_t> > &pipeline_set)
{
    int gpc=0;
    for(unsigned s=0; s < pipeline_set.size(); s++)
    {
        for(set<int_t>::iterator it = pipeline_set[s].begin(); it != pipeline_set[s].end(); ++it)
        {
            if(*it == 1) continue; //do not count input node!
            if(s > 0) if(pipeline_set[s-1].find(*it) != pipeline_set[s-1].end()) continue; //do not count register!

            IF_VERBOSE(3) cout << "GPC of node " << *it << " is " << gpc_min_map[*it] << endl;
            gpc += gpc_min_map[*it];
        }
    }
    return gpc;
}

void opag::insert_pmcm_ilp_formulation1(bool use_simplified_model)
{
    const double infinity = solver->infinity();

    //create adder/register variables:
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        for(int_set_t::iterator iter=single_set_vec[s].begin(); iter != single_set_vec[s].end(); ++iter)
        {
            stringstream strstra;
            strstra << "a_" << s << "_" << *iter;
            if(lp_relaxation)
                adder_variable_map_vec[s][*iter] = solver->MakeNumVar(0.0,1.0,strstra.str());
            else
                adder_variable_map_vec[s][*iter] = solver->MakeBoolVar(strstra.str());

            if(adder_depth(*iter) < s)
            {
                stringstream strstrr;
                strstrr << "r_" << s << "_" << *iter;

                if(lp_relaxation)
                    register_variable_map_vec[s][*iter] = solver->MakeNumVar(0.0,1.0,strstrr.str());
                else
                    register_variable_map_vec[s][*iter] = solver->MakeBoolVar(strstrr.str());
            }
        }
    }

    //create pair variables:
    for(int s=2; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;
                int_pair_t uv_pair = make_pair<int_t,int_t>((int_t) u,(int_t) v);
                map<int_pair_t ,MPVariable*>::iterator pair_map_it = uv_pair_variable_map_vec[s].find(uv_pair);

                if(pair_map_it == uv_pair_variable_map_vec[s].end())
                {
                    stringstream strstr;
                    strstr << "a_" << s << "_" << u << "_" << v;
                    if(use_simplified_model || lp_relaxation)
                        uv_pair_variable_map_vec[s][uv_pair] = solver->MakeNumVar(0.0,1.0,strstr.str());
                    else
                        uv_pair_variable_map_vec[s][uv_pair] = solver->MakeBoolVar(strstr.str());
                }
                else
                {
                }
            }
        }
    }

    //create objective:
    MPObjective* const objective = solver->MutableObjective();
    objective->SetMinimization();

    double cost_w_add = cost_add; //init with HIGH_LEVEL cost
    double cost_w_reg = cost_reg; //init with HIGH_LEVEL cost

    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        for(int_set_t::iterator iter=single_set_vec[s].begin(); iter != single_set_vec[s].end(); ++iter)
        {
            if(costModel == LOW_LEVEL)
            {
                cost_w_add = (log2c_64(*iter) + input_wordsize) * cost_fa;
            }
            objective->SetCoefficient(adder_variable_map_vec[s][*iter], cost_w_add);
            map<int_t,MPVariable*>::iterator regit = register_variable_map_vec[s].find(*iter);
            if(regit != register_variable_map_vec[s].end())
            {
                if(costModel == LOW_LEVEL)
                {
                    cost_w_reg = (log2c_64(*iter) + input_wordsize) * cost_ff;
                }
                objective->SetCoefficient((*regit).second, cost_w_reg);
            }
        }
    }


    //create constraints:
    //constraints C1:
    int i=0;
    for(int_set_t::iterator target_it=target_set->begin(); target_it != target_set->end(); ++target_it)
    {
        //C1: r_s_w + a_s_w = 1
        int s;
        switch(problemType)
        {
        case PMCM:
            s = no_of_pipeline_stages;
            break;
        case MCM_MAD:
        case MCM_RGPC:
        case MCM_MGPC:
            if(*target_it == 1) continue; //skipp 1 as it does not exist as a variable
            s = adder_depth(*target_it);
            break;
        }

        stringstream strstr;
        strstr << "C1." << i++;

        MPConstraint* const c = solver->MakeRowConstraint(1, 1, strstr.str());
        c->SetCoefficient(adder_variable_map_vec[s][*target_it], 1);
        map<int_t,MPVariable*>::iterator regit = register_variable_map_vec[s].find(*target_it);
        if(regit != register_variable_map_vec[s].end())
        {
            c->SetCoefficient((*regit).second, 1);
        }

    }

    //constraints C2 not necessary as the corresponding variables were not created
    //constraints C3:
    i=0;
    for(int s=2; s <= no_of_pipeline_stages; s++)
    {
        for(int_set_t::iterator iter=single_set_vec[s].begin(); iter != single_set_vec[s].end(); ++iter)
        {
            map<int_t,MPVariable*>::iterator regit1 = register_variable_map_vec[s].find(*iter);
            if(regit1 != register_variable_map_vec[s].end())
            {
                stringstream strstr;
                strstr << "C3." << i++;
                MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());
                c->SetCoefficient((*regit1).second, 1);
                c->SetCoefficient(adder_variable_map_vec[s-1][*iter], -1);

                map<int_t,MPVariable*>::iterator regit2 = register_variable_map_vec[s-1].find(*iter);
                if(regit2 != register_variable_map_vec[s-1].end())
                {
                    c->SetCoefficient((*regit2).second, -1);
                }
            }
        }
    }

    //constraints C4:
    i=0;
    for(int s=2; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            stringstream strstr;
            strstr << "C4." << i++;
            MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());

            c->SetCoefficient(adder_variable_map_vec[s][(*pair_map_iter).first], 1);

            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;
                int_pair_t uv_pair = make_pair<int_t,int_t>((int_t) u,(int_t) v);

                c->SetCoefficient(uv_pair_variable_map_vec[s][uv_pair], -1);
            }
        }
    }

    //constraints C5 a and b (a_s_u_v - r_(s-1)_u - a_(s-1)_u and a_s_u_v - r_(s-1)_v - a_(s-1)_v)
    i=0;
    for(int s=2; s <= no_of_pipeline_stages; s++)
    {
        for(map<int_pair_t,MPVariable* >::iterator pair_map_it = uv_pair_variable_map_vec[s].begin(); pair_map_it != uv_pair_variable_map_vec[s].end(); ++pair_map_it)
        {
            //C5a:
            {
                stringstream strstr;
                strstr << "C5." << i++;
                MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());
                c->SetCoefficient((*pair_map_it).second, 1);
                c->SetCoefficient(adder_variable_map_vec[s-1][(*pair_map_it).first.first],-1);

                map<int_t,MPVariable*>::iterator regit = register_variable_map_vec[s-1].find((*pair_map_it).first.first);
                if(regit != register_variable_map_vec[s-1].end())
                {
                    c->SetCoefficient((*regit).second, -1);
                }
            }
            //C5b:
            {
                stringstream strstr;
                strstr << "C5." << i++;
                MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());
                c->SetCoefficient((*pair_map_it).second, 1);
                c->SetCoefficient(adder_variable_map_vec[s-1][(*pair_map_it).first.second],-1);

                map<int_t,MPVariable*>::iterator regit = register_variable_map_vec[s-1].find((*pair_map_it).first.second);
                if(regit != register_variable_map_vec[s-1].end())
                {
                    c->SetCoefficient((*regit).second, -1);
                }
            }
        }
    }
}

void opag::insert_pmcm_ilp_formulation2(bool use_simplified_model)
{
    const double infinity = solver->infinity();

    //create adder variables:
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        for(int_set_t::iterator iter=single_set_vec[s].begin(); iter != single_set_vec[s].end(); ++iter)
        {
            stringstream strstra;
            strstra << "a_" << s << "_" << *iter;

            if(lp_relaxation)
                adder_variable_map_vec[s][*iter] = solver->MakeNumVar(0.0,1.0,strstra.str());
            else
                adder_variable_map_vec[s][*iter] = solver->MakeBoolVar(strstra.str());
        }
    }

    //create triplet variables:
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w = (*pair_map_iter).first;
            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;

                triplet<int_t,int_t,int_t> uvw_triplet(u,v,w);
                map<int_triplet_t ,MPVariable*>::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].find(uvw_triplet);

                if(triplet_map_it == uvw_triplet_variable_map_vec[s].end())
                {
                    stringstream strstr;
                    strstr << "a_" << s << "_" << u << "_" << v << "_" << w;
                    if((use_simplified_model && (problemType != MCM_MGPC)) || lp_relaxation)
                        uvw_triplet_variable_map_vec[s][uvw_triplet] = solver->MakeNumVar(0.0,1.0,strstr.str());
                    else
                        uvw_triplet_variable_map_vec[s][uvw_triplet] = solver->MakeBoolVar(strstr.str());
                }
            }
        }
    }

    if(problemType == MCM_MGPC)
    {
        //create GPC variables:
        int gpc_max = (1 << no_of_pipeline_stages)-1; //the maximum GPC for S pipeline stages is 2^S - 1 (when realization is complete multiplicative)
        for(int s=1; s <= no_of_pipeline_stages; s++)
        {
            for(int_set_t::iterator iter=single_set_vec[s].begin(); iter != single_set_vec[s].end(); ++iter)
            {
                stringstream strstr;
                strstr << "gpc_" << s << "_" << *iter;

                if(lp_relaxation)
                    gpc_variable_map_vec[s][*iter] = solver->MakeNumVar(0.0,1.0,strstr.str());
                else
                    gpc_variable_map_vec[s][*iter] = solver->MakeIntVar(0,gpc_max,strstr.str());
            }
        }
    }

    //create objective:
    MPObjective* const objective = solver->MutableObjective();
    objective->SetMinimization();

    double cost_w_add = cost_add; //init with HIGH_LEVEL cost
    double cost_w_reg = cost_reg; //init with HIGH_LEVEL cost

    if(problemType == MCM_MGPC)
    {
        for(int s=1; s <= no_of_pipeline_stages; s++)
        {
            for(map<int_t,MPVariable*>::iterator iter=gpc_variable_map_vec[s].begin(); iter != gpc_variable_map_vec[s].end(); ++iter)
            {
                objective->SetCoefficient((*iter).second, 1);
            }
        }
    }
    else
    {
        for(int s=1; s <= no_of_pipeline_stages; s++)
        {
            for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
            {
                int_t u = (*triplet_map_it).first.first;
                int_t w = (*triplet_map_it).first.third;

                if(u == w)
                {
                    //variable describes a register:
                    if(costModel == LOW_LEVEL)
                    {
                        cost_w_reg = (log2c_64(w) + input_wordsize) * cost_ff;
                    }
                    objective->SetCoefficient((*triplet_map_it).second, cost_w_reg);
                }
                else
                {
                    //variable describes an adder:
                    if(costModel == LOW_LEVEL)
                    {
                        cost_w_add = (log2c_64(w) + input_wordsize) * cost_fa;
                    }
                    objective->SetCoefficient((*triplet_map_it).second, cost_w_add);
                }
            }
        }
    }

    //create constraints:
    //constraints C1:
    int i=0;

    for(int_set_t::iterator target_it=target_set->begin(); target_it != target_set->end(); ++target_it)
    {
        //C1: a_s_w = 1
        int s;
        switch(problemType)
        {
        case PMCM:
            s = no_of_pipeline_stages;
            break;
        case MCM_MAD:
        case MCM_RGPC:
        case MCM_MGPC:
            if(*target_it == 1) continue; //skipp 1 as it does not exist as a variable
            s = adder_depth(*target_it);
            break;
        }

        stringstream strstr;
        strstr << "C1." << i++;

        MPConstraint* const c = solver->MakeRowConstraint(1, 1, strstr.str());
        c->SetCoefficient(adder_variable_map_vec[s][*target_it], 1);
    }

    //constraints C2:
    i=0;
    for(int s=1; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w = (*pair_map_iter).first;
            stringstream strstr;
            strstr << "C2." << i++;
            MPConstraint* const c = solver->MakeRowConstraint(0, 0, strstr.str());

            c->SetCoefficient(adder_variable_map_vec[s][(*pair_map_iter).first], 1);

            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;


                triplet<int_t,int_t,int_t> uvw_triplet(u,v,w);

                c->SetCoefficient(uvw_triplet_variable_map_vec[s][uvw_triplet], -1);
            }
        }
    }

    //constraints C3 a and b (a_s_u_v_w - a_(s-1)_u <= 0 and a_s_u_v_w - a_(s-1)_v <= 0)
    i=0;
    for(int s=2; s <= no_of_pipeline_stages; s++)
    {
        for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
        {
            //C3a:
            {
                stringstream strstr;
                strstr << "C3." << i++;
                MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());
                c->SetCoefficient((*triplet_map_it).second, 1);
                c->SetCoefficient(adder_variable_map_vec[s-1][(*triplet_map_it).first.first],-1);
            }
            //C3b:
            {
                stringstream strstr;
                strstr << "C3." << i++;
                MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());
                c->SetCoefficient((*triplet_map_it).second, 1);
                c->SetCoefficient(adder_variable_map_vec[s-1][(*triplet_map_it).first.second],-1);
            }
        }
    }

    if(problemType == MCM_MGPC)
    {
        //minGPC constraints:
        i=0;
        int_t large_constant = 10000;
        for(int s=1; s <= no_of_pipeline_stages; s++)
        {
            for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
            {
                int_t u = (*triplet_map_it).first.first;
                int_t v = (*triplet_map_it).first.second;
                int_t w = (*triplet_map_it).first.third;

                {
                    stringstream strstr;
                    strstr << "C4." << i << "a";
                    MPConstraint* const c = solver->MakeRowConstraint(-large_constant+1, infinity, strstr.str());
                    c->SetCoefficient(gpc_variable_map_vec[s][w], 1);
                    if(s > 1)
                    {
                        c->SetCoefficient(gpc_variable_map_vec[s-1][u], -1);
                        c->SetCoefficient(gpc_variable_map_vec[s-1][v], -1);
                    }
                    c->SetCoefficient((*triplet_map_it).second, -large_constant);
                }
                {
                    stringstream strstr;
                    strstr << "C4." << i++ << "b";
                    MPConstraint* const c = solver->MakeRowConstraint(-infinity, large_constant+1, strstr.str());
                    c->SetCoefficient(gpc_variable_map_vec[s][w], 1);
                    if(s > 1)
                    {
                        c->SetCoefficient(gpc_variable_map_vec[s-1][u], -1);
                        c->SetCoefficient(gpc_variable_map_vec[s-1][v], -1);
                    }
                    c->SetCoefficient((*triplet_map_it).second, large_constant);
                }
            }
        }

    }
}

void opag::insert_pmcm_ilp_formulation3()
{

    if(problemType == MCM_MGPC)
    {
        cerr << "Error: minimum GPC not supported for this ILP formulation." << endl;
        exit(-1);
    }

    int lowest_pipeline_stage;

    if(optimize_single_stage)
        lowest_pipeline_stage = no_of_pipeline_stages-1;
    else
        lowest_pipeline_stage = 1; //default

    const double infinity = solver->infinity();

    //create triplet variables:
    for(int s=lowest_pipeline_stage; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w = (*pair_map_iter).first;
            for(int_pair_set_t::iterator pair_set_iter = (*pair_map_iter).second.begin(); pair_set_iter != (*pair_map_iter).second.end(); ++pair_set_iter)
            {
                int_t u = (*pair_set_iter).first;
                int_t v = (*pair_set_iter).second;

                triplet<int_t,int_t,int_t> uvw_triplet(u,v,w);
                map<int_triplet_t ,MPVariable*>::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].find(uvw_triplet);

                if(triplet_map_it == uvw_triplet_variable_map_vec[s].end())
                {
                    stringstream strstr;
                    strstr << "a_" << s << "_" << u << "_" << v << "_" << w;

                    if(lp_relaxation)
                        uvw_triplet_variable_map_vec[s][uvw_triplet] = solver->MakeNumVar(0.0,1.0,strstr.str());
                    else
                        uvw_triplet_variable_map_vec[s][uvw_triplet] = solver->MakeBoolVar(strstr.str());

                }
            }
        }
    }

    //create objective:
    MPObjective* const objective = solver->MutableObjective();
    objective->SetMinimization();

    double cost_w_add = cost_add; //init with HIGH_LEVEL cost
    double cost_w_reg = cost_reg; //init with HIGH_LEVEL cost

    for(int s=lowest_pipeline_stage; s <= no_of_pipeline_stages; s++)
    {
        for(map<int_triplet_t,MPVariable* >::iterator triplet_map_it = uvw_triplet_variable_map_vec[s].begin(); triplet_map_it != uvw_triplet_variable_map_vec[s].end(); ++triplet_map_it)
        {
            int_t u = (*triplet_map_it).first.first;
            int_t w = (*triplet_map_it).first.third;

            if(u == w)
            {
                //variable describes a register:
                if(costModel == LOW_LEVEL)
                {
                    cost_w_reg = (log2c_64(w) + input_wordsize) * cost_ff;
                }
                objective->SetCoefficient((*triplet_map_it).second, cost_w_reg);
            }
            else
            {
                //variable describes an adder:
                if(costModel == LOW_LEVEL)
                {
                    cost_w_add = (log2c_64(w) + input_wordsize) * cost_fa;
                }
                objective->SetCoefficient((*triplet_map_it).second, cost_w_add);
            }
        }
    }

    //create constraints:
    //constraints C1:
    int i=0;
    for(int_set_t::iterator target_it=target_set->begin(); target_it != target_set->end(); ++target_it)
    {
        int_t w=*target_it;

        int s;
        switch(problemType)
        {
        case PMCM:
            s = no_of_pipeline_stages;
            break;
        case MCM_MAD:
        case MCM_RGPC:
        case MCM_MGPC:
            if(w == 1) continue; //skipp 1 as it does not exist as a variable
            s = adder_depth(w);
            break;
        }
        stringstream strstr;
        strstr << "C1." << i++;
        MPConstraint* const c = solver->MakeRowConstraint(1, 1, strstr.str());

        for(int_pair_set_t::iterator pair_it=(pair_map[s])[w].begin(); pair_it != (pair_map[s])[w].end(); ++pair_it)
        {
            int_t u=(*pair_it).first;
            int_t v=(*pair_it).second;

            int_triplet_t uvw_triplet(u,v,w);

            c->SetCoefficient(uvw_triplet_variable_map_vec[s][uvw_triplet], 1);
        }
    }

    //constraints C2:
    i=0;
    for(int s=lowest_pipeline_stage+1; s <= no_of_pipeline_stages; s++)
    {
        int_int_pair_set_map_t::iterator pair_map_iter;
        for(pair_map_iter = pair_map[s].begin(); pair_map_iter != pair_map[s].end(); ++pair_map_iter)
        {
            int_t w = (*pair_map_iter).first;

            for(int_pair_set_t::iterator w_pair_set_iter = (*pair_map_iter).second.begin(); w_pair_set_iter != (*pair_map_iter).second.end(); ++w_pair_set_iter)
            {
                int_t u = (*w_pair_set_iter).first;
                int_t v = (*w_pair_set_iter).second;

                //constraints 2a:
                {
                    stringstream strstr;
                    strstr << "C2a." << i;
                    MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());

                    triplet<int_t,int_t,int_t> uvw_triplet(u,v,w);

                    c->SetCoefficient(uvw_triplet_variable_map_vec[s][uvw_triplet], 1);

                    for(int_pair_set_t::iterator u_pair_set_iter = pair_map[s-1][u].begin(); u_pair_set_iter != pair_map[s-1][u].end(); ++u_pair_set_iter)
                    {
                        int_t u_u = (*u_pair_set_iter).first;
                        int_t u_v = (*u_pair_set_iter).second;

                        triplet<int_t,int_t,int_t> uvw_triplet(u_u,u_v,u);
                        c->SetCoefficient(uvw_triplet_variable_map_vec[s-1][uvw_triplet], -1);
                    }
                }
                //constraints 2b:
                {
                    stringstream strstr;
                    strstr << "C2b." << i++;
                    MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0, strstr.str());

                    triplet<int_t,int_t,int_t> uvw_triplet(u,v,w);

                    c->SetCoefficient(uvw_triplet_variable_map_vec[s][uvw_triplet], 1);

                    for(int_pair_set_t::iterator u_pair_set_iter = pair_map[s-1][v].begin(); u_pair_set_iter != pair_map[s-1][v].end(); ++u_pair_set_iter)
                    {
                        int_t v_u = (*u_pair_set_iter).first;
                        int_t v_v = (*u_pair_set_iter).second;

                        triplet<int_t,int_t,int_t> uvw_triplet(v_u,v_v,v);
                        c->SetCoefficient(uvw_triplet_variable_map_vec[s-1][uvw_triplet], -1);
                    }
                }

            }
        }
    }
}
