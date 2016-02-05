% find_best_predecessor_pair_from_msd searches for the predecessor pair
% (x,y) for which the most elements in C can be realized.
% Only x and y values that can be computed from the MSD representation 
% of coefficients in C are considered and limited to those with an adder
% depth < max_adder_depth
%
% All possible (x,y) pairs are found using find_msd_potentials()
function [x_best,y_best] = find_best_predecessor_pair_from_msd(C,max_adder_depth) 

global verbose;

if verbose >= 2
  disp('searching for best predecessor pair from MSD...');
end

all_pairs = find_msd_potentials(C,max_adder_depth);

all_pairs = sortrows(all_pairs);
all_pairs_size = size(all_pairs);
last_pair=[-1,-1];
i_best=0;
hit_cnt=0;
hit_cnt_max=0;
for i=1:all_pairs_size(1)
  act_pair = sort(all_pairs(i,:));
  if act_pair == last_pair
    hit_cnt = hit_cnt + 1;
    if hit_cnt > hit_cnt_max
      hit_cnt_max = hit_cnt;
      i_best = i;
      if verbose >= 3
        disp(['better pair found: (',num2str(all_pairs(i,:)), ') with ',num2str(hit_cnt+1),' hits']);
      end
    end
  else
    hit_cnt=0;
  end
  last_pair = act_pair;
end

if hit_cnt_max > 0
  x_best = all_pairs(i_best,1);
  y_best = all_pairs(i_best,2);
  if verbose >= 1
    disp(['best predecessor pair found from MSD: (',num2str(x_best),',',num2str(y_best),') with ',num2str(hit_cnt_max+1),' hits']);
  end
else
  if verbose >= 1
    disp('no predecessor pair found from MSD...');
  end
  x_best = -1;
  y_best = -1;
end  
