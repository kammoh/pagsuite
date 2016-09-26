function fact = fact_components(coeff)

coeff_fun = setdiff(unique(fundamental(coeff)),1);

fact=[];
for i=1:length(coeff_fun)
  fact = [fact,factor(coeff_fun(i))];
end

fact = unique(fact);