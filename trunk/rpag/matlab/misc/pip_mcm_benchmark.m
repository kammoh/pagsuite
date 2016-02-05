clear
%clc
addpath('../common_algorithms');
addpath('../common_algorithms/csd_lib');
addpath('pip_mcm1');
addpath('pip_mcm2');
addpath('pip_mcm3');
addpath('pip_mcm4');
addpath('pip_mcm5');

%benchmark_set = {'AKSOY08_NORCHIP_K25','AKSOY08_NORCHIP_C30','AKSOY08_NORCHIP_A40','AKSOY08_NORCHIP_E40','AKSOY08_NORCHIP_G40','AKSOY08_NORCHIP_I40','AKSOY08_NORCHIP_H60','AKSOY08_NORCHIP_J60','AKSOY08_NORCHIP_B80','AKSOY08_NORCHIP_D80','AKSOY08_NORCHIP_F80'};
%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151'};
%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151','AKSOY08_NORCHIP_K25','AKSOY08_NORCHIP_C30','AKSOY08_NORCHIP_A40','AKSOY08_NORCHIP_E40','AKSOY08_NORCHIP_G40','AKSOY08_NORCHIP_I40','AKSOY08_NORCHIP_H60','AKSOY08_NORCHIP_J60','AKSOY08_NORCHIP_B80','AKSOY08_NORCHIP_D80','AKSOY08_NORCHIP_F80'};
%benchmark_set = {'MIRZAEI10_6'};
benchmark_set = {'AKSOY11_A30','AKSOY11_A80','AKSOY11_A60','AKSOY11_A40','AKSOY11_B80','AKSOY11_A300'};
%benchmark_set = {'test'};
%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151','AKSOY11_A30','AKSOY11_A80','AKSOY11_A60','AKSOY11_A40','AKSOY11_B80','AKSOY11_A300'};

bw=16; %bit width

algorithm = 'pip_mcm5'; %name of algorithm for output file name
mode='B'; %mode (may be 'A' or 'B')

% cost_model may be 'hl_fpga' (high-level FPGA) or 'hl_asic'
%(high-level ASIC) or 'mcm_min_ad' (MCM Problem with minimal adder depth)
%cost_model = 'hl_fpga'; 
%cost_model = 'hl_asic';
cost_model = 'll_asic';
%cost_model = 'mcm_min_ad';

exhaustive = false;
rand_variance=1;%0.75;
no_of_runs=50;

plot_results=false; %plot pipelined adder graphs?

% a global verbose level is used by functions to surpress to much
% information (dirty hack but works well :)
% verbose values:
% 0:  no output during optimization
% 1:  print predecessor values and results
% 2:  print details of optimization decisions during optimization
% >2: print all debug infos

global verbose; 
verbose=0;

op_cnt = zeros(1,length(benchmark_set)*no_of_runs); %stores registers operations (add, sub, register)
registers = zeros(1,length(benchmark_set)*no_of_runs); %stores register count
add_sub_ops = zeros(1,length(benchmark_set)*no_of_runs); %stores add/sub
comp_time = zeros(1,length(benchmark_set)*no_of_runs); %stores computation time
icost = zeros(1,length(benchmark_set)*no_of_runs); %stores Aksoy's icost value
solution = cell(1,length(benchmark_set)*no_of_runs); %stores solutions

for i=1:length(benchmark_set)
  disp(['testing filter ',benchmark_set{i}]);
  if strcmp(benchmark_set{i},'test')
    coeff = round(rand(1,3)*1000);
%    coeff = [819,728,176;];
%    coeff = [360,189,1];
%    coeff = [1*2^7,19*2^2,43*2^1];
%    coeff = [7*2^3,19*2^2,43*2^1];
    coeff = [11*2^2,65*2^1,43*2^2];
  else
    coeff = filtersfromliterature(benchmark_set{i});
  end
  disp(['coefficients=',num2str(coeff)]);
  
  decisions = cell(0);
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;
    switch algorithm
      case 'pip_mcm1'
        [op_cnt(k),solution{k},comp_time(k)] = pip_mcm1(coeff,mode);
      case 'pip_mcm2'
        [op_cnt(k),solution{k},comp_time(k)] = pip_mcm2(coeff,mode);
      case 'pip_mcm3'
        [op_cnt(k),solution{k},comp_time(k)] = pip_mcm3(coeff,mode);
      case 'pip_mcm4'
        [op_cnt(k),solution{k},comp_time(k)] = pip_mcm4(coeff,mode,cost_model);
      case 'pip_mcm5'
        if j==1
          rand_variance_run = 0;
        else
        	rand_variance_run = rand_variance;
        end
        if rand_variance_run ~= 0
          decisions = cell(0); %reset decisions in random mode
        end
        [op_cnt(k),solution{k},comp_time(k),decisions] = pip_mcm5(coeff,bw,mode,cost_model,exhaustive,rand_variance_run,decisions);
%        disp(['decisions=',num2str(cell2mat(decisions))]);
        if rand_variance_run == 0
          if ~isempty(decisions)
            decision_index = mod(j-1,length(decisions))+1;
            decision_strength = ceil(j/length(decisions));
            decisions = num2cell(zeros(1,length(decisions)));
            decisions{decision_index} = decisions{decision_index} + decision_strength;
          end
        end
      otherwise
        error(['algorithm ',algorithm,' undefined']);
    end
    %all unique elements except 1 has to be computed using add/sub:
    add_sub_ops(k) = length(setdiff(unique(cell2mat(solution{k})),1));
    %The sum of number of elements in all sets is equal to the register count
    registers(k) = length(cell2mat(solution{k}));

%    disp(['add/sub ops=',num2str(add_sub_ops(k))]);
%    disp(['registers=',num2str(registers(k))]);


    pipelined_realization = conv_pipeline_sets_to_pipelined_realization(solution{k},coeff);
    [icost_add(k),icost_reg(k)] = aksoy_icost(bw,pipelined_realization);
    if plot_results
      plot_pipelined_realization_graph(pipelined_realization,'name',[benchmark_set{i},'_',algorithm,mode,'_',cost_model,'_',num2str(j)]);
    end
    disp([benchmark_set{i}, ' & ',num2str(icost_add(k)+icost_reg(k)), ' & ',num2str(icost_add(k)), ' & ',num2str(icost_reg(k)), ' & ',num2str(add_sub_ops(k)), ' & ',num2str(registers(k)-add_sub_ops(k)), ' & ',num2str(registers(k)), ' & ',num2str(comp_time(k),'%.2f'),' & ',num2str(cell2mat(decisions))]);
  end
end

% latex output:
disp(' ');
disp(['result for ',algorithm,' mode ',mode,' ',cost_model]);
disp('filter & icost & icost add & icost reg & registered add/sub ops & pure registers & registered ops & comp. time');

for i=1:length(benchmark_set)
  for j=1:no_of_runs
    k=(i-1)*no_of_runs+j;

%for k=1:length(benchmark_set)
    disp([benchmark_set{i}, ' & ',num2str(icost_add(k)+icost_reg(k)), ' & ',num2str(icost_add(k)), ' & ',num2str(icost_reg(k)), ' & ',num2str(add_sub_ops(k)), ' & ',num2str(registers(k)-add_sub_ops(k)), ' & ',num2str(registers(k)), ' & ',num2str(comp_time(k),'%.2f')]);
  end
end
disp(['avg.: ',num2str(mean(icost_add+icost_reg),'%.2f'), ' & ',num2str(mean(icost_add),'%.2f'), ' & ',num2str(mean(icost_reg),'%.2f'), ' & ',num2str(mean(add_sub_ops),'%.2f'), ' & ',num2str(mean(registers-add_sub_ops),'%.2f'),' & ',num2str(mean(registers),'%.2f'), ' & ',num2str(mean(comp_time),'%.2f')]);

save(['results_',algorithm,'_',mode,'.mat'],'solution','add_sub_ops','registers','comp_time','icost_add','icost_reg');
