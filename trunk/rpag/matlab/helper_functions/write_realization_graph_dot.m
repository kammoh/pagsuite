function write_realization_graph_dot(filename, realization)

% open dot-file and write header:
fid = fopen(filename, 'wt');
fprintf(fid, 'digraph G {\n');

%write root node '1'
%fprintf(fid, ['1 [label="1",shape="box"];\n']);
fprintf(fid, ['1 [label="1",shape="ellipse"];\n']);

%write nodes
realization_size = size(realization);
for l=1:realization_size(1)
    if (sign(realization(l,2))*sign(realization(l,4))) < 0
      sign_str='-';
    else
      sign_str='+';
    end
    %check end node without add or sub (only shift)
    if realization(l,4) == 0
      sign_str='';
    end
    if realization(l,4) == 0
      %this is a simple shift without add/sub (output)
      if sign(realization(l,1)) == -1
        sgn = 'm';
      else
        sgn = '';
      end
      fprintf(fid, [['sh',sgn,num2str(abs(realization(l,1)))],' [label="',num2str(realization(l,1)),'",shape="none",height=0.0];\n']);
    else
      %this is a add/sub node:
%      fprintf(fid, [num2str(realization(l,1)),' [label="',sign_str,'\\n',num2str(realization(l,1)),'",shape="box"];\n']);
      fprintf(fid, [num2str(realization(l,1)),' [label="',sign_str,'\\n',num2str(realization(l,1)),'",shape="ellipse"];\n']);
    end
end

for l=1:realization_size(1)
  %check end node without add or sub (only shift)
  if realization(l,4) == 0
%    fprintf(fid, '%d -> %d [label="%d"]\n', abs(realization(l,2)), realization(l,1), realization(l,3));
      if sign(realization(l,1)) == -1
        sgn = 'm';
      else
        sgn = '';
      end
      fprintf(fid, ['%d -> sh',sgn,'%d [label="%d ",fontsize=12]\n'], abs(realization(l,2)), abs(realization(l,1)), realization(l,3));
  else
    fprintf(fid, '%d -> %d [label="%d ",fontsize=12]\n', abs(realization(l,2)), realization(l,1), realization(l,3));
    fprintf(fid, '%d -> %d [label="%d ",fontsize=12]\n', abs(realization(l,4)), realization(l,1), realization(l,5));
  end
end

fprintf(fid, '}\n');
fclose(fid);
