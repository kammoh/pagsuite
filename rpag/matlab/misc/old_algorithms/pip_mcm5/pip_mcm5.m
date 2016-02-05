function [op_cnt,S,comp_time,decisions] = pip_mcm5(coeff,bw,mode,cost_model,exhaustive,rand_variance,decisions)

% used variables:
%
% C: Coefficient set of current pipeline stage
%
% P: predecessor set of C, if the complete set P is found, all coefficients
% from C can be realized by add/sub of shifted elements of P
%
% s: current pipeline stage
% s_max: number of pipeline stages
%
% A{s}: Cell, with sets containting all integers with property adder depth <= s
%
% As: set containting all integers with property adder depth <= s, which
% are not in P
%
% x,y: Single elements of A{n} that are used to find the next values of P

global verbose;

if ~exist('mode','var')
  mode = 'A'; %default mode
end

switch cost_model
  case 'hl_fpga'
    cost_add = @cost_add_hl_fpga;
    cost_reg = @cost_reg_hl_fpga;
  case 'hl_asic'
    cost_add = @cost_add_hl_asic;
    cost_reg = @cost_reg_hl_asic;
  case 'mcm_min_ad'
    cost_add = @cost_add_mcm_min_ad;
    cost_reg = @cost_reg_mcm_min_ad;
  case 'll_asic'
    cost_add = @cost_add_asic_icost;
    cost_reg = @cost_reg_asic_icost;
end

coeff_unique = setdiff(unique(abs(coeff)),[0]);
coeff_fun = unique(fundamental(coeff_unique));
c_max = 2^(ceil(log2(max(coeff_fun)))+1);

if(verbose >= 2)
  disp(['fundamentals=',num2str(coeff_fun)]);
  disp(['c_max=',num2str(c_max)]);
end
tic();


s_max=0;
c_ad_max=0;
for i=1:length(coeff_fun)
  s=adder_depth(coeff_fun(i));
  if s > s_max
    s_max = s;
    c_ad_max = coeff_fun(i);
  end
end

if(verbose >= 1)
  disp(['number of pipeline stages: ',num2str(s_max), ' for coefficient ',num2str(c_ad_max)]);
end

%precalculate the A-sets and store in cell array
A = cell(1,s_max);
A{1}=[1];
for i=2:s_max
  if mode == 'A'
    A{i} = succ_set(A{i-1},c_max,true);
  elseif mode == 'B'
    A{i} = succ_set(A{i-1},c_max); %!!!! the exclusion from lower AD seems to be beneficialy !!!
  elseif mode == 'C'
    A{i} = succ_set(A{i-1},c_max,true,true); %!!!! the exclusion from lower AD seems to be beneficialy !!!
  end
  if(verbose >= 2)
    disp(['no. of possibilities of pipeline stage ',num2str(i),':',num2str(length(A{i}))]);
  end
end

C=coeff_fun;

%cell for solutions
S = cell(1,s_max);
S{1}=1;
decision_index = 1;

for s=s_max:-1:2
  if(verbose >= 2)
    disp(['****** optimizing pipeline stage ',num2str(s-1),' ******']);
  end
  S{s}=C;
  P=[];
  As=A{s};
    
  while ~isempty(C)
    if(verbose >= 2)
      disp(['P=[',num2str(P),']']);
      disp(['C=[',num2str(C),']']);
    end


    if(exhaustive)
      [x_best,gain_single,decisions,decision_index] = find_best_single_predecessor5b(C,P,A,s-1,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index);
      [x_i_best,x_j_best,gain_msd_pair] = find_best_predecessor_pair_from_msd5(C,P,s-1,c_max,bw,cost_add,cost_reg);
      
      if gain_single >= gain_msd_pair
        if verbose >= 1
          disp(['choosing ',num2str(x_best)]);
        end
        R = succ_set([x_best,P],c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x=',num2str(x_best),' with gain=',num2str(gain_single)]);
        end
        C=setdiff(C,R); %remove realized coefficients from C
        P=[P,x_best];
      else
        if verbose >= 1
          disp(['choosing (',num2str(x_i_best),',',num2str(x_j_best),')']);
        end
        R = succ(x_i_best,x_j_best,c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x_i=',num2str(x_i_best),' and x_j=',num2str(x_j_best),' with gain=',num2str(gain_msd_pair)]);
        end
        C=setdiff(C,R); %remove realized coefficients from C
        if x_i_best==x_j_best
          P=[P,x_i_best];
        else
          P=[P,x_i_best,x_j_best];
        end

      end
    else
    
      verbose_old=verbose;
%      verbose=4;
%      [x_i_best,gain_single] = find_best_single_predecessor5a(C,P,As,c_max,cost_add,cost_reg);
      [x_i_best,gain_single,decisions,decision_index] = find_best_single_predecessor5b(C,P,A,s-1,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index);
%      if (x_i_best == 7) && (s == 2)
%        [x_i_best,gain_single] = find_best_single_predecessor5a(C,P,As,c_max,cost_add,cost_reg);
%        return;
%      end
      verbose=verbose_old;

      if x_i_best > 0
        %a single predecessor x was selected:
        R = succ_set([x_i_best,P],2*c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x=',num2str(x_i_best),' (gain=',num2str(gain_single),')']);
        end
        C=setdiff(C,R); %remove realized coefficients from C
        P=[P,x_i_best];
      else

%        warning('find_best_predecessor_pair_from_msd5');

        [x_i_best,x_j_best,gain_msd_pair] = find_best_predecessor_pair_from_msd5(C,P,s-1,c_max,bw,cost_add,cost_reg);
        if x_i_best < 0
          error('find_best_predecessor_pair4');
          [x_i_best,x_j_best] = find_best_predecessor_pair4(C,P,As,c_max,cost_add,cost_reg);
        end

        %a pair (x,y) was was selected:
        R = succ(x_i_best,x_j_best,c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x_i=',num2str(x_i_best),' and x_j=',num2str(x_j_best),' (gain=',num2str(gain_msd_pair),')']);
        end
        C=setdiff(C,R); %remove realized coefficients from C
        if x_i_best==x_j_best
          P=[P,x_i_best];
        else
          P=[P,x_i_best,x_j_best];
        end
      end
    end    
    As=setdiff(As,P);
  end
  if(verbose >= 2)
    disp(['P=[',num2str(P),']']);
    disp(['C=[',num2str(C),']']);
    disp('-----------------------------------');
  end  
  C=P;
end
S{1}=C;

comp_time = toc();
if(verbose >= 1)
  disp(['Elapsed time is ',num2str(comp_time),' seconds.']);
  disp('solution:');
end

op_cnt=0;
for s=1:s_max
  S{s} = unique(S{s}); %!!! this should not be neccessary
  if(verbose >= 1)
    disp(['fundamentals for pipeline stage ',num2str(s),'=[',num2str(S{s}),']']);
  end
  op_cnt=op_cnt+length(S{s});
end
if(verbose >= 1)
  disp(['operations needed: ',num2str(op_cnt)]);
end

if(verbose >= 2)
  disp('checking solution...');
end
  for s=2:s_max
    if length(S{s}) ~= length(intersect(S{s},succ_set(S{s-1},2*c_max)));
      S{s}
      intersect(S{s},succ_set(S{s-1},2*c_max))
      error('solution is not valid!');
    end
  end
if(verbose >= 2)
  disp('done!');
end

decisions = {decisions{1:decision_index-1}};

% A high level FPGA cost model:
% registerd adders are assumed to have the same costs than pure registers
function cost = cost_add_hl_fpga(x,u,v,bw,c_max)
  cost=1;

function cost = cost_reg_hl_fpga(x,bw)
  cost=1;

% A high level ASIC cost model:
% registerd adders are assumed to have twice the costs of pure registers as
% a full adder approximately needs the same area than a flip-flop

function cost = cost_add_hl_asic(x,u,v,bw,c_max)
  cost=2;

function cost = cost_reg_hl_asic(x,bw)
  cost=1;

% A MCM cost model for minimal adder depth

function cost = cost_add_mcm_min_ad(x,u,v,bw,c_max)
  cost=1;

function cost = cost_reg_mcm_min_ad(x,bw)
  cost=0.1;

% A low level ASIC cost model:
function cost = cost_add_ll_asic(x,u,v,bw,c_max,signed)
  cost_HA = 26;
  cost_FA = 62;
  cost_HAb = 26;
  cost_INV = 6;

  realization = compute_exponents(x,u,v,c_max,false);

  [N_FA,N_HA,N_HAb,N_INV] = add_sub_ll_components(realization,bw,signed);
  
  cost = N_FA*cost_FA+N_HA*cost_HA+N_HAb*cost_HAb+N_INV*cost_INV;
  disp(['N_FA=',num2str(N_FA),' N_HA=',num2str(N_HA),' N_HAb=',num2str(N_HAb),' N_INV=',num2str(N_INV)]);
  disp(['cost=',num2str(cost)]);
  
function cost = cost_reg_ll_asic(x,bw)
  cost_FF = 52;
  cost = cost_FF*(bw+ceil(log2(x)));

function cost = cost_add_asic_icost(x,u,v,bw,c_max,signed)
  realization = compute_exponents(x,u,v,c_max,false);
  if ~isempty(realization)
    cost = add_sub_icost(realization,bw);
  else
    warning('no realization found!!!');
    cost = 1;
  end
  cost = cost + cost_reg_asic_icost(x,bw);
  
function cost = cost_reg_asic_icost(x,bw)
  cost_FF = 52;
  cost = cost_FF*(bw+ceil(log2(x)));
