function [posodd_input_one,posodd_input_two,oper_cost] = determine_oper_cost(x_bitwidth,input_one,input_two)

the_oper=[];
the_partial=input_one+input_two;

[the_oper(1,2),the_oper(1,1)]=make_number_posodd(the_partial);
if input_one<0
    the_oper(1,3)=-1;
else
    the_oper(1,3)=1;
end
[the_oper(1,5),the_oper(1,4)]=make_number_posodd(input_one);
if input_two<0
    the_oper(1,6)=-1;
else
    the_oper(1,6)=1;
end
[the_oper(1,8),the_oper(1,7)]=make_number_posodd(input_two);

gate_cost=[13 6 10 26 13];

if and(the_oper(1,3)==1,the_oper(1,6)==1)
    if the_oper(1,2)==0
        oper_cost=adder_type_one(x_bitwidth,gate_cost,the_oper);
    else
        oper_cost=adder_type_two(x_bitwidth,gate_cost,the_oper);
    end
else
    if or(and(the_oper(1,3)==1,the_oper(1,5)),and(the_oper(1,6)==1,the_oper(1,8)))
        oper_cost=subtractor_type_one(x_bitwidth,gate_cost,the_oper);
    elseif or(and(the_oper(1,3)==-1,the_oper(1,5)),and(the_oper(1,6)==-1,the_oper(1,8)))
        oper_cost=subtractor_type_two(x_bitwidth,gate_cost,the_oper);
    elseif the_oper(1,2)
        oper_cost=subtractor_type_three(x_bitwidth,gate_cost,the_oper);
    end
end

posodd_input_one=the_oper(1,4);
posodd_input_two=the_oper(1,7);
