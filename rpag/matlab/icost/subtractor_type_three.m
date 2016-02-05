function oper_cost = subtractor_type_three(x_bitwidth,gate_cost,the_oper)

oper_cost=0;
the_shift=the_oper(1,2);
the_partial=the_oper(1,1);
digit_partial=ceil(log2(the_partial))+x_bitwidth;

if the_oper(1,3)==1
    partial_one=the_oper(1,4);
    digit_partial_one=ceil(log2(partial_one))+x_bitwidth;

    partial_two=the_oper(1,7);
    digit_partial_two=ceil(log2(partial_two))+x_bitwidth;
elseif the_oper(1,3)==-1
    partial_one=the_oper(1,7);
    digit_partial_one=ceil(log2(partial_one))+x_bitwidth;

    partial_two=the_oper(1,4);
    digit_partial_two=ceil(log2(partial_two))+x_bitwidth;
end

% fprintf(fid_blif,'\n');
% fprintf(fid_blif,'# %d>>%d = +%d<<0 -%d<<0 \n',the_partial,the_shift,partial_one,partial_two);
% fprintf(fid_blif,'\n');

the_partials=zeros(2,digit_partial_one);

if digit_partial+the_shift>digit_partial_one
    fprintf('\n');
    fprintf('It seems there is a misunderstanding in A<<0-B<<0 \n');
    fprintf('\n');
end

for i=1:digit_partial_one
    the_partials(1,i)=1;
end

for i=1:digit_partial_two
    the_partials(2,i)=2;
end

for i=digit_partial_two+1:1:digit_partial_one
    the_partials(2,i)=-1;
end

carry_count=0;
digit_count_partial=0;
digit_count_one=the_shift;
digit_count_two=the_shift;

for i=the_shift+1:1:digit_partial+the_shift
    if i==the_shift+1
        oper_cost=oper_cost+gate_cost(1,2)+gate_cost(1,3)+gate_cost(1,4);
    elseif and(the_partials(1,i)==1,the_partials(2,i)==2)
        oper_cost=oper_cost+2*gate_cost(1,1)+gate_cost(1,2)+gate_cost(1,3)+2*gate_cost(1,5);
    elseif and(the_partials(1,i)==1,the_partials(2,i)==-1)
        oper_cost=oper_cost+gate_cost(1,3)+gate_cost(1,4);
    end
end
