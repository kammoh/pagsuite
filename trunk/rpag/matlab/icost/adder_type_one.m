function oper_cost = adder_type_one(x_bitwidth,gate_cost,the_oper)

oper_cost=0;
the_partial=the_oper(1,1);
digit_partial=ceil(log2(the_partial))+x_bitwidth;

the_partials=zeros(2,digit_partial);

if the_oper(1,5)==0
    partial_one=the_oper(1,4);
    digit_one=ceil(log2(partial_one))+x_bitwidth;
    for i=1:digit_one
        the_partials(1,i)=1;
    end
    
    the_shift=the_oper(1,8);
    partial_two=the_oper(1,7);
    digit_two=ceil(log2(partial_two))+x_bitwidth;
    for i=the_shift+1:1:digit_two+the_shift
        the_partials(2,i)=2;
    end
else
    partial_one=the_oper(1,7);
    digit_one=ceil(log2(partial_one))+x_bitwidth;
    for i=1:digit_one
        the_partials(1,i)=1;
    end
    
    the_shift=the_oper(1,5);
    partial_two=the_oper(1,4);
    digit_two=ceil(log2(partial_two))+x_bitwidth;
    for i=the_shift+1:1:digit_two+the_shift
        the_partials(2,i)=2;
    end
end

% fprintf(fid_blif,'\n');
% fprintf(fid_blif,'# %d>>0 = +%d<<0 +%d<<%d \n',the_partial,partial_one,partial_two,the_shift);
% fprintf(fid_blif,'\n');

equal_count=0;
if max(digit_one,digit_two+the_shift)==digit_partial
    equal_count=1;
end

carry_count=0;
digit_count_one=0;
digit_count_two=0;
digit_count_partial=0;

say_ha=0;
say_fa=0;

final_add=0;
shift_over=0;
ha_reached=0;

for i=1:digit_partial
    if and(and(the_partials(1,i)==1,the_partials(2,i)==0),shift_over==0)
%        disp(['bit ',num2str(i), ': wire found']);
        oper_cost=oper_cost+0;
    elseif and(and(the_partials(1,i)==0,the_partials(2,i)==0),shift_over==0)
        oper_cost=oper_cost+0;
    elseif and(and(the_partials(1,i)==0,the_partials(2,i)==0),final_add==1)
        oper_cost=oper_cost+0;
    elseif and(and(the_partials(1,i)==0,the_partials(2,i)==2),and(shift_over==1,ha_reached==0))
        oper_cost=oper_cost+0;
    elseif and(and(the_partials(1,i)==1,the_partials(2,i)==2),ha_reached==0)
%        disp(['bit ',num2str(i), ': HA found']);
        ha_reached=1;
        oper_cost=oper_cost+gate_cost(1,1)+gate_cost(1,5);
    elseif and(and(the_partials(1,i)==1,the_partials(2,i)==2),ha_reached==1)
%        disp(['bit ',num2str(i), ': FA found']);
        oper_cost=oper_cost+2*gate_cost(1,1)+gate_cost(1,3)+2*gate_cost(1,5);
    elseif and(and(the_partials(1,i)==0,the_partials(2,i)==2),ha_reached==1)
%        disp(['bit ',num2str(i), ': HA found']);
        oper_cost=oper_cost+gate_cost(1,1)+gate_cost(1,5);
    elseif and(and(the_partials(1,i)==1,the_partials(2,i)==0),ha_reached==1)
        oper_cost=oper_cost+gate_cost(1,1)+gate_cost(1,5);
    elseif and(and(the_partials(1,i)==0,the_partials(2,i)==0),and(final_add==0,ha_reached==1))
        oper_cost=oper_cost+0;
    end
    
    if i==the_shift
        shift_over=1;
    end
end
