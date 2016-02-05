% find_best_predecessor_pair_from_msd searches for the predecessor pair
% (x,y) for which the most elements in C can be realized.
% Only x and y values that can be computed from the MSD representation 
% of coefficients in C are considered and limited to those with an adder
% depth < max_adder_depth
%
% All possible (x,y) pairs are found using find_msd_potentials()
function [x_i_best,x_j_best,gain_max] = find_best_predecessor_pair_from_msd5(C,P,max_adder_depth,c_max,bw,cost_add,cost_reg) 

global verbose;

if verbose >= 2
  disp('searching for best predecessor pair from MSD...');
end

all_pairs = find_msd_potentials(C,max_adder_depth);

x_i_best=-1;
x_j_best=-1;
gain_max=0;
hits_best=0;

all_pairs = unique(all_pairs,'rows');
all_pairs_size = size(all_pairs);

for i=1:all_pairs_size(1)
  act_pair = sort(all_pairs(i,:));
  x_i=act_pair(1);
  x_j=act_pair(2);

  R = intersect(succ(x_i,x_j,c_max),C); % c_max*2 !!!
  
%  R2 = intersect(succ_set([act_pair,P],c_max),C);
%  disp(['R=',num2str(R)]);
%  disp(['R2=',num2str(R2)]);
%  assert(R == R2);
  
  hits = length(R);
  
  gain=0;
  
  if(hits > 0)
    for j=1:length(R);
      if((R(j) == x_i) || (R(j) == x_j))
        gain = gain + 1/cost_reg(R(j),bw);
      else
        gain = gain + 1/cost_add(R(j),x_i,x_j,bw,c_max);
      end
    end
    gain = gain / 2; %divide by two as two predecessors are neccessary
  end
    
  if verbose >= 3
    disp(['Evaluating pair (',num2str(x_i),',',num2str(x_j),')...']);
  end

  if gain > gain_max
    if verbose >= 3
      disp(['better MSD pair found: (',num2str(x_i),',',num2str(x_j), ') with ',num2str(hits),' hits and gain=',num2str(gain)]);
    end
    gain_max = gain;
    hits_best = hits;
    x_i_best = x_i;
    x_j_best = x_j;
  end
end
if verbose >= 2
  disp(['best MSD pair: (',num2str(x_i_best),',',num2str(x_j_best), ') with ',num2str(hits_best),' hits and gain=',num2str(gain_max)]);
end

