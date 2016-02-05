function [x_best,gain_max,decisions,decision_index] = find_best_single_predecessor5b(C,P,A,s,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index) 

%if isempty(P) 
%  x_best  = 0;
%  return;
%end

global verbose;

if verbose >= 2
  disp('searching for best single predecessor 5b...');
end

gain_max = 0;
x_best=-1;
gain_map = containers.Map(0,0);

for i=1:length(C)
  c = C(i);
  % case 1:
  if adder_depth(c) <= s
    if verbose >= 4
      disp(['**case 1 met for c=',num2str(c),' (p''=',num2str(c),')']);
    end
    if gain_map.isKey(c)
      gain_map(c) = gain_map(c)+1/cost_reg(c,bw);
    else
      gain_map(c) = 1/cost_reg(c,bw);
    end
  end
  % case 2:
  j=1;
  while A{2}(j) <= c
    if (mod(c,A{2}(j))==0) && (A{2}(j) ~= 1)
      div=c/A{2}(j);
      if adder_depth(div) <= s
        if verbose >= 4
          disp(['**case 2 met for c=',num2str(c),' (p''=',num2str(div),')']);
        end
        if gain_map.isKey(div)
          gain_map(div) = gain_map(div)+1/cost_add(c,div,div,bw,c_max);
        else
          gain_map(div) = 1/cost_add(c,div,div,bw,c_max);
        end
      end
    end
    j=j+1;
  end
  predecessor_ignore_list = cell(0); %list or predecessors that have to be ignored to avoid double counting same predecessors
  % case 3:
  for j=1:length(P)
    PP = succ(c,P(j),c_max,true,false); %set of potential predecessors
    if verbose >= 5
      disp(['PP(',num2str(c),',',num2str(P(j)),')=',num2str(PP)]);
    end
    for k=1:length(PP)
      if adder_depth(PP(k)) <= s
        if verbose >= 4
          disp(['**case 3 met for c=',num2str(c),' (p''=',num2str(PP(k)),',p=',num2str(P(j)),')']);
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
