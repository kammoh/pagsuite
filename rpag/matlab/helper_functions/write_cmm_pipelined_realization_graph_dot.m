% writes a '.dot' file for a pipelined adder graph of a pipelined_realization
function write_cmm_pipelined_realization_graph_dot(filename, pipelined_realization, varargin)

% open dot-file and write header:
fid = fopen(filename, 'wt');
fprintf(fid, 'digraph G {\n');
shortnames=false;

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
    case 'shortnames'
      if varargin{k+1} == true
        shortnames=true;
      end
  end
end

no_of_inputs = length(pipelined_realization{1}{1});
s_last=-1; %remembers the last pipeline stage

%write root node(s) '1'
for i=1:no_of_inputs
  input_vec = zeros(1,no_of_inputs);
  input_vec(i) = 1;
  if shortnames
    label = num2str(i);
  else
    label = vec2dotlab(input_vec,',');
  end
  fprintf(fid, ['x_',vec2dot(input_vec,'_'),'_s0 [label="',label,'",shape="ellipse"];\n']);
end

nodecnt = no_of_inputs+1; %for short labels
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
      if ~isempty(vec)
        vec2_nonzero = vec(vec~=0);
        if ~isempty(vec2_nonzero) && ~isempty(vec1_nonzero)
          if (sign(vec1_nonzero(1))*sign(vec2_nonzero(1))) < 0
            sign_str='-';
          else
            sign_str='+';
          end
        else
          sign_str='';
        end
      else
        sign_str='';
      end
    end
    if pipelined_realization_element{2} ==  pipelined_realization_element{4} %no delay -> this is a node without register (output)
%    if pipelined_realization_element{6} == 0 %no second argument, this is an output
      
      if shortnames
        label = num2str(nodecnt);
        nodecnt = nodecnt + 1;
      else
        label = vec2dotlab(pipelined_realization_element{1},',');
      end
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{1},'_'),' [label="',label,'",shape="none",height=0.0];\n']);
    else
      %this is a node without register:
      if shortnames
        label = num2str(nodecnt);
        nodecnt = nodecnt + 1;
      else
        label = vec2dotlab(pipelined_realization_element{1},',');
      end
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{1}','_'),'_s',num2str(pipelined_realization_element{2}),' [label="',sign_str,'\\n',label,'",shape="box"];\n']);
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
      label = vec2dotlab(pipelined_realization_element{5},',');
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),' [label="',label,'",fontsize=12]\n']);
    else
      %this is a register:
      label = mat2str(pipelined_realization_element{5});
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',label,'",fontsize=12]\n']);
    end
  else
    %two input adders:
    label = num2str(pipelined_realization_element{5});
    fprintf(fid, ['x_',vec2dot(pipelined_realization_element{3},'_'),'_s',num2str(pipelined_realization_element{4}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',label,' ",fontsize=12]\n']);
    label = num2str(pipelined_realization_element{8});
    fprintf(fid, ['x_',vec2dot(pipelined_realization_element{6},'_'),'_s',num2str(pipelined_realization_element{7}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',label,' ",fontsize=12]\n']);
    %check no of adder inputs:
    if size(pipelined_realization_element,2) == 11
      %three input adder:
      label = num2str(pipelined_realization_element{11});
      fprintf(fid, ['x_',vec2dot(pipelined_realization_element{9},'_'),'_s',num2str(pipelined_realization_element{10}),' -> x_',vec2dot(pipelined_realization_element{1},'_'),'_s',num2str(pipelined_realization_element{2}),' [label="',label,' ",fontsize=12]\n']);
    end
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

function str = vec2dotlab(vec,delim)
  if length(vec) > 1
    str = ['(',vec2dot(vec,delim),')'];
  else
    str = vec2dot(vec,delim);
  end
end