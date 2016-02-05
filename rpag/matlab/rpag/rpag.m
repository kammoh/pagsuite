%% rpag
% Implementation of the RPAG algorithm, which computes the pipeline sets of 
% a pipelined multiplier block or pipelined adder graph (PAG).

%% Syntax
% X = rpag(coeff,cost_model)
%
% [X,comp_time,decisions] = rpag(coeff,cost_model,bw,exhaustive,rand_variance,decisions)

%% Description
% X = rpag(coeff,cost_model) computes the pipeline sets X for 
% coefficients coeff. X is a cell array where X{s} contains the elements 
% of pipeline stage s. The cost model is defined by 'cost_model' which must
% be one following strings:
%
% * 'hl_fpga'    : high level FPGA cost model
% * 'll_fpga'    : low level FPGA cost model
% * 'hl_asic'    : high level ASIC cost model
% * 'll_asic'    : low level ASIC cost model
% * 'asic_icost' : more detailed model by Aksoy
% * 'mcm_min_ad' : minimal toatal adder depth model
%
% [X,comp_time,decisions] = rpag(coeff,cost_model,bw,exhaustive,rand_variance,decisions)
% uses all optional arguments which are:
%
% * bw            : Bit width of the multiplier block input
% * exhaustive    : Except from standard mode, the exhaustive mode seaches 
%                   for both single predecessor and predecessor pairs,
%                   choosing the best found. 
%                   (default: false)
% * rand_variance : Variance for random selection, e.g. a variance of 1
%                   means the best or 2nd best is used, variance 2 means 
%                   the 1st, 2nd or 3rd best one is used, etc. 
%                   (default: 0)
% * decisions     : A cell containing decisions. The value n at index i
%                   chooses the n-th best for decision no i. An empty
%                   decision cell is equal to the value 0 and means best
%                   decision. It can be used to explore better solutions by
%                   manipulating decisions in several runs. 
%                   (default: zero vector)
%
% The return values 'comp_time' and 'decisions' containts the computation
% time of the algorithm and the decision cell of the current run respectively.

%% Examples
% X = rpag([123,321],'hl_fpga') computes the pipeline sets for the
% coefficients 123 and 321

function [X,comp_time,decisions] = rpag(coeff,cost_model,bw,exhaustive,rand_variance,use_graph_search,decisions)

% used variables:
%
% W: Coefficient set of current pipeline stage
%
% P: predecessor set of W, if the complete set P is found, all coefficients
% from W can be realized by add/sub of shifted elements of P
%
% s: current pipeline stage
% S: number of pipeline stages
%
% X: Cell array containing all pipeline sets

global verbose;
use_graph_search=false; %!!!!!!!!!!

switch cost_model
  case 'hl_fpga'
    cost_add = @cost_add_hl_fpga;
    cost_reg = @cost_reg_hl_fpga;
  case 'hl_asic'
    cost_add = @cost_add_hl_asic;
    cost_reg = @cost_reg_hl_asic;
  case 'll_asic'
    cost_add = @cost_add_ll_asic;
    cost_reg = @cost_reg_ll_asic;
  case 'll_fpga'
    cost_add = @cost_add_ll_fpga;
    cost_reg = @cost_reg_ll_fpga;
  case 'mcm_min_ad'
    cost_add = @cost_add_mcm_min_ad;
    cost_reg = @cost_reg_mcm_min_ad;
  case 'asic_icost'
    cost_add = @cost_add_asic_icost;
    cost_reg = @cost_reg_asic_icost;
end

if(~exist('exhaustive'))
  exhaustive = false;
end
if(~exist('rand_variance'))
  rand_variance = 0;
end
if(~exist('decisions'))
  decisions = cell(0);
end
if(~exist('bw'))
  bw=0;
end

adder_depth_cached_prepare() % prepare cache for adder depth

coeff_unique = setdiff(unique(abs(coeff)),[0]);
coeff_fun = unique(fundamental(coeff_unique));
c_max = 2^(ceil(log2(max(coeff_fun)))+1);

%c_max = c_max*8; %!!!!!!

if(verbose >= 2)
  disp(['fundamentals=',num2str(coeff_fun)]);
  disp(['c_max=',num2str(c_max)]);
end
tic();

%find max. AD for number of pipeline stages:
S=0;
c_ad_max=0;
for i=1:length(coeff_fun)
  s=adder_depth(coeff_fun(i));
  if s > S
    S = s;
    c_ad_max = coeff_fun(i);
  end
end
if(verbose >= 1)
  disp(['number of pipeline stages: ',num2str(S), ' for coefficient ',num2str(c_ad_max)]);
end

W=coeff_fun;

%cell for solutions
X = cell(1,S);
X{1}=1;
decision_index = 1;

for s=S:-1:2
  if(verbose >= 2)
    disp(['****** optimizing pipeline stage ',num2str(s-1),' ******']);
  end
  X{s}=W;
  P=[];
    
  while ~isempty(W)
    if(verbose >= 2)
      disp(['P=[',num2str(P),']']);
      disp(['W=[',num2str(W),']']);
    end


    if(exhaustive)
      
      [x_best,gain_single,decisions,decision_index] = rpag_find_best_single_predecessor(W,P,s-1,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index);
      [x_i_best,x_j_best,gain_pair] = rpag_find_best_predecessor_pair(W,P,s-1,c_max,bw,cost_add,cost_reg,use_graph_search);
      
      if (gain_single == 0) && (gain_pair == 0)
        [x_i_best,x_j_best,gain_pair] = rpag_find_best_predecessor_pair(W,P,s-1,c_max,bw,cost_add,cost_reg,false);
      end
      
      if gain_single >= gain_pair
        if verbose >= 1
          disp(['choosing ',num2str(x_best)]);
        end
        R = succ_set([x_best,P],c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(W,R)),'] can be realized using x=',num2str(x_best),' with gain=',num2str(gain_single)]);
        end
        P=[P,x_best];
      else
        if verbose >= 1
          disp(['choosing (',num2str(x_i_best),',',num2str(x_j_best),')']);
        end
        R = succ(x_i_best,x_j_best,c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(W,R)),'] can be realized using x_i=',num2str(x_i_best),' and x_j=',num2str(x_j_best),' with gain=',num2str(gain_pair)]);
        end
        if x_i_best==x_j_best
          P=[P,x_i_best];
        else
          P=[P,x_i_best,x_j_best];
        end
      end
    else
      [x_i_best,gain_single,decisions,decision_index] = rpag_find_best_single_predecessor(W,P,s-1,c_max,rand_variance,bw,cost_add,cost_reg,decisions,decision_index);

      if x_i_best > 0
        %a single predecessor x was selected:
        R = succ_set([x_i_best,P],2*c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(W,R)),'] can be realized using x=',num2str(x_i_best),' (gain=',num2str(gain_single),')']);
        end
        P=[P,x_i_best];
      else
        [x_i_best,x_j_best,gain_pair] = rpag_find_best_predecessor_pair(W,P,s-1,c_max,bw,cost_add,cost_reg,use_graph_search);
        if x_i_best < 0
          error('could not find a predecessor pair (this should never happen!)');
        end

        %a pair (x,y) was was selected:
        R = succ(x_i_best,x_j_best,2*c_max); %determine realized coefficients
        if verbose >= 1
          disp(['coefficient(s) [',num2str(intersect(W,R)),'] can be realized using x_i=',num2str(x_i_best),' and x_j=',num2str(x_j_best),' (gain=',num2str(gain_pair),')']);
        end
        if x_i_best==x_j_best
          P=[P,x_i_best];
        else
          P=[P,x_i_best,x_j_best];
        end
      end
    end
    W=setdiff(W,R); %remove realized coefficients from W
  end
  if(verbose >= 2)
    disp(['P=[',num2str(P),']']);
    disp(['W=[',num2str(W),']']);
    disp('-----------------------------------');
  end  
  W=P;
end
X{1}=W;

comp_time = toc();
if(verbose >= 1)
  disp(['Elapsed time is ',num2str(comp_time),' seconds.']);
  disp('solution:');
end

for s=1:S
  X{s} = unique(X{s}); %!!! this should not be neccessary
  if(verbose >= 1)
    disp(['fundamentals for pipeline stage ',num2str(s),'=[',num2str(X{s}),']']);
  end
end

if(verbose >= 2)
  disp('checking solution...');
end
  for s=2:S
    if length(X{s}) ~= length(intersect(X{s},succ_set(X{s-1},2*c_max)));
      disp(['X{s}=',mat2str(X{s})]);
      disp(['X{s-1}=',mat2str(X{s-1})]);
      disp(['intersect(X{s},succ_set(X{s-1},2*c_max))=',mat2str(intersect(X{s},succ_set(X{s-1},2*c_max)))]);
      error('solution is not valid!');
    end
  end
if(verbose >= 2)
  disp('done!');
end

decisions = {decisions{1:decision_index-1}};


%%%% Definition of cost functions %%%%

% A high level FPGA cost model:
% registerd adders are assumed to have the same costs than pure registers
function cost = cost_add_hl_fpga(x,u,v,bw,c_max)
  cost=1;

function cost = cost_reg_hl_fpga(x,bw)
  cost=1;

% A low level FPGA cost model:
% registerd adders are assumed to have the same costs than pure registers
% but are weighted by the bit width
function cost = cost_add_ll_fpga(x,u,v,bw,c_max)
  cost=bw+ceil(log2(x));

function cost = cost_reg_ll_fpga(x,bw)
  cost=bw+ceil(log2(x));

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

function cost = cost_add_asic_icost(x,u,v,bw,c_max)
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
