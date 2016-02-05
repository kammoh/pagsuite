clc;
clear;

addpath('helper_functions');
addpath('../../../mcm_lut_mult_opt/matlab');
addpath('../../../da_reconf_fir/matlab');

%benchmark_set = {'MIRZAEI10_6','MIRZAEI10_10','MIRZAEI10_13','MIRZAEI10_20','MIRZAEI10_28','MIRZAEI10_41','MIRZAEI10_61','MIRZAEI10_119','MIRZAEI10_151'};
%benchmark_set = {'MIRZAEI10_41','MIRZAEI10_41_alt1','MIRZAEI10_41_alt2','MIRZAEI10_41_alt3','MIRZAEI10_41_alt4','MIRZAEI10_41_alt5','MIRZAEI10_41_alt6','MIRZAEI10_41_alt7','MIRZAEI10_41_alt8','MIRZAEI10_41_alt9','MIRZAEI10_41_alt10'};
benchmark_set = {'MIRZAEI10_41'};


verbose=0;
input_wordsize=12; %input bit width

% cost_model may be 'hl_fpga' (high-level FPGA) or 'hl_asic'
%(high-level ASIC) or 'mcm_min_ad' (MCM Problem with minimal total adder depth)
cost_model = 'hl_fpga'; 
%cost_model = 'll_fpga';
%cost_model = 'hl_asic';
%cost_model = 'll_asic';
%cost_model = 'asic_icost'; %more precise model by Aksoy
%cost_model = 'min_add';

meta_greedy_search_depth = 100;
rand_variance=0;
no_of_runs=1;
%meta_greedy_search_depth = -1;
%rand_variance=1;
%no_of_runs=50;
ternary_adders=true;
sign_correction=false; %!!!
show_adder_graph=true;
rand_seed=-1;%-1

rpag_path = '../bin/';

additional_flags = '';
if ternary_adders
  additional_flags = [additional_flags,' --ternary_adders'];
end

if sign_correction
 additional_flags = [additional_flags,' --sign_correction'];
end

if show_adder_graph
 additional_flags = [additional_flags,' --show_adder_graph'];
end

for i=1:length(benchmark_set)
%  disp(['testing filter ',benchmark_set{i}]);
  coeff = get_benchmark_coefficients(benchmark_set{i});
%  disp(['coefficients=',num2str(coeff)]);
  
  coeff_str = mat2str(unique(fundamental(coeff)));
  coeff_str = coeff_str(2:end-1);
  
  cmd_line_str = ['rpag',' --verbose=',num2str(verbose),' --matlab_out_address=',num2str(i),' --rand_variance=',num2str(rand_variance),' --no_of_runs=',num2str(no_of_runs),' --rand_seed=',num2str(rand_seed),' --input_wordsize=',num2str(input_wordsize),' --meta_greedy_search_depth=',num2str(meta_greedy_search_depth),' --cost_model=',cost_model,additional_flags,' ',coeff_str];
%  disp(cmd_line_str);
  eval(['!',rpag_path,cmd_line_str]);
end