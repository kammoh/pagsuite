% norm(vec) normalizes a vector such that the first non-zero of vec is
% positive and at least one odd element appears
% Used for CMM problems
function [vec,shift,sign] = norm_vec(vec)

shift=0;
sign=1;

if all(vec==0)
  return;
end

while all(mod(vec,2)==0)
  vec = vec/2;
  shift = shift + 1;
end

vec_nz = vec(vec ~= 0);
if vec_nz(1) < 0
  vec = -vec;
  sign = -1;
end