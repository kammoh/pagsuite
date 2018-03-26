% adder_depth(x) computes the adder depth of coefficient matrix M (for CMM)
function ad = adder_depth_mat(M)

ad=0;
for r=1:size(M,1)
  ad = max(ad,adder_depth_vec(M(r,:)));
end