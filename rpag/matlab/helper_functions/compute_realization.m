% Computes the realizations of coefficient set 'coeff_set' which must consist
% of a set of coefficients including intermediate values, such that the
% distance from any target to the rest of the coefficient set is one.
% 
% The output format of 'realization' is one row for each coefficient with
% the following format: [coeff,r1,i,r2,j]
% Each coefficient can be realized by coeff = r1 * 2^i+r2*2^j
% r2 can be nagative to support subtraction.
% 
% The method uses the optimal algorithm and tracks all possible realizations
% in realization_set.

function [realization] = compute_realization(coeff_set, coeff_lim, varargin)

if ~exist('coeff_lim')
  coeff_lim = 2^(ceil(log2(max(coeff_set)))+1);
end

use_matrix = true; %if true, the old matrix format to describe an adder graph is used
coeff_realized=true;
global verbose;
realized_set=1;
realization_set = cell(1,coeff_lim);
successor_set = [];
coeff_set_init = coeff_set;

[coeff_set, coeff_pow] = fundamental(coeff_set);

output_coeffs=[];
for k=1:2:size(varargin,2)
  switch(varargin{k})
    case 'output_coeffs'
      output_coeffs = varargin{k+1};
    case 'use_cell'
      use_matrix = ~varargin{k+1};
    otherwise
      error(['Unknown parameter: ',varargin{k}])
  end
end

while coeff_realized
    coeff_realized=false;

    Nc=length(coeff_set);
    if verbose > 2
        disp(['computing successor set...']);
    end
    
    Nr=length(realized_set);
%    successor_set = zeros(1,coeff_lim*length(realized_set)^2); % allocate memory to max. size of array for faster computation
    i=1;
    for l=1:Nr
        for m=1:Nr
            r1=realized_set(l);
            r2=realized_set(m);
            if verbose > 3 disp(['(',num2str(r1),',',num2str(r2),')']); end;

            % r1 must be greater than r2, if not swap them
            if r1<r2
                tmp=r1;
                r1=r2;
                r2=tmp;
            end
            k_max=ceil(log((coeff_lim+r2)/r1)/log(2));
            l_max=ceil(log((coeff_lim+r1)/r2)/log(2));
            k_max = max(k_max,l_max);

            successor_subset = zeros(1,4*k_max);

            for k=0:k_max
                tmp = r1 * 2^k + r2;
                if k==0 [tmp,pow] = fundamental(tmp); else pow = 0; end; %for k=0, tmp is even (r1 and r2 are odd per definition) so make it fundamental
                if tmp < coeff_lim && tmp > 0
                    if length(realization_set{tmp}) == 0
                        realization_set{tmp} = [r1,k-pow,r2,-pow];
                    end
                    successor_subset(4*k+1) = tmp;
                else
                    successor_subset(4*k+1) = 0;
                end

                tmp = r1 + r2 * 2^k;
                if k==0 [tmp,pow] = fundamental(tmp); else pow = 0; end; %for k=0, tmp is even (r1 and r2 are odd per definition) so make it fundamental
                if tmp < coeff_lim && tmp > 0
                    if length(realization_set{tmp}) == 0
                        realization_set{tmp} = [r1,-pow,r2,k-pow];
                    end
                    successor_subset(4*k+2) = tmp;
                else
                    successor_subset(4*k+2) = 0;
                end
                
                tmp = abs(r1 * 2^k - r2);
                if k==0 [tmp,pow] = fundamental(tmp); else pow = 0; end; %for k=0, tmp is even (r1 and r2 are odd per definition) so make it fundamental
                if tmp < coeff_lim && tmp > 0
                    if length(realization_set{tmp}) == 0
                        if r1 * 2^k > r2
                            realization_set{tmp} = [r1,k-pow,-r2,-pow];
                        else
                            realization_set{tmp} = [-r1,k-pow,r2,-pow];
                        end
                    end
                    successor_subset(4*k+3) = tmp;
                else
                    successor_subset(4*k+3) = 0;
                end
                
                tmp = abs(r1 - r2 * 2^k);
                if k==0 [tmp,pow] = fundamental(tmp); else pow = 0; end; %for k=0, tmp is even (r1 and r2 are odd per definition) so make it fundamental
                if tmp < coeff_lim && tmp > 0
                    if length(realization_set{tmp}) == 0
                        if r1  > r2 * 2^k
                            realization_set{tmp} = [r1,-pow,-r2,k-pow];
                        else
                            realization_set{tmp} = [-r1,-pow,r2,k-pow];
                        end
                    end
                    successor_subset(4*k+4) = tmp;
                else
                    successor_subset(4*k+4) = 0;
                end
            end
            successor_set = setdiff(unique([successor_set,successor_subset]),[0,1]);
            i=i+length(successor_subset)+1;
        end
    end
    
    for l=1:Nc
        if is_member_of(successor_set, coeff_set(l))
            %realize coeff
%            [realized_set,coeff_set] = realize_coefficient(realized_set, coeff_set, coeff_set(l));
            realized_set = [realized_set, coeff_set(l)];
            coeff_set = setdiff(coeff_set,coeff_set(l));

            Nc = Nc-1;
            coeff_realized = true;
            break
        end
    end
end
realized_set = setdiff(realized_set,[0,1]);

if(use_matrix)
  realization = zeros(length(realized_set), 5);
else
  realization = cell(1,length(realized_set));
end

for i=1:length(realized_set)
    coeff=realized_set(i);
    
    %normalize, swapp coefficients when 1st is negative:
    if realization_set{coeff}(1) < 0
      t = realization_set{coeff}(1);
      realization_set{coeff}(1) = realization_set{coeff}(3);
      realization_set{coeff}(3) = t;

      t = realization_set{coeff}(2);
      realization_set{coeff}(2) = realization_set{coeff}(4);
      realization_set{coeff}(4) = t;      
    end
    
    if(use_matrix)
      realization(i,:) = [coeff, realization_set{coeff}];
    else
      realization{i} = {'A',coeff, realization_set{coeff}(1), realization_set{coeff}(2), realization_set{coeff}(3), realization_set{coeff}(4)};
    end
    if verbose > 0
        disp(['realizing coefficient ',num2str(coeff),' as ',num2str(realization_set{coeff}(1)),'*2^',num2str(realization_set{coeff}(2)),'+',num2str(realization_set{coeff}(3)),'*2^',num2str(realization_set{coeff}(4)),'(=',num2str(realization_set{coeff}(1)*2^realization_set{coeff}(2)+realization_set{coeff}(3)*2^realization_set{coeff}(4)),')']); 
    end;    
end

if isempty(output_coeffs)
  %add coefficients based on fundamentals:
  for i=1:length(coeff_pow)
    if coeff_pow(i) ~= 0
      if(use_matrix)
        realization = [realization; coeff_set_init(i), fundamental(coeff_set_init(i)), coeff_pow(i), 0, 0];
      else
        realization = {realization, {'A',coeff_set_init(i), fundamental(coeff_set_init(i)), coeff_pow(i), 0, 0}};
      end
    end
  end
else
  %add all output coefficients:
  [output_coeffs_fun,output_coeffs_pow] = fundamental(abs(output_coeffs));
  for i=1:length(output_coeffs)
    if is_member_of([1,0,realized_set], output_coeffs_fun(i))
      if(use_matrix)
        realization = [realization; output_coeffs(i), output_coeffs_fun(i), output_coeffs_pow(i), 0, 0];
      else
        realization = {realization, {'A',output_coeffs(i), output_coeffs_fun(i), output_coeffs_pow(i), 0, 0}};
      end
    else
      error(['Output coefficient ',num2str(output_coeffs_fun(i)),' is not in realization.'])
    end
  end
end
coeff_set  = setdiff(coeff_set,[0,1]);
if coeff_set
    disp(['WARNING: Coefficient(s) ', num2str(coeff_set), ' could not be realized as they have an adder distance > 1 to the other coefficients!']);
end
%successor_set