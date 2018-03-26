function [pipelined_adder_graph, pag_cost] = run_rpag_cmm(M,arguments)

rpag_path = '~/research/develop/pagsuite/trunk/rpag/bin';

global verbose;


if ~exist('arguments','var')
  arguments='';
end

colstr='';
for c=1:size(M,1)
  for r=1:size(M,2)
    if M(c,r) < 0
      sign_str = '-';
    else
      sign_str = '+';
    end
    colstr = [colstr,sign_str,num2str(abs(M(c,r)))];
  end
  colstr=[colstr,' '];
end
%colstr = strrep(colstr, ' ', '');
%disp(colstr);
id = round(rand()*1E9); %produce unique ID
filename = ['rpag_result_',num2str(id)];

rpag_cmd = [rpag_path,'/rpag --cmm --file_output=',filename,'.m ',arguments,' ',colstr];
if verbose > 0
  disp(['calling ',rpag_cmd]);
end
eval(['!',rpag_cmd]);
evalc(filename);
eval(['!rm ',filename,'.m']);

pipelined_adder_graph = pipelined_adder_graph;
pag_cost = pag_cost;
