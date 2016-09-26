function realization = realization_add_output_coeff(output_coeff, realization)

output_coeff = unique(output_coeff);
realization_size = size(realization);

for i=1:length(output_coeff)
  [coeff_fun,coeff_exp] = fundamental(abs(output_coeff(i)));
  if coeff_fun == 1
    realization = [realization; output_coeff(i),coeff_fun,coeff_exp,0,0];
  else
    for j=1:realization_size(1)
      if coeff_fun == fundamental(realization(j,1))
         realization = [realization; output_coeff(i),realization(j,1),log2(abs(output_coeff(i))/realization(j,1)),0,0];
         break;
      end
    end
  end
end