function [fpga_ll_cost,fpga_ll_cost_add,fpga_ll_cost_reg] = fpga_ll_cost_pipelined_realization(pipelined_realization,wordsize)

global verbose;
fpga_ll_cost = 0;
fpga_ll_cost_add = 0;
fpga_ll_cost_reg = 0;

pipelined_realization_size = size(pipelined_realization);

% for l=1:pipelined_realization_size(1)
%   if pipelined_realization(l,2) ~= pipelined_realization(l,4)
%     fpga_ll_cost = fpga_ll_cost + wordsize + ceil(log2(pipelined_realization(l,1)));
%   end
% end

%cost function of Meyer-Baese (costs of output fundamentals are ignored)
S = max(pipelined_realization(:,2));
for l=1:pipelined_realization_size(1)
  stage_is_reg = (pipelined_realization(l,1) == pipelined_realization(l,3)) && (pipelined_realization(l,6) == 0) && (pipelined_realization(l,2) > pipelined_realization(l,4));
  stage_is_add = (pipelined_realization(l,1) ~= pipelined_realization(l,3)) && (pipelined_realization(l,6) ~= 0);
  if stage_is_reg
    if verbose >= 3
      disp(['  using ',num2str(wordsize + ceil(log2(pipelined_realization(l,1)))), ' FF for element ',num2str(pipelined_realization(l,1)),' in stage ',num2str(pipelined_realization(l,2))]);
    end
    fpga_ll_cost_reg = fpga_ll_cost_reg + wordsize + ceil(log2(pipelined_realization(l,1)));
  elseif stage_is_add
    cost_right_shift=0;
    if pipelined_realization(l,5) < 0
      assert(pipelined_realization(l,8) < 0);
      cost_right_shift=abs(pipelined_realization(l,8));
      if verbose >= 2
        disp(['!!additional ',num2str(abs(pipelined_realization(l,8))), ' FAs for right shift of element ',num2str(pipelined_realization(l,1)),' in stage ',num2str(pipelined_realization(l,2))]);
      end
    end
    if verbose >= 3
      disp(['  using ',num2str(wordsize + ceil(log2(pipelined_realization(l,1))) + cost_right_shift), ' FA for element ',num2str(pipelined_realization(l,1)),' in stage ',num2str(pipelined_realization(l,2))]);
    end
    fpga_ll_cost_add = fpga_ll_cost_add + wordsize + ceil(log2(pipelined_realization(l,1))) + cost_right_shift;
  end
  fpga_ll_cost = fpga_ll_cost_add + fpga_ll_cost_reg;
end
