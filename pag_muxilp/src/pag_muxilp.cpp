#include "pag_muxilp.h"
#include <algorithm>
#include <typeinfo>

bool comparer_stage(const adder_graph_base_node_t* first, const adder_graph_base_node_t* second )
{
    return (first->stage > second->stage);
}

bool output_reg_selector(adder_graph_base_node_t*& t){
    return ( is_a<register_node_t>(*t) && static_cast<register_node_t*>(t)->stage == static_cast<register_node_t*>(t)->input->stage );
}

pag_muxilp::pag_muxilp(string complete_graph,string instance_name, bool quiet)
{
    instance = instance_name;
    source_graph = new adder_graph_t();
    if(source_graph->parse_to_graph(complete_graph))
    {
        //source_graph->check_and_correct(complete_graph);
        //source_graph->nodes_list.remove_if(&output_reg_selector);
        //source_graph->normalize_graph();
        source_graph->check_and_correct(complete_graph);
        no_conf = source_graph->nodes_list.front()->output_factor.size();
        source_graph->drawdot(instance_name+"_sa_input.dot",quiet);
    }
    else throw runtime_error("parse failed");
}


pag_muxilp::~pag_muxilp()
{
    delete source_graph;
}

void pag_muxilp::generate_input_file(string input_string, string instance_name)
{
    stringstream outstream;
    outstream << input_string;

    ofstream file;
    string filename = instance_name;

    file.open(filename.c_str());
    file << outstream.str();
    file.close();
}


int pag_muxilp::getMuxIn(){
    int mux_count = 0;
    // at the moment only LUTs for used MUX inputs are considered
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (is_a<mux_node_t>(*(*it))) {
            // get maximum value of MUX
            int max_output = 0;
            for (unsigned int i=0; i<((mux_node_t*)(*it))->output_factor.size(); i++)
            {
                if (*std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()))
                    max_output = *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end());
            }

            //get required MUX inputs by evaluating set size of input,shift pairs
            set<pair<adder_graph_base_node_t*,int> > unique_inputs;
            for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                if (((mux_node_t*)(*it))->inputs[i] != nullptr){
                    unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
                  //  cout << "insert: " << ((mux_node_t*)(*it))->inputs[i] <<" shift "<< ((mux_node_t*)(*it))->input_shifts[i] <<  endl;
                }
            }
            //luts+=ceil((double)((mux_node_t*)(*it))->inputs.size()/4) * (floor(log2(max_output))+1) ;
            if (unique_inputs.size()>=2){ //mux is not a mux, change pointers
                {
                mux_count+=unique_inputs.size()-1;
               // cout << "MUX: " << ((mux_node_t*)(*it))->output_factor << " stage " << ((mux_node_t*)(*it))->stage << " adds " << unique_inputs.size()-1 << endl;
                }
            }
            unique_inputs.clear();
        }
        if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
        }
        if (is_a<adder_subtractor_node_t>(*(*it))) {
        }
        if (is_a<register_node_t>(*(*it))){
        }
    }
    return mux_count;
}

void pag_muxilp::generateOptimalMUXDistribution(){
    s.quiet=true; // disable solver output
    constra = constra + "\nsubject to\n";
    //cout << endl << "Constraints:" << endl;
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (successors_map[*it].size() == 0){ //output_node found
            configuration_output = (*it);
            if (is_a<mux_node_t>(*(*it))) {
                //go through tree to get pathes
                for (unsigned int i=0; i<no_conf; i++)
                {
                    string temp_string;
                    ILP::Term path_term;
                    temp_string = " so"+to_string(i)+" = ";
                    string name = "so"+to_string(i);
                    variables.insert(name);
                    ILP::Variable shift_var = ILP::newIntegerVariable(name);
                    // var.insert(shift_var);

                    auto p = var.insert({name,shift_var});
                    if(!p.second) std::cerr << name << " already defined - l105" << std::endl;


                    path_term.add(shift_var,1);
                    travelPath(i,((mux_node_t*)(*it))->inputs[i],temp_string, ((mux_node_t*)(*it))->input_shifts[i],path_term);
                    // cout << "found input node with value " << ((mux_node_t*)(*it))->inputs[i]->output_factor << endl;
                }

            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
                for (unsigned int config=0; config<no_conf; config++){
                    //cout << endl << "found output node with value " << (*it)->output_factor << " " << ((conf_adder_subtractor_node_t*)(*it))->inputs.size() << endl;
                    for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(*it))->inputs.size(); i++)
                    {
                        int path_value = ((conf_adder_subtractor_node_t*)(*it))->input_shifts[i];
                        string travel_string = " s" + to_string(((conf_adder_subtractor_node_t*)(*it))->stage) + to_string(i) + to_string(config) + " =";
                        string name= "s" + to_string(((conf_adder_subtractor_node_t*)(*it))->stage) + to_string(i) + to_string(config);
                        variables.insert(name);
                        ILP::Variable shift_var = ILP::newIntegerVariable(name);

                        auto p = var.insert({name,shift_var});
                        if(!p.second) std::cerr << name << " already defined - l126" << std::endl;

                        ILP::Term travel_term = shift_var;
                        travelPath(config,((conf_adder_subtractor_node_t*)(*it))->inputs[i],travel_string,path_value,travel_term);
                    }
                }

            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {
                for (unsigned int config=0; config<no_conf; config++){
                    //cout << endl << "found output node with value " << (*it)->output_factor << " " << ((adder_subtractor_node_t*)(*it))->inputs.size() << endl;
                    for (unsigned int i=0; i<((adder_subtractor_node_t*)(*it))->inputs.size(); i++)
                    {
                        int path_value = ((adder_subtractor_node_t*)(*it))->input_shifts[i];
                        string travel_string = " s" + to_string(((adder_subtractor_node_t*)(*it))->stage) + to_string(i) + to_string(config)+ " =";
                        string name ="s" + to_string(((adder_subtractor_node_t*)(*it))->stage) + to_string(i) + to_string(config);
                        variables.insert(name);
                        ILP::Variable shift_var = ILP::newIntegerVariable(name);
                        //var.insert(shift_var);

                        auto p = var.insert({name,shift_var});
                        if(!p.second) std::cerr << name << " already defined -l147" << std::endl;

                        ILP::Term travel_term = shift_var;
                        travelPath(config,((adder_subtractor_node_t*)(*it))->inputs[i],travel_string,path_value,travel_term);
                    }
                }
            }


            for (auto bins: variables){
                set<string> local_binaries;
                ILP::Term shift_con;
                ILP::Term shift_lim;
                ILP::Variable tmp_shift_var = var[bins];
                shift_lim.add(tmp_shift_var,1);
                constra = constra + bins + " - 0 " + "b" + "0" + bins;
                string name= "b0" + bins;
                shift_binaries.insert(name);
                ILP::Variable tmp_sh;
                if (shift_bin.find(name)==shift_bin.end())
                {
                    tmp_sh = ILP::newBinaryVariable(name);
                    auto p = shift_bin.insert({name,tmp_sh});
                    if(!p.second) std::cerr << name << " already defined -l170" << std::endl;
                }
                else
                {

                    tmp_sh = shift_bin[name];
                }
                //shift_bin.insert(tmp_sh);
                shift_con.add(tmp_sh,1);
                shift_lim.add(tmp_sh,-0);
                local_binaries.insert("b0" + bins);
                for (int sh=1;sh<18;sh++){
                    constra = constra +  " - " + to_string(sh) + " b" + to_string(sh) + bins;
                    name = "b"+to_string(sh)+bins;
                    shift_binaries.insert(name);
                    ILP::Variable tmp_sh;
                    if (shift_bin.find(name)==shift_bin.end())
                    {
                        tmp_sh = ILP::newBinaryVariable(name);
                        auto p = shift_bin.insert({name,tmp_sh});
                        if(!p.second) std::cerr << name << " already defined -l188" << std::endl;
                    }
                    else{

                        tmp_sh = shift_bin[name];
                    }
                    //shift_bin.insert(tmp_sh);
                    local_binaries.insert("b"+to_string(sh)+bins);
                    shift_con.add(tmp_sh,1);
                    shift_lim.add(tmp_sh,-sh);
                }
                constra = constra +  " = 0 \n";
                for (auto bin_cons: local_binaries){
                    constra = constra + bin_cons + " + ";
                }
                constra = constra.substr(0,constra.size()-2) + " = 1\n";
                s.addConstraint(shift_con == 1);
                s.addConstraint(shift_lim == 0);
            }

        }
    }
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (is_a<mux_node_t>(*(*it))) {
            mux_node_t* cur_mux = ((mux_node_t*)(*it));
            for (unsigned int i=0; i<cur_mux->inputs.size(); i++)
            {

                if (successors_map[*it].size() == 0){
                    for (unsigned int j=i+1; j<cur_mux->inputs.size(); j++){
                        if (cur_mux->inputs[i] != nullptr && cur_mux->inputs[i]==cur_mux->inputs[j]){
                            if (!(var.find("so"+to_string(i))==var.end())&&!(var.find("so"+to_string(j))==var.end())){
                                for (int sh=0;sh<18;sh++){
                                    ILP::Variable tmp_bin_var;
                                    stringstream  mb_name;
                                    mb_name << "mb" <<sh<< cur_mux->inputs[i] << "so";
                                    if (mux_bin.find(mb_name.str())==mux_bin.end())
                                    {
                                        tmp_bin_var = ILP::newBinaryVariable(mb_name.str());
                                        auto p = mux_bin.insert({mb_name.str(),tmp_bin_var});
                                        if(!p.second) std::cerr << "mb" << to_string(sh) << "so" << " already defined -l209" << std::endl;
                                    }
                                    else
                                        tmp_bin_var = mux_bin[mb_name.str()];
                                    ILP::Variable tmp_shift_var1 = shift_bin["b" +to_string(sh) + "so" + to_string(i)];
                                    ILP::Variable tmp_shift_var2 = shift_bin["b" +to_string(sh) + "so" + to_string(j)];
                                    ILP::Term t1; t1.add(tmp_shift_var1,-1);
                                    ILP::Term t2; t2.add(tmp_shift_var2,-1);
                                    constra = constra + mb_name.str() + " - " + "b" +to_string(sh) + "so" + to_string(i) + ">=0\n";
                                    s.addConstraint(tmp_bin_var + t1 >= 0);
                                    constra = constra + mb_name.str() + " - " + "b" +to_string(sh) + "so" + to_string(j) + ">=0\n";
                                    s.addConstraint(tmp_bin_var + t2 >= 0);
                                    mux_binaries.insert(mb_name.str());
                                }
                            }
                        }
                    }

                } else{
                    for (unsigned int j=i+1; j<cur_mux->inputs.size(); j++){
                        if (cur_mux->inputs[i] != nullptr && cur_mux->inputs[i]==cur_mux->inputs[j]){
                            //cout << "entered for " << cur_mux << " in stage " << cur_mux->stage << " which is mbxs"+to_string(cur_mux->stage+1)+"0"<<  endl;
                            for (int sh=0;sh<18;sh++){

                                ILP::Variable tmp_bin_var;

                                set<adder_graph_base_node_t*> succs;
                                succs = successors_map[cur_mux] ;
                                adder_graph_base_node_t* pred_element = *(succs.begin());
                                mux_node_t* pred_mux0;
                                mux_node_t* pred_mux1;
                                if (is_a<conf_adder_subtractor_node_t>(*pred_element)){
                                    pred_mux0 = ((mux_node_t*)(((conf_adder_subtractor_node_t*)(pred_element))->inputs[0]));
                                    pred_mux1 = ((mux_node_t*)(((conf_adder_subtractor_node_t*)(pred_element))->inputs[1]));
                                }
                                else{
                                    pred_mux0 = ((mux_node_t*)(((adder_subtractor_node_t*)(pred_element))->inputs[0]));
                                    pred_mux1 = ((mux_node_t*)(((adder_subtractor_node_t*)(pred_element))->inputs[1]));

                                }

                                if (pred_mux0 == cur_mux && !(var.find("s"+to_string(cur_mux->stage+1)+"0"+to_string(i))==var.end())&& !(var.find("s"+to_string(cur_mux->stage+1)+"0"+to_string(j))==var.end())){
                                    stringstream mb_name;
                                    mb_name << "mb"<<sh << "s"<<cur_mux->stage+1<<"0"<< cur_mux->inputs[i];
                                    if (mux_bin.find(mb_name.str())==mux_bin.end())
                                    {
                                        tmp_bin_var = ILP::newBinaryVariable(mb_name.str());
                                        auto p = mux_bin.insert({mb_name.str(),tmp_bin_var});
                                        if(!p.second) std::cerr << "mb" <<to_string(sh) << "s"<<to_string(cur_mux->stage+1)<<"0" << " already defined -l240" << std::endl;
                                    }
                                    else
                                    {

                                        tmp_bin_var = mux_bin[mb_name.str()];
                                    }
                                    ILP::Variable tmp_shift_var1 = shift_bin["b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"0" + to_string(i)];
                                    ILP::Variable tmp_shift_var2 = shift_bin["b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"0" + to_string(j)];
                                    ILP::Term t1; t1.add(tmp_shift_var1,-1);
                                    ILP::Term t2; t2.add(tmp_shift_var2,-1);
                                    constra = constra + mb_name.str() +  " - " + "b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"0" + to_string(i)  + ">=0\n";
                                    constra = constra + mb_name.str() +  " - " + "b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"0" + to_string(j) + ">=0\n";

                                    s.addConstraint(tmp_bin_var + t1 >= 0);
                                    s.addConstraint(tmp_bin_var + t2 >= 0);
                                    mux_binaries.insert(mb_name.str());
                                }

                                if (pred_mux1 == cur_mux &&!(var.find("s"+to_string(cur_mux->stage+1)+"1"+to_string(i))==var.end())&&!(var.find("s"+to_string(cur_mux->stage+1)+"1"+to_string(j))==var.end())){
                                    stringstream mb_name;
                                    mb_name << "mb"<<sh << "s"<<cur_mux->stage+1<<"1"<< cur_mux->inputs[i];

                                    if (mux_bin.find(mb_name.str())==mux_bin.end())
                                    {
                                        tmp_bin_var = ILP::newBinaryVariable(mb_name.str());
                                        auto p = mux_bin.insert({mb_name.str(),tmp_bin_var});
                                        if(!p.second) std::cerr << "mb" <<to_string(sh) << "s"<<to_string(cur_mux->stage+1)<<"1" << " already defined -l250" << std::endl;
                                    }
                                    else{

                                        tmp_bin_var = mux_bin[mb_name.str()];
                                    }


                                    ILP::Variable tmp_shift_var1 = shift_bin["b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"1" + to_string(i)];
                                    ILP::Variable tmp_shift_var2 = shift_bin["b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"1" + to_string(j)];
                                    ILP::Term t3; t3.add(tmp_shift_var1,-1);
                                    ILP::Term t4; t4.add(tmp_shift_var2,-1);
                                    constra = constra + mb_name.str() + " - " + "b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"1" + to_string(i) + ">=0\n";
                                    constra = constra + mb_name.str() + " - " + "b" +to_string(sh) + "s"+to_string(cur_mux->stage+1)+"1" + to_string(j) + ">=0\n";
                                    s.addConstraint(tmp_bin_var + t3 >= 0);
                                    s.addConstraint(tmp_bin_var + t4 >= 0);
                                    mux_binaries.insert(mb_name.str());
                                }
                            }
                        }
                    }

                }
            }

        }
    }
    string lp_objective ="";
    for (auto minimize_this : mux_binaries){
        lp_objective = lp_objective + minimize_this + " + ";
    }



    ILP::Term sum;
    for (auto bins: mux_bin){
        sum = sum + bins.second;
    }

    s.setObjective(ILP::minimize(sum));

    stringstream lp_problem;
    lp_problem << "minimize\n";
    lp_problem << lp_objective.substr(0,lp_objective.size()-2);
    lp_problem << constra;
    lp_problem << "\nGenerals\n";
    lp_problem << variables;
    lp_problem << "\nbinaries\n";
    lp_problem << shift_binaries << "\n";
    lp_problem << mux_binaries;

    generate_input_file(lp_problem.str(),"my.lp");
    s.writeLP("test.lp");
    //std::cout << s.showLP() << std::endl;

    // ILP::status stat =
    s.solve();

    /*  //print results
    std::cout << "The result is " << stat << std::endl;
    if(stat==ILP::status::OPTIMAL || stat==ILP::status::FEASIBLE)
    {
        ILP::Result r = s.getResult();

        for (auto v_res: var){
            std::cout << v_res.first << " = " << r.values[v_res.second] << std::endl;
        }

        //for(std::pair<const ILP::Variable,double> &p:r.values)
        {
            //std::cout << p.first << "=" << p.second << std::endl;
        }
    }*/

}

void pag_muxilp::travelPath(int config, adder_graph_base_node_t* pred, string temp_string, int path_value, ILP::Term path_term){

    if (is_a<mux_node_t>(*(pred))) {
        //cout << endl << "found output node with value " << (*it)->output_factor << " " << ((mux_node_t*)(*it))->inputs.size() << endl;
        path_value += ((mux_node_t*)(pred))->input_shifts[config];
        travelPath(config,((mux_node_t*)(pred))->inputs[config],temp_string,path_value,path_term);
    }
    if (is_a<conf_adder_subtractor_node_t>(*(pred))) {
        for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(pred))->inputs.size(); i++)
        {
            path_value += ((conf_adder_subtractor_node_t*)(pred))->input_shifts[i];
            string travel_string = " s" + to_string(((conf_adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config) + " +"  + temp_string;
            string name = "s" + to_string(((conf_adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config);
            ILP::Variable shift_var;
            if (var.find(name)==var.end())
            {
                shift_var = ILP::newIntegerVariable(name);
                auto p = var.insert({name,shift_var});
                if(!p.second) std::cerr << name << " already defined -325" << std::endl;}
            else
            {
                shift_var = var[name];
            }

            ILP::Term travel_term = path_term + shift_var;
            variables.insert("s" + to_string(((conf_adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config));
            travelPath(config,((conf_adder_subtractor_node_t*)(pred))->inputs[i],travel_string,path_value,travel_term);
        }
    }
    if (is_a<adder_subtractor_node_t>(*(pred))) {
        for (unsigned int i=0; i<((adder_subtractor_node_t*)(pred))->inputs.size(); i++)
        {
            path_value += ((adder_subtractor_node_t*)(pred))->input_shifts[i];
            string travel_string = " s" + to_string(((adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config)+ " +"  + temp_string;
            string name = "s" + to_string(((adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config);
            ILP::Variable shift_var;
            if (var.find(name)==var.end())
            {
                shift_var = ILP::newIntegerVariable(name);
                auto p = var.insert({name,shift_var});
                if(!p.second) std::cerr << name << " already defined -342" << std::endl;}
            else
            {
                shift_var = var[name];
            }


            ILP::Term travel_term = path_term + shift_var;
            variables.insert("s" + to_string(((adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config));
            //var.insert(ILP::newIntegerVariable("s" + to_string(((adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config)));
            travelPath(config,((adder_subtractor_node_t*)(pred))->inputs[i],travel_string,path_value,travel_term);
        }
    }
    if (is_a<input_node_t>(*(pred))) {
        constra = constra + temp_string + " " + to_string(path_value) + "\n";
        s.addConstraint(path_term == path_value);
    }
}


void pag_muxilp::applySolution(){
    res = s.getResult();
    vector<adder_graph_base_node_t*> outputs;
    outputs.resize(no_conf);
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){

        if (successors_map[*it].size() == 0){ //output_node found
            if (is_a<mux_node_t>(*(*it))) {
                //go through tree to get pathes
                for (unsigned int i=0; i<no_conf; i++)
                {

                    //cout << ((adder_graph_base_node_t*)(((mux_node_t*)(*it))->inputs[i]))->output_factor << endl;
                    string name = "so"+to_string(i);
                    if (is_a<conf_adder_subtractor_node_t>(*((mux_node_t*)(*it))->inputs[i]))
                    {
                        ((conf_adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]))->output_factor[i][0] = ((conf_adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]))->output_factor[i][0] * pow(2,(((mux_node_t*)(*it))->input_shifts[i]-res.values[var[name]]));
                        outputs[i] = ((conf_adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]));
                    }
                    if (is_a<adder_subtractor_node_t>(*((mux_node_t*)(*it))->inputs[i]))
                    {
                        ((adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]))->output_factor[i][0] = ((adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]))->output_factor[i][0] * pow(2,(((mux_node_t*)(*it))->input_shifts[i]-res.values[var[name]]));
                        outputs[i] = ((adder_subtractor_node_t*)(((mux_node_t*)(*it))->inputs[i]));
                    }
                    if (is_a<input_node_t>(*((mux_node_t*)(*it))->inputs[i])){
                         outputs[i] = ((input_node_t*)(((mux_node_t*)(*it))->inputs[i]));
                    }
                    ((mux_node_t*)(*it))->input_shifts[i] = res.values[var[name]];
                    setMUXPath(i,((mux_node_t*)(*it))->inputs[i]);

                }

            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
                for (unsigned int config=0; config<no_conf; config++){
                    outputs[config] = ((conf_adder_subtractor_node_t*)(*it));
                    for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(*it))->inputs.size(); i++)
                    {
                        setMUXPath(config,*it);
                    }
                }

            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {
                for (unsigned int config=0; config<no_conf; config++){
                    outputs[config] = ((adder_subtractor_node_t*)(*it));
                    for (unsigned int i=0; i<((adder_subtractor_node_t*)(*it))->inputs.size(); i++)
                    {
                        setMUXPath(config,*it);
                    }
                }
            }
        }
    }

    for (unsigned int c=0; c<no_conf; c++)
    {
       // cout << outputs[c]->output_factor[c][0] << endl;
        if (outputs[c]->output_factor[c][0]!=DONT_CARE){

            if (is_a<conf_adder_subtractor_node_t>(*(outputs[c]))) {
                if (((conf_adder_subtractor_node_t*)(outputs[c]))->input_is_negative[c][0])
                    outputs[c]->output_factor[c][0]= - updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) + updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);
                else if (((conf_adder_subtractor_node_t*)(outputs[c]))->input_is_negative[c][1])
                    outputs[c]->output_factor[c][0]= updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) - updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);
                else
                    outputs[c]->output_factor[c][0]= updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) + updateNodeValues((mux_node_t*)(((conf_adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);



            }
            if (is_a<adder_subtractor_node_t>(*(outputs[c]))) {
                if (((adder_subtractor_node_t*)(outputs[c]))->input_is_negative[0])
                    outputs[c]->output_factor[c][0]= - updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) + updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);
                else if (((adder_subtractor_node_t*)(outputs[c]))->input_is_negative[1])
                    outputs[c]->output_factor[c][0]= updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) - updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);
                else
                    outputs[c]->output_factor[c][0]= updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[0]),c) + updateNodeValues((mux_node_t*)(((adder_subtractor_node_t*)(outputs[c]))->inputs[1]),c);
            }
        }
    }

}


void pag_muxilp::setMUXPath(int config, adder_graph_base_node_t* pred){

    if (is_a<mux_node_t>(*(pred))) {
        for (unsigned int i=0; i<no_conf; i++)
        {
            if (((mux_node_t*)(pred))->inputs[i] != nullptr){
                setMUXPath(i,((mux_node_t*)(pred))->inputs[i]);
            }
        }
    }
    if (is_a<conf_adder_subtractor_node_t>(*(pred))) {
        conf_adder_subtractor_node_t* cur_add = ((conf_adder_subtractor_node_t*)(pred));
        for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(pred))->inputs.size(); i++)
        {
            mux_node_t* cur_mux = ((mux_node_t*)(cur_add->inputs[i]));
            string name = "s" + to_string(cur_add->stage) + to_string(i) + to_string(config);
            if (cur_mux->inputs[config] != nullptr){
                if(cur_mux->input_shifts[config]!=res.values[var[name]]){
                    cur_mux->input_shifts[config] =  res.values[var[name]];
                }
                setMUXPath(config,((conf_adder_subtractor_node_t*)(pred))->inputs[i]);
            }
        }
    }
    if (is_a<adder_subtractor_node_t>(*(pred))) {
        adder_subtractor_node_t* cur_add = ((adder_subtractor_node_t*)(pred));
        for (unsigned int i=0; i<((adder_subtractor_node_t*)(pred))->inputs.size(); i++)
        {
            mux_node_t* cur_mux = ((mux_node_t*)(cur_add->inputs[i]));
            // string name = "s" + to_string(((conf_adder_subtractor_node_t*)(pred))->stage) + to_string(i) + to_string(config);
            string name = "s" + to_string(cur_add->stage) + to_string(i) + to_string(config);
            if (cur_mux->inputs[config] != nullptr){
                if(cur_mux->input_shifts[config]!=res.values[var[name]]){
                    cur_mux->input_shifts[config] =  res.values[var[name]];
                }
                setMUXPath(config,((conf_adder_subtractor_node_t*)(pred))->inputs[i]);
            }
        }
    }
}

int pag_muxilp::getCost(int cost_model){
    switch (cost_model){ //1: FPGA MUX, 2: DAG FUSION, 3: 2:1 MUXes, 4: FPGA ALL
    case 1:{
        int mux_luts = 0;
        // at the moment only LUTs for used MUX inputs are considered
        for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
            if (is_a<mux_node_t>(*(*it))) {
                // get maximum value of MUX
                int max_output = 0;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end());
                }

                //get required MUX inputs by evaluating set size of input,shift pairs
                set<pair<adder_graph_base_node_t*,int> > unique_inputs;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                    if (((mux_node_t*)(*it))->inputs[i] != nullptr)
                        unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
                }
                //luts+=ceil((double)((mux_node_t*)(*it))->inputs.size()/4) * (floor(log2(max_output))+1) ;
                if (unique_inputs.size()>=2){ //mux is not a mux, change pointers
                    mux_luts +=ceil(((double)unique_inputs.size())/4) * (floor(log2(max_output))+1);
                }
                unique_inputs.clear();
            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {
            }
            if (is_a<register_node_t>(*(*it))){
            }
        }
        return mux_luts;}
    case 2:{
        int area = 0;
        // at the moment only LUTs for used MUX inputs are considered
        for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
            if (is_a<mux_node_t>(*(*it))) {
                // get maximum value of MUX
                int max_output = 0;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end());
                }

                //get required MUX inputs by evaluating set size of input,shift pairs
                set<pair<adder_graph_base_node_t*,int> > unique_inputs;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                    if (((mux_node_t*)(*it))->inputs[i] != nullptr)
                        unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
                }
                //luts+=ceil((double)((mux_node_t*)(*it))->inputs.size()/4) * (floor(log2(max_output))+1) ;
                if (unique_inputs.size()>=2){ //mux is not a mux, change pointers
                    area += ((floor(log2(max_output))+1)+INPUTBITWITH) * MUXSIZE;
                    if (unique_inputs.size()>2)
                        area += (unique_inputs.size()-2) * ((floor(log2(max_output))+1)+INPUTBITWITH) * MUXPLUS;
                }
                unique_inputs.clear();
            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
                int max_output = 0;
                for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end());
                }

                area += ((floor(log2(max_output))+1)+INPUTBITWITH)  * ADDSUBSIZE;
            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {

                int max_output = 0;
                for (unsigned int i=0; i<((adder_subtractor_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end());
                }

                area += ((floor(log2(max_output))+1)+INPUTBITWITH)  * ADDSIZE;
            }
            if (is_a<register_node_t>(*(*it))){
            }
        }
        return area;}

    case 3:{
        int mux_count = 0;
        // at the moment only LUTs for used MUX inputs are considered
        for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
            if (is_a<mux_node_t>(*(*it))) {
                // get maximum value of MUX
                int max_output = 0;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end());
                }

                //get required MUX inputs by evaluating set size of input,shift pairs
                set<pair<adder_graph_base_node_t*,int> > unique_inputs;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                    if (((mux_node_t*)(*it))->inputs[i] != nullptr)
                        unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
                }
                //luts+=ceil((double)((mux_node_t*)(*it))->inputs.size()/4) * (floor(log2(max_output))+1) ;
                if (unique_inputs.size()>=2){ //mux is not a mux, change pointers
                    mux_count+=unique_inputs.size()-1;
                }
                unique_inputs.clear();
            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {
            }
            if (is_a<register_node_t>(*(*it))){
            }
        }
        return mux_count;}

    case 4:{
        int luts = 0;
        // at the moment only LUTs for used MUX inputs are considered
        for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
            if (is_a<mux_node_t>(*(*it))) {
                // get maximum value of MUX
                int max_output = 0;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((mux_node_t*)(*it))->output_factor[i].begin(),((mux_node_t*)(*it))->output_factor[i].end());
                }

                //get required MUX inputs by evaluating set size of input,shift pairs
                set<pair<adder_graph_base_node_t*,int> > unique_inputs;
                for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                    if (((mux_node_t*)(*it))->inputs[i] != nullptr)
                        unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
                }
                //luts+=ceil((double)((mux_node_t*)(*it))->inputs.size()/4) * (floor(log2(max_output))+1) ;
                if (unique_inputs.size()>=2){ //mux is not a mux, change pointers
                    luts +=ceil(((double)unique_inputs.size())/4) * (floor(log2(max_output))+1+INPUTBITWITH);
                }
                else //node will be a register
                {
                    //luts += (floor(log2(max_output))+1)/2;
                }
                unique_inputs.clear();
            }
            if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
                int max_output = 0;
                for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((conf_adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((conf_adder_subtractor_node_t*)(*it))->output_factor[i].end());
                }
                luts +=(floor(log2(max_output))+1+INPUTBITWITH);
            }
            if (is_a<adder_subtractor_node_t>(*(*it))) {
                int max_output = 0;
                for (unsigned int i=0; i<((adder_subtractor_node_t*)(*it))->output_factor.size(); i++)
                {
                    if (*std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end()) != DONT_CARE && max_output < *std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end()))
                        max_output = *std::max_element(((adder_subtractor_node_t*)(*it))->output_factor[i].begin(),((adder_subtractor_node_t*)(*it))->output_factor[i].end());
                }
                luts +=(floor(log2(max_output))+1+INPUTBITWITH);
            }
            if (is_a<register_node_t>(*(*it))){
            }
        }
        return luts;}
    }
    return 0;
}



void pag_muxilp::fill_successors_map(){
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (is_a<mux_node_t>(*(*it))) {
            for (unsigned int i=0;i<((mux_node_t*)(*it))->inputs.size();i++)
            {
                if (((mux_node_t*)(*it))->inputs[i]!= nullptr)
                {
                    set<adder_graph_base_node_t*> tmp_set;
                    map<adder_graph_base_node_t*,set<adder_graph_base_node_t*>>::const_iterator search_it = successors_map.find((((mux_node_t*)(*it))->inputs[i]));
                    if (search_it!=successors_map.end())
                    {
                        tmp_set = successors_map[(((mux_node_t*)(*it))->inputs[i])];
                        tmp_set.insert((*it));
                    }
                    else
                    {
                        tmp_set.insert((*it));
                    }
                    //cout << "successor of " <<(((adder_subtractor_node_t*)(*it))->inputs[i])<< " is "<< (*it) << endl;
                    successors_map[(((mux_node_t*)(*it))->inputs[i])]=tmp_set;
                }
            }
        }
        if (is_a<conf_adder_subtractor_node_t>(*(*it))) {
            for (unsigned int i=0;i<((conf_adder_subtractor_node_t*)(*it))->inputs.size();i++)
            {
                if (((conf_adder_subtractor_node_t*)(*it))->inputs[i]!= nullptr)
                {
                    set<adder_graph_base_node_t*> tmp_set;
                    map<adder_graph_base_node_t*,set<adder_graph_base_node_t*>>::const_iterator search_it = successors_map.find((((conf_adder_subtractor_node_t*)(*it))->inputs[i]));
                    if (search_it!=successors_map.end())
                    {
                        tmp_set = successors_map[(((conf_adder_subtractor_node_t*)(*it))->inputs[i])];
                        tmp_set.insert((*it));
                    }
                    else
                    {
                        tmp_set.insert((*it));
                    }
                    //cout << "successor of " <<(((adder_subtractor_node_t*)(*it))->inputs[i])<< " is "<< (*it) << endl;
                    successors_map[(((conf_adder_subtractor_node_t*)(*it))->inputs[i])]=tmp_set;
                }
            }
        }
        if (is_a<adder_subtractor_node_t>(*(*it))) {
            for (unsigned int i=0;i<((adder_subtractor_node_t*)(*it))->inputs.size();i++)
            {
                if (((adder_subtractor_node_t*)(*it))->inputs[i]!= nullptr)
                {
                    set<adder_graph_base_node_t*> tmp_set;
                    map<adder_graph_base_node_t*,set<adder_graph_base_node_t*>>::const_iterator search_it = successors_map.find((((adder_subtractor_node_t*)(*it))->inputs[i]));
                    if (search_it!=successors_map.end())
                    {
                        tmp_set = successors_map[(((adder_subtractor_node_t*)(*it))->inputs[i])];
                        tmp_set.insert((*it));
                    }
                    else
                    {
                        tmp_set.insert((*it));
                    }
                    //cout << "successor of " <<(((adder_subtractor_node_t*)(*it))->inputs[i])<< " is "<< (*it) << endl;
                    successors_map[(((adder_subtractor_node_t*)(*it))->inputs[i])]=tmp_set;
                }
            }
        }
        if (is_a<register_node_t>(*(*it))){
            // cerr << "Registers are not yet supported"; }
        }
        //cout << successors_map.size() << endl;
        //for (auto it_map : successors_map)
        // cout << it_map.first << " is predecessor of " << it_map.second << endl;
    }
}

void pag_muxilp::addNonMux(){ //add mux on each in and remove balancing registers
    adder_graph_base_node_t* output = nullptr;
    adder_graph_base_node_t* input = nullptr;
    map<vector<vector<int64_t>>,mux_node_t*> added_muxes;
    set<adder_graph_base_node_t*> tmp_nodes_list;
    set<adder_graph_base_node_t*> remove_nodes_list;

    fill_successors_map();

    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (successors_map[*it].size() == 0)
            output = (*it);
        if (is_a<input_node_t>(*(*it))) input = *it;
        if (is_a<conf_adder_subtractor_node_t>(*(*it))) {

            for (unsigned int i=0;i<((conf_adder_subtractor_node_t*)(*it))->inputs.size();i++)
            { //Remove Registers

                if (is_a<register_node_t>(*(((conf_adder_subtractor_node_t*)(*it))->inputs[i]))){ //remove it
                    register_node_t* tmp_r = (register_node_t*)((conf_adder_subtractor_node_t*)(*it))->inputs[i];
                    if (!is_a<register_node_t>(*(tmp_r->input))){
                        ((conf_adder_subtractor_node_t*)(*it))->inputs[i] = tmp_r->input;
                        ((conf_adder_subtractor_node_t*)(*it))->input_shifts[i] = ((conf_adder_subtractor_node_t*)(*it))->input_shifts[i] + ((register_node_t*)(tmp_r))->input_shift;
                        remove_nodes_list.insert(tmp_r);
                    }
                }
            }

            for (unsigned int i=0;i<((conf_adder_subtractor_node_t*)(*it))->inputs.size();i++)
            {
                if(!is_a<mux_node_t>(*(((conf_adder_subtractor_node_t*)(*it))->inputs[i]))){
                    //Add a MUX here
                    // if (added_muxes.find(shift_output_factor((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((conf_adder_subtractor_node_t*)(*it))->input_shifts[i]))==added_muxes.end()){
                    if (added_muxes.find((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor)==added_muxes.end()){
                        mux_node_t* t_mux = new mux_node_t();
                        //t_mux->output_factor = shift_output_factor((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((conf_adder_subtractor_node_t*)(*it))->input_shifts[i]);
                        t_mux->output_factor = (((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor;
                        for (unsigned int j=0;j<no_conf;j++){
                            if ((((conf_adder_subtractor_node_t*)(*it))->output_factor[j][0] != DONT_CARE))
                                t_mux->inputs.push_back((((conf_adder_subtractor_node_t*)(*it))->inputs[i]));
                            else
                                t_mux->inputs.push_back(nullptr);
                            t_mux->input_shifts.push_back((((conf_adder_subtractor_node_t*)(*it))->input_shifts[i]));//((conf_adder_subtractor_node_t*)(*it))->input_shifts[i]);
                            t_mux->stage = ((conf_adder_subtractor_node_t*)(*it))->stage-1;
                            t_mux->output_factor[j][0] = t_mux->output_factor[j][0] * pow(2,(((conf_adder_subtractor_node_t*)(*it))->input_shifts[i]));
                        }
                        tmp_nodes_list.insert(t_mux);
                        // added_muxes[shift_output_factor((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((conf_adder_subtractor_node_t*)(*it))->input_shifts[i])]=t_mux;
                        // added_muxes[(((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor]=t_mux;
                        ((conf_adder_subtractor_node_t*)(*it))->inputs[i] = t_mux;
                        ((conf_adder_subtractor_node_t*)(*it))->input_shifts[i] = 0;
                    }
                    else{
                        //    ((conf_adder_subtractor_node_t*)(*it))->inputs[i] = added_muxes[shift_output_factor((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((conf_adder_subtractor_node_t*)(*it))->input_shifts[i])];
                        ((conf_adder_subtractor_node_t*)(*it))->inputs[i] = added_muxes[(((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor];

                    }
                    //cout << (*it)->output_factor << "says: MUX has to be placed here" << endl;

                }
            }
        }

        if (is_a<adder_subtractor_node_t>(*(*it))) {
            for (unsigned int i=0;i<((adder_subtractor_node_t*)(*it))->inputs.size();i++)
            { //Remove Registers
                if (is_a<register_node_t>(*(((adder_subtractor_node_t*)(*it))->inputs[i]))){ //remove it
                    register_node_t* tmp_r = (register_node_t*)((adder_subtractor_node_t*)(*it))->inputs[i];
                    ((adder_subtractor_node_t*)(*it))->inputs[i] =tmp_r->input;
                    ((adder_subtractor_node_t*)(*it))->input_shifts[i] = ((adder_subtractor_node_t*)(*it))->input_shifts[i] + ((register_node_t*)(tmp_r))->input_shift;
                    remove_nodes_list.insert(tmp_r);
                }
            }

            for (unsigned int i=0;i<((adder_subtractor_node_t*)(*it))->inputs.size();i++)
            {
                if(!is_a<mux_node_t>(*(((adder_subtractor_node_t*)(*it))->inputs[i]))){

                    //                    if (added_muxes.find(shift_output_factor((((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((adder_subtractor_node_t*)(*it))->input_shifts[i]))==added_muxes.end()){
                    if (added_muxes.find((((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor)==added_muxes.end()){

                        mux_node_t* t_mux = new mux_node_t();
                        //   t_mux->output_factor = shift_output_factor((((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((adder_subtractor_node_t*)(*it))->input_shifts[i]);
                        t_mux->output_factor = (((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor;
                        for (unsigned int j=0;j<no_conf;j++){
                            if ((((adder_subtractor_node_t*)(*it))->output_factor[j][0] != DONT_CARE))
                                t_mux->inputs.push_back((((adder_subtractor_node_t*)(*it))->inputs[i]));
                            else
                                t_mux->inputs.push_back(nullptr);

                            t_mux->input_shifts.push_back(((adder_subtractor_node_t*)(*it))->input_shifts[i]);//((adder_subtractor_node_t*)(*it))->input_shifts[i]);
                            t_mux->stage = ((conf_adder_subtractor_node_t*)(*it))->stage-1;
                            t_mux->output_factor[j][0] = t_mux->output_factor[j][0] * pow(2,((adder_subtractor_node_t*)(*it))->input_shifts[i]);
                        }
                        tmp_nodes_list.insert(t_mux);
                        //  added_muxes[shift_output_factor((((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((adder_subtractor_node_t*)(*it))->input_shifts[i])]=t_mux;
                        //added_muxes[(((adder_subtractor_node_t*)(*it))->inputs[i])->output_factor]=t_mux;
                        ((adder_subtractor_node_t*)(*it))->inputs[i] = t_mux;
                        ((adder_subtractor_node_t*)(*it))->input_shifts[i] = 0;
                    }
                    else{
                        //  ((adder_subtractor_node_t*)(*it))->inputs[i] = added_muxes[shift_output_factor((((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor,(int)((conf_adder_subtractor_node_t*)(*it))->input_shifts[i])];
                        ((adder_subtractor_node_t*)(*it))->inputs[i] = added_muxes[(((conf_adder_subtractor_node_t*)(*it))->inputs[i])->output_factor];
                    }
                    //cout << (*it)->output_factor << "says: MUX has to be placed here" << endl;
                }
            }
        }
    }
    // source_graph->drawdot("remR.dot",false);
    for (set<adder_graph_base_node_t*>::iterator it = tmp_nodes_list.begin(); it!=tmp_nodes_list.end();++it){
        source_graph->nodes_list.push_back(*it);
        // cout << "Add MUX" << endl;
    }
    // source_graph->drawdot("addedM.dot",false);
    for (set<adder_graph_base_node_t*>::iterator it = remove_nodes_list.begin(); it!=remove_nodes_list.end();++it){
        source_graph->nodes_list.remove(*it);
        // cout << "Remove Reg" << endl;
    }


    fill_successors_map();
    if (input != nullptr)
        updateStagesASAP(input,0);
    else throw runtime_error("No input present");

    //cout << "output stage is "<<output->stage << endl;
    if (output != nullptr)
        updateStagesALAP(output);
    else throw runtime_error("No output present");
}

void pag_muxilp::removeNonMux(){
    list<adder_graph_base_node_t*> toberemoved;
    // at the moment only LUTs for used MUX inputs are considered
    for (list<adder_graph_base_node_t*>::iterator it = this->source_graph->nodes_list.begin(); it!=this->source_graph->nodes_list.end();++it){
        if (is_a<mux_node_t>(*(*it))) {
            //get required MUX inputs by evaluating set size of input,shift pairs
            set<pair<adder_graph_base_node_t*,int> > unique_inputs;
            for (unsigned int i=0; i<((mux_node_t*)(*it))->inputs.size(); i++){
                if (((mux_node_t*)(*it))->inputs[i] != nullptr)
                    unique_inputs.insert(make_pair(((mux_node_t*)(*it))->inputs[i],((mux_node_t*)(*it))->input_shifts[i]));
            }
            if (unique_inputs.size()==1){ //mux is not a mux, change pointers
                fill_successors_map();
                //cout << "size is: " <<successors_map.size() << endl;
                set<adder_graph_base_node_t*> cur_succ;
                cur_succ = successors_map[*it];
                for (auto succs: cur_succ){
                    if (is_a<mux_node_t>(*succs)) {
                        for (unsigned int i=0; i<((mux_node_t*)(succs))->inputs.size();i++)
                        {
                            if (((adder_graph_base_node_t*)(((mux_node_t*)(succs))->inputs[i]))->output_factor == (*it)->output_factor){
                                ((mux_node_t*)(succs))->inputs[i] = unique_inputs.begin()->first;
                                ((mux_node_t*)(succs))->input_shifts[i] = ((mux_node_t*)(succs))->input_shifts[i] + unique_inputs.begin()->second;
                            }
                        }
                    }
                    if (is_a<conf_adder_subtractor_node_t>(*succs)) {
                        //cout << "Found " << succs << " with " << ((conf_adder_subtractor_node_t*)(succs))->inputs.size() << " inputs" << endl;
                        for (unsigned int i=0; i<((conf_adder_subtractor_node_t*)(succs))->inputs.size();i++)
                        {
                            // cout << "of 1: " << ((adder_graph_base_node_t*)(((conf_adder_subtractor_node_t*)(succs))->inputs[i]))->output_factor << endl;
                            // cout << "of 2: " << (*it)->output_factor << endl;
                            if (((adder_graph_base_node_t*)(((conf_adder_subtractor_node_t*)(succs))->inputs[i]))->output_factor == (*it)->output_factor){
                                ((conf_adder_subtractor_node_t*)(succs))->inputs[i] = unique_inputs.begin()->first;
                                ((conf_adder_subtractor_node_t*)(succs))->input_shifts[i] = ((conf_adder_subtractor_node_t*)(succs))->input_shifts[i] +  unique_inputs.begin()->second;
                                // cout << "moving " << succs << " " << *it << " to " << unique_inputs.begin()->first << endl;
                            }
                        }
                    }
                    if (is_a<adder_subtractor_node_t>(*succs)) {
                        for (unsigned int i=0; i<((adder_subtractor_node_t*)(succs))->inputs.size();i++)
                        {
                            if (((adder_graph_base_node_t*)(((adder_subtractor_node_t*)(succs))->inputs[i]))->output_factor == (*it)->output_factor){
                                ((adder_subtractor_node_t*)(succs))->inputs[i] = unique_inputs.begin()->first;
                                ((adder_subtractor_node_t*)(succs))->input_shifts[i] = ((adder_subtractor_node_t*)(succs))->input_shifts[i] + unique_inputs.begin()->second;

                            }
                        }
                    }
                    if (is_a<register_node_t>(*succs)){
                        //cerr << "Registers are not yet supported";
                    }

                }
                toberemoved.push_back(*it);
            }
        }

    }
    for (auto remover: toberemoved){
        //cout << "removing non mux: " << remover << endl;
        source_graph->nodes_list.remove(remover);
    }
}

int pag_muxilp::updateNodeValues(adder_graph_base_node_t* node, int config){
    //cout << node->output_factor << " processed for config " << config << " which is a " ;
    if (is_a<mux_node_t>(*node)) {
        //cout << "a mux_node_t" << endl;
        if (((mux_node_t*)(node))->inputs[config]!=nullptr){
            if(is_a<conf_adder_subtractor_node_t>(*(((mux_node_t*)(node))->inputs[config])))
                ((mux_node_t*)(node))->output_factor[config][0] = updateNodeValues((conf_adder_subtractor_node_t*)((((mux_node_t*)(node))->inputs[config])),config)*pow(2,((mux_node_t*)(node))->input_shifts[config]);
            else
                ((mux_node_t*)(node))->output_factor[config][0] = updateNodeValues((adder_subtractor_node_t*)((((mux_node_t*)(node))->inputs[config])),config)*pow(2,((mux_node_t*)(node))->input_shifts[config]);
        }
        return  ((mux_node_t*)(node))->output_factor[config][0];
    }
    if (is_a<conf_adder_subtractor_node_t>(*node)){
        // cout << "a conf_adder_subtractor_node_t" << endl;
        if (((conf_adder_subtractor_node_t*)(node))->output_factor[config][0] != DONT_CARE){
            if (((conf_adder_subtractor_node_t*)(node))->input_is_negative[config][0])
                ((conf_adder_subtractor_node_t*)(node))->output_factor[config][0] = - updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[0])),config) +  updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[1])),config);
            else if (((conf_adder_subtractor_node_t*)(node))->input_is_negative[config][1])
                ((conf_adder_subtractor_node_t*)(node))->output_factor[config][0] = updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[0])),config) -  updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[1])),config);
            else
                ((conf_adder_subtractor_node_t*)(node))->output_factor[config][0] = updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[0])),config) +  updateNodeValues(((mux_node_t*)(((conf_adder_subtractor_node_t*)(node))->inputs[1])),config);
        }
        return ((conf_adder_subtractor_node_t*)(node))->output_factor[config][0];
    }
    if (is_a<adder_subtractor_node_t>(*node)){
        // cout << "a adder_subtractor_node_t" << endl;
        if (((adder_subtractor_node_t*)(node))->output_factor[config][0] != DONT_CARE){
            if (((adder_subtractor_node_t*)(node))->input_is_negative[0])
                ((adder_subtractor_node_t*)(node))->output_factor[config][0] = - updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[0])),config) +  updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[1])),config);
            else if (((adder_subtractor_node_t*)(node))->input_is_negative[1])
                ((adder_subtractor_node_t*)(node))->output_factor[config][0] = updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[0])),config) -  updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[1])),config);
            else
                ((adder_subtractor_node_t*)(node))->output_factor[config][0] = updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[0])),config) +  updateNodeValues(((mux_node_t*)(((adder_subtractor_node_t*)(node))->inputs[1])),config);
        }
        return ((adder_subtractor_node_t*)(node))->output_factor[config][0];
    }
    if (is_a<input_node_t>(*node)){
        return 1;
    }
    else
        return 0;

}

void pag_muxilp::updateStagesASAP(adder_graph_base_node_t* node, int p_stage){

    node->stage = max(node->stage,p_stage);
    //cout << "updated " << node->output_factor << " to " << node->stage << endl;
    set<adder_graph_base_node_t*> cur_succ;
    cur_succ = successors_map[node];
    for(auto it: cur_succ){
        updateStagesASAP(it,p_stage+1);
    }

}

void pag_muxilp::updateStagesALAP(adder_graph_base_node_t* node){

    // cout << "stage of" << node->output_factor << " is " << node->stage << endl;
    if (is_a<conf_adder_subtractor_node_t>(*node)){
        ((conf_adder_subtractor_node_t*)(node))->inputs[0]->stage =  node->stage-1;
        ((conf_adder_subtractor_node_t*)(node))->inputs[1]->stage =  node->stage-1;
        updateStagesALAP(((conf_adder_subtractor_node_t*)(node))->inputs[0]);
        updateStagesALAP(((conf_adder_subtractor_node_t*)(node))->inputs[1]);
    }
    else if (is_a<adder_subtractor_node_t>(*node)){
        ((adder_subtractor_node_t*)(node))->inputs[0]->stage =  node->stage-1;
        ((adder_subtractor_node_t*)(node))->inputs[1]->stage =  node->stage-1;
        updateStagesALAP(((conf_adder_subtractor_node_t*)(node))->inputs[0]);
        updateStagesALAP(((conf_adder_subtractor_node_t*)(node))->inputs[1]);
    }
    else if (is_a<mux_node_t>(*node)){
        for(auto it : ((mux_node_t*)(node))->inputs){
            if (it!=nullptr){
                //  it->stage =  node->stage-1;

            }
        }
        for(auto it : ((mux_node_t*)(node))->inputs){
            if (it!=nullptr){
                updateStagesALAP(((adder_graph_base_node_t*)(it)));
            }
        }
    }
}
