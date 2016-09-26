function str = cell2str(c)

str='';

if iscell(c)
  str = [str,'{'];
  for i=1:length(c)-1
    str = [str,cell2str(c{i}),','];
  end
  str = [str,cell2str(c{length(c)}),'}'];
else %if ismatrix(c)
  str = mat2str(c);
end