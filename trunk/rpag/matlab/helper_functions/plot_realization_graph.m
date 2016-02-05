% plots the adder graph realization from the realized coefficients
% (inclunding intermediate/non-output fundamentals)
% or from the realization matrix
function plot_realization_graph(coeffs_realization, varargin)

%config:
if strcmp(computer,'MACI') || strcmp(computer,'MACI64')
  pdf_path = '/Users/pluto/research/projects/fir_mcm_opt/matlab/graph_plots';
  delete_cmd = 'rm';
elseif strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
  pdf_path = 'temporary path for pdfs';
  delete_cmd = 'del';
end

coeffs_realization_size=size(coeffs_realization);

if coeffs_realization_size(1) == 1
  % coeffs_realization is a vector, interpret as coefficient values and
  % compute realization:
  coeffs = coeffs_realization;
  c_max = 2^(ceil(log2(max(fundamental(coeffs))))+1);
  realization = compute_realization(coeffs,c_max);
else
  % coeffs_realization is a matrix, interpret as realization matrix:
  coeffs = coeffs_realization(:,1)';
  c_max = 2^(ceil(log2(max(fundamental(coeffs))))+1);
  realization = coeffs_realization;
end



file_name = 0;
for k=1:2:size(varargin,2)
    switch(varargin{k})
        case 'name'
            file_name = varargin{k+1};
    end
end
if file_name == 0
  file_name = 'realization';

  realization_size = size(realization);
  for i=1:realization_size(1)
    file_name = [file_name, '_', num2str(realization(i,1))];
  end
  %limit file name length to 100:
  file_name = file_name(1:min(length(file_name),100));
end

[pdf_path,'/',file_name,'.dot'];
write_realization_graph_dot([pdf_path,'/',file_name,'.dot'], realization);
dot2pdf([pdf_path,'/',file_name]);
system([delete_cmd,' ',pdf_path,'/',file_name,'.dot']);
showpdf([pdf_path,'/',file_name,'.pdf'])
