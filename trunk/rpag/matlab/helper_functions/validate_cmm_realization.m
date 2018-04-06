function validate_cmm_realization(pipelined_realization,M)

if ~iscell(pipelined_realization)
  error('Input argument ''pipelined_realization'' is not a cell array');
end

if ~exist('M','var');
  M=[];
end

for l=1:length(pipelined_realization)
  pipelined_realization_element = pipelined_realization{l};
  
  if pipelined_realization_element{1}=='A'
    if length(pipelined_realization_element) == 12
      %ternary adder
      if ~all(pipelined_realization_element{2} == pipelined_realization_element{4}*2^pipelined_realization_element{6}+pipelined_realization_element{7}*2^pipelined_realization_element{9}+pipelined_realization_element{10}*2^pipelined_realization_element{12})
        error(['Element ',mat2str(pipelined_realization_element{2}),' can not be computed from ',mat2str(pipelined_realization_element{4}),'*2^',num2str(pipelined_realization_element{6}),' + ',mat2str(pipelined_realization_element{7}),'*2^',num2str(pipelined_realization_element{9}),' + ',mat2str(pipelined_realization_element{10}),'*2^',num2str(pipelined_realization_element{12})]);
      end
    else
      %2-input adder
      if ~all(pipelined_realization_element{2} == pipelined_realization_element{4}*2^pipelined_realization_element{6}+pipelined_realization_element{7}*2^pipelined_realization_element{9})
        error(['Element ',mat2str(pipelined_realization_element{2}),' can not be computed from ',mat2str(pipelined_realization_element{4}),'*2^',num2str(pipelined_realization_element{6}),' + ',mat2str(pipelined_realization_element{7}),'*2^',num2str(pipelined_realization_element{9})]);
      end
    end
    if pipelined_realization_element{5}>0 %input stage 0 is always present
      found_1st_arg = false;
      found_2nd_arg = false;
      for i=1:length(pipelined_realization)
        if all(norm_vec(pipelined_realization{i}{2}) == norm_vec(pipelined_realization_element{4})) && (pipelined_realization{i}{3} == pipelined_realization_element{5})
          found_1st_arg = true;
        end
        if all(norm_vec(pipelined_realization{i}{2}) == norm_vec(pipelined_realization_element{7})) && (pipelined_realization{i}{3} == pipelined_realization_element{8})
          found_2nd_arg = true;
        end
      end
      if ~found_1st_arg
        error(['Element ',mat2str(pipelined_realization_element{2}),' in stage ',num2str(pipelined_realization_element{3}),' depends on ',mat2str(pipelined_realization_element{4}),' in stage ',num2str(pipelined_realization_element{5}),' which is missing']);
      end
      if ~found_2nd_arg
        error(['Element ',mat2str(pipelined_realization_element{2}),' in stage ',num2str(pipelined_realization_element{3}),' depends on ',mat2str(pipelined_realization_element{7}),' in stage ',num2str(pipelined_realization_element{8}),' which is missing']);
      end
    end
  elseif pipelined_realization_element{1}=='R'
    if ~all(pipelined_realization_element{2} == pipelined_realization_element{4}) || (pipelined_realization_element{3}~=pipelined_realization_element{5}+1)
      error(['Register ',mat2str(pipelined_realization_element{2}),' in stage ',num2str(pipelined_realization_element{3}),' depends on ',mat2str(pipelined_realization_element{4}),' in stage ',num2str(pipelined_realization_element{5})]);
    end
  end
end


for r=1:size(M,1)
  output_found=false;
  row_vec = norm_vec(M(r,:));
  for l=1:length(pipelined_realization)
    pipelined_realization_element = pipelined_realization{l};
    if pipelined_realization_element{1}=='O'
      if norm_vec(pipelined_realization_element{2}) == row_vec
        output_found=true;
        break;
      end
    end
  end
  if ~output_found
    error(['Output ',mat2str(row_vec),' not found as output in adder graph solution']);
  end
end


