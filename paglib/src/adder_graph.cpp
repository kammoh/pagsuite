/* ADDER_GRAPH.cpp
 * Version:
 * Datum: 20.11.2014
 */

#include "pagsuite/adder_graph.h"
//#include "paglib_copa.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <assert.h>

namespace PAGSuite
{

    adder_graph_t::adder_graph_t()
    {
      quiet = false;
    }


    adder_graph_t::~adder_graph_t()
    {

    }

    void adder_graph_t::clear()
    {
      while (!nodes_list.empty())
      {
        delete nodes_list.back();
        nodes_list.pop_back();
      }
    }

/* pipeline the adder graph KONRAD*/
    void adder_graph_t::pipeline_graph()
    {
      adder_graph_t mygraph = *this;
      std::map<pair<vector<vector<int64_t> >, int>, adder_graph_base_node_t *> tmp_reg_list;

      for (std::list<adder_graph_base_node_t *>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end();
           it != it_end; ++it)
      {
        if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          adder_subtractor_node_t *t = (adder_subtractor_node_t *) (*it); //
          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
          {
            int diff = (*it)->stage - t->inputs[k]->stage; //Differenz der Stufen
            for (int j = 1; j < diff; ++j)
            {
              register_node_t *tmp = new register_node_t;
              tmp->stage = t->stage - j;
              tmp->output_factor = t->inputs[k]->output_factor;
              if (j == 1)
              {
                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] ==
                    NULL) //Register existiert noch nicht
                {
                  tmp->input = t->inputs[k];
                  t->inputs[k] = tmp;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                }
                else
                {
                  t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)];
                }
              }
              else if (j == diff - 1)
              {

                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                {
                  tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) + 1)]))->input = tmp;
                }
                else
                {
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) +
                                                                                         1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, tmp->stage)];
                }
              }
              else
              {
                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                {
                  tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) + 1)]))->input = tmp;
                }
                else
                {
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) +
                                                                                         1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, tmp->stage)];
                }
              }
            }
          }
        }
        else if (is_a<mux_node_t>(*(*it)))
        {
          mux_node_t *t = (mux_node_t *) (*it); //
          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
          {
            if (t->inputs[k] != NULL)
            {
              int diff = (*it)->stage - t->inputs[k]->stage; //Differenz der Stufen
              for (int j = 1; j < diff; ++j)
              {
                register_node_t *tmp = new register_node_t;
                tmp->stage = t->stage - j;
                tmp->output_factor = t->inputs[k]->output_factor;
                if (j == 1)
                {
                  if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] ==
                      NULL) //Register existiert noch nicht
                  {
                    tmp->input = t->inputs[k];
                    t->inputs[k] = tmp;
                    tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                  }
                  else
                  {
                    t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                    tmp->stage)];
                  }
                }
                else if (j == diff - 1)
                {

                  if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                  {
                    tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                        t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                    tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                    ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                           (tmp->stage) +
                                                                                           1)]))->input = tmp;
                  }
                  else
                  {
                    ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                           (tmp->stage) +
                                                                                           1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                        t->inputs[k]->output_factor, tmp->stage)];
                  }
                }
                else
                {
                  if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                  {
                    tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                        t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                    tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                    ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                           (tmp->stage) +
                                                                                           1)]))->input = tmp;
                  }
                  else
                  {
                    ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                           (tmp->stage) +
                                                                                           1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                        t->inputs[k]->output_factor, tmp->stage)];
                  }
                }
              }
            }
          }
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
        {
          conf_adder_subtractor_node_t *t = (conf_adder_subtractor_node_t *) (*it); //conf_adder_subtractor_node_t*
          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
          {
            int diff = (*it)->stage - t->inputs[k]->stage; //Differenz der Stufen
            for (int j = 1; j < diff; ++j)
            {
              register_node_t *tmp = new register_node_t;
              tmp->stage = t->stage - j;
              tmp->output_factor = t->inputs[k]->output_factor;
              if (j == 1)
              {
                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] ==
                    NULL) //Register existiert noch nicht
                {
                  tmp->input = t->inputs[k];
                  t->inputs[k] = tmp;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                }
                else
                {
                  t->inputs[k] = tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)];
                }
              }
              else if (j == diff - 1)
              {

                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                {
                  tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) + 1)]))->input = tmp;
                }
                else
                {
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) +
                                                                                         1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, tmp->stage)];
                }
              }
              else
              {
                if (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] == NULL)
                {
                  tmp->input = ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, (tmp->stage) + 1)]))->input;
                  tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor, tmp->stage)] = tmp;
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) + 1)]))->input = tmp;
                }
                else
                {
                  ((register_node_t *) (tmp_reg_list[pair<vector<vector<int64_t> >, int>(t->inputs[k]->output_factor,
                                                                                         (tmp->stage) +
                                                                                         1)]))->input = tmp_reg_list[pair<vector<vector<int64_t> >, int>(
                      t->inputs[k]->output_factor, tmp->stage)];
                }
              }
            }
          }

        }
      }

      for (std::map<pair<vector<vector<int64_t> >, int>, adder_graph_base_node_t *>::iterator iter = tmp_reg_list.begin(), iter_end = tmp_reg_list.end();
           iter != iter_end; ++iter)
      {
        mygraph.nodes_list.push_back((*iter).second);
      }
      std::cout << "Finished Pipelining" << std::endl;
    }

    std::vector<std::vector<int64_t> > adder_graph_t::normalize(std::vector<std::vector<int64_t> > &factor)
    {
      if (factor.size() == 0) return factor;

      std::vector<std::vector<int64_t> > factor_norm(factor.size(), std::vector<int64_t>(factor[0].size()));

      bool found_leading_nonzero = false;

      for (int r = 0; r < factor.size(); r++)
      {
        for (int c = 0; c < factor[r].size(); c++)
        {
          if (!found_leading_nonzero)
          {
            factor_norm[r][c] = factor[r][c];
            if (factor[r][c] != 0)
            {
              found_leading_nonzero = true;
              if (factor[r][c] > 0)
              {
                return factor;
              }
              else
              {
                factor_norm[r][c] = -factor[r][c];
              }
            }
          }
          else
          {
            //now, matrix needs normalization
            factor_norm[r][c] = -factor[r][c];
          }
        }
      }

      return factor_norm;
    }

    string adder_graph_t::matrix_to_string(const std::vector<std::vector<int64_t> > &matrix)
    {
      std::stringstream stream;

      for (int r = 0; r < matrix.size(); r++)
      {
        for (int c = 0; c < matrix[r].size(); c++)
        {
          if (matrix[r][c] < 0) stream << "m";

          stream << abs(matrix[r][c]);
          if (c < matrix[r].size() - 1) stream << "_";
        }
        if (r < matrix.size() - 1) stream << "__";
      }
      return stream.str();
    }

    string adder_graph_t::node_to_string(const adder_graph_base_node_t *node)
    {
      std::stringstream stream;

      if (is_a<input_node_t>(*node))
      {
        stream << "I";
      }
      else if ((is_a<adder_subtractor_node_t>(*node)) || (is_a<conf_adder_subtractor_node_t>(*node)))
      {
        stream << "A";
      }
      else if (is_a<register_node_t>(*node))
      {
        stream << "R";
      }
      else if (is_a<output_node_t>(*node))
      {
        stream << "O";
      }
      else if (is_a<mux_node_t>(*node))
      {
        stream << "M";
      }
      else
      {
        throw runtime_error("node_to_string: unknown node of type " + string(typeid(*node).name()));
      }
      stream << matrix_to_string(node->output_factor) << "_s" << node->stage;
      return stream.str();
    }

    adder_graph_base_node_t *adder_graph_t::get_node_from_output_factor_in_stage(std::vector<std::vector<int64_t> > &output_factor, int stage)
    {
//  for (std::list<adder_graph_base_node_t *>::iterator it = nodes_list.begin(), it_end = nodes_list.end(); it != it_end; ++it)
      for (adder_graph_base_node_t *node : nodes_list)
      {
        if(!quiet) cout << "  checking " << node->output_factor << " with stage=" << node->stage << endl;
        if ((node->output_factor == output_factor) && (node->stage == stage))
        {
          if(!quiet) cout << "found node " << node << endl;
          return node;
        }
      }
      return nullptr;
    }

/* plot the adder graph to dot KONRAD*/
    void adder_graph_t::drawdot(string filename)
    {
      adder_graph_t mygraph = *this;
      //string filename = "./k_adder_graph.dot";

      FILE *graphfilepointer = NULL;

      graphfilepointer = fopen(filename.c_str(), "w");

      time_t mytime;
      time(&mytime);
      fprintf(graphfilepointer, "// File generated:  %s\n", ctime(&mytime));
      fprintf(graphfilepointer, "digraph DAG {\n");
      std::map<string, int> realized_edges;

      // plot nodes
      for (std::list<adder_graph_base_node_t *>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {
        stringstream node_string;
        node_string << node_to_string((*it)) << "[label=\"";
        node_string << (*it)->output_factor;
        double height = 0.3 * (*it)->output_factor.size();
        node_string << "\",fontsize=12,shape=";
        if (is_a<input_node_t>(*(*it)))
        {
          node_string << "ellipse];\n";
        }
        else if (is_a<adder_subtractor_node_t>(*(*it)))
        {
//      node_string << "box,height = " << height << ",width=.4];\n";
          node_string << "box];\n";
        }
        else if (is_a<mux_node_t>(*(*it)))
        {
          node_string << "polygon,sides=4,distortion=.7,fixedsize=\"true\",height = " << height - 0.3 * height << ",width=1.2];\n";
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
        {
          node_string << "ellipse,height =" << height << ",width=.4];\n";
        }
        else if (is_a<register_node_t>(*(*it)))
        {
          node_string << "box];\n";
        }
        else if (is_a<output_node_t>(*(*it)))
        {
          node_string << "none];\n";
        }
        else
        {
          adder_graph_base_node_t *p = *it;
          throw runtime_error("unknown node of type " + string(typeid(*p).name()));
        }
        fprintf(graphfilepointer, "%s", node_string.str().c_str());
      }

      for (std::list<adder_graph_base_node_t *>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {
        if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          for (int i = 0, i_end = (int) ((adder_subtractor_node_t *) (*it))->inputs.size(); i < i_end; ++i)
          {
            stringstream edge_string;
            edge_string << node_to_string(((adder_subtractor_node_t *) (*it))->inputs[i]) << " -> " << node_to_string((*it));
            edge_string << " [label=\"";
            if (((adder_subtractor_node_t *) (*it))->input_shifts[i] != 0)
              edge_string << ((adder_subtractor_node_t *) (*it))->input_shifts[i];
            if (((adder_subtractor_node_t *) (*it))->input_is_negative[i])
              edge_string << "(-)";
            edge_string << "\",fontsize=12]\n";
            if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end())
            {
              realized_edges[edge_string.str().c_str()] = 1;
              fprintf(graphfilepointer, "%s", edge_string.str().c_str());
            }
          }
        }
        else if (is_a<mux_node_t>(*(*it)))
        {
          for (int i = 0, i_end = (int) ((mux_node_t *) (*it))->inputs.size(); i < i_end; ++i)
          {
            stringstream edge_string;
            if (((mux_node_t *) (*it))->inputs[i] != 0)
              edge_string << node_to_string(((mux_node_t *) (*it))->inputs[i]) << " -> " << node_to_string(*it);

            if (((mux_node_t *) (*it))->input_shifts[i] != 0 && ((mux_node_t *) (*it))->input_shifts[i] != DONT_CARE)
            {
              edge_string << " [label=\"";
              edge_string << ((mux_node_t *) (*it))->input_shifts[i];
              edge_string << "\",fontsize=12]\n";
            }
            else
              edge_string << "\n";
            if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end())
            {
              realized_edges[edge_string.str().c_str()] = 1;
              fprintf(graphfilepointer, "%s", edge_string.str().c_str());
            }
          }
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
        {
          for (int i = 0, i_end = (int) ((conf_adder_subtractor_node_t *) (*it))->inputs.size(); i < i_end; ++i)
          {
            stringstream edge_string;
            edge_string << node_to_string(((conf_adder_subtractor_node_t *) (*it))->inputs[i]) << " -> " << node_to_string((*it));
            edge_string << " [label=\"";
            if (((conf_adder_subtractor_node_t *) (*it))->input_shifts[i] != 0)
              edge_string << ((conf_adder_subtractor_node_t *) (*it))->input_shifts[i] << "\\n";
            for (int j = 0, j_end = (int) ((conf_adder_subtractor_node_t *) (*it))->input_is_negative.size(); j < j_end; ++j)
            {
              if (((conf_adder_subtractor_node_t *) (*it))->input_is_negative[j][i])
                edge_string << "-" << "\\n";
              else
                edge_string << "+" << "\\n";
            }
            edge_string << "\",fontsize=12]\n";
            if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end())
            {
              realized_edges[edge_string.str().c_str()] = 1;
              fprintf(graphfilepointer, "%s", edge_string.str().c_str());
            }
          }
        }
        else if (is_a<register_node_t>(*(*it)) || is_a<output_node_t>(*(*it)))
        {
          stringstream edge_string;
          edge_string << node_to_string(((register_node_t *) (*it))->input) << " -> " << node_to_string(*it);
          if (((register_node_t *) (*it))->input_shift > 0)
          {
            edge_string << "[label=\"" << ((register_node_t *) (*it))->input_shift << "\",fontsize=12]";
          }
          edge_string << "\n";
          if (realized_edges.find(edge_string.str().c_str()) == realized_edges.end())
          {
            realized_edges[edge_string.str().c_str()] = 1;
            fprintf(graphfilepointer, "%s", edge_string.str().c_str());
          }
        }
        else if (is_a<input_node_t>(*(*it)))
        {
          //do nothing
        }
        else
        {
          throw runtime_error("node type not supported");
        }

      }

      fprintf(graphfilepointer, "}\n");
      fflush(graphfilepointer);
      fclose(graphfilepointer);
      if (!quiet) std::cout << "DOT-File generated" << std::endl;
    }

/* write the graph down in mat syntax KONRAD*/

    void adder_graph_t::writesyn(string filename)
    { //, string filename
      ofstream graphfilestream;
      graphfilestream.open(filename.c_str(), ofstream::out | ofstream::trunc);
      writesyn(graphfilestream);
      graphfilestream.close();
    }

    void adder_graph_t::writesyn(ostream &graphoutstream)
    { //, string filename
      adder_graph_t mygraph = *this;

      int current_id = -1;
      time_t mytime;
      time(&mytime);

      graphoutstream << "{";
      // plot nodes
      for (std::list<adder_graph_base_node_t *>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {

        //node_t* tmp = (node_t*)(*it);
        if (!is_a<input_node_t>(*(*it)))
        {
          current_id++;
          if (current_id > 0) graphoutstream << ",";

          stringstream node_string;
          stringstream pre_node_string;
          stringstream type_string;
          type_string << "{";
          node_string << "[";
          for (int i = 0, i_end = (int) (*it)->output_factor.size(); i < i_end; ++i)
          {
            for (int j = 0, j_end = (int) (*it)->output_factor[i].size(); j < j_end; ++j)
            {
              if (j == (int) (*it)->output_factor[i].size() - 1)
              {
                if ((*it)->output_factor[i][j] != DONT_CARE)
                  node_string << (*it)->output_factor[i][j];
                else
                  node_string << "NaN";
              }
              else
              {
                if ((*it)->output_factor[i][j] != DONT_CARE)
                  node_string << (*it)->output_factor[i][j] << ",";
                else
                  node_string << "NaN,";
              }

            }
            if (i < (int) (*it)->output_factor.size() - 1)
              node_string << ";";
          }
          node_string << "]," << (*it)->stage << ",";

          if (is_a<adder_subtractor_node_t>(*(*it)) || is_a<conf_adder_subtractor_node_t>(*(*it)))
          {
            type_string << "'A',";
            for (int k = 0, k_end = (int) ((adder_subtractor_node_t *) (*it))->inputs.size(); k < k_end; ++k)
            {
              if (k == 0)
                pre_node_string << "[";
              else
                pre_node_string << ",[";
              //set outputs of pre_node
              for (int i = 0, i_end = (int) ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor.size();
                   i < i_end; ++i)
              {
                for (int j = 0, j_end = (int) ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i].size();
                     j < j_end; ++j)
                {
                  if (j == (int) ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i].size() - 1)
                  {
                    if (((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                    {
                      bool negate = false;
                      if (is_a<adder_subtractor_node_t>(*(*it)))
                      {
                        if (((adder_subtractor_node_t *) (*it))->input_is_negative[k])
                          negate = true;
                      }
                      else
                      {
                        if (((conf_adder_subtractor_node_t *) (*it))->input_is_negative[i][k])
                          negate = true;
                      }
                      if (negate)
                        pre_node_string << (-1 * ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j]);
                      else pre_node_string << ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j];
                    }
                    else
                      pre_node_string << "NaN";
                  }
                  else
                  {
                    if (((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                    {
                      bool negate = false;
                      if (is_a<adder_subtractor_node_t>(*(*it)))
                      {
                        if (((adder_subtractor_node_t *) (*it))->input_is_negative[k])
                          negate = true;
                      }
                      else
                      {
                        if (((conf_adder_subtractor_node_t *) (*it))->input_is_negative[i][k])
                          negate = true;
                      }
                      if (negate)
                        pre_node_string << (-1 * ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j])
                                        << ",";
                      else pre_node_string << ((adder_subtractor_node_t *) (*it))->inputs[k]->output_factor[i][j] << ",";
                    }
                    else
                      pre_node_string << "NaN,";
                  }

                }
                if (i < (int) (*it)->output_factor.size() - 1)
                  pre_node_string << ";";
              }
              //get input shift
              pre_node_string << "]," << ((adder_subtractor_node_t *) (*it))->inputs[k]->stage << ","
                              << ((adder_subtractor_node_t *) (*it))->input_shifts[k];
            }
            //}
          }
          if (is_a<mux_node_t>(*(*it)))
          {
            type_string << "'M',";
            map<adder_graph_base_node_t *, bool> already_seen;
            already_seen.clear();

            bool k_first = true;
            for (int k = 0, k_end = (int) ((mux_node_t *) (*it))->inputs.size(); k < k_end; ++k)
            {
              if (already_seen.find(((mux_node_t *) (*it))->inputs[k]) == already_seen.end() &&
                  ((mux_node_t *) (*it))->inputs[k] != NULL)
              {
                already_seen[((mux_node_t *) (*it))->inputs[k]] = true;
                if (k_first)
                {
                  pre_node_string << "[";
                  k_first = false;
                }
                else
                  pre_node_string << ",[";
                //get outputs of pre_node
                for (int i = 0, i_end = (int) ((mux_node_t *) (*it))->inputs[k]->output_factor.size(); i < i_end; ++i)
                {
                  for (int j = 0, j_end = (int) ((mux_node_t *) (*it))->inputs[k]->output_factor[i].size(); j < j_end; ++j)
                  {
                    if (j == (int) ((mux_node_t *) (*it))->inputs[k]->output_factor[i].size() - 1)
                    {
                      if (((mux_node_t *) (*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                      {
                        pre_node_string << ((mux_node_t *) (*it))->inputs[k]->output_factor[i][j];
                      }
                      else
                      {
                        pre_node_string << "NaN";
                      }
                    }
                    else
                    {
                      if (((mux_node_t *) (*it))->inputs[k]->output_factor[i][j] != DONT_CARE)
                      {
                        pre_node_string << ((mux_node_t *) (*it))->inputs[k]->output_factor[i][j] << ",";
                      }
                      else
                      {
                        pre_node_string << "NaN,";
                      }
                    }
                  }
                  if (i < (int) (*it)->output_factor.size() - 1)
                    pre_node_string << ";";
                }
                pre_node_string << "]," << ((mux_node_t *) (*it))->inputs[k]->stage << ",[";
                //get input shifts
                for (int i = 0, i_end = (int) ((mux_node_t *) (*it))->input_shifts.size(); i < i_end; ++i)
                {

                  if (((mux_node_t *) (*it))->input_shifts[k] != DONT_CARE && ((mux_node_t *) (*it))->inputs[k] != NULL)
                  {
                    bool same = true;
                    for (unsigned int j = 0, j_end = ((mux_node_t *) (*it))->output_factor[i].size(); j < j_end; ++j)
                    {
                      if (((mux_node_t *) (*it))->output_factor[i][j] !=
                          ((mux_node_t *) (*it))->inputs[k]->output_factor[i][j] << ((mux_node_t *) (*it))->input_shifts[i])
                        same = false;
                    }

                    if (!same)
                      pre_node_string << "NaN";
                    else
                      pre_node_string << ((mux_node_t *) (*it))->input_shifts[i];
                  }
                  else
                    pre_node_string << "NaN";
                  if (i < (int) ((mux_node_t *) (*it))->input_shifts.size() - 1)
                    pre_node_string << ";";
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
            for (int i = 0, i_end = (int) ((register_node_t *) (*it))->input->output_factor.size(); i < i_end; ++i)
            {
              for (int j = 0, j_end = (int) ((register_node_t *) (*it))->input->output_factor[i].size(); j < j_end; ++j)
              {
                if (j == (int) ((register_node_t *) (*it))->input->output_factor[i].size() - 1)
                {
                  if (((register_node_t *) (*it))->input->output_factor[i][j] != DONT_CARE)
                  {
                    pre_node_string << ((register_node_t *) (*it))->input->output_factor[i][j];
                  }
                  else
                    pre_node_string << "NaN";
                }
                else
                {
                  if (((register_node_t *) (*it))->input->output_factor[i][j] != DONT_CARE)
                    pre_node_string << ((register_node_t *) (*it))->input->output_factor[i][j] << ",";
                  else
                    pre_node_string << "NaN,";
                }
              }
              if (i < (int) (*it)->output_factor.size() - 1)
              {
                pre_node_string << ";";
              }
            }
            pre_node_string << "]," << ((register_node_t *) (*it))->input->stage;
            if (((register_node_t *) (*it))->input_shift != 0)
              pre_node_string << "," << ((register_node_t *) (*it))->input_shift;
          }

          pre_node_string << "}";


          if (!(*it)->specific_parameters.empty())
          {
            pre_node_string << ":{";
            for (std::map<std::string, std::string>::iterator its = (*it)->specific_parameters.begin();
                 its != (*it)->specific_parameters.end(); ++its)
            {
              if (!(*its).first.empty() && !(*its).second.empty())
              {
                if (its != (*it)->specific_parameters.begin())
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
      if (!specific_parameters.empty())
      {
        graphoutstream << ":{";
        for (std::map<std::string, std::string>::iterator its = specific_parameters.begin();
             its != specific_parameters.end(); ++its)
        {
          if (!(*its).first.empty() && !(*its).second.empty())
          {
            if (its != specific_parameters.begin())
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
      if (!quiet) std::cout << "Finished Syntax Output" << std::endl;
    }

    bool adder_graph_t::parse_to_graph(string commandLine, bool ignore_outnodes)
    {
      if (!quiet) cout << "parsing string: " << commandLine << endl;

      typedef enum
      {
          START, PARSING_NODE_BEGIN, PARSING_NODE_END, PARSING_NODE, PARSING_NODE_DELIM
      } state_t;
      state_t state = START;

      noOfInputs = -1;
      noOfConfigurations = -1;

      int pos = 0;
      bool complete = false;

      adder_graph_base_node_t *node;
      adder_graph_base_node_t *node_tmp;
      try
      {
        do
        {
          int nodeEndPos;
          string nodeStr;

          switch (state)
          {
            case START:
              if (commandLine.at(pos) == '{')
              {
                state = PARSING_NODE_BEGIN;
              }
              else
                throw runtime_error("{ expected");

              break;
            case PARSING_NODE_BEGIN:
              if (commandLine.at(pos) == '{')
              {
                state = PARSING_NODE;
              }
              else
                throw runtime_error("'{' expected (got '" + commandLine.substr(pos, 1) + "')");

              break;
            case PARSING_NODE:
              nodeEndPos = commandLine.find('}', pos);
              nodeStr = commandLine.substr(pos, nodeEndPos - pos);
              node = parse_node(nodeStr);

              if(!is_a<output_node_t>(*node))
                node_tmp = get_node_from_output_factor_in_stage(node->output_factor, node->stage);
              else
                node_tmp = nullptr; //ignore output nodes as there will be a node with same output factor

              if(node_tmp != nullptr)
              {
                if (is_a<adder_graph_base_node_t>(*node_tmp))
                {
                  //temporary node found, remove that node and replace by new node:
                  if (!quiet) cout << "found identical output factor, replace node " << node_tmp << " by new node " << node << endl;
                  nodes_list.remove(node_tmp);
                  for (adder_graph_base_node_t *n : nodes_list)
                  {
                    if (is_a<adder_subtractor_node_t>(*n))
                    {
                      for (int i = 0; i < ((adder_subtractor_node_t *) n)->inputs.size(); i++)
                      {
                        if (((adder_subtractor_node_t *) n)->inputs[i] == node_tmp)
                        {
                          ((adder_subtractor_node_t *) n)->inputs[i] = node; //replace node
                          if (!quiet) cout << "replacing input " << i << " of node " << n << ": node " << node_tmp << " is replaced by " << node << endl;
                        }
                      }
                    }
                    else if (is_a<mux_node_t>(*n))
                    {
                      for (int i = 0; i < ((mux_node_t *) n)->inputs.size(); i++)
                      {
                        if (((mux_node_t *) n)->inputs[i] == node_tmp)
                        {
                          ((mux_node_t *) n)->inputs[i] = node; //replace node
                          if (!quiet) cout << "replacing input " << i << " of node " << n << ": node " << node_tmp << " is replaced by " << node << endl;
                        }
                      }
                    }
                    else if (is_a<mux_node_t>(*n))
                    {
                      for (int i = 0; i < ((mux_node_t *) n)->inputs.size(); i++)
                      {
                        if (((mux_node_t *) n)->inputs[i] == node_tmp)
                        {
                          ((mux_node_t *) n)->inputs[i] = node; //replace node
                          if (!quiet) cout << "replacing input " << i << " of node " << n << ": node " << node_tmp << " is replaced by " << node << endl;
                        }
                      }
                    }
                    else if ((is_a<register_node_t>(*n)) || (is_a<output_node_t>(*n)))
                    {
                      if (((register_node_t *) n)->input == node_tmp)
                      {
                        if (!quiet) cout << "replacing input of node " << n << ": node " << node_tmp << " is replaced by " << node << endl;
                        ((register_node_t *) n)->input = node;
                      }
                    }
                    else if ((is_a<adder_graph_base_node_t>(*n)) || (is_a<input_node_t>(*n)))
                    {
                      //ignore temporary nodes, those don't have an assigned input
                    }
                    else
                    {
                      throw runtime_error("unsupported node type " + string(typeid(*n).name()));
                    }
                  }
                }
                else if (is_a<output_node_t>(*node_tmp))
                {
                  //ignore output nodes as those will have the same value as other nodes and are not temporary nodes
                }
                else
                {
                  throw runtime_error("unsupported node type of temporary node " + string(typeid(*node_tmp).name()));
                }
              }
              nodes_list.push_back(node);

              pos += nodeEndPos - pos - 1;
              state = PARSING_NODE_END;
              break;
            case PARSING_NODE_END:
              if (commandLine.at(pos) == '}')
              {
                state = PARSING_NODE_DELIM;
              }
              else
                throw runtime_error("'}' expected (got '" + commandLine.substr(pos, 1) + "')");

              break;
            case PARSING_NODE_DELIM:
              if (commandLine.at(pos) == ',')
              {
                state = PARSING_NODE_BEGIN;
              }
              else if (commandLine.at(pos) == '}')
              {
                complete = true;
              }
              else
                throw runtime_error("',' or '}' expected (got '" + commandLine.substr(pos, 1) + "')");
              break;
            default:
              throw runtime_error("invalid state in parse_to_graph()");
          }
          pos++;
        } while (!complete);
      }
      catch (const std::exception &e)
      {
        cout << commandLine << endl;
        for (int i = 0; i < pos; i++)
          cout << "-";
        cout << "^" << endl;
        cout << "parsing error: " << e.what() << endl;
        exit(-1);//!!!
      }

      if (!quiet) cout << "parsing successful!" << endl;
      return true;
    }


    adder_graph_base_node_t *adder_graph_t::parse_node(string nodeStr)
    {
      if (!quiet) cout << "parsing node {" << nodeStr << "}" << endl;

      typedef enum
      {
          NODE_ID_DELIMITER1,
          NODE_ID_DELIMITER2,
          NODE_ID,
          NODE_ELEMENT_DELIMITER,
          NODE_OUT_FACTOR,
          NODE_OUT_STAGE,
          NODE_ARG_VALUE,
          NODE_ARG_STAGE,
          NODE_ARG_SHIFT
      } state_t;
      state_t state = NODE_ID_DELIMITER1;
      state_t stateNext;

      int pos = 0;
      bool complete = false;
      char nodeId;
      int elemEndPos, elemBeginPos;
      string elemStr;
      int stage;
      int shift;
      int argNo = 0;
      adder_graph_base_node_t *node;

      try
      {
        std::vector<std::vector<int64_t> > factor_norm;
        std::vector<std::vector<int64_t> > factor;

        do
        {
          adder_graph_base_node_t *input_node;

          switch (state)
          {
            case NODE_ID_DELIMITER1:
              if (nodeStr.at(pos) == '\'')
              {
                state = NODE_ID;
              }
              else
                throw runtime_error("'\'' expected (got '" + nodeStr.substr(pos, 1) + "')");

              break;
            case NODE_ID:
              nodeId = nodeStr.at(pos);
              if (!quiet) cout << "found node of type " << nodeId << endl;
              switch (nodeId)
              {
                case 'A':
                  node = new adder_subtractor_node_t();
                  break;
                case 'R':
                  node = new register_node_t();
                  break;
                case 'O':
                  node = new output_node_t();
                  break;
                case 'M':
                  node = new mux_node_t();
                  break;
                default:
                  throw runtime_error("node with identifier '" + nodeStr.substr(pos, 1) + "' unknown or not supported");
              }
              state = NODE_ID_DELIMITER2;

              break;
            case NODE_ID_DELIMITER2:
              if (nodeStr.at(pos) == '\'')
              {
                state = NODE_ELEMENT_DELIMITER;
                stateNext = NODE_OUT_FACTOR;
              }
              else
                throw runtime_error("'\'' expected (got '" + nodeStr.substr(pos, 1) + "')");

              break;
            case NODE_ELEMENT_DELIMITER:
              if (nodeStr.at(pos) == ',')
              {
                state = stateNext;
              }
              else
                throw runtime_error("',' expected (got '" + nodeStr.substr(pos, 1) + "')");

              break;

            case NODE_OUT_FACTOR:
              elemBeginPos = nodeStr.find('[', pos);

              if (elemBeginPos == string::npos)
                throw runtime_error("'[' expected (got '" + nodeStr.substr(pos, 1) + "')");

              elemEndPos = nodeStr.find(']', pos + 1);
              if (elemEndPos == string::npos)
                throw runtime_error("']' expected");

              elemStr = nodeStr.substr(elemBeginPos, elemEndPos - elemBeginPos + 1);
              parse_factor(elemStr, &(node->output_factor));
              pos += elemEndPos - elemBeginPos;
              state = NODE_ELEMENT_DELIMITER;
              stateNext = NODE_OUT_STAGE;
              break;

            case NODE_OUT_STAGE:
              elemEndPos = nodeStr.find(',', pos);
              elemStr = nodeStr.substr(pos, elemEndPos - pos);

              node->stage = stoi(elemStr);

              if (!quiet) cout << " stage of node is " << node->stage << endl;

              pos += elemEndPos - pos - 1;
              state = NODE_ELEMENT_DELIMITER;
              stateNext = NODE_ARG_VALUE;
              argNo = 1;
              break;

            case NODE_ARG_VALUE:
              elemBeginPos = nodeStr.find('[', pos);

              if (elemBeginPos == string::npos)
                throw runtime_error("'[' expected (got '" + nodeStr.substr(pos, 1) + "')");

              elemEndPos = nodeStr.find(']', pos + 1);
              if (elemEndPos == string::npos)
                throw runtime_error("']' expected");

              elemStr = nodeStr.substr(elemBeginPos, elemEndPos - elemBeginPos + 1);
              parse_factor(elemStr, &factor);

              factor_norm = normalize(factor);

              pos += elemEndPos - elemBeginPos;
              state = NODE_ELEMENT_DELIMITER;
              stateNext = NODE_ARG_STAGE;
              break;

            case NODE_ARG_STAGE:
              elemEndPos = nodeStr.find(',', pos);
              elemStr = nodeStr.substr(pos, elemEndPos - pos);

              stage = stoi(elemStr);

              if (!quiet) cout << " stage of arg is " << stage << endl;

              input_node = get_node_from_output_factor_in_stage(factor_norm, stage);

              if (input_node == nullptr)
              {
                //create temporary node:
                input_node = new adder_graph_base_node_t();
                input_node->output_factor = factor_norm;
                input_node->stage = stage;
                if (!quiet) cout << "adding temporary node " << input_node << " with factor " << input_node->output_factor << " at stage " << input_node->stage << endl;
                nodes_list.push_back(input_node);
              }
              if (!quiet) cout << "adding input to node " << node << endl;
              if (nodeId == 'A')
              {
                ((adder_subtractor_node_t *) node)->inputs.push_back(input_node);
                ((adder_subtractor_node_t *) node)->input_is_negative.push_back(!(factor_norm == factor));
              }
              else if (nodeId == 'M')
              {
                ((mux_node_t *) node)->inputs.push_back(input_node);
              }
              else if ((nodeId == 'R') || (nodeId == 'O'))
              {
                ((register_node_t *) node)->input = input_node;
              }

              pos += elemEndPos - pos - 1;
              state = NODE_ELEMENT_DELIMITER;
              stateNext = NODE_ARG_SHIFT;

              if (nodeId == 'R')
                complete = true; //for registers, we are done

              break;

            case NODE_ARG_SHIFT:
              elemEndPos = nodeStr.find(',', pos);
              elemStr = nodeStr.substr(pos, elemEndPos - pos);

              if (elemEndPos < 0) //no ',' was found, so take the shift until the end of the string
              {
                elemEndPos = nodeStr.length() - 1;
              }

              if (nodeId == 'M')
              {
                std::vector<std::vector<int64_t> > f;
                parse_factor(elemStr, &f);
                assert(f.size() > 0);
                for (int i = 0; i < f[0].size(); i++)
                  ((mux_node_t *) node)->input_shifts.push_back(f[0][i]);
              }
              else if (nodeId == 'A')
              {
                shift = stoi(elemStr);
                ((adder_subtractor_node_t *) node)->input_shifts.push_back(shift);
              }
              else if ((nodeId == 'R') || (nodeId == 'O'))
              {
                shift = stoi(elemStr);
                ((register_node_t *) node)->input_shift = shift;
              }
              else
              {
                throw runtime_error("node type not handled");
              }

              pos += elemEndPos - pos - 1;
              state = NODE_ELEMENT_DELIMITER;
              stateNext = NODE_ARG_VALUE;
              argNo++;

              if (pos + 1 == nodeStr.length() - 1)
                complete = true;//!!!
              break;

            default:
              cout << "invalid state in parse_node()" << endl;
              throw runtime_error("invalid state");
          }
          pos++;
        } while (!complete);
      }
      catch (const std::exception &e)
      {
        cout << "node parsing error: " << e.what() << endl;
        cout << nodeStr << endl;
        for (int i = 0; i < pos + 1; i++)
          cout << "-";
        cout << "^" << endl;
        exit(-1);
      }
      return node;
    }

    bool adder_graph_t::parse_factor(string factorStr, std::vector<std::vector<int64_t> > *factor)
    {
      if (!quiet) cout << "parsing factor " << factorStr << endl;

      factorStr = factorStr.substr(1, factorStr.length() - 2); //remove closing brackets

      typedef enum
      {
          FACTOR_DELIMITER, FACTOR_VALUE
      } state_t;
      state_t state = FACTOR_VALUE;

      int row = 0; //row of factor matrix
      int col = 0; //column of factor matrix

      int pos = 0;
      bool complete = false;
      int commaPos;
      int semicolonPos;
      int stringEndPos;
      int minPos;
      int64_t value;

      string elemStr;

      if ((noOfInputs == -1) && (noOfConfigurations == -1))
      {
        factor->resize(1); //allocate the first row
      }
      else
      {
        factor->resize(noOfConfigurations, std::vector<int64_t>(noOfInputs)); //allocate complete matrix
      }

      try
      {
        do
        {
          switch (state)
          {
            case FACTOR_VALUE:
              commaPos = factorStr.find(',', pos);
              semicolonPos = factorStr.find(';', pos);
              stringEndPos = factorStr.length();

              commaPos < 0 ? commaPos = INT_MAX : commaPos;
              semicolonPos < 0 ? semicolonPos = INT_MAX : semicolonPos;

              minPos = min(commaPos, semicolonPos);
              minPos = min(minPos, stringEndPos);

              elemStr = factorStr.substr(pos, minPos - pos);

              flush(cout);
              if (elemStr == "NaN")
                value = DONT_CARE;
              else
                value = stoi(elemStr);

              //resize matrix if necessary:
              if ((*factor).size() <= row)
                (*factor).resize(row + 1);

              if ((*factor)[row].size() <= col)
                (*factor)[row].resize(col + 1);

              (*factor)[row][col] = value;

              pos = minPos - 1;

              state = FACTOR_DELIMITER;
              break;
            case FACTOR_DELIMITER:

              if (pos == factorStr.length())
              {
                complete = true;
                break;
              }
              else if (factorStr[pos] == ' ') //ignore spaces
                pos++;
              else if (factorStr[pos] == ',')
              {
                col++;
                state = FACTOR_VALUE;
              }
              else if (factorStr[pos] == ';')
              {
                row++;
                state = FACTOR_VALUE;
              }
              break;

            default:
              cout << "invalid state in parse_factor()" << endl;
              throw runtime_error("invalid state");
          }
          pos++;
        } while (!complete);
      }
      catch (const std::exception &e)
      {
        cout << "factor parsing error: " << e.what() << endl;
        cout << factorStr << endl;
        for (int i = 0; i < pos + 1; i++)
          cout << "-";
        cout << "^" << endl;
        exit(-1);
      }

      if ((noOfInputs == -1) && (noOfConfigurations == -1))
      {
        noOfConfigurations = (*factor).size();
        noOfInputs = (*factor)[0].size();

        for (int i = 0; i < noOfInputs; i++)
        {
          input_node_t *input_node = new input_node_t();
          input_node->output_factor.resize(noOfConfigurations);
          for (int r = 0; r < input_node->output_factor.size(); r++)
          {
            input_node->output_factor[r].resize(noOfInputs);
            for (int c = 0; c < input_node->output_factor[r].size(); c++)
            {
              input_node->output_factor[r][c] = (i == c);
            }
          }
          nodes_list.push_back(input_node);
        }
      }
      else
      {
        assert(noOfConfigurations == (*factor).size());
        assert(noOfInputs == (*factor)[0].size());
      }
      if (!quiet) cout << "factor is a " << (*factor).size() << " x " << (*factor)[0].size() << " matrix: " << (*factor) << endl;
      return complete;

    }

    void adder_graph_t::print_graph()
    {
      adder_graph_t mygraph = *this;

      if (!mygraph.specific_parameters.empty())
      {
        std::cout << "Attached parameters to this graph:" << std::endl;
        for (std::map<std::string, std::string>::iterator its = mygraph.specific_parameters.begin();
             its != mygraph.specific_parameters.end(); ++its)
        {
          if (!(*its).first.empty() && !(*its).second.empty())
          {
            std::cout << "\t" << (*its).first << " = " << (*its).second << ";" << std::endl;
          }
        }
      }

      for (std::list<adder_graph_base_node_t *>::iterator it = mygraph.nodes_list.begin(), it_end = mygraph.nodes_list.end(); it != it_end; ++it)
      {
        adder_graph_base_node_t *p = *it;
        std::cout << "\nNode type of node " << (*it) << " in stage " << (*it)->stage << " is: " << typeid(*p).name() << std::endl;
        std::cout << "The factor is " << (*it)->output_factor << endl;
        std::cout << "The outputs are: ";
        //std::cout << "Size:"<< (*it)->output_factor.size();
        for (unsigned int k = 0, k_end = (*it)->output_factor.size(); k < k_end; ++k)
        {
          if (k > 0) std::cout << "; ";
          for (unsigned int f = 0, f_end = (*(*it)).output_factor[k].size(); f < f_end; ++f)
          {
            if (f > 0) std::cout << ", ";

            std::cout << (*(*it)).output_factor[k][f] << " ";
          }
        }
        std::cout << std::endl;

        if (is_a<input_node_t>(*(*it)))
        {
          //std::cout << "No input" << std::endl;
        }
        else if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          adder_subtractor_node_t *t = (adder_subtractor_node_t *) (*it);

          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
            std::cout << "Input: " << t->inputs[k] << ", shift: " << t->input_shifts[k] << ", in stage "
                      << t->inputs[k]->stage << ", is negative: " << t->input_is_negative[k] << std::endl;
        }
        else if (is_a<mux_node_t>(*(*it)))
        {
          mux_node_t *t = (mux_node_t *) (*it);
          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
            if (t->inputs[k] != NULL)
            {
              std::cout << "Input: " << t->inputs[k] << ", shift: ";
              if (t->input_shifts[k] != DONT_CARE)
                std::cout << t->input_shifts[k];
              else
                std::cout << "NaN";
              std::cout << ", in stage " << t->inputs[k]->stage << std::endl;
            }
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
        {
          conf_adder_subtractor_node_t *t = (conf_adder_subtractor_node_t *) (*it);
          for (int k = 0, k_end = (int) t->inputs.size(); k < k_end; ++k)
          {
            std::cout << "Input: " << t->inputs[k] << ", shift: " << t->input_shifts[k] << ", in stage "
                      << t->inputs[k]->stage << ", is negative: ";
            for (int j = 0, j_end = (int) t->input_is_negative.size(); j < j_end; ++j)
              std::cout << t->input_is_negative[j][k] << ", ";
            std::cout << std::endl;
          }
        }
        else if ((is_a<register_node_t>(*(*it))) || (is_a<output_node_t>(*(*it))))
        {
          register_node_t *t = (register_node_t *) (*it);
          std::cout << "Input: " << t->input << ", shift: " << t->input_shift << ", in stage " << t->input->stage
                    << std::endl;
        }
        else
        {
          throw runtime_error("Node type unknown");
        }

        if (!(*it)->specific_parameters.empty())
        {
          std::cout << "Attached parameters:" << std::endl;
          for (std::map<std::string, std::string>::iterator its = (*it)->specific_parameters.begin();
               its != (*it)->specific_parameters.end(); ++its)
          {
            if (!(*its).first.empty() && !(*its).second.empty())
            {
              std::cout << "\t" << (*its).first << " = " << (*its).second << ";" << std::endl;
            }
          }
        }
      }
      std::cout << "Printed all" << std::endl;
    }

    void adder_graph_t::check_and_correct(string graphstring)
    {
      if (graphstring == "") graphstring = "STRINGSYN NOT AVAILABLE";

      list<string> errorList;
      map<int, vector<string> > configurationCountMap;
      map<int, vector<string> > inputCountMap;
      configurationCountMap.clear();
      inputCountMap.clear();
      //check for errors
      for (list<adder_graph_base_node_t *>::iterator it = nodes_list.begin(), it_end = nodes_list.end();
           it != it_end;
           ++it)
      {
        stringstream node_name;
        if (is_a<input_node_t>(*(*it))) node_name << "INP@";
        else if (is_a<adder_subtractor_node_t>(*(*it))) node_name << "ADD@";
        else if (is_a<mux_node_t>(*(*it))) node_name << "MUX@";
        else if (is_a<conf_adder_subtractor_node_t>(*(*it))) node_name << "CONF_ADD@";
        else if (is_a<register_node_t>(*(*it))) node_name << "REG@";
        node_name << "[";
        for (unsigned int cfg = 0, cfg_end = (*it)->output_factor.size(); cfg < cfg_end; ++cfg)
        {
          if (cfg > 0) node_name << ";";
          for (unsigned int v = 0, v_end = (*it)->output_factor[cfg].size(); v < v_end; ++v)
          {
            if (v > 0) node_name << ",";
            if ((*it)->output_factor[cfg][v] == DONT_CARE) node_name << "-";
            else node_name << (*it)->output_factor[cfg][v];
          }
        }
        node_name << "],stage " << (*it)->stage;

        if ((*it)->output_factor.size() == 0)
        {
          errorList.push_back("OUTPUTFACTOR MISSING: " + node_name.str());
          continue;
        }
        if ((*it)->output_factor[0].size() == 0)
        {
          errorList.push_back("OUTPUTFACTOR INNER MISSING: " + node_name.str());
          continue;
        }
        map<int, vector<string> >::iterator pr1 = configurationCountMap.find((*it)->output_factor.size());
        if (pr1 != configurationCountMap.end())
        {
          (*pr1).second.push_back(node_name.str());
        }
        else
        {
          vector<string> newvec;
          newvec.push_back(node_name.str());
          configurationCountMap.insert(make_pair((*it)->output_factor.size(), newvec));
        }

        map<int, vector<string> >::iterator pr2 = inputCountMap.find((*it)->output_factor[0].size());
        if (pr2 != inputCountMap.end())
        {
          (*pr2).second.push_back(node_name.str());
        }
        else
        {
          vector<string> newvec;
          newvec.push_back(node_name.str());
          inputCountMap.insert(make_pair((*it)->output_factor[0].size(), newvec));
        }

        if (is_a<input_node_t>(*(*it)))
        {

        }
        else if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          adder_subtractor_node_t *t = (adder_subtractor_node_t *) (*it);
          //check for NULL Inputs
          int inputs_notnull = 0;
          bool not_found_error = false;
          int max_neg_shift = 0;
          for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
          {
            if (t->inputs[i] != NULL)
            {
              inputs_notnull++;
              if (std::find(nodes_list.begin(), nodes_list.end(), t->inputs[i]) == nodes_list.end())
              {
                errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str());
                not_found_error = true;
              }
              if (t->input_shifts[i] < 0 && t->input_shifts[i] < max_neg_shift)
                max_neg_shift = t->input_shifts[i];
            }
          }

          max_neg_shift = abs(max_neg_shift);
          if (not_found_error) continue;
          if (inputs_notnull == 0)
          {
            errorList.push_back("ALL INPUTS NULL: " + node_name.str());
            continue;
          }
          else if (inputs_notnull == 1)
          {
            errorList.push_back("INPUT NULL: " + node_name.str());
            continue;
          }

          if (t->input_is_negative.size() != t->inputs.size())
          {
            errorList.push_back("ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str());
            continue;
          }

          //check output values
          for (unsigned int cfg = 0, cfg_end = t->output_factor.size(); cfg < cfg_end; ++cfg)
          {
            for (unsigned int v = 0, v_end = t->output_factor[cfg].size(); v < v_end; ++v)
            {
              int64_t computed_output = 0;
              for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
              {
                if (t->inputs[i] != NULL)
                {
                  if (t->input_is_negative[i])
                  {
                    computed_output -= (t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i] + max_neg_shift));
                  }
                  else
                  {
                    computed_output += (t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i] + max_neg_shift));
                  }
                }
              }
              if ((computed_output >> max_neg_shift) != t->output_factor[cfg][v] && t->output_factor[cfg][v] != DONT_CARE)
              {
                stringstream tmp;
                tmp << "\t(computed:" << (computed_output >> abs(max_neg_shift)) << " != given:" << t->output_factor[cfg][v]
                    << ")";
                errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() + tmp.str());
                continue;
              }
            }
          }
        }
        else if (is_a<mux_node_t>(*(*it)))
        {
          mux_node_t *t = (mux_node_t *) (*it);
          int muxInputCount = 0;
          bool not_found_error = false;
          for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
          {
            if (t->inputs[i] != NULL)
            {
              muxInputCount++;
              if (std::find(nodes_list.begin(), nodes_list.end(), t->inputs[i]) == nodes_list.end())
              {
                errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str());
                not_found_error = true;
              }
            }
          }
          if (not_found_error) continue;
          if (muxInputCount == 0)
          {
            errorList.push_back("ALL INPUTS NULL: " + node_name.str());
            continue;
          }
          else
          {
            for (unsigned int cfg = 0, cfg_end = t->output_factor.size(); cfg < cfg_end; ++cfg)
            {
              for (unsigned int v = 0, v_end = t->output_factor[cfg].size(); v < v_end; ++v)
              {
                if (t->inputs[cfg] == NULL)
                {
                  if (t->output_factor[cfg][v] != 0 && t->output_factor[cfg][v] != DONT_CARE)
                  {
                    errorList.push_back("OUTPUTFACTOR MISSMATCH INPUT NULL: " + node_name.str());
                  }
                }
                else
                {
                  if ((t->inputs[cfg]->output_factor[cfg][v] << t->input_shifts[cfg]) != t->output_factor[cfg][v] &&
                      t->output_factor[cfg][v] != DONT_CARE)
                  {

                    stringstream tmp;
                    tmp << "\t(" << t->inputs[cfg]->output_factor[cfg][v] << " != " << t->input_shifts[cfg] << ")";
                    errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() + " cfg " + to_string(cfg) + " input " + to_string(v) + tmp.str());
                  }
                }
              }
            }
          }
        }
        else if (is_a<conf_adder_subtractor_node_t>(*(*it)))
        {
          conf_adder_subtractor_node_t *t = (conf_adder_subtractor_node_t *) (*it);

          bool not_found_error = false;
          int max_neg_shift = 0;
          for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
          {
            if (t->inputs[i] != NULL)
            {
              if (std::find(nodes_list.begin(), nodes_list.end(), t->inputs[i]) == nodes_list.end())
              {
                errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str());
                not_found_error = true;
              }
              if (t->input_shifts[i] < 0 && t->input_shifts[i] < max_neg_shift)
                max_neg_shift = t->input_shifts[i];
            }
          }
          max_neg_shift = abs(max_neg_shift);
          if (not_found_error) continue;

          if (t->input_is_negative.size() != t->output_factor.size())
          {
            errorList.push_back("OUTER ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str());
            continue;
          }

          for (unsigned int cfg = 0; cfg < t->input_is_negative.size(); cfg++)
          {
            if (t->input_is_negative[cfg].size() != t->inputs.size())
            {
              errorList.push_back("INNER ISNEGATIVE VECTOR SIZE DOESNT MATCH INPUT SIZE: " + node_name.str());
              continue;
            }

          }

          for (unsigned int cfg = 0, cfg_end = t->output_factor.size(); cfg < cfg_end; ++cfg)
          {
            for (unsigned int v = 0, v_end = t->output_factor[cfg].size(); v < v_end; ++v)
            {
              //int64_t neg_shift=0;
              int64_t computed_output = 0;
              for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
              {
                if (t->inputs[i] != NULL)
                {
                  if (t->input_is_negative[cfg][i])
                  {
                    computed_output -= (t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i] + max_neg_shift));
                  }
                  else
                  {
                    computed_output += (t->inputs[i]->output_factor[cfg][v] << (t->input_shifts[i] + max_neg_shift));
                  }
                }
              }
              if ((computed_output >> abs(max_neg_shift)) != t->output_factor[cfg][v] &&
                  t->output_factor[cfg][v] != DONT_CARE)
              {
                stringstream tmp;
                tmp << "\t(computed:" << (computed_output >> abs(max_neg_shift)) << " != given:" << t->output_factor[cfg][v]
                    << ")";
                errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str() + tmp.str());
              }
            }
          }
        }
        else if (is_a<register_node_t>(*(*it)))
        {
          register_node_t *t = (register_node_t *) (*it);
          for (unsigned int cfg = 0, cfg_end = t->output_factor.size(); cfg < cfg_end; ++cfg)
          {
            for (unsigned int v = 0, v_end = t->output_factor[cfg].size(); v < v_end; ++v)
            {
              if (t->input == NULL)
              {
                errorList.push_back("INPUT OF REG IS NULL: " + node_name.str());
              }
              else
              {
                if (std::find(nodes_list.begin(), nodes_list.end(), t->input) == nodes_list.end())
                {
                  errorList.push_back("NODE_LIST DOES NOT CONTAIN INPUT: " + node_name.str());
                  v = v_end;
                  cfg = cfg_end;
                  break;
                }

              }
              if (abs(t->input->output_factor[cfg][v] << t->input_shift) != abs(t->output_factor[cfg][v]) &&
                  t->output_factor[cfg][v] != DONT_CARE)
              {
                errorList.push_back("OUTPUTFACTOR MISSMATCH: " + node_name.str());
              }
            }
          }
        }
      }
      if (configurationCountMap.size() > 1)
      {
        stringstream error;
        error << "CONFIGURATION COUNT MISSMATCH:" << endl;
        for (map<int, vector<string> >::iterator pr1 = configurationCountMap.begin(), pr1_end = configurationCountMap.end();
             pr1 != pr1_end;
             ++pr1)
        {
          error << "\tNODES WITH " << (*pr1).first << " CFG(S):" << endl;
          for (unsigned int i = 0, i_end = (*pr1).second.size(); i < i_end; ++i)
          {
            error << "\t\t" << (*pr1).second[i] << endl;
          }
        }
        errorList.push_back(error.str());
      }
      if (inputCountMap.size() > 1)
      {
        stringstream error;
        error << "INPUT COUNT MISSMATCH:" << endl;
        for (map<int, vector<string> >::iterator pr1 = inputCountMap.begin(), pr1_end = inputCountMap.end();
             pr1 != pr1_end;
             ++pr1)
        {
          error << "\tNODES WITH " << (*pr1).first << " INPUT(S):" << endl;
          for (unsigned int i = 0, i_end = (*pr1).second.size(); i < i_end; ++i)
          {
            error << "\t\t" << (*pr1).second[i] << endl;
          }
        }
        errorList.push_back(error.str());
      }

      if (errorList.size() > 0)
      {
        for (list<string>::iterator iter = errorList.begin(), iter_end = errorList.end(); iter != iter_end; ++iter)
        {
          cout << *iter << endl;
        }
        throw runtime_error("Parse check failed");
      }
    }

    void adder_graph_t::normalize_graph()
    {

      list<adder_graph_base_node_t *> neg_shift_list;
      int max_stage = 0;
      for (list<adder_graph_base_node_t *>::iterator it = nodes_list.begin(), it_end = nodes_list.end();
           it != it_end;
           ++it)
      {
        if (max_stage < (*it)->stage) max_stage = (*it)->stage;

        if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          adder_subtractor_node_t *t = (adder_subtractor_node_t *) *it;
          if (t->input_shifts[0] < 0) neg_shift_list.push_back(*it);
        }
      }

      for (list<adder_graph_base_node_t *>::iterator it = nodes_list.begin(), it_end = nodes_list.end();
           it != it_end;
           ++it)
      {
        if ((*it)->stage == max_stage) continue;

        if (is_a<adder_subtractor_node_t>(*(*it)))
        {
          adder_subtractor_node_t *t = (adder_subtractor_node_t *) (*it);
          for (unsigned int i = 0, i_end = t->inputs.size(); i < i_end; ++i)
          {
            list<adder_graph_base_node_t *>::iterator find = std::find(neg_shift_list.begin(), neg_shift_list.end(),
                                                                       t->inputs[i]);
            if (find != neg_shift_list.end())
            {
              int neg_shift = ((adder_subtractor_node_t *) (*find))->input_shifts[0];
              for (unsigned int j = 0, j_end = ((adder_subtractor_node_t *) (*find))->input_shifts.size(); j < j_end; ++j)
              {
                ((adder_subtractor_node_t *) (*find))->input_shifts[j] = 0;
              }
              t->input_shifts[i] += neg_shift;
            }
          }
        }
        else if (is_a<register_node_t>(*(*it)))
        {
          register_node_t *t = (register_node_t *) (*it);
          list<adder_graph_base_node_t *>::iterator find = std::find(neg_shift_list.begin(), neg_shift_list.end(),
                                                                     t->input);
          if (find != neg_shift_list.end())
          {
            int neg_shift = ((adder_subtractor_node_t *) (*find))->input_shifts[0];
            for (unsigned int j = 0, j_end = ((adder_subtractor_node_t *) (*find))->input_shifts.size(); j < j_end; ++j)
            {
              ((adder_subtractor_node_t *) (*find))->input_shifts[j] = 0;
            }
            t->input_shift += neg_shift;
          }
        }
      }

    }

    string adder_graph_t::convert_old_syntax(string commandLine)
    {
      bool replace_active = false;
      for (unsigned int i = 0, i_end = commandLine.size(); i < i_end; ++i)
      {
        if (commandLine[i] == '[') replace_active = true;
        if (commandLine[i] == ']') replace_active = false;
        if (replace_active && commandLine[i] == ';') commandLine[i] = ',';
      }

      string zeroString;
      {
        int p1 = commandLine.find("["), p2 = commandLine.find("]");
        string firstOutput = commandLine.substr(p1 + 1, p2 - p1 - 1);

        int cmm_size = 1;
        for (unsigned int f = 0, f_end = firstOutput.size(); f < f_end; ++f)
        {
          if (firstOutput[f] == ',') cmm_size++;
        }
        zeroString = "[";
        for (int i = 0; i < cmm_size; ++i)
        {
          if (i > 0) zeroString += ",";
          zeroString += "0";
        }
        zeroString += "]";
      }

      stringstream new_k_adder_graph;
      new_k_adder_graph << "{";

      commandLine = commandLine.substr(commandLine.find("{") + 1);
      commandLine = commandLine.substr(0, commandLine.find_last_of("}"));
      bool first = true;
      while (commandLine.find("{") != string::npos)
      {
        if (!first)
        { new_k_adder_graph << ","; }
        first = false;
        new_k_adder_graph << "{";
        int i1 = commandLine.find("{") + 1, i2 = commandLine.find("}");
        string node_string = commandLine.substr(i1, i2 - i1);

        if (node_string.find(zeroString.c_str()) != string::npos)
        {
          int i3 = node_string.find(zeroString.c_str());
          new_k_adder_graph << "'R',";
          string tmp_node_string = node_string.substr(0, i3 - 1);
          if (tmp_node_string.substr(tmp_node_string.find_last_of(",") + 1) == "0")
          {
            new_k_adder_graph << tmp_node_string.substr(0, tmp_node_string.find_last_of(","));
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
        commandLine = commandLine.substr(i2 + 1);
      }
      new_k_adder_graph << "}";
      return new_k_adder_graph.str();
    }

    std::ostream &operator<<(std::ostream &stream, const std::vector<std::vector<int64_t> > &matrix)
    {
      for (int r = 0; r < matrix.size(); r++)
      {
        for (int c = 0; c < matrix[r].size(); c++)
        {
          stream << matrix[r][c];
          if (c < matrix[r].size() - 1) stream << ",";
        }
        if (r < matrix.size() - 1) stream << ";";
      }
      return stream;
    }

}