function [x_best] = find_best_single_predecessor(C,P,As,c_max) 

%if isempty(P) 
%  x_best  = 0;
%  return;
%end

global verbose;

if verbose >= 2
  disp('searching for best single predecessor...');
end

x_hits_max_best = 0;
x_best=-1;
for i=1:length(As)
  x=As(i);

  xP = succ_set([x,P],c_max);
  xP=intersect(xP,C);
  x_hits_max = length(xP);
  if x_hits_max > x_hits_max_best
    x_hits_max_best = x_hits_max;
    x_best = x;
    if verbose >= 3
      disp(['better x found: x_hits=',num2str(x_hits_max),' for x=',num2str(x)]);
    end
  end
end
if verbose >= 2
  disp(['best single predecessor: x=',num2str(x_best),' with ',num2str(x_hits_max_best),' hit(s) in C']);
end