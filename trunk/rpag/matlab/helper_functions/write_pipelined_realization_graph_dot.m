% writes a '.dot' file for a pipelined adder graph of a pipelined_realization
function write_pipelined_realization_graph_dot(filename, pipelined_realization, varargin)
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

  if length(pipelined_realization)==0
    return
  end
  %write root (input) node(s):
  first_factor=pipelined_realization{1}(1);
  vec_size = size(first_factor{1},2);
  for k=1:vec_size
    input_vec = zeros(1,vec_size);
    input_vec(k) = 1;
    fprintf(fid, ['x_',node2str(input_vec),'_s0 [label="',mat2str(input_vec),'",shape="ellipse"];\n']);
  end
  s_last=-1; %remembers the last pipeline stage
  %write nodes
  for l=1:size(pipelined_realization,2)
      pipelined_realization_node = pipelined_realization{l};
      if (pipelined_realization_node{2} ~= s_last) || (pipelined_realization_node{2} == pipelined_realization_node{4})
        if s_last ~= -1
          fprintf(fid, '}\n');
        end
        fprintf(fid, '{ rank=same\n');
        s_last = pipelined_realization_node{2};
      end
      
      no_of_positive_args=(sign(pipelined_realization_node{3}(1))+1)/2+(sign(pipelined_realization_node{6}(1))+1)/2;
      if size(pipelined_realization_node,2) == 11
        no_of_positive_args=no_of_positive_args+(sign(pipelined_realization_node{9}(1))+1)/2;
      end
      
      if (no_of_positive_args >= 2)
        sign_str='+';
      else
        sign_str='-';
      end
      if size(pipelined_realization_node,2) == 11
        if (no_of_positive_args <= 2)
          sign_str=[sign_str,' -'];
        else
          sign_str=[sign_str,' +'];
        end
      end
      %check end node without add or sub (only shift)
      if pipelined_realization_node{6} == 0
        sign_str='';
      end
      if (pipelined_realization_node{2} == pipelined_realization_node{4}) && (pipelined_realization_node{2} ~= 0)
        %no delay -> this is a node without register (output)
        fprintf(fid, ['x_',node2str(pipelined_realization_node{1}),' [label="',mat2str(pipelined_realization_node{1}),'",shape="none",height=0.0];\n']);
      else
        %this is a node with register:
        fprintf(fid, ['x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',sign_str,'\\n',mat2str(pipelined_realization_node{1}),'",shape="box"];\n']);
      end
  end
  fprintf(fid, '}\n');

  %write edges
  for l=1:size(pipelined_realization,2)
    pipelined_realization_node = pipelined_realization{l};
    %check end node without add or sub (only shift)
    if pipelined_realization_node{6} == 0
      if pipelined_realization_node{2} == pipelined_realization_node{4}
        %no delay -> this is a node without register (output)
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{3})),'_s',mat2str(pipelined_realization_node{4}),' -> x_',node2str(pipelined_realization_node{1}),' [label="',mat2str(pipelined_realization_node{5}),' ",fontsize=12]\n']);
      else
        %this is a node with register:
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{3})),'_s',mat2str(pipelined_realization_node{4}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{5}),' ",fontsize=12]\n']);
      end
    else
      %check no of adder inputs:
      if size(pipelined_realization_node,2) == 8
        %two input adder:
        if(pipelined_realization_node{1} ~= pipelined_realization_node{3}*2^pipelined_realization_node{5} + pipelined_realization_node{6} * 2^pipelined_realization_node{8})
          error(['Graph inconsistent (',num2str(pipelined_realization_node{1}),' != ',num2str(pipelined_realization_node{3}),'*2^',num2str(pipelined_realization_node{5}),' + ',num2str(pipelined_realization_node{6}),' * 2^',num2str(pipelined_realization_node{8}),')']);
        end
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{3})),'_s',mat2str(pipelined_realization_node{4}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{5}),' ",fontsize=12]\n']);
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{6})),'_s',mat2str(pipelined_realization_node{7}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{8}),' ",fontsize=12]\n']);
      else
        %three input adder:
        if(pipelined_realization_node{1} ~= pipelined_realization_node{3}*2^pipelined_realization_node{5} + pipelined_realization_node{6} * 2^pipelined_realization_node{8} + pipelined_realization_node{9} * 2^pipelined_realization_node{11})
          error(['Graph inconsistent (',num2str(pipelined_realization_node{1}),' != ',num2str(pipelined_realization_node{3}),'*2^',num2str(pipelined_realization_node{5}),' + ',num2str(pipelined_realization_node{6}),' * 2^',num2str(pipelined_realization_node{8}),' + ',num2str(pipelined_realization_node{9}),' * 2^',num2str(pipelined_realization_node{11}),')']);
        end
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{3})),'_s',mat2str(pipelined_realization_node{4}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{5}),' ",fontsize=12]\n']);
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{6})),'_s',mat2str(pipelined_realization_node{7}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{8}),' ",fontsize=12]\n']);
        fprintf(fid, ['x_',node2str(norm_vec(pipelined_realization_node{9})),'_s',mat2str(pipelined_realization_node{10}),' -> x_',node2str(pipelined_realization_node{1}),'_s',mat2str(pipelined_realization_node{2}),' [label="',mat2str(pipelined_realization_node{11}),' ",fontsize=12]\n']);
      end
    end
  end

  fprintf(fid, '}\n');
  fclose(fid);
  
  function vec_normed = norm_vec(vec)
    for i=1:length(vec)
      if vec(i) ~= 0
        if vec(i) < 0
          vec_normed = -vec;
        else
          vec_normed = vec;
        end
        return;
      end
    end
  end
  
end