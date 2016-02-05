function pipelined_realization = pipelined_realization_add_output_coeff(output_coeff, pipelined_realization)

output_coeff = unique(output_coeff);

pipeline_stages = max(pipelined_realization(:,2));

for i=1:length(output_coeff)
  [coeff_fun,coeff_exp] = fundamental(abs(output_coeff(i)));
  
  pipelined_realization = [pipelined_realization; output_coeff(i), pipeline_stages, coeff_fun,pipeline_stages,coeff_exp,0,0,0];
end