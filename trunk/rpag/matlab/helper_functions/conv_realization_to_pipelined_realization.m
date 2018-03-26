function [pipelined_realization_graph,register_cost] = conv_realization_to_pipelined_realization(realization_graph, output_coeffs)

register_cost=0;

AD = compute_adder_depth(realization_graph);
no_of_stages = max(AD(:,2));

computed_nodes = cell(1,no_of_stages+1);
computed_nodes{1}=1; %index is stage+1

output_coeffs = fundamental(abs(output_coeffs));

pipelined_realization_graph={};
%pipelined_realization_graph=cell(1,length(realization_graph))
k=1;
for stage=1:no_of_stages
  for i=1:size(realization_graph,1)
    if realization_graph(i,1) > 0 %skip already processed elements
      if ~any(abs(realization_graph(i,2))==computed_nodes{stage})
        if (stage > 1) && any(abs(realization_graph(i,2))==computed_nodes{stage-1})
          %input 1 of adder is not available so it has to be duplicated using a register
          pipelined_realization_graph{k} = {'R',abs(realization_graph(i,2)),stage-1,abs(realization_graph(i,2)),stage-2};
          computed_nodes{stage} = [computed_nodes{stage}, abs(realization_graph(i,2))];
          register_cost = register_cost+1;
          k = k + 1;
        end
      end
      if ~any(abs(realization_graph(i,4))==computed_nodes{stage})
        if (stage > 1) && any(abs(realization_graph(i,4))==computed_nodes{stage-1})
          %input 2 of adder is not available so it has to be duplicated using a register
          pipelined_realization_graph{k} = {'R',abs(realization_graph(i,4)),stage-1,abs(realization_graph(i,4)),stage-2};
          computed_nodes{stage} = [computed_nodes{stage}, abs(realization_graph(i,4))];
          register_cost = register_cost+1;
          k = k + 1;
        end
      end
      if any(abs(realization_graph(i,2))==computed_nodes{stage}) && any(abs(realization_graph(i,4))==computed_nodes{stage})
        %node can be computed in current stage
        pipelined_realization_graph{k} = {'A',realization_graph(i,1),stage,realization_graph(i,2),stage-1,realization_graph(i,3),realization_graph(i,4),stage-1,realization_graph(i,5)};
        computed_nodes{stage+1} = [computed_nodes{stage+1}, realization_graph(i,1)];
        realization_graph(i,:) = -ones(1,size(realization_graph(i,:),2)); %remove row from matrix
        k = k + 1;
      end
    end
  end
end  
for stage=2:no_of_stages
  missing_coeffs = setdiff(output_coeffs,computed_nodes{stage+1});
  for i=1:length(missing_coeffs)
    if any(computed_nodes{stage} == missing_coeffs(i))
      disp(['coeff ',num2str(missing_coeffs(i)),' missing, adding register']);
      pipelined_realization_graph{k} = {'R',abs(missing_coeffs(i)),stage,abs(missing_coeffs(i)),stage-1};
      computed_nodes{stage+1} = [computed_nodes{stage+1}, abs(missing_coeffs(i))];
      register_cost = register_cost+1;
      k = k + 1;
    end
  end
end

