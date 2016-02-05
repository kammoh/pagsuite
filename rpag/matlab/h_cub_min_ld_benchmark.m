clear;
addpath('/Users/pluto/research/projects/fir_mcm_opt/matlab/common_algorithms');

%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151'};
%benchmark_set = {'AKSOY11_ECCTD_A30','AKSOY11_ECCTD_A80','AKSOY11_ECCTD_A60','AKSOY11_ECCTD_A40','AKSOY11_ECCTD_B80','AKSOY11_ECCTD_A300'};
%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151','AKSOY11_ECCTD_A30','AKSOY11_ECCTD_A80','AKSOY11_ECCTD_A60','AKSOY11_ECCTD_A40','AKSOY11_ECCTD_B80','AKSOY11_ECCTD_A300'};
benchmark_set = {'MIRZAEI10_6'};
plot_results=true; %plot adder graphs?

no_of_runs = 1;

adder_cost = zeros(1,length(benchmark_set)*no_of_runs);
comp_time = zeros(1,length(benchmark_set)*no_of_runs);
realization = cell(0);

%for k=1:length(benchmark_set)
for i=1:length(benchmark_set)
  disp(['testing filter ',benchmark_set{i}]);
  coeff = filtersfromliterature(benchmark_set{i});
  disp(['coefficients=',num2str(coeff)]);
  coeff_fun = setdiff(unique(fundamental(abs(coeff))),[0,1]);
  
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;

    tic();
    if j==1
      [adder_cost(k),realized_coeffs,realization{k}] = vp_mcm(coeff_fun,'nofs',true,'max_depth',0);
    else
      [adder_cost(k),realized_coeffs,realization{k}] = vp_mcm(coeff_fun,'max_depth',0);
    end
    if plot_results
      realization{k} = add_output_coeff(unique(coeff), realization{k})
      plot_realization_graph(realization{k},'name',[benchmark_set{i},'_Hcub_LD_min']);
    end

    comp_time(k) = toc();
  end
end
disp(' ');
disp(['result for Hcub AD min']);
disp('filter & add/sub ops & comp. time');
for i=1:length(benchmark_set)
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;
    disp([num2str(k), ' & ',benchmark_set{i}, ' & ',num2str(adder_cost(k)), ' & ',num2str(comp_time(k),'%.2f')]);
  end
end
disp(['avg.: & ',num2str(mean(adder_cost),'%.2f'), ' & ',num2str(mean(comp_time),'%.2f')]);
