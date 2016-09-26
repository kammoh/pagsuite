% adder_depth_cached(x) computes the adder depth of coefficient x using a
% LUT

function ad = adder_depth_cached(x)

global numberdata;

if x <= 1048576
  C=double(numberdata(x,2));
else
  C=nnz(dec2csd(fundamental(abs(x))));
  %C=csdcost(abs(x))+1;
end
ad = ceil(log2(C));
