function str = cell2str(c)

str='';

if iscell(c)
  str = [str,'{'];
  if length(c) > 0
    for i=1:length(c)-1
      str = [str,cell2str(c{i}),','];
    end
    str = [str,cell2str(c{length(c)})];
  end
  str = [str,'}'];
else %if ismatrix(c)
  str = mat2str(c);
end