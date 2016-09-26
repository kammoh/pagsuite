% find_best_predecessor_pair_from_msd searches for the predecessor pair
% (x,y) for which the most elements in C can be realized.
% Only x and y values that can be computed from the MSD representation 
% of coefficients in C are considered and limited to those with an adder
% depth < max_adder_depth
%
% All possible (x,y) pairs are found using find_msd_potentials()
function [x_i_best,x_j_best,cost_min] = find_best_predecessor_pair_from_msd4(C,P,max_adder_depth,c_max,cost_add,cost_reg) 

global verbose;

if verbose >= 2
  disp('searching for best predecessor pair from MSD...');
end

all_pairs = find_msd_potentials(C,max_adder_depth);

x_best=-1;
y_best=-1;
cost_min=Inf;
hits_best=0;

all_pairs = unique(all_pairs,'rows');
all_pairs_size = size(all_pairs);

for i=1:all_pairs_size(1)
  act_pair = sort(all_pairs(i,:));
  x_i=act_pair(1);
  x_j=act_pair(2);
  if(is_member_of(C,x_i))
    cost = cost_reg(x_i);
  else
    cost = cost_add(x_i);
  end
  if(is_member_of(C,x_j))
    cost = cost + cost_reg(x_j);
  else
    cost = cost + cost_add(x_j);
  end
  
  if verbose >= 3
    disp(['Evaluating pair (',num2str(x_i),',',num2str(x_j),')...']);
  end
  hits = length(intersect(succ_set([act_pair,P],c_max),C));
  cost = cost/hits;
  
  if cost < cost_min
    if verbose >= 3
      disp(['better MSD pair found: (',num2str(x_i),',',num2str(x_j), ') with ',num2str(hits),' hits and cost=',num2str(cost)]);
    end
    cost_min = cost;
    hits_best = hits;
    x_i_best = x_i;
    x_j_best = x_j;
  end
end
if verbose >= 2
  disp(['best MSD pair: (',num2str(x_i_best),',',num2str(x_j_best), ') with ',num2str(hits_best),' hits and cost=',num2str(cost_min)]);
end

