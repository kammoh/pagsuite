/* ADDER_GRAPH.cpp
 * Version:
 * Datum: 20.11.2014
 */

#include "adder_graph.h"
#include "paglib_copa.h"
#include <stdexcept>
#include <algorithm>

    adder_graph_t::~adder_graph_t(){

    }

    void adder_graph_t::clear(){
        while(!nodes_list.empty()){
            delete nodes_list.back();
            nodes_list.pop_back();
        }
    }

  /* pipeline the adder graph KONRAD*/
  void adder_graph_t::pipeline_graph()
  {
      adder_graph_t mygraph = *this;
      std::map<pair<vector<vector<int64_t> >,int>,adder_graph_base_node_t*> tmp_reg_list;

      for(std::list<adder_graph_base_node_t*>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {
          if (is_a<adder_subtractor_node_t>(*(*it)))
          {
             adder_subtractor_node_t* t = (adder_subtractor_node_t*)(*it); //
             for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k)
             {
                 int diff = (*it)->stage-t->inputs[k]->stage; //Differenz der Stufen
                 for (int j=1; j<diff;++j)
                 {
                     register_node_t* tmp = new register_node_t;
                     tmp->stage = t->stage-j;
                     tmp->output_factor = t->inputs[k]->output_factor;
                     if (j==1)
                     {
                         if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL) //Register existiert noch nicht
                         {
                             tmp->input = t->inputs[k];
                             t->inputs[k] = tmp;
                             tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                         }
                         else
                         {
                             t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                         }
                     }
                     else if (j==diff-1)
                     {

                         if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                         {
                             tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                             tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                             ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                         }
                         else
                         {
                             ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                         }
                     }
                     else
                     {
                         if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                         {
                             tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                             tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                             ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                         }
                         else
                         {
                             ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                         }
                     }
                 }
             }
         }
         else if (is_a<mux_node_t>(*(*it)))
         {
             mux_node_t* t = (mux_node_t*)(*it); //
             for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k){
                 if (t->inputs[k] != NULL){
                     int diff = (*it)->stage-t->inputs[k]->stage; //Differenz der Stufen
                     for (int j=1; j<diff;++j)
                     {
                         register_node_t* tmp = new register_node_t;
                         tmp->stage = t->stage-j;
                         tmp->output_factor = t->inputs[k]->output_factor;
                         if (j==1)
                         {
                             if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL) //Register existiert noch nicht
                             {
                                 tmp->input = t->inputs[k];
                                 t->inputs[k] = tmp;
                                 tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                             }
                             else
                             {
                                 t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                             }
                         }
                         else if (j==diff-1)
                         {

                             if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                             {
                                 tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                                 tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                                 ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                             }
                             else
                             {
                                 ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                             }
                         }
                         else
                         {
                             if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                             {
                                 tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                                 tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                                 ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                             }
                             else
                             {
                                 ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                             }
                         }
                     }
                 }
             }
         }
         else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
         {
              conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)(*it); //conf_adder_subtractor_node_t*
              for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k){
                  int diff = (*it)->stage-t->inputs[k]->stage; //Differenz der Stufen
                  for (int j=1; j<diff;++j)
                  {
                      register_node_t* tmp = new register_node_t;
                      tmp->stage = t->stage-j;
                      tmp->output_factor = t->inputs[k]->output_factor;
                      if (j==1)
                      {
                          if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL) //Register existiert noch nicht
                          {
                              tmp->input = t->inputs[k];
                              t->inputs[k] = tmp;
                              tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                          }
                          else
                          {
                              t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                          }
                      }
                      else if (j==diff-1)
                      {

                          if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                          {
                              tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                              tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                              ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                          }
                          else
                          {
                              ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                          }
                      }
                      else
                      {
                          if ( tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] == NULL)
                          {
                              tmp->input = ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input;
                              tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)] = tmp;
                              ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp;
                          }
                          else
                          {
                              ((register_node_t*)(tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,(tmp->stage)+1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >,int>(t->inputs[k]->output_factor,tmp->stage)];
                          }
                      }
                  }
              }

      }
   }

      for (std::map<pair<vector<vector<int64_t> >,int>,adder_graph_base_node_t*>::iterator iter = tmp_reg_list.begin(), iter_end = tmp_reg_list.end(); iter != iter_end; ++iter)
      {
          mygraph.nodes_list.push_back((*iter).second);
      }
      std::cout << "Finished Pipelining" << std::endl;
  }

  /* plot the adder graph to dot KONRAD*/
  void adder_graph_t::drawdot(string filename,bool quiet) {
      adder_graph_t mygraph = *this;
      //string filename = "./k_adder_graph.dot";

      FILE * graphfilepointer = NULL;

      graphfilepointer = fopen(filename.c_str(),"w");

      time_t mytime;
      time(&mytime);
      fprintf(graphfilepointer,"// File generated:  %s\n",ctime(&mytime));
      fprintf(graphfilepointer,"digraph DAG {\n");
      std::map<string,int> realized_edges;

      // plot nodes
      for(std::list<adder_graph_base_node_t*>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end ; ++it)
      {
          stringstream node_string;
          node_string << "node" << (*it) << "[label=\"";
          for(int i = 0,i_end=(int)(*it)->output_factor.size(); i < i_end; ++i){
              for(int j = 0,j_end=(int)(*it)->output_factor[i].size(); j < j_end; ++j){
                  if (j == (int)(*it)->output_factor[i].size()-1){
                      if ((*it)->output_factor[i][j] != DONT_CARE)
                      {
                          node_string << (*it)->output_factor[i][j];
                      }
                      else
                      {
                          node_string << "-";
                          break;
                      }
                  }
                  else{
                      if ((*it)->output_factor[i][j] != DONT_CARE)
                      {
                          node_string << (*it)->output_factor[i][j] << "," ;
                      }
                      else
                      {
                          node_string << "-";
                          break;
                      }
                  }
              }
              if (i != (int)(*it)->output_factor.size()-1)
              node_string << "\\" << "n" ;
          }
          double height = 0.3*(*it)->output_factor.size();
          node_string << "\",fontsize=12,shape=" ;
          if (is_a<input_node_t>(*(*it)))
          {
              node_string << "ellipse];\n";
          }
          if (is_a<adder_subtractor_node_t>(*(*it)))
          {
               node_string << "ellipse,height = " << height<< ",width=.4];\n";
          }
          if (is_a<mux_node_t>(*(*it)))
          {
              node_string << "polygon,sides=4,distortion=.7,fixedsize=\"true\",height = " << height-0.3*height<< ",width=1.2];\n";
          }
          if (is_a<conf_adder_subtractor_node_t>(*(*it)))
          {
               node_string << "ellipse,height =" << height<< ",width=.4];\n";
          }
          if (is_a<register_node_t>(*(*it)))
          {
               node_string << "box];\n";
          }
          fprintf(graphfilepointer,"%s",node_string.str().c_str());
      }

      for(std::list<adder_graph_base_node_t*>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {

         if (is_a<adder_subtractor_node_t>(*(*it)))
         {
              for (int i = 0, i_end = (int)((adder_subtractor_node_t*)(*it))->inputs.size(); i < i_end;++i){
                  stringstream edge_string;
                  edge_string << "node" << ((adder_subtractor_node_t*)(*it))->inputs[i] << " -> node" << *it;
                  edge_string << " [label=\"";
                      if (((adder_subtractor_node_t*)(*it))->input_shifts[i] != 0)
                          edge_string << ((adder_subtractor_node_t*)(*it))->input_shifts[i];
                      if (((adder_subtractor_node_t*)(*it))->input_is_negative[i])
                          edge_string << "(-)";
                      edge_string << "\",fontsize=12]\n";
                  if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end()){
                      realized_edges[edge_string.str().c_str()] = 1;
                      fprintf(graphfilepointer,"%s",edge_string.str().c_str());
                  }
              }
         }
         if (is_a<mux_node_t>(*(*it)))
         {
              for (int i = 0, i_end = (int)((mux_node_t*)(*it))->inputs.size(); i<i_end;++i){
                  stringstream edge_string;
                  if (((mux_node_t*)(*it))->inputs[i]!=0)
                    edge_string << "node" << ((mux_node_t*)(*it))->inputs[i]  << " -> node" << *it;

                  if (((mux_node_t*)(*it))->input_shifts[i] != 0 && ((mux_node_t*)(*it))->input_shifts[i] != DONT_CARE){
                      edge_string << " [label=\"";
                      edge_string << ((mux_node_t*)(*it))->input_shifts[i];
                      edge_string << "\",fontsize=12]\n";
                  }
                  else
                      edge_string << "\n";
                  if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end()){
                      realized_edges[edge_string.str().c_str()] = 1;
                      fprintf(graphfilepointer,"%s",edge_string.str().c_str());
                  }
              }
         }
         if (is_a<conf_adder_subtractor_node_t>(*(*it)))
                {
                     for (int i = 0, i_end = (int)((conf_adder_subtractor_node_t*)(*it))->inputs.size(); i<i_end;++i){
                         stringstream edge_string;
                         edge_string << "node" << ((conf_adder_subtractor_node_t*)(*it))->inputs[i]  << " -> node" << *it;
                         edge_string << " [label=\"";
                         if (((conf_adder_subtractor_node_t*)(*it))->input_shifts[i] != 0)
                             edge_string << ((conf_adder_subtractor_node_t*)(*it))->input_shifts[i] <<"\\n";
                         for (int j = 0, j_end = (int)((conf_adder_subtractor_node_t*)(*it))->input_is_negative.size(); j < j_end; ++j){
                             if (((conf_adder_subtractor_node_t*)(*it))->input_is_negative[j][i])
                                 edge_string << "-" << "\\n";
                             else
                                 edge_string << "+"<< "\\n";
                         }
                         edge_string << "\",fontsize=12]\n";
                         if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end()){
                             realized_edges[edge_string.str().c_str()] = 1;
                             fprintf(graphfilepointer,"%s",edge_string.str().c_str());
                         }
                     }
                }
         if (is_a<register_node_t>(*(*it)))
         {
             stringstream edge_string;
             edge_string << "node" << ((register_node_t*)(*it))->input << " -> node" << *it;
             if( ((register_node_t*)(*it))->input_shift > 0 )
             {
                 edge_string << "[label=\"" << ((register_node_t*)(*it))->input_shift << "\",fontsize=12]";
             }
             edge_string << "\n";
             if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end()){
                 realized_edges[edge_string.str().c_str()] = 1;
                 fprintf(graphfilepointer,"%s",edge_string.str().c_str());
             }
         }

      }

      fprintf(graphfilepointer,"}\n");
      fflush(graphfilepointer);
      fclose(graphfilepointer);
      if(!quiet) std::cout << "DOT-File generated" << std::endl;
  }

  /* write the graph down in mat syntax KONRAD*/

  void  adder_graph_t::writesyn(string filename, bool quiet) { //, string filename
        ofstream graphfilestream;
        graphfilestream.open(filename.c_str(),ofstream::out | ofstream::trunc);
        writesyn(graphfilestream,quiet);
        graphfilestream.close();
  }

  void  adder_graph_t::writesyn(ostream& graphoutstream,bool quiet) { //, string filename
      adder_graph_t mygraph = *this;

      int current_id = -1;
      time_t mytime;
      time(&mytime);

      graphoutstream << "{";
      // plot nodes
      for(std::list<adder_graph_base_node_t*>::iterator it = mygraph.nodes_list.begin(), it_end= mygraph.nodes_list.end(); it != it_end; ++it)
      {

          //node_t* tmp = (node_t*)(*it);
          if (!is_a<input_node_t>(*(*it)))
          {
              current_id++;
              if(current_id>0) graphoutstream << ",";

              stringstream node_string;
              stringstream pre_node_string;
              stringstream type_string;
              type_string << "{" ;
              node_string << "[";
              for(int i = 0, i_end = (int)(*it)->output_factor.size(); i < i_end; ++i){
                  for(int j = 0, j_end = (int)(*it)->output_factor[i].size(); j < j_end; ++j){
                      if (j == (int)(*it)->output_factor[i].size()-1){
                          if ((*it)->output_factor[i][j] != DONT_CARE)
                              node_string << (*it)->output_factor[i][j];
                          else
                              node_string << "NaN";
                      }
                      else{
                          if ((*it)->output_factor[i][j] != DONT_CARE)
                              node_string << (*it)->output_factor[i][j] << "," ;
                          else
                              node_string << "NaN,";
                      }

                  }
                  if (i < (int)(*it)->output_factor.size()-1)
                  node_string << ";";
              }
              node_string << "]," << (*it)->stage << ",";

              if (is_a<adder_subtractor_node_t>(*(*it)) || is_a<conf_adder_subtractor_node_t>(*(*it)))
              {
                  type_string << "'A',";
                  for (int k = 0, k_end = (int)((adder_subtractor_node_t*)(*it))->inputs.size(); k < k_end;++k){
                          if (k == 0)
                              pre_node_string << "[";
                          else
                              pre_node_string << ",[";
                          //set outputs of pre_node
                          for(int i = 0, i_end = (int)((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor.size(); i < i_end; ++i){
                              for(int j = 0, j_end = (int)((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i].size(); j < j_end; ++j){
                                 if (j == (int)((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i].size()-1)
                                 {
                                     if (((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j] != DONT_CARE){
                                         bool negate=false;
                                         if (is_a<adder_subtractor_node_t>(*(*it)))
                                         {
                                              if(((adder_subtractor_node_t*)(*it))->input_is_negative[k])
                                                  negate = true;
                                         }
                                         else
                                         {
                                              if(((conf_adder_subtractor_node_t*)(*it))->input_is_negative[i][k])
                                                  negate = true;
                                         }
                                         if(negate) pre_node_string << (-1*((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j]);
                                         else pre_node_string << ((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j];
                                     }
                                     else
                                         pre_node_string << "NaN";
                                 }
                                 else
                                 {
                                     if (((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                                     {
                                         bool negate=false;
                                         if (is_a<adder_subtractor_node_t>(*(*it)))
                                         {
                                              if(((adder_subtractor_node_t*)(*it))->input_is_negative[k])
                                                  negate = true;
                                         }
                                         else
                                         {
                                              if(((conf_adder_subtractor_node_t*)(*it))->input_is_negative[i][k])
                                                  negate = true;
                                         }
                                         if(negate) pre_node_string << (-1*((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j]) <<",";
                                         else pre_node_string << ((adder_subtractor_node_t*)(*it))->inputs[k]->output_factor[i][j] <<",";
                                     }
                                     else
                                         pre_node_string << "NaN,";
                                 }

                             }
                              if (i < (int)(*it)->output_factor.size()-1)
                                pre_node_string << ";";
                          }
                          //get input shift
                          pre_node_string << "]," << ((adder_subtractor_node_t*)(*it))->inputs[k]->stage << "," << ((adder_subtractor_node_t*)(*it))->input_shifts[k];
                      }
                  //}
              }
              if (is_a<mux_node_t>(*(*it)))
              {
                  type_string << "'M',";
                  map<adder_graph_base_node_t*,bool> already_seen;
                  already_seen.clear();

                  bool k_first=true;
                  for (int k = 0, k_end = (int)((mux_node_t*)(*it))->inputs.size(); k < k_end;++k){
                      if (already_seen.find(((mux_node_t*)(*it))->inputs[k]) == already_seen.end() && ((mux_node_t*)(*it))->inputs[k] != NULL){
                          already_seen[((mux_node_t*)(*it))->inputs[k]] = true;
                          if ( k_first )
                          {
                              pre_node_string << "[";
                              k_first=false;
                          }
                          else
                              pre_node_string << ",[";
                          //get outputs of pre_node
                          for(int i = 0, i_end = (int)((mux_node_t*)(*it))->inputs[k]->output_factor.size(); i < i_end; ++i){
                              for(int j = 0, j_end = (int)((mux_node_t*)(*it))->inputs[k]->output_factor[i].size(); j < j_end; ++j){
                                  if (j == (int)((mux_node_t*)(*it))->inputs[k]->output_factor[i].size()-1){
                                      if (((mux_node_t*)(*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                                      {
                                          pre_node_string << ((mux_node_t*)(*it))->inputs[k]->output_factor[i][j];
                                      }
                                      else
                                      {
                                          pre_node_string << "NaN";
                                      }
                                  }
                                  else{
                                      if (((mux_node_t*)(*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                                      {
                                          pre_node_string << ((mux_node_t*)(*it))->inputs[k]->output_factor[i][j] << "," ;
                                      }
                                      else
                                      {
                                          pre_node_string << "NaN,";
                                      }
                                  }
                              }
                              if (i < (int)(*it)->output_factor.size()-1)
                              pre_node_string << ";";
                          }
                          pre_node_string << "]," << ((mux_node_t*)(*it))->inputs[k]->stage << ",[";
                          //get input shifts
                          for(int i = 0, i_end = (int)((mux_node_t*)(*it))->input_shifts.size(); i < i_end; ++i){

                              if (((mux_node_t*)(*it))->input_shifts[k] != DONT_CARE && ((mux_node_t*)(*it))->inputs[k] != NULL)
                              {
                                  bool same = true;
                                  for(unsigned int j = 0, j_end = ((mux_node_t*)(*it))->output_factor[i].size(); j<j_end;++j )
                                  {
                                      if( ((mux_node_t*)(*it))->output_factor[i][j] != ((mux_node_t*)(*it))->inputs[k]->output_factor[i][j] << ((mux_node_t*)(*it))->input_shifts[i] )
                                            same = false;
                                  }

                                  if( !same )
                                      pre_node_string << "NaN";
                                  else
                                      pre_node_string << ((mux_node_t*)(*it))->input_shifts[i];
                              }
                              else
                                  pre_node_string << "NaN";
                              if (i < (int)((mux_node_t*)(*it))->input_shifts.size()-1)
                                  pre_node_string<< ";";
                          }
                          pre_node_string << "]";
                      }
                  }
              }

              if (is_a<register_node_t>(*(*it)))
              {
                  type_string << "'R',";
                  pre_node_string << "[";
                  //set outputs of pre_node
                  for(int i = 0, i_end = (int)((register_node_t*)(*it))->input->output_factor.size(); i < i_end; ++i){
                      for(int j = 0, j_end = (int)((register_node_t*)(*it))->input->output_factor[i].size(); j < j_end; ++j){
                         if (j == (int)((register_node_t*)(*it))->input->output_factor[i].size()-1){
                             if (((register_node_t*)(*it))->input->output_factor[i][j] != DONT_CARE) {
                                 pre_node_string << ((register_node_t*)(*it))->input->output_factor[i][j];
                             }
                             else
                                 pre_node_string << "NaN";
                         }
                         else{
                             if (((register_node_t*)(*it))->input->output_factor[i][j] != DONT_CARE)
                                 pre_node_string << ((register_node_t*)(*it))->input->output_factor[i][j] << "," ;
                             else
                                 pre_node_string << "NaN,";
                         }
                     }
                      if (i < (int)(*it)->output_factor.size()-1) {
                          pre_node_string << ";";
                      }
                 }
                 pre_node_string << "]," << ((register_node_t*)(*it))->input->stage;
                 if (((register_node_t*)(*it))->input_shift != 0)
                         pre_node_string << "," << ((register_node_t*)(*it))->input_shift;
              }

              pre_node_string << "}";


              if( !(*it)->specific_parameters.empty() ){
                  pre_node_string << ":{";
                  for(std::map<std::string,std::string>::iterator its = (*it)->specific_parameters.begin();
                      its != (*it)->specific_parameters.end(); ++its)
                  {
                      if( !(*its).first.empty() && !(*its).second.empty() ){
                          if( its != (*it)->specific_parameters.begin() )
                              pre_node_string << ",";

                          pre_node_string << (*its).first << "=" << (*its).second;
                      }
                  }
                  pre_node_string << "}";
              }

              graphoutstream << type_string.str() << node_string.str() << pre_node_string.str();
              //fprintf(graphfilepointer,"%s%s%s",type_string.str().c_str(),node_string.str().c_str(),pre_node_string.str().c_str());

          }
      }
      graphoutstream << "}";
      if( !specific_parameters.empty() ){
          graphoutstream << ":{";
          for(std::map<std::string,std::string>::iterator its = specific_parameters.begin();
              its != specific_parameters.end(); ++its)
          {
              if( !(*its).first.empty() && !(*its).second.empty() ){
                  if( its != specific_parameters.begin() )
                      graphoutstream << ",";

                  graphoutstream << (*its).first << "=" << (*its).second;
              }
          }
          graphoutstream << "}";
      }

      //fprintf(graphfilepointer,"}\n");

      graphoutstream.flush();

      //fflush(graphfilepointer);
      //fclose(graphfilepointer);
      if(!quiet) std::cout << "Finished Syntax Output" << std::endl;
  }

  bool adder_graph_t::parse_to_graph(string commandLine){
      if( commandLine.find_first_of("AMR") == string::npos )
            commandLine = convert_old_syntax(commandLine);

      adder_graph_t* parsed_graph = Paglib_copa::parse(commandLine);
      if(parsed_graph != NULL)
      {
          nodes_list.clear();
          nodes_list = parsed_graph->nodes_list;
          delete parsed_graph;
          return true;
      }
      else
          return false;
  }

  void adder_graph_t::print_graph()
  {
      adder_graph_t mygraph = *this;

      if( !mygraph.specific_parameters.empty() ){
          std::cout << "Attached parameters to this graph:" << std::endl;
          for(std::map<std::string,std::string>::iterator its = mygraph.specific_parameters.begin();
              its != mygraph.specific_parameters.end(); ++its)
          {
              if( !(*its).first.empty() && !(*its).second.empty() ){
                  std::cout << "\t" << (*its).first << " = " << (*its).second << ";" << std::endl;
              }
          }
      }
      for(std::list<adder_graph_base_node_t*>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {
          std::cout << "\nNode type of node " << (*it) << " in stage " << (*it)->stage << " is: " << typeid(*(*it)).name()<< std::endl;
          std::cout << "The outputs are: ";
              //std::cout << "Size:"<< (*it)->output_factor.size();
              for (unsigned int k= 0 , k_end = (*it)->output_factor.size(); k < k_end; ++k)
              {
                  if( k > 0 ) std::cout << "; ";
                  for(unsigned int f = 0, f_end = (*(*it)).output_factor[k].size(); f < f_end;++f )
                  {
                      if( f > 0 ) std::cout << ", ";

                      std::cout << (*(*it)).output_factor[k][f] << " ";
                  }
              }
              std::cout << std::endl;

          if (is_a<input_node_t>(*(*it)))
          {
              std::cout << "No input" << std::endl;

          }
          if (is_a<adder_subtractor_node_t>(*(*it)))
          {
              adder_subtractor_node_t* t = (adder_subtractor_node_t*)(*it);
              for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k)
                  std::cout << "Input: " << t->inputs[k] << ", shift: "<< t->input_shifts[k] << ", in stage " << t->inputs[k]->stage << ", is negative: " << t->input_is_negative[k] << std::endl;
          }
          if (is_a<mux_node_t>(*(*it)))
          {
              mux_node_t* t = (mux_node_t*)(*it);
              for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k)
                  if (t->inputs[k] != NULL)
                  std::cout << "Input: " << t->inputs[k] << ", shift: "<< t->input_shifts[k] << ", in stage " << t->inputs[k]->stage << std::endl;
          }
          if (is_a<conf_adder_subtractor_node_t>(*(*it)))
          {
              conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)(*it);
              for (int k= 0 , k_end = (int)t->inputs.size(); k < k_end; ++k){
                  std::cout << "Input: " << t->inputs[k] << ", shift: "<< t->input_shifts[k] << ", in stage " << t->inputs[k]->stage << ", is negative: ";
                  for (int j = 0, j_end = (int)t->input_is_negative.size(); j < j_end; ++j)
                      std::cout << t->input_is_negative[j][k] << ", ";
                  std::cout << std::endl;
              }
          }
          if (is_a<register_node_t>(*(*it)))
          {
              register_node_t* t = (register_node_t*)(*it);
                  std::cout << "Input: " << t->input << ", shift: "<< t->input_shift <<", in stage " << t->input->stage << std::endl;
          }
          if( !(*it)->specific_parameters.empty() ){
              std::cout << "Attached parameters:" << std::endl;
              for(std::map<std::string,std::string>::iterator its = (*it)->specific_parameters.begin();
                  its != (*it)->specific_parameters.end(); ++its)
              {
                  if( !(*its).first.empty() && !(*its).second.empty() ){
                      std::cout << "\t" << (*its).first << " = " << (*its).second << ";" << std::endl;
                  }
              }
          }
      }
      std::cout << "Printed all" << std::endl;
  }

  void adder_graph_t::check_and_correct(string graphstring)
    {
        if(graphstring=="") graphstring = "STRINGSYN NOT AVAILABLE";

        list<string> errorList;
        map<int,vector<string> > configurationCountMap;
        map<int,vector<string> > inputCountMap;
		configurationCountMap.clear();
		inputCountMap.clear();
        //check for errors
        for(list<adder_graph_base_node_t*>::iterator it= nodes_list.begin(), it_end = nodes_list.end();
            it!=it_end;
            ++it)
		{
            stringstream node_name;
            if (is_a<input_node_t>(*(*it))) node_name << "INP@";
            else if (is_a<adder_subtractor_node_t>(*(*it))) node_name<<"ADD@";
            else if (is_a<mux_node_t>(*(*it))) node_name << "MUX@";
            else if (is_a<conf_adder_subtractor_node_t>(*(*it))) node_name << "CONF_ADD@";
            else if (is_a<register_node_t>(*(*it))) node_name << "REG@";
            node_name << "[";
            for(unsigned int cfg=0, cfg_end = (*it)->output_factor.size();cfg<cfg_end;++cfg)
            {
                if(cfg>0) node_name << ";";
                for(unsigned int v=0, v_end = (*it)->output_factor[cfg].size();v<v_end;++v)
                {
                  if(v>0) node_name << ",";
                  if((*it)->output_factor[cfg][v] == DONT_CARE) node_name << "-";
                  else node_name << (*it)->output_factor[cfg][v];
                }
            }
            node_name << "],stage " <<(*it)->stage;

            if( (*it)->output_factor.size() == 0 )
            {
                errorList.push_back("OUTPUTFACTOR MISSING: " + node_name.str() );
                continue;
            }
            if( (*it)->output_factor[0].size() == 0 )
            {
                errorList.push_back("OUTPUTFACTOR INNER MISSING: " + node_name.str() );
                continue;
            }
            map<int,vector<string> >::iterator pr1 =  configurationCountMap.find( (*it)->output_factor.size()  );
            if(pr1 != configurationCountMap.end())
            {
                (*pr1).second.push_back(node_name.str());
            }
            else
            {
                vector<string> newvec;
                newvec.push_back( node_name.str() );
                configurationCountMap.insert( make_pair<int,vector<string> >((*it)->output_factor.size(),newvec));
            }
			
			map<int,vector<string> >::iterator pr2 =  inputCountMap.find( (*it)->output_factor[0].size()  );
            if(pr2 != inputCountMap.end())
            {
                (*pr2).second.push_back(node_name.str());
            }
            else
            {
                vector<string> newvec;
                newvec.push_back( node_name.str() );
                inputCountMap.insert( make_pair<int,vector<string> >((*it)->output_factor[0].size(),newvec));
            }

            if (is_a<input_node_t>(*(*it)))
            {

            }
            else if (is_a<adder_subtractor_node_t>(*(*it)))
            {
                adder_subtractor_node_t* t = (adder_subtractor_node_t*)(*it);
                //check for NULL Inputs
                int inputs_notnull=0;
                bool not_found_error=false;
                int max_neg_shift=0;
                for(unsigned int i=0, i_end = t->inputs.size();i<i_end;++i)
                {
                    if(t->inputs[i] != NULL)
                    {
                        inputs_notnull++;
                        if( std::find(nodes_list.begin(),nodes_list.end(),t->inputs[i]) == nodes_list.end() ){
                            errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str() );
                            not_found_error=true;
                        }
                        if( t->input_shifts[i]<0 && t->input_shifts[i]<max_neg_shift )
                            max_neg_shift = t->input_shifts[i];
                    }
                }

                max_neg_shift = abs(max_neg_shift);
                if(not_found_error) continue;
                if(inputs_notnull == 0)
                {
                    errorList.push_back("ALL INPUTS NULL: " + node_name.str() );
                    continue;
                }
                else if(inputs_notnull == 1)
                {
                    errorList.push_back("INPUT NULL: " + node_name.str() );
                    continue;
                }
				
                if(t->input_is_negative.size() !=  t->inputs.size() )
				{
                    errorList.push_back("ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str() );
                    continue;
				}

                //check output values
                for(unsigned int cfg=0, cfg_end = t->output_factor.size();cfg<cfg_end;++cfg )
                {
                      for(unsigned int v=0, v_end = t->output_factor[cfg].size();v<v_end;++v)
                      {
                          int64_t computed_output=0;
                          for(unsigned int i=0, i_end = t->inputs.size();i<i_end;++i)
                          {
                              if(t->inputs[i]!=NULL )
                              {
                                  if( t->input_is_negative[i] )
                                  {
                                        computed_output -= ( t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i]+max_neg_shift) );
                                  }
                                  else
                                  {
                                        computed_output += ( t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i]+max_neg_shift) );
                                  }
                              }
                          }
                          if( (computed_output>>max_neg_shift) != t->output_factor[cfg][v] && t->output_factor[cfg][v] != DONT_CARE)
                          {
                              stringstream tmp;
                              tmp << "\t(computed:" << (computed_output>>abs(max_neg_shift)) << " != given:" << t->output_factor[cfg][v] << ")";
                              errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() + tmp.str() );
                              continue;
                          }
                      }
                }
            }
            else if (is_a<mux_node_t>(*(*it)))
            {
                mux_node_t* t = (mux_node_t*)(*it);
                int muxInputCount = 0;
                bool not_found_error=false;
                for(unsigned int i =0, i_end = t->inputs.size();i<i_end;++i)
                {
                    if(t->inputs[i] != NULL){
                        muxInputCount++;
                        if( std::find(nodes_list.begin(),nodes_list.end(),t->inputs[i]) == nodes_list.end() ){
                            errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str() );
                            not_found_error=true;
                        }
                    }
                }
                if(not_found_error) continue;
                if(muxInputCount == 0)
                {
                    errorList.push_back("ALL INPUTS NULL: " + node_name.str() );
                    continue;
                }
                else
                {
                    for(unsigned int cfg=0, cfg_end = t->output_factor.size();cfg<cfg_end;++cfg )
                    {
                          for(unsigned int v=0, v_end = t->output_factor[cfg].size();v<v_end;++v)
                          {
                              if(t->inputs[cfg] == NULL)
                              {
                                  if(t->output_factor[cfg][v]!=0 && t->output_factor[cfg][v]!= DONT_CARE)
                                  {
                                      errorList.push_back("OUTPUTFACTOR MISSMATCH INPUT NULL: " + node_name.str() );
                                  }
                              }
                              else
                              {
                                  if( (t->inputs[cfg]->output_factor[cfg][v]<<t->input_shifts[cfg]) != t->output_factor[cfg][v] && t->output_factor[cfg][v] != DONT_CARE)
                                  {
                                      errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() );
                                  }
                              }
                          }
                    }
                }
            }
            else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
            {
                conf_adder_subtractor_node_t* t = (conf_adder_subtractor_node_t*)(*it);

                bool not_found_error=false;
                int max_neg_shift=0;
                for(unsigned int i =0, i_end = t->inputs.size();i<i_end;++i)
                {
                    if(t->inputs[i] != NULL){
                        if( std::find(nodes_list.begin(),nodes_list.end(),t->inputs[i]) == nodes_list.end() ){
                            errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str() );
                            not_found_error=true;
                        }
                        if( t->input_shifts[i]<0 && t->input_shifts[i]<max_neg_shift )
                            max_neg_shift = t->input_shifts[i];
                    }
                }
                max_neg_shift = abs(max_neg_shift);
                if(not_found_error) continue;

                if(t->input_is_negative.size() !=  t->output_factor.size() )
                {
                    errorList.push_back("OUTER ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str() );
                    continue;
                }

                for(unsigned int cfg =0;cfg<t->input_is_negative.size();cfg++){
                    if( t->input_is_negative[cfg].size() != t->inputs.size() ){
                        errorList.push_back("INNER ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str() );
                        continue;
                    }

                }

                for(unsigned int cfg=0, cfg_end = t->output_factor.size();cfg<cfg_end;++cfg )
                {
                      for(unsigned int v=0, v_end = t->output_factor[cfg].size();v<v_end;++v)
                      {
                          //int64_t neg_shift=0;
                          int64_t computed_output=0;
                          for(unsigned int i=0, i_end = t->inputs.size();i<i_end;++i)
                          {
                              if(t->inputs[i]!=NULL )
                              {
                                  if(t->input_is_negative[cfg][i])
                                  {
                                        computed_output -= ( t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i]+max_neg_shift) );
                                  }
                                  else
                                  {
                                        computed_output += ( t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i]+max_neg_shift) );
                                  }
                              }
                          }
                          if( (computed_output>>abs(max_neg_shift)) != t->output_factor[cfg][v] && t->output_factor[cfg][v] != DONT_CARE)
                          {
                              stringstream tmp;
                              tmp << "\t(computed:" << (computed_output>>abs(max_neg_shift)) << " != given:" << t->output_factor[cfg][v] << ")";
                              errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() + tmp.str() );
                          }
                      }
                }
            }
            else if (is_a<register_node_t>(*(*it)))
            {
                register_node_t* t = (register_node_t*)(*it);
                for(unsigned int cfg=0, cfg_end = t->output_factor.size();cfg<cfg_end;++cfg )
                {
                      for(unsigned int v=0, v_end = t->output_factor[cfg].size();v<v_end;++v)
                      {
                          if(t->input == NULL)
                          {
                              errorList.push_back("INPUT OF REG IS NULL: " + node_name.str() );
                          }
                          else{
                              if( std::find(nodes_list.begin(),nodes_list.end(),t->input) == nodes_list.end() ){
                                  errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str() );
                                  v = v_end;
                                  cfg = cfg_end;
                                  break;
                              }

                          }
                          if( abs(t->input->output_factor[cfg][v] << t->input_shift) != abs(t->output_factor[cfg][v]) && t->output_factor[cfg][v] != DONT_CARE)
                          {
                              errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() );
                          }
                      }
                }
            }
        }
        if(configurationCountMap.size()>1)
        {
            stringstream error;
            error << "CONFIGURATION COUNT MISSMATCH:" << endl;
            for( map<int,vector<string> >::iterator pr1 = configurationCountMap.begin(), pr1_end = configurationCountMap.end();
                 pr1 != pr1_end;
                 ++pr1)
            {
                    error << "\tNODES WITH " << (*pr1).first << " CFG(S):"<<endl;
                    for(unsigned int i=0, i_end = (*pr1).second.size();i<i_end;++i )
                    {
                        error << "\t\t" <<   (*pr1).second[i] << endl;
                    }
            }
            errorList.push_back(error.str());
        }
        if(inputCountMap.size()>1)
        {
            stringstream error;
            error << "INPUT COUNT MISSMATCH:" << endl;
            for( map<int,vector<string> >::iterator pr1 = inputCountMap.begin(), pr1_end = inputCountMap.end();
                 pr1 != pr1_end;
                 ++pr1)
            {
                    error << "\tNODES WITH " << (*pr1).first << " INPUT(S):"<<endl;
                    for(unsigned int i=0, i_end = (*pr1).second.size();i<i_end;++i )
                    {
                        error << "\t\t" <<   (*pr1).second[i] << endl;
                    }
            }
            errorList.push_back(error.str());
        }

		if(errorList.size()>0)
        {
            for( list<string>::iterator iter = errorList.begin(), iter_end = errorList.end(); iter!=iter_end;++iter )
			{
				cout << *iter << endl;
			}
           throw runtime_error("Parse check failed");
        }
    }

  void adder_graph_t::normalize_graph()
  {

      list<adder_graph_base_node_t*> neg_shift_list;
      int max_stage=0;
      for(list<adder_graph_base_node_t*>::iterator it= nodes_list.begin(), it_end = nodes_list.end();
          it!=it_end;
          ++it)
      {
          if(  max_stage < (*it)->stage ) max_stage = (*it)->stage;

          if (is_a<adder_subtractor_node_t>(*(*it)))
          {
              adder_subtractor_node_t* t = (adder_subtractor_node_t*)*it;
              if( t->input_shifts[0] < 0 ) neg_shift_list.push_back(*it);
          }
      }

      for(list<adder_graph_base_node_t*>::iterator it= nodes_list.begin(), it_end = nodes_list.end();
          it!=it_end;
          ++it)
      {
          if( (*it)->stage == max_stage ) continue;

          if (is_a<adder_subtractor_node_t>(*(*it)))
          {
              adder_subtractor_node_t* t = (adder_subtractor_node_t*)(*it);
              for(unsigned int i=0, i_end = t->inputs.size();i<i_end;++i)
              {
                  list<adder_graph_base_node_t*>::iterator find = std::find(neg_shift_list.begin(),neg_shift_list.end(),t->inputs[i]);
                  if( find != neg_shift_list.end() )
                  {
                      int neg_shift = ((adder_subtractor_node_t*)(*find))->input_shifts[0];
                      for(unsigned int j=0, j_end = ((adder_subtractor_node_t*)(*find))->input_shifts.size();j<j_end;++j)
                      {
                          ((adder_subtractor_node_t*)(*find))->input_shifts[j] = 0;
                      }
                      t->input_shifts[i]+=neg_shift;
                  }
              }
          }
          else if (is_a<register_node_t>(*(*it)))
          {
              register_node_t* t = (register_node_t*)(*it);
              list<adder_graph_base_node_t*>::iterator find = std::find(neg_shift_list.begin(),neg_shift_list.end(),t->input);
              if( find != neg_shift_list.end() )
              {
                  int neg_shift = ((adder_subtractor_node_t*)(*find))->input_shifts[0];
                  for(unsigned int j=0, j_end = ((adder_subtractor_node_t*)(*find))->input_shifts.size();j<j_end;++j)
                  {
                      ((adder_subtractor_node_t*)(*find))->input_shifts[j] = 0;
                  }
                  t->input_shift+=neg_shift;
              }
          }
      }

  }

  string adder_graph_t::convert_old_syntax(string commandLine)
  {
      bool replace_active=false;
      for(unsigned int i=0, i_end = commandLine.size();i<i_end;++i)
      {
          if(commandLine[i]=='[') replace_active=true;
          if(commandLine[i]==']') replace_active=false;
          if( replace_active && commandLine[i]==';') commandLine[i]=',';
      }

      string zeroString;
      {
          int p1=commandLine.find("["),p2=commandLine.find("]");
          string firstOutput = commandLine.substr(p1+1,p2-p1-1);

          int cmm_size = 1;
          for(unsigned int f=0, f_end = firstOutput.size();f <f_end;++f)
          {
              if(firstOutput[f]==',') cmm_size++;
          }
          zeroString = "[";
          for(int i=0;i<cmm_size;++i)
          {
              if(i>0) zeroString += ",";
              zeroString += "0";
          }
          zeroString += "]";
      }

      stringstream new_k_adder_graph;
      new_k_adder_graph << "{";

      commandLine = commandLine.substr( commandLine.find("{")+1 );
      commandLine = commandLine.substr( 0,commandLine.find_last_of("}") );
      bool first = true;
      while( commandLine.find("{") != string::npos )
      {
          if(!first){ new_k_adder_graph <<","; }
          first = false;
          new_k_adder_graph << "{";
          int i1 = commandLine.find("{")+1,i2 = commandLine.find("}");
          string node_string = commandLine.substr(i1,i2-i1);

          if( node_string.find(zeroString.c_str()) != string::npos )
          {
              int i3 = node_string.find(zeroString.c_str());
              new_k_adder_graph << "'R',";
              string tmp_node_string = node_string.substr(0,i3-1);
              if( tmp_node_string.substr( tmp_node_string.find_last_of(",")+1 ) == "0" )
              {
                  new_k_adder_graph <<tmp_node_string.substr(0, tmp_node_string.find_last_of(",") );
              }
              else
              {
                  new_k_adder_graph << tmp_node_string;
              }
          }
          else
          {
              new_k_adder_graph << "'A',";
              new_k_adder_graph << node_string;
          }
          new_k_adder_graph << "}";
          commandLine = commandLine.substr(i2+1);
      }
      new_k_adder_graph << "}";
      return new_k_adder_graph.str();
  }
