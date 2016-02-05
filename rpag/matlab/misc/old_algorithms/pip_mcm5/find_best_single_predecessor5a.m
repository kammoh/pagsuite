function [x_best,gain_max] = find_best_single_predecessor5a(C,P,As,c_max,cost_add,cost_reg) 

global verbose;

if verbose >= 2
  disp('searching for best single predecessor 5a...');
end

gain_max = 0;
x_best=-1;

for i=1:length(As)
  x=As(i);

  R = intersect(succ_set([x,P],c_max),C); % R: realized elements of C
  hits = length(R);
  if(hits > 0)
    gain=0;
    for j=1:length(R);
      if(R(j) == x)
        gain = gain + 1/cost_reg(R(j));
      else
        gain = gain + 1/cost_add(R(j));
      end
    end
    if(gain > 0)
      if verbose >= 4
        disp(['single predecessor ',num2str(x),' with ',num2str(hits),' target hits and gain=',num2str(gain),' for R=',num2str(R)]);
      end
    end
    if(gain > gain_max)
      if verbose >= 3
        disp(['better single predecessor ',num2str(x),' with ',num2str(hits),' target hits and gain=',num2str(gain),' for R=',num2str(R)]);
      end
      gain_max = gain;
      x_best = x;
    end
  end
end
if verbose >= 2
  disp(['best single predecessor ',num2str(x_best),' with gain=',num2str(gain_max)]);
end