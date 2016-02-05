clear
%clc
addpath('rpag');
addpath('icost');
addpath('helper_functions');
addpath('../../mcm_lut_mult_opt/matlab');
addpath('../../pipelined_mcm_opt/matlab/common_algorithms');

%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151'};
%benchmark_set = {'AKSOY11_ECCTD_A30','AKSOY11_ECCTD_A80','AKSOY11_ECCTD_A60','AKSOY11_ECCTD_A40','AKSOY11_ECCTD_B80','AKSOY11_ECCTD_A300'};
%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151','AKSOY11_ECCTD_A30','AKSOY11_ECCTD_A80','AKSOY11_ECCTD_A60','AKSOY11_ECCTD_A40','AKSOY11_ECCTD_B80','AKSOY11_ECCTD_A300'};
%benchmark_set = {'gaussian_3x3_8bit','gaussian_5x5_12bit','laplacian_3x3_8bit','unsharp_3x3_8bit','unsharp_3x3_12bit','lowpass_5x5_8bit','lowpass_9x9_10bit','lowpass_15x15_12bit','highpass_5x5_8bit','highpass_9x9_10bit','highpass_15x15_12bit'};
%benchmark_set = {'gaussian_5x5_12bit'};
%benchmark_set = {'test1','test2','test3'};
%benchmark_set = {'sqrt2_double'};
benchmark_set = {'MIRZAEI10_6'};

% a global verbose level is used by functions to surpress to much
% information (dirty hack but works well :)
% verbose values:
% 0:  no output during optimization
% 1:  print predecessor values and results
% 2:  print details of optimization decisions during optimization
% >2: print all debug infos

global verbose; 
verbose=2;


bw=12; %input bit width
%bw=8; %input bit width

% cost_model may be 'hl_fpga' (high-level FPGA) or 'hl_asic'
%(high-level ASIC) or 'mcm_min_ad' (MCM Problem with minimal total adder depth)
cost_model = 'hl_fpga'; 
%cost_model = 'hl_asic';
%cost_model = 'll_asic';
%cost_model = 'll_fpga';
%cost_model = 'asic_icost'; %more precise model by Aksoy
%cost_model = 'mcm_min_ad';

exhaustive = false;
rand_variance=1;%1;
no_of_runs=2;

plot_best_ll_fpga_solution=false; %plot pipelined adder graphs?
plot_best_hl_fpga_solution=false; %plot pipelined adder graphs?
plot_all_results=false; %plot pipelined adder graphs?

registers = zeros(1,length(benchmark_set)*no_of_runs); %stores register count
add_sub_ops = zeros(1,length(benchmark_set)*no_of_runs); %stores add/sub
comp_time = zeros(1,length(benchmark_set)*no_of_runs); %stores computation time
icost = zeros(1,length(benchmark_set)*no_of_runs); %stores Aksoy's icost value
solution = cell(1,length(benchmark_set)*no_of_runs); %stores solutions
fpga_ll_cost = zeros(1,length(benchmark_set)*no_of_runs); %stores FPGA low level costs

for i=1:length(benchmark_set)
  disp(['testing filter ',benchmark_set{i}]);
  coeff = get_benchmark_coefficients(benchmark_set{i});
  disp(['coefficients=',num2str(coeff)]);
  
  decisions = cell(0);
  tic_all_runs=tic();
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;
    if j==1
      rand_variance_run = 0;
    else
      rand_variance_run = rand_variance;
    end
    if rand_variance_run ~= 0
      decisions = cell(0); %reset decisions in random mode
    end
    [solution{k},comp_time(k),decisions] = rpag(coeff,cost_model,bw,exhaustive,rand_variance_run,decisions);
%        disp(['decisions=',num2str(cell2mat(decisions))]);
    if rand_variance_run == 0
      if ~isempty(decisions)
        decision_index = mod(j-1,length(decisions))+1;
        decision_strength = ceil(j/length(decisions));
        decisions = num2cell(zeros(1,length(decisions)));
        decisions{decision_index} = decisions{decision_index} + decision_strength;
      end
    end
    %all unique elements except 1 has to be computed using add/sub:
    add_sub_ops(k) = length(setdiff(unique(cell2mat(solution{k})),1));
    %The sum of number of elements in all sets is equal to the register count
    registers(k) = length(cell2mat(solution{k}));

%    disp(['add/sub ops=',num2str(add_sub_ops(k))]);
%    disp(['registers=',num2str(registers(k))]);


    pipelined_realization = conv_pipeline_sets_to_pipelined_realization(solution{k},coeff);
    [icost_add(k),icost_reg(k)] = icost_aksoy(bw,pipelined_realization);
    %compute FPGA LL cost:
    fpga_ll_cost(k) = fpga_ll_cost_pipelined_realization(pipelined_realization,bw);

    if plot_all_results
      plot_pipelined_realization_graph(pipelined_realization,'name',[benchmark_set{i},'_',cost_model,'_',num2str(j)]);
    end
%    disp([benchmark_set{i}, ' & ',num2str(icost_add(k)+icost_reg(k)), ' & ',num2str(icost_add(k)), ' & ',num2str(icost_reg(k)), ' & ',num2str(add_sub_ops(k)), ' & ',num2str(registers(k)-add_sub_ops(k)), ' & ',num2str(registers(k)), ' & ',num2str(comp_time(k),'%.2f'),' & ',num2str(cell2mat(decisions))]);
    disp([str2str_fl(benchmark_set{i},20), ' & ',num2str_fl(add_sub_ops(k),3), ' & ',num2str_fl(registers(k)-add_sub_ops(k),3), ' & ',num2str_fl(registers(k),3), ' & ',num2str_fl(fpga_ll_cost(k),3),' & ',num2str(comp_time(k),'%.2f'),'\\']);
  end
  disp('time for all runs:');
  toc(tic_all_runs)
end


% latex output:
rpag_benchmark_latex_output

