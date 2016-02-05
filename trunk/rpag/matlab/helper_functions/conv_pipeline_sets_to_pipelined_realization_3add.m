function pipelined_realization = conv_pipeline_sets_to_pipelined_realization_3add(X,output_coeff,c_max)

s_max = length(X);

%if output coeffs are not defined, use last pipeline stage as output values
if ~exist('output_coeff')
  output_coeff = X{s_max};
end
if ~exist('c_max')
  c_max = 2^(ceil(log2(max([X{:}])))+1);
end

pipelined_realization = cell(0);
for s=1:s_max
  if ~isempty(X{s})
    if s==1
      for i=1:length(X{1})
        %1st stage:
        realization = compute_exponents(X{1}(i),1,1,c_max);
        if ~isempty(realization)
          pipelined_realization{end+1} = {realization(1),1,realization(2),0,realization(3),realization(4),0,realization(5),0,0,0};
        else
          realization = compute_exponents_3add(X{1}(i),1,1,1,c_max);
          if ~isempty(realization)
            pipelined_realization{end+1} = {realization(1),1,realization(2),0,realization(3),realization(4),0,realization(5),realization(6),0,realization(7)};
          else
            warning(['Coefficient ',num2str(X{1}(i)),' can not be realized in stage 1']);
            pipelined_realization{end+1} = {X{s}(i),s,1,0,0,0,0,0,0,0,0};
          end
        end
      end
    else
      %n'th stage (n>1):
      for i=1:length(X{s})
        realization_found=false;
        if is_member_of(X{s-1},X{s}(i))
          pipelined_realization{end+1} = {X{s}(i),s,X{s}(i),s-1,0,0,s-1,0,0,s-1,0};
          realization_found = true;
          continue
        end
        %search for realization of X{s}(i) from X{s-1}

        %search for 2-input realization:
        for j=1:length(X{s-1})
          for k=1:length(X{s-1})
            realization = compute_exponents(X{s}(i),X{s-1}(j),X{s-1}(k),c_max);
            if(length(realization) > 0)
              pipelined_realization{end+1} = {realization(1),s,realization(2),s-1,realization(3),realization(4),s-1,realization(5),0,0,0};
              realization_found=true;
              break
            end
          end
          if realization_found
            break;
          end
        end
        if realization_found
          continue;
        end
        
        %search for 3-input realization:
        for j=1:length(X{s-1})
          for k=1:length(X{s-1})
            for l=1:length(X{s-1})
              realization = compute_exponents_3add(X{s}(i),X{s-1}(j),X{s-1}(k),X{s-1}(l),c_max);
              if(length(realization) > 0)
                pipelined_realization{end+1} = {realization(1),s,realization(2),s-1,realization(3),realization(4),s-1,realization(5),realization(6),s-1,realization(7)};
                realization_found=true;
                break
              end
            end
            if realization_found
              break;
            end
          end
          if realization_found
            break;
          end
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
    output_coeff_found=false;
    for k=1:length(pipelined_realization)
      if(cell2mat(pipelined_realization{k}(1)) == output_coeff_fun(i))
        output_coeff_found=true;
      end
    end
    if ~output_coeff_found
      error(['no realization for output coefficient ',num2str(output_coeff(i)),' possible']);
    end
    pipelined_realization{end+1} = {output_coeff(i),s,output_coeff_fun(i),s,output_coeff_exp(i),0,s,0,0,s,0};
  end

%  pipelined_realization = remove_redundant_pipelined_realizations(pipelined_realization);
end
