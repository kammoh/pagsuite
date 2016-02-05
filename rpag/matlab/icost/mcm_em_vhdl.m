clear;

% algo_name=1;
% x_bitwidth=16;

% for arch_type=1:1
%     for dave=1:9
%         file_mcm=['fir0',num2str(dave),'_ragn.solution'];
%         if algo_name
%             file_coef=['fir0',num2str(dave),'.coef'];
%         end

file_mcm=input('Enter the MCM file: ','s');
x_bitwidth=input('Enter the bit-width of the input: ');
algo_name=input('Enter the algorithm used to find the solution (0) Ours (1) RAG-n/Hcub:  ');
if algo_name
    file_coef=input('Enter the filename where coefficients exist: ','s');
end

file_name='';
for i=1:length(file_mcm)
    if file_mcm(1,i)~='.'
        file_name=[file_name,file_mcm(1,i)];
    else
        break
    end
end

if ~algo_name
    [is_checked,say_list,say_array,say_partial,say_inv,say_oper,coef_list,coef_array,partial_list,inv_list,oper_line,oper_list,max_level]=file_read_ourmcm(file_mcm);
else
    [is_checked,say_list,say_array,say_partial,say_inv,say_oper,coef_list,coef_array,partial_list,inv_list,oper_line,oper_list,max_level]=file_read_theirmcm(file_mcm,file_coef);
end

file_write_sa_vhdl(x_bitwidth,file_mcm,file_name,say_list,coef_list,say_oper,oper_line,oper_list,say_partial,partial_list,max_level);
file_write_sa_testbench(x_bitwidth,file_name,say_list,coef_list);
file_write_sa_rtlscript(file_name);
file_write_sa_tcfscript(file_name);

% end
% end
