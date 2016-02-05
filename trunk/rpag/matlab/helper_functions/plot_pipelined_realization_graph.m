function plot_pipelined_realization_graph(pipelined_realization, varargin)

%config:
if strcmp(computer,'MACI') || strcmp(computer,'MACI64')
  pdf_path = '/Users/pluto/research/projects/fir_mcm_opt/matlab/graph_plots';
  delete_cmd = 'rm';
elseif strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
  pdf_path = 'temporary path for pdfs';
  delete_cmd = 'del';
elseif strcmp(computer,'GLNXA') || strcmp(computer,'GLNXA64')
  pdf_path = '/work5/hardieck/tmp';
  delete_cmd = 'rm';
end

file_name = 0;
keeporder = false;
ranksep = -1;
for k=1:2:size(varargin,2)
  switch(varargin{k})
    case 'name'
      file_name = varargin{k+1};
    case 'keeporder'
      if varargin{k+1} == true
        keeporder = true;
      end
    case 'ranksep'
      ranksep = varargin{k+1};
  end
end

if file_name == 0
  file_name = 'realization';

  coeff_values = [];
  for i=1:size(pipelined_realization,2)
    coeff_values = [coeff_values,pipelined_realization{i}(1)];
  end
  coeff_values = unique(cell2mat(coeff_values));
  for i=1:length(coeff_values)
    file_name = [file_name, '_', num2str(coeff_values(i))];
  end
  %limit file name length to 100:
  file_name = file_name(1:min(length(file_name),100));
end

[pdf_path,'/',file_name,'.dot'];
write_pipelined_realization_graph_dot([pdf_path,'/',file_name,'.dot'], pipelined_realization, 'keeporder', keeporder,'ranksep',ranksep);
dot2pdf([pdf_path,'/',file_name]);
system([delete_cmd,' ',pdf_path,'/',file_name,'.dot']);
showpdf([pdf_path,'/',file_name,'.pdf'])
