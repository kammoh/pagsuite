function realization = conv_pipelined_realization_to_realization(pipelined_realization)

realization = pipelined_realization((pipelined_realization(:,6) ~= 0)',:);
realization = unique(realization(:,[1,3,5:6,8]),'rows');
if realization(1,1) == 1
  realization_size = size(realization);
  realization = realization(2:realization_size(1),:);
end
%realization = add_output_coeff(output_coeff, realization);
