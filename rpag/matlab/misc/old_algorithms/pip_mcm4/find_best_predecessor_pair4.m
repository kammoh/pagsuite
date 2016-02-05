function [x_best,y_best] = find_best_predecessor_pair4(C,P,As,c_max,cost_add,cost_reg) 

global verbose;

if verbose >= 2
  disp('searching for best predecessor pair...');
end

% !!! P may be further used to improve results !!!

y_hits_max_best = -1;
for i=1:length(As)
  x=As(i);
  hit_map = containers.Map(0,0);
  for j=1:length(C)
    y=succ(x,C(j),c_max);
    y=intersect(y,As);
    for k=1:length(y)
      if hit_map.isKey(y(k))
        hit_map(y(k)) = hit_map(y(k))+1;
      else
        hit_map(y(k)) = 1;
      end
    end
  end
  

  [y_hits_max,y_hits_key] = max(cell2mat(hit_map.values));
  if verbose >= 5
    key_cell=hit_map.keys;
    disp(['evaluating pair: x=',num2str(x),' and y=',num2str(key_cell{y_hits_key}),' with ',num2str(y_hits_max),' hits']);
  end
  if y_hits_max > y_hits_max_best
    key_cell=hit_map.keys;
    y_hits_max_best = y_hits_max;
    x_best = x;
    y_best = key_cell{y_hits_key};
    if verbose >= 4
      disp(['better pair found: y_hits=',num2str(y_hits_max_best),' for x=',num2str(x_best),' and y=',num2str(y_best)]);
    end
  end
end
if verbose >= 2
  disp(['best single predecessor pair: x=',num2str(x_best),' and y=',num2str(y_best),' with ',num2str(y_hits_max_best),' hit(s) in C']);
end