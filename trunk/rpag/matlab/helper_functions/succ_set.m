% [successor_set] = succ_set(r_set,c_max,exclude_r_set,exclude_1) returns 
% the set of realizable coefficients with adder distance 1, related
% to elements of r_set within the limit 'c_max'
% e.g. all elements of successor_set can be realized by adding or subtracting
% shifted values of elements of r_set
%
% if the optional flag 'exclude_r_set' is false, r_set is not excluded from
% successor_set (default: exclude_r_set=false)
% if the optional flag 'exclude_1' is false, the value '1' is not excluded from
% successor_set (default: exclude_1=false)
%
% note:
% the result of succ_set([a b],c_max) is *different* to succ(a,b,c_max) as
% succ_set([a b],c_max) computes the union of sets 
% succ(a,a,c_max), succ(a,b,c_max) and succ(b,b,c_max)

function [successor_set] = succ_set(r_set,c_max,exclude_r_set,exclude_1)

k_max_max=ceil(log((c_max-min(r_set))/min(r_set))/log(2));
successor_set = zeros(1,4*(k_max_max+1));
set_i=1;

if(~exist('exclude_r_set'))
  exclude_r_set=false;
end
if(~exist('exclude_1'))
  exclude_1=false;
end


for i=1:length(r_set)
  for j=i:length(r_set)
    r1=r_set(i);
    r2=r_set(j);

    % r1 must be greater than r2, if not swap them
    if r1<r2
      tmp=r1;
      r1=r2;
      r2=tmp;
    end

    k_max=ceil(log2((max(0,c_max-r1))/r2));

    %k=0:
    successor_set(set_i) = fundamental(r1 + r2); set_i = set_i+1;
    successor_set(set_i) = fundamental(r1 - r2); set_i = set_i+1;

    for k=1:k_max
      successor_set(set_i) = r1 * 2^k + r2; set_i = set_i+1;
      successor_set(set_i) = r1 + r2 * 2^k; set_i = set_i+1;
      successor_set(set_i) = r1 * 2^k - r2; set_i = set_i+1;
      successor_set(set_i) = abs(r1 - r2 * 2^k); set_i = set_i+1;
    end
  end
end

if exclude_r_set
  if exclude_1
    successor_set = setdiff(unique(successor_set),[0,1,r_set]);
  else
    successor_set = setdiff(unique(successor_set),[0,r_set]);
  end
else
  if exclude_1
    successor_set = setdiff(unique(successor_set),[0,1]);
  else
    successor_set = setdiff(unique(successor_set),[0]);
  end
end

c_max_end = sum(successor_set <= c_max);
successor_set = successor_set(1:c_max_end);

