% adder_depth(x) computes the adder depth of coefficient vector x (for CMM)
function ad = adder_depth_vec(x)

nnz_tot=0;
for i=1:length(x)
  nnz_tot = nnz_tot + nnz(dec2csd(fundamental(abs(x(i)))));
end

ad = ceil(log2(nnz_tot));
