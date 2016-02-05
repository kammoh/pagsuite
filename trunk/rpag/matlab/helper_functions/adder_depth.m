% adder_depth(x) computes the adder depth of coefficient x

function ad = adder_depth(x, cached)

if ~exist('cached')
  cached = false;
end
ad = zeros(1,length(x));
for i=1:length(x)
  if cached
    ad(i) = adder_depth_cached(x);
  else
    ad(i) = ceil(log2(nnz(dec2csd(fundamental(abs(x(i)))))));
    %ad(i) = ceil(log2(csdcost(abs(x(i)))+1));
  end
end