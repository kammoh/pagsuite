function [op_cnt,S,comp_time] = pip_mcm3(coeff,mode)

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

coeff_unique = setdiff(unique(abs(coeff)),[0,1]);
coeff_fun = unique(fundamental(coeff_unique));
c_max = 2^(ceil(log2(max(coeff_fun))));

if(verbose >= 3)
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

for s=s_max:-1:2
  if(verbose >= 2)
    disp(['****** optimizing pipeline stage ',num2str(s-1),' ******']);
  end
  S{s}=C;
  P=[];
  As=A{s};
  
  while length(C)~=0
    if(verbose >= 2)
      disp(['P=[',num2str(P),']']);
      disp(['C=[',num2str(C),']']);
    end
    
    [x_best] = find_best_single_predecessor(C,P,As,c_max);

    if x_best > 0
      %a single predecessor x was selected:
      R = succ_set([x_best,P],c_max); %determine realized coefficients
      if verbose >= 1
        disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x=',num2str(x_best)]);
      end
      C=setdiff(C,R); %remove realized coefficients from C
      P=[P,x_best];
    else
      [x_best,y_best] = find_best_predecessor_pair_from_msd(C,s-1);
      if x_best < 0
        [x_best,y_best] = find_best_predecessor_pair(C,P,As,c_max);
      end
      
      %a pair (x,y) was was selected:
      R = succ(x_best,y_best,c_max); %determine realized coefficients
      if verbose >= 1
        disp(['coefficient(s) [',num2str(intersect(C,R)),'] can be realized using x=',num2str(x_best),' and y=',num2str(y_best)]);
      end
      C=setdiff(C,R); %remove realized coefficients from C
      if x_best==y_best
        P=[P,x_best];
      else
        P=[P,x_best,y_best];
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
%    IS=intersect(S{s},succ_set(S{s-1},c_max,true,false))
%    S1=S{s}
%    S0=S{s-1}
    assert(length(S{s})==length(intersect(S{s},succ_set(S{s-1},c_max))));
  end
if(verbose >= 2)
  disp('done!');
end