function [x_best,gain_max,decisions,decision_index] = rpag_find_best_single_predecessor(C,P,s,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index) 

global verbose;

if verbose >= 2
  disp('searching for best single predecessor...');
end

gain_max = 0;
x_best=-1;
gain_map = containers.Map(0,0);

for i=1:length(C)
  c = C(i);
  % case a:
  if adder_depth(c,true) <= s
    if verbose >= 4
      disp(['**case a met for c=',num2str(c),' (p''=',num2str(c),')']);
    end
    if gain_map.isKey(c)
      gain_map(c) = gain_map(c)+1/cost_reg(c,bw);
    else
      gain_map(c) = 1/cost_reg(c,bw);
    end
  end
  % case b:
  k=2;
  div2=3;
  while div2 <= c
    div1 = 2^k+1;
    div2 = 2^k-1;
    if (mod(c,div1)==0)
      quot=c/div1;
    elseif (mod(c,div2)==0)
      quot=c/div2;
    else
      quot=0;
    end
    if quot > 0
      if adder_depth(quot,true) <= s
        if verbose >= 4
          disp(['**case b met for c=',num2str(c),' (p''=',num2str(quot),')']);
        end
        if gain_map.isKey(quot)
          gain_map(quot) = gain_map(quot)+1/cost_add(c,quot,quot,bw,c_max);
        else
          gain_map(quot) = 1/cost_add(c,quot,quot,bw,c_max);
        end
      end
    end
    k=k+1;
  end
  predecessor_ignore_list = cell(0); %list for predecessors that have to be ignored to avoid double counting same predecessors
  % case c:
  for j=1:length(P)
    PP = succ(c,P(j),c_max,true,false); %set of potential predecessors
    if verbose >= 5
      disp(['PP(',num2str(c),',',num2str(P(j)),')=',num2str(PP)]);
    end
    for k=1:length(PP)
      if adder_depth(PP(k),true) <= s
        if verbose >= 4
          disp(['**case c met for c=',num2str(c),' (p''=',num2str(PP(k)),',p=',num2str(P(j)),')']);
        end
        if gain_map.isKey(PP(k))
          if ~is_member_of([predecessor_ignore_list{:}],PP(k))
            gain_map(PP(k)) = gain_map(PP(k))+1/cost_add(c,PP(k),P(j),bw,c_max);
          else
            if verbose >= 4
              disp(['**-> ignored for gain']);
            end
          end
        else
          gain_map(PP(k)) = 1/cost_add(c,PP(k),P(j),bw,c_max);
        end
        predecessor_ignore_list{length(predecessor_ignore_list)+1} = PP(k);
      end
    end
  end
end

[gain_max,x_best_index] = max(cell2mat(gain_map.values));
gain_key_cell = gain_map.keys;
x_best = gain_key_cell{x_best_index};

if verbose >= 4
  [gain_map.keys;gain_map.values]
end

%if rand_variance > 0
gain_mat = sortrows(cell2mat([gain_map.keys;gain_map.values])',[-2 1]);
if verbose >= 3
  disp('gain_mat:');
  disp(num2str(gain_mat));
end
gain_mat_size = size(gain_mat);
if gain_mat_size(1) > 1
  best_cnt = 1;
  while gain_mat(gain_mat_size(1)-best_cnt,2) == gain_max
    best_cnt = best_cnt + 1;
    if best_cnt >= gain_mat_size(1)
      break;
    end
  end
    if (best_cnt > 1) && (verbose >= 3)
      disp(['++more than one possibilities found: ',num2str(gain_mat(gain_mat_size(1)-best_cnt+1:gain_mat_size(1),1)')]);
    end
    if decision_index > length(decisions)
      if rand_variance > 0
        decision = round(rand(1)*min(rand_variance,gain_mat_size(1)));
      else
        decision = 0;
      end
      decisions{decision_index}=decision;
    else
      decision = decisions{decision_index};
    end
    decision_index = decision_index + 1;
    x_best = gain_mat(1+decision,1);
end

if verbose >= 2
  disp(['best single predecessor ',num2str(x_best),' with gain=',num2str(gain_max)]);
end
