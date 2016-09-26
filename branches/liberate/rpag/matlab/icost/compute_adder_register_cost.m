function [adder_cost,register_cost,max_level,register_cnt] = compute_adder_register_cost(x_bitwidth,say_coef,coef_list,say_partial,partial_list,say_oper,oper_list)

adder_cost=0;
register_cost=0;

max_level=0;
oper_list(1,9)=0;
partial_list(2,1)=-1;
partial_list(3,1)=0;

gate_cost=[13 6 10 26 13];

level_determined=1;
while level_determined~=say_partial
    for i=say_oper:-1:1
        if ~oper_list(i,9)
            input_one=oper_list(i,4);
            input_two=oper_list(i,7);
                        
            indis_one=where_is_inside(input_one,say_partial,partial_list);
            if partial_list(2,indis_one)
                if partial_list(2,indis_one)==-1
                    level_one=0;
                else
                    level_one=partial_list(2,indis_one);
                end

                indis_two=where_is_inside(input_two,say_partial,partial_list);
                if partial_list(2,indis_two)
                    if partial_list(2,indis_two)==-1
                        level_two=0;
                    else
                        level_two=partial_list(2,indis_two);
                    end

                    level_determined=level_determined+1;
                    the_level=max(level_one,level_two)+1;
                    oper_list(i,9)=the_level;
                    
                    indis_partial=where_is_inside(oper_list(i,1),say_partial,partial_list);
                    partial_list(2,indis_partial)=the_level;
                    
                    if the_level-1>partial_list(3,indis_one)
                        partial_list(3,indis_one)=the_level-1;
                    end
                    if the_level-1>partial_list(3,indis_two)
                        partial_list(3,indis_two)=the_level-1;
                    end
                    if the_level>max_level
                        max_level=the_level;
                    end
                end
                
            end
        end
    end
end

partial_list(2,1)=0;

for i=1:say_partial
    if is_inside(partial_list(1,i),say_coef,coef_list)
        partial_list(3,i)=max_level;
    end
end

for i=1:say_oper
    the_oper=oper_list(i,1:8);

    % Test, by M. Kumm
    assert(the_oper(1) == (the_oper(3)*the_oper(4)*2^the_oper(5)+the_oper(6)*the_oper(7)*2^the_oper(8))*2^(-the_oper(2)));
    
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

    adder_cost=adder_cost+oper_cost;
end

% 52 stands for the D flip-flop.
register_cnt=0;
for j=partial_list(2,1)+1:1:partial_list(3,1)
    register_cost=register_cost+x_bitwidth*52;
    register_cnt=register_cnt+1; %mod by M. Kumm (count no of registers)
end
for i=2:say_partial
    for j=partial_list(2,i):1:partial_list(3,i)
      register_cost=register_cost+(ceil(log2(partial_list(1,i)))+x_bitwidth)*52;
      register_cnt=register_cnt+1; %mod by M. Kumm (count no of registers)
    end
end
