function pipelined_realization = remove_redundant_pipelined_realizations(pipelined_realization)

global verbose;

pipelined_realization_out={};
k=1;
for i=1:length(pipelined_realization)
  elem1 = pipelined_realization{i};
  elem1_found = false;
  for j=i+1:length(pipelined_realization)
    elem2 = pipelined_realization{j};
    if (elem1{1} == elem2{1}) && all(elem1{2} == elem2{2}) && (elem1{3} == elem2{3})
      if verbose > 0
        disp(['removing one of redundant element(s) ',cell2str(elem1),' and ',cell2str(elem2)]);
      end
      elem1_found = true;
      break;
    end
  end
  if ~elem1_found
    pipelined_realization_out{k} = elem1;
    k=k+1;
  end
end
pipelined_realization = pipelined_realization_out;
