function oper_cost = subtractor_type_one(x_bitwidth,gate_cost,the_oper)

oper_cost=0;
the_partial=the_oper(1,1);
digit_partial=ceil(log2(the_partial))+x_bitwidth;

if the_oper(1,3)==1
    the_shift=the_oper(1,5);
    partial_one=the_oper(1,4);
    digit_one=ceil(log2(partial_one))+x_bitwidth;

    partial_two=the_oper(1,7);
    digit_two=ceil(log2(partial_two))+x_bitwidth;
else
    the_shift=the_oper(1,8);
    partial_one=the_oper(1,7);
    digit_one=ceil(log2(partial_one))+x_bitwidth;

    partial_two=the_oper(1,4);
    digit_two=ceil(log2(partial_two))+x_bitwidth;
end

% fprintf(fid_blif,'\n');
% fprintf(fid_blif,'# %d>>0 = +%d<<%d -%d<<0 \n',the_partial,partial_one,the_shift,partial_two);
% fprintf(fid_blif,'\n');

the_partials=zeros(2,max(digit_one+the_shift,digit_two));

for i=the_shift+1:1:digit_one+the_shift
    the_partials(1,i)=1;
end

for i=1:digit_two
    the_partials(2,i)=2;
end

for i=digit_two+1:1:digit_one+the_shift
    the_partials(2,i)=-1;
end

if digit_one+the_shift<digit_two
    the_oper
    fprintf('There is a misunderstanding on the size of partials\n');
    fprintf('\n');
    fprintf('# %d>>0 = +%d<<%d -%d<<0 \n',the_partial,partial_one,the_shift,partial_two);
    fprintf('\n');
    the_partials
    pause
end

carry_count=0;
digit_count_one=0;
digit_count_two=0;
digit_count_partial=0;

for i=1:digit_partial
    if i==1
        oper_cost=oper_cost+gate_cost(1,2);
    elseif and(the_partials(1,i)==0,the_partials(2,i)==-1)
        oper_cost=oper_cost+gate_cost(1,2);
    elseif and(the_partials(1,i)==0,the_partials(2,i)==2)
        oper_cost=oper_cost+gate_cost(1,1)+gate_cost(1,2)+gate_cost(1,5);
    elseif and(the_partials(1,i)==1,the_partials(2,i)==2)
        oper_cost=oper_cost+2*gate_cost(1,1)+gate_cost(1,2)+gate_cost(1,3)+2*gate_cost(1,5);
    elseif and(the_partials(1,i)==1,the_partials(2,i)==-1)
        oper_cost=oper_cost+gate_cost(1,3)+gate_cost(1,4);
    end
end
