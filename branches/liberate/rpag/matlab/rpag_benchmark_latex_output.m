% latex output:
disp(' ');
disp(['*** optimization results for model ',cost_model,' ***']);
disp('inst & filter & registered add/sub ops & pure registers & registered ops & FPGA ll costs & comp. time');
best_add_index = zeros(1,length(benchmark_set));
best_reg_ops_index = zeros(1,length(benchmark_set));
best_fpga_ll_cost_index = zeros(1,length(fpga_ll_cost));

algorithm='rpag';

for i=1:length(benchmark_set)
  add_sub_ops_best = Inf;
  best_reg_ops_best = Inf;
  best_fpga_ll_cost = Inf;
  
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;

    if add_sub_ops(k) < add_sub_ops_best
      add_sub_ops_best = add_sub_ops(k);
      best_add_index(i) = k;
    end
    if registers(k)+add_sub_ops(k) < best_reg_ops_best
      best_reg_ops_best = registers(k)+add_sub_ops(k);
      best_reg_ops_index(i) = k;
    end
    if fpga_ll_cost(k) < best_fpga_ll_cost
      best_fpga_ll_cost = fpga_ll_cost(k);
      best_fpga_ll_cost_index(i) = k;
    end

    disp([num2str(k), ' & ',benchmark_set{i}, ' & ', ' & ',num2str(add_sub_ops(k)), ' & ',num2str(registers(k)-add_sub_ops(k)), ' & ',num2str(registers(k)), ' & ',num2str(fpga_ll_cost(k)), ' & ',num2str(comp_time(k),'%.2f')]);
  end
end

disp(' ');
disp(['*** solutions: ***']);
for i=1:length(benchmark_set)
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;
    disp(['solution{''',num2str(k),'''}=',cell2str(solution{k}),';']);
  end
end

disp(['avg.: & ',num2str(mean(add_sub_ops),'%.2f'), ' & ',num2str(mean(registers-add_sub_ops),'%.2f'),' & ',num2str(mean(registers),'%.2f'), ' & ',num2str(mean(fpga_ll_cost)), ' & ',num2str(mean(comp_time),'%.2f')]);


disp(' ');
disp(['best FPGA hl solutions:']);
for i=1:length(benchmark_set)
  k = best_reg_ops_index(i);
  disp([algorithm,'_solution_hl_fpga(''',benchmark_set{i},''')=',cell2str(solution{k}),';']);
  if plot_best_hl_fpga_solution
    pipelined_realization = conv_pipeline_sets_to_pipelined_realization(solution{k},coeff);
    plot_pipelined_realization_graph(pipelined_realization,'name',[benchmark_set{i},'_',cost_model,'_',num2str(j)]);
  end
end

disp(' ');
disp(['best FPGA ll solutions:']);
for i=1:length(benchmark_set)
  k = best_fpga_ll_cost_index(i);
  disp([algorithm,'_solution_ll_fpga(''',benchmark_set{i},''')=',cell2str(solution{k}),';']);
  if plot_best_ll_fpga_solution
    pipelined_realization = conv_pipeline_sets_to_pipelined_realization(solution{k},coeff);
    plot_pipelined_realization_graph(pipelined_realization,'name',[benchmark_set{i},'_',cost_model,'_',num2str(j)]);
  end
end

disp(' ');
disp(['best FPGA hl results:']);
for i=1:length(benchmark_set)
  k = best_reg_ops_index(i);
  disp([num2str_fl(k,3), ' & ',str2str_fl(benchmark_set{i},20), ' & ',num2str_fl(add_sub_ops(k),3), ' & ',num2str_fl(registers(k)-add_sub_ops(k),3), ' & ',num2str_fl(registers(k),3), ' & ',num2str_fl(fpga_ll_cost(k),3),' & ',num2str(comp_time(k),'%.2f'),'\\']);
end
disp(' ');

disp(['best FPGA ll results:']);
for i=1:length(benchmark_set)
  k = best_fpga_ll_cost_index(i);
  disp([num2str_fl(k,3), ' & ',str2str_fl(benchmark_set{i},20), ' & ',num2str_fl(add_sub_ops(k),3), ' & ',num2str_fl(registers(k)-add_sub_ops(k),3), ' & ',num2str_fl(registers(k),3), ' & ',num2str_fl(fpga_ll_cost(k),3),' & ',num2str(comp_time(k),'%.2f'),'\\']);
end



