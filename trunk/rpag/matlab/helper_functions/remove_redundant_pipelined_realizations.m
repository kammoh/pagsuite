function pipelined_realization = remove_redundant_pipelined_realizations(pipelined_realization)

global verbose;

S=max(pipelined_realization(:,2));

for s=1:S-1
  elements_of_stage_s = pipelined_realization(pipelined_realization(:,2)==s,1)';
  if size(pipelined_realization,2) == 8
    predecessors_of_stage_s_plus_one = abs([pipelined_realization(pipelined_realization(:,2)==s+1,3)',pipelined_realization(pipelined_realization(:,2)==s+1,6)']);
  elseif size(pipelined_realization,2) == 11
    predecessors_of_stage_s_plus_one = abs([pipelined_realization(pipelined_realization(:,2)==s+1,3)',pipelined_realization(pipelined_realization(:,2)==s+1,6)',pipelined_realization(pipelined_realization(:,2)==s+1,9)']);
  else
    error('invalid pipelined realization')
  end
  redundant_elements = setdiff(elements_of_stage_s,predecessors_of_stage_s_plus_one);
  if ~isempty(redundant_elements)
    if verbose > 0
      warning(['removing redundant element(s) ',num2str(redundant_elements),' in stage ',num2str(s)]);
      for i=1:length(redundant_elements)
        pipelined_realization = pipelined_realization(not(pipelined_realization(:,1)==redundant_elements(i) & pipelined_realization(:,2)==s),:);
      end
    end
  end
end