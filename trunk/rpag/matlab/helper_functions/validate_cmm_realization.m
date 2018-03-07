function analyze_cmm_solution(pipelined_realization)

if ~iscell(pipelined_realization)
  error('Input argument ''pipelined_realization'' is not a cell array');
end

no_of_inputs = length(pipelined_realization{1}{1});

for l=1:size(pipelined_realization,2)
  pipelined_realization_element = pipelined_realization{l};
  
  if ~all(pipelined_realization_element{1} == pipelined_realization_element{3}*2^pipelined_realization_element{5}+pipelined_realization_element{6}*2^pipelined_realization_element{8})
    error(['Element ',mat2str(pipelined_realization_element{1}),' can not be computed from ',mat2str(pipelined_realization_element{3}),'*2^',num2str(pipelined_realization_element{5}),' + ',mat2str(pipelined_realization_element{6}),'*2^',num2str(pipelined_realization_element{8})]);
  end
end