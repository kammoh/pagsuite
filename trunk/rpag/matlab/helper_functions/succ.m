% [successor_set] = succ(r1,r2,c_max) returns the set of realizable coefficients 
% with adder distance 1, related to r1 and r2 which is within the limit 'c_max'
% e.g. all elements of successor_set can be realized by adding or subtracting
% shifted values of r1 and r2
%
% if the optional flag 'exclude_r_set' is false, r_set is not excluded from
% successor_set (default: exclude_r_set=true)
% if the optional flag 'exclude_1' is false, the value '1' is not excluded from
% successor_set (default: exclude_1=true)

function [successor_set] = succ(r1,r2,c_max,exclude_r1_r2,exclude_1)

if(~exist('exclude_r1_r2'))
  exclude_r1_r2=false;
end
if(~exist('exclude_1'))
  exclude_1=false;
end

% r1 must be greater than r2, if not swap them
if r1<r2
    tmp=r1;
    r1=r2;
    r2=tmp;
end
%assert(max(r1,r2)<c_max, ['c_max(=',num2str(c_max),') must be greater than the maximum of r1(=',num2str(r1),') and r2(=',num2str(r2),')'])
if max(r1,r2)>c_max
  warning(['c_max(=',num2str(c_max),') must be greater than the maximum of r1(=',num2str(r1),') and r2(=',num2str(r2),')'])
  successor_set = [];
  return;
end

k_max=ceil(log((c_max-r1)/r2)/log(2));

successor_set = zeros(1,4*(k_max+1));

%k=0:
successor_set(1) = fundamental(r1 + r2); 
successor_set(2) = fundamental(r1 - r2); 

for k=1:k_max
    successor_set(4*(k-1)+3) = r1 * 2^k + r2;
    successor_set(4*(k-1)+4) = r1 + r2 * 2^k;
    successor_set(4*(k-1)+5) = r1 * 2^k - r2;
    successor_set(4*(k-1)+6) = abs(r1 - r2 * 2^k);
end

if exclude_r1_r2
  if exclude_1
    successor_set = setdiff(unique(successor_set),[0,1,r1,r2]);
  else
    successor_set = setdiff(unique(successor_set),[0,r1,r2]);
  end
else
  if exclude_1
    successor_set = setdiff(unique([successor_set,r1,r2]),[0,1]);
  else
    successor_set = setdiff(unique([successor_set,r1,r2]),0);
  end
end

c_max_end = sum(successor_set <= c_max);
successor_set = successor_set(1:c_max_end);

