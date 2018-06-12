% conv_pipeline_sets_to_pipelined_realization(X,output_coeff,c_max)
% converts a cell array of pipeline sets X, for which all elements of X{s}
% are computable from X{s-1} using one addition (X{0} is defined to {1}),
% to a pipelined realization matrix which has the following format:
%
% pipelined_realization(i,1): output value of adder
% pipelined_realization(i,2): pipeline stage of adder
% pipelined_realization(i,3): input 1
% pipelined_realization(i,4): pipeline stage of input 1
% pipelined_realization(i,5): shift factor of input 1
% pipelined_realization(i,6): input 2
% pipelined_realization(i,7): pipeline stage of input 2
% pipelined_realization(i,8): shift factor of input 2
%
% The output coefficient (output_coeff) should be computable from the last
% pipeline set using shifts only
% c_max is the optional coefficient range limit

function pipelined_realization = conv_pipeline_sets_to_pipelined_realization(X,output_coeff,c_max)

s_max = length(X);

%if output coeffs are not defined, use last pipeline stage as output values
if ~exist('output_coeff')
  output_coeff = X{s_max};
end
if ~exist('c_max')
  c_max = 2^(ceil(log2(max([X{:}])))+1);
end

pipelined_realization = [];
for s=1:s_max
  if ~isempty(X{s})
    if s==1
      for i=1:length(X{1})
        %1 +/- 2^k = X{1}(i)
        realization = compute_exponents(X{1}(i),1,1,c_max);
        assert(~isempty(realization), ['Coefficient ',num2str(X{1}(i)),' can not be realized in stage 1']);
        pipelined_realization = [pipelined_realization; realization(1),1,realization(2),0,realization(3),realization(4),0,realization(5)];
      end
    else
      if ~isempty(X{s-1})
        %search for X{s-1} +/- X{s-1}*2^k = X{s}
        for i=1:length(X{s})
          realization_found=false;

          if is_member_of(X{s-1},X{s}(i))
            pipelined_realization = [pipelined_realization; X{s}(i),s,X{s}(i),s-1,0,0,s-1,0];
            realization_found = true;
            continue
          end
          %search for realization of X{s}(i) from X{s-1}
          for j=1:length(X{s-1})
            for k=1:length(X{s-1})
              realization = compute_exponents(X{s}(i),X{s-1}(j),X{s-1}(k),c_max);
              if(length(realization) > 0)
                pipelined_realization = [pipelined_realization; realization(1),s,realization(2),s-1,realization(3),realization(4),s-1,realization(5)];
                realization_found=true;
                break
              end
            end
            if realization_found
              break;
            end
          end
          assert(realization_found, ['Coefficient ',num2str(X{s}(i)),' can not be realized in stage ',num2str(s)]);
        end
      end
    end
  end
end
%append output coeffs:
if ~isempty(pipelined_realization)
  output_coeff = setdiff(unique(abs(output_coeff)),0);
  [output_coeff_fun,output_coeff_exp] = fundamental(output_coeff);
  for i=1:length(output_coeff)
    assert(nnz(pipelined_realization(:,1)==output_coeff_fun(i))>0,['no realization for output coefficient ',num2str(output_coeff(i)),' possible']);
    pipelined_realization = [pipelined_realization; output_coeff(i),s,output_coeff_fun(i),s,output_coeff_exp(i),0,s,0];
  end

%  pipelined_realization = remove_redundant_pipelined_realizations(pipelined_realization);
end
