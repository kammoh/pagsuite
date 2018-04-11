function [add_count,register_count,mux_count,pag_cost,no_of_stages] = analyze_pipelined_realization(pipelined_adder_graph)

global verbose;

no_of_stages=-1;
for i=1:size(pipelined_adder_graph,2)
  no_of_stages = max(no_of_stages,pipelined_adder_graph{i}{3});
end


add_count=0;
register_count=0;
pag_cost=0;
mux_count=0;

register_count_per_stage = zeros(1,no_of_stages);

for i=1:size(pipelined_adder_graph,2)
  if pipelined_adder_graph{i}{1} == 'A'
    add_count = add_count + 1;
    pag_cost = pag_cost + 1;
  elseif pipelined_adder_graph{i}{1} == 'R'
    register_count = register_count + 1;
    register_count_per_stage(pipelined_adder_graph{i}{3}) = register_count_per_stage(pipelined_adder_graph{i}{3}) + 1;
    pag_cost = pag_cost + 1;
  elseif pipelined_adder_graph{i}{1} == 'M'
    mux_count = mux_count + 1;
    pag_cost = pag_cost + 1;
  elseif pipelined_adder_graph{i}{1} == 'O'
%    disp(strrep(mat2str(pipelined_adder_graph{i}{2}), ' ', ','));
  else
    error(['Unexpected token ',pipelined_adder_graph{i}{1}]);
  end
  
  no_of_stages = max(no_of_stages,pipelined_adder_graph{i}{3});
end
assert(pag_cost == add_count + register_count + mux_count);

for s=1:no_of_stages
  if verbose > 0
    disp(['registers in stage ',num2str(s),':',num2str(register_count_per_stage(s))]);
  end
end
  
