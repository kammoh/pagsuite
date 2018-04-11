function pipelined_realization = remove_redundant_pipelined_realizations(pipelined_realization)

global verbose;

%cell2str(pipelined_realization)

%remove elements which are computed more than once:
pipelined_realization_out=cell(1,length(pipelined_realization));
k=1;
for i=1:length(pipelined_realization)
  elem1 = pipelined_realization{i};
  elem1_found = false;
  for j=i+1:length(pipelined_realization)
    elem2 = pipelined_realization{j};
    if (elem1{1}~='O') && (elem2{1}~='O') && all(elem1{2} == elem2{2}) && (elem1{3} == elem2{3})
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
pipelined_realization = pipelined_realization_out(~cellfun('isempty',pipelined_realization_out));

elements_removed = true;
while(elements_removed) %in case that a redundant element is removed, it may be the case that another element becomes redundant, so call again
  elements_removed = false;
  %remove elements which are not required:
  pipelined_realization_out={};
  k=1;
  for i=1:length(pipelined_realization)
    elem1 = pipelined_realization{i};
    if elem1{1} ~= 'O'
      elem1_found = false;
      for j=1:length(pipelined_realization)
        elem2 = pipelined_realization{j};
        if (elem2{1} == 'R') || elem2{1} == 'O'
          if all(norm_vec(elem1{2}) == norm_vec(elem2{4})) && (elem1{3} == elem2{5})
            elem1_found = true; %element is used from a following register or output
            break;
          end
        elseif elem2{1} == 'A'
          if all(norm_vec(elem1{2}) == norm_vec(elem2{4})) && (elem1{3} == elem2{5})
            elem1_found = true; %element is used at the 1st input of a following adder
            break;
          end
          if all(norm_vec(elem1{2}) == norm_vec(elem2{7})) && (elem1{3} == elem2{8})
            elem1_found = true; %element is used at the 2nd input of a following adder
            break;
          end
          if length(elem2) == 12
            %ternary adder
            if all(norm_vec(elem1{2}) == norm_vec(elem2{10})) && (elem1{3} == elem2{11})
              elem1_found = true; %element is used at the 2nd input of a following adder
              break;
            end
          end
        end
      end
    else
      elem1_found = true;
    end
    if elem1_found
      pipelined_realization_out{k} = elem1;
      k=k+1;
    else
      elements_removed = true;
      if verbose > 0
        disp(['removing redundant element ',cell2str(elem1)]);
      end    
    end
  end
  pipelined_realization = pipelined_realization_out(~cellfun('isempty',pipelined_realization_out));
end

