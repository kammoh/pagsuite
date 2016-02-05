% find_best_predecessor_pair_from_msd searches for the predecessor pair
% (x,y) for which the most elements in C can be realized.
% Only x and y values that can be computed from the MSD representation 
% of coefficients in C are considered and limited to those with an adder
% depth < max_adder_depth
%
% All possible (x,y) pairs are found using find_msd_potentials()
function [x_i_best,x_j_best,gain_max] = rpag_find_best_predecessor_pair(C,P,max_adder_depth,c_max,bw,cost_add,cost_reg,use_graph_search) 

global verbose;
if ~exist('use_graph_search')
  use_graph_search = false;
end

x_i_best=-1;
x_j_best=-1;
gain_max=0;

if use_graph_search
  if verbose >= 2
    disp('searching for best predecessor pair from graph representation...');
  end
  if length(C) < 2
    return
  end
  all_pairs = compute_predecessor_pair_graph(C,max_adder_depth);
  if isempty(all_pairs)
    return;
  end
else
  if verbose >= 2
    disp('searching for best predecessor pair from MSD...');
  end
  all_pairs = find_msd_potentials(C,max_adder_depth);
end

hits_best=0;

all_pairs = unique(all_pairs,'rows');
all_pairs_size = size(all_pairs);

for i=1:all_pairs_size(1)
  act_pair = sort(all_pairs(i,:));
  x_i=act_pair(1);
  x_j=act_pair(2);

  if (x_i < 2*c_max) && (x_j < 2*c_max)
    R = intersect(succ(x_i,x_j,c_max*2),C);
  
%  R2 = intersect(succ_set([act_pair,P],c_max),C);
%  disp(['R=',num2str(R)]);
%  disp(['R2=',num2str(R2)]);
%  assert(R == R2);
  
    hits = length(R);

    gain=0;

    assert(hits > 0); % a predecessor pair should allways result in at least one hit!

    if(hits > 0)
      for j=1:length(R);
        if((R(j) == x_i) || (R(j) == x_j))
          gain = gain + 1/cost_reg(R(j),bw); %this should never occur in non-exhaustive mode!
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
        disp(['better pair found: (',num2str(x_i),',',num2str(x_j), ') with ',num2str(hits),' hits and gain=',num2str(gain)]);
      end
      gain_max = gain;
      hits_best = hits;
      x_i_best = x_i;
      x_j_best = x_j;
    end
  end
end
if verbose >= 2
  disp(['best pair: (',num2str(x_i_best),',',num2str(x_j_best), ') with ',num2str(hits_best),' hits and gain=',num2str(gain_max)]);
end

