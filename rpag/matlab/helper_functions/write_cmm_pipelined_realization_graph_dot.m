% writes a '.dot' file for a pipelined adder graph of a pipelined_realization
function write_cmm_pipelined_realization_graph_dot(filename, pipelined_realization, varargin)

% open dot-file and write header:
fid = fopen(filename, 'wt');
fprintf(fid, 'digraph G {\n');

for k=1:2:size(varargin,2)
  switch(varargin{k})
    case 'keeporder'
      if varargin{k+1} == true
        fprintf(fid, 'ordering=out;\n');
      end
    case 'ranksep'
      if varargin{k+1} > 0
        fprintf(fid, 'ranksep=%f;\n',varargin{k+1});
      end
  end
end

no_of_inputs = length(pipelined_realization{1}{1});
s_last=-1; %remembers the last pipeline stage

%write root node '1'
for i=1:no_of_inputs
  input_vec = zeros(1,no_of_inputs);
  input_vec(i) = 1;
  fprintf(fid, ['x_',vec2dot(input_vec,'_'),'_s0 [label="(',vec2dot(input_vec,','),')",shape="ellipse"];\n']);
end

%write nodes
for l=1:size(pipelined_realization,2)
  pipelined_realization_element = pipelined_realization{l};
  
    if (pipelined_realization_element{2} ~= s_last) || (pipelined_realization_element{2} == pipelined_realization_element{4})
      if s_last ~= -1
        fprintf(fid, '}\n');
      end
      fprintf(fid, '{ rank=same\n');
      s_last = pipelined_realization_element{2};
    end

    %check end node without add or sub (only shift)
    if pipelined_realization_element{6} == 0
      sign_str='';
    else
      vec = pipelined_realization_element{3};
      vec1_nonzero = vec(vec~=0);
      vec = pipelined_realization_element{6};
      if length(vec) > 0
        vec2_nonzero = vec(vec~=0);
        if (sign(vec1_nonzero(1))*sign(vec2_nonzero(1))) < 0
          sign_str='-';
        else
          sign_str='+';
        end
      else
        sign_str='';
      end
    end
    if pipelined_realization_element{2} == pipelined_realization_element{4}
      %no delay -> this is a node without register (output)
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{1},'_'),' [label="(',vec2dot(pipelined_realization_element{1},','),')",shape="none",height=0.0];\n']);
    else
      %this is a node without register:
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{1}','_'),'_s',num2str(pipelined_realization_element{2}),' [label="',sign_str,'\\n(',vec2dot(pipelined_realization_element{1}',','),')",shape="box"];\n']);
    end
end
fprintf(fid, '}\n');

%write edges
for l=1:size(pipelined_realization,2)
  pipelined_realization_element = pipelined_realization{l};
  
  %check end node without add or sub (only shift)
  node_is_register = false;
  if ~isempty(pipelined_realization_element{6})
    if(pipelined_realization_element{6} == 0)
      node_is_register = true;
    end
  else
    node_is_register = true;
  end
  if node_is_register
    if pipelined_realization_element{2} == pipelined_realization_element{4}
      %no delay -> this is a node without register (output)
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),' [label="(',vec2dot(pipelined_realization_element{5},','),')",fontsize=12]\n']);
    else
      %this is a register:
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',mat2str(pipelined_realization_element{5}),'",fontsize=12]\n']);
    end
  else
    %two input adders:
    fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',num2str(pipelined_realization_element{5}),' ",fontsize=12]\n']);
    fprintf(fid, ['x_',vec2dot(pipelined_realization_element{6},'_'),'_s',num2str(pipelined_realization_element{7}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',num2str(pipelined_realization_element{8}),' ",fontsize=12]\n']);
    
    
%     %check no of adder inputs:
%     if size(pipelined_realization,2) == 8
%       %two input adders:
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,3))),'_s',num2str(pipelined_realization(l,4)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,5)),' ",fontsize=12]\n']);
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,6))),'_s',num2str(pipelined_realization(l,7)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,8)),' ",fontsize=12]\n']);
%     elseif nnz(pipelined_realization(l,9:11)) == 0
%       %two input adders:
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,3))),'_s',num2str(pipelined_realization(l,4)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,5)),' ",fontsize=12]\n']);
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,6))),'_s',num2str(pipelined_realization(l,7)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,8)),' ",fontsize=12]\n']);
%     else
%       %three input adders:
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,3))),'_s',num2str(pipelined_realization(l,4)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,5)),' ",fontsize=12]\n']);
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,6))),'_s',num2str(pipelined_realization(l,7)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,8)),' ",fontsize=12]\n']);
%       fprintf(fid, ['x_',num2str(abs(pipelined_realization(l,9))),'_s',num2str(pipelined_realization(l,10)),' -> x_',num2str(pipelined_realization(l,1)),'_s',num2str(pipelined_realization(l,2)),' [label="',num2str(pipelined_realization(l,11)),' ",fontsize=12]\n']);
%     end
    
    
  end
end

fprintf(fid, '}\n');
fclose(fid);

end

function str = vec2dot(vec,delim)

  str='';
  vec_nonzero = vec(vec~=0);
  if length(vec_nonzero) > 0
    if vec_nonzero(1) < 0
      vec = vec*-1;
    end
    for i=1:length(vec)
      sgn_str='';
      if vec(i) < 0
        if (delim == '_')
          sgn_str = 'm';
        else
          sgn_str = '-';
        end
      end
      if i==length(vec)
        delim='';
      end
      str = [str,sgn_str,num2str(abs(vec(i))),delim];
    end
  else
    str = '0';
  end
end

