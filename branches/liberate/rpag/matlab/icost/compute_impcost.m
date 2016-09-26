clear;

addpath('/Users/pluto/research/projects/fir_mcm_opt/matlab/common_algorithms');

%files_mcm={'fir01_ilp.result','fir02_ilp.result','fir03_ilp.result','fir04_ilp.result','fir05_ilp.result','fir06_ilp.result'};
files_mcm={'fir01_ilp.result'};

adder_cnt_all = zeros(1,length(files_mcm));
register_cnt_all = zeros(1,length(files_mcm));
total_cost_all = zeros(1,length(files_mcm));

for k=1:length(files_mcm)

x_bitwidth=16;
file_mcm=files_mcm{k};%'fir01_ilp.result';
algo_name=0;

%x_bitwidth=input('Enter the bit-width of the input variable: ');
%file_mcm=input('Enter the filename where the MCM operation is described: ','s');
%algo_name=input('Enter the format used in describing the MCM operation (0) Ours (1) RAG-n/Hcub:  ');
%if algo_name
%    file_coef=input('Enter the filename where coefficients exist: ','s');
%end

file_name='';
for i=1:length(file_mcm)
    if file_mcm(1,i)~='.'
        file_name=[file_name,file_mcm(1,i)];
    else
        break
    end
end

if ~algo_name
    [is_checked,say_list,say_partial,say_oper,coef_list,partial_list,oper_list]=file_read_ourmcm(file_mcm);
else
    [is_checked,say_list,say_partial,say_oper,coef_list,partial_list,oper_list]=file_read_theirmcm(file_mcm,file_coef);
end

[adder_cost,register_cost,max_level,register_cnt]=compute_adder_register_cost(x_bitwidth,say_list,coef_list,say_partial,partial_list,say_oper,oper_list);

d_cost=register_cost/52;
total_cost=adder_cost+register_cost;

fprintf('\n');
fprintf('*** Filter %s ***\n',file_mcm);
fprintf('* Number of operations: %d\n',say_oper);
fprintf('* Number of pipeline registers: %d\n',register_cnt);
fprintf('* Maximum number of adder-steps: %d\n',max_level);
fprintf('* Number of D flip-flops: %d\n',d_cost);
fprintf('* Implementation cost of operations: %d\n',adder_cost);
fprintf('* Implementation cost of pipeline registers: %d\n',register_cost);
fprintf('* Total implementation cost: %d\n',total_cost);
fprintf('\n');

adder_cnt_all(k) = say_oper;
register_cnt_all(k) = register_cnt;
total_cost_all(k) = total_cost;

end

disp('filter & registered add/sub ops & pure registers & registered ops & icost');
for k=1:length(files_mcm)
  disp([files_mcm{k}, ' & ',num2str(adder_cnt_all(k)),' & ',num2str(register_cnt_all(k)-adder_cnt_all(k)),' & ',num2str(register_cnt_all(k)),' & ',num2str(total_cost_all(k)),'\\']);
end
disp(['avg.: ',num2str(mean(adder_cnt_all),'%.2f'), ' & ',num2str(mean(register_cnt_all-adder_cnt_all),'%.2f'), ' & ',num2str(mean(register_cnt_all),'%.2f'),' & ',num2str(mean(total_cost_all),'%.2f')]);
