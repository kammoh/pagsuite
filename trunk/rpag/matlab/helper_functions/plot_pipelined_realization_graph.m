function plot_pipelined_realization_graph(pipelined_realization, varargin)

%config:
if strcmp(computer,'MACI') || strcmp(computer,'MACI64')
  pdf_path = '/Users/pluto/research/projects/fir_mcm_opt/matlab/graph_plots';
  delete_cmd = 'rm';
elseif strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
  pdf_path = 'temporary path for pdfs';
  delete_cmd = 'del';
elseif strcmp(computer,'GLNXA') || strcmp(computer,'GLNXA64')
  pdf_path = '/home/hardieck/Work/tmp';
  delete_cmd = 'rm';
end

file_name = 0;
keeporder = false;
ranksep = -1;
shortnames=false;
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
    case 'shortnames'
      if varargin{k+1} == true
        shortnames=true;
      end
  end
end

if file_name == 0
  file_name = 'realization';

  if ~iscell(pipelined_realization)
    coeff_values = unique(pipelined_realization(:,1)');
    for i=1:length(coeff_values)
      file_name = [file_name, '_', num2str(coeff_values(i))];
    end
    %limit file name length to 100:
    file_name = file_name(1:min(length(file_name),100));
  end
end

%validate_cmm_realization(pipelined_realization);

if ~isempty(pipelined_realization)
  if iscell(pipelined_realization)
    if(isstr(pipelined_realization{1}{1}))
      %cell array is the (most modern) cell format where the first element
      %describes the operation
      %lets convert it to the 'old' format:
      pipelined_realization_old = cell(1,length(pipelined_realization));
      for i=1:length(pipelined_realization)
        switch pipelined_realization{i}{1}
          case 'A'
            if length(pipelined_realization{i}) == 12
              %ternary adder:
              pipelined_realization_old{i} = {pipelined_realization{i}{2},pipelined_realization{i}{3},pipelined_realization{i}{4},pipelined_realization{i}{5},pipelined_realization{i}{6},pipelined_realization{i}{7},pipelined_realization{i}{8},pipelined_realization{i}{9},pipelined_realization{i}{10},pipelined_realization{i}{11},pipelined_realization{i}{12}};
            else
              %2-input adder:
              pipelined_realization_old{i} = {pipelined_realization{i}{2},pipelined_realization{i}{3},pipelined_realization{i}{4},pipelined_realization{i}{5},pipelined_realization{i}{6},pipelined_realization{i}{7},pipelined_realization{i}{8},pipelined_realization{i}{9}};
            end
          case 'R'
            pipelined_realization_old{i} = {pipelined_realization{i}{2},pipelined_realization{i}{3},pipelined_realization{i}{4},pipelined_realization{i}{5},0,0,0,0};
          case 'O'
            pipelined_realization_old{i} = {pipelined_realization{i}{2},pipelined_realization{i}{3},pipelined_realization{i}{4},pipelined_realization{i}{5},pipelined_realization{i}{6},0,0,0};
        end
      end
      pipelined_realization = pipelined_realization_old;
    end
  end


  [pdf_path,'/',file_name,'.dot'];
  if ~iscell(pipelined_realization)
    %matrices are used to describe PMCM solutions with 2-input adders or ternary adders:
    write_pipelined_realization_graph_dot([pdf_path,'/',file_name,'.dot'], pipelined_realization, 'keeporder', keeporder,'ranksep',ranksep);
  else
    %cells are used to describe PCMM solutions (with 2-input adders):
    write_cmm_pipelined_realization_graph_dot([pdf_path,'/',file_name,'.dot'], pipelined_realization, 'keeporder', keeporder,'ranksep',ranksep,'shortnames',shortnames);
  end
  dot2pdf([pdf_path,'/',file_name]);
  %system([delete_cmd,' ',pdf_path,'/',file_name,'.dot']);
  showpdf([pdf_path,'/',file_name,'.pdf'])
end
