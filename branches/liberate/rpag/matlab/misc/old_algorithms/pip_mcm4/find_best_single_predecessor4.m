function [x_best,cost_min] = find_best_single_predecessor4(C,P,As,c_max,cost_add,cost_reg) 

%if isempty(P) 
%  x_best  = 0;
%  return;
%end

global verbose;

if verbose >= 2
  disp('searching for best single predecessor...');
end

cost_min = Inf;
x_best=-1;

for i=1:length(As)
  x=As(i);

  hits = length(intersect(succ_set([x,P],c_max),C));
  if(hits > 0)
    if(is_member_of(C,x))
      cost = cost_reg(x)/hits;
    else
      cost = cost_add(x)/hits;
    end
    if(cost < cost_min)
      if verbose >= 3
        disp(['better single predecessor ',num2str(x),' with ',num2str(hits),' target hits and cost=',num2str(cost),' for x=',num2str(x)]);
      end
      cost_min = cost;
      x_best = x;
    end
  end
end
if verbose >= 2
  disp(['best single predecessor ',num2str(x_best),' with cost=',num2str(cost_min)]);
end