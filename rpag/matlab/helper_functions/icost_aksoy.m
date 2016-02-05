function [adder_cost,reg_cost] = icost_aksoy(x_bitwidth, pipelined_realization)

gate_cost=[13 6 10 26 13];
ff_cost = 52;

pipelined_realization_size = size(pipelined_realization);

adder_cost = 0;
reg_cost = 0;
for i=1:pipelined_realization_size(1)
  coeff = pipelined_realization(i,1);
  if pipelined_realization(i,6) == 0
    %register
    bw = x_bitwidth+ceil(log2(coeff));
    reg_cost = reg_cost + bw*ff_cost;
  else
    %add or sub operation:
    u = abs(pipelined_realization(i,3));
    v = abs(pipelined_realization(i,6));
    u_sgn = sign(pipelined_realization(i,3));
    v_sgn = sign(pipelined_realization(i,6));
    l1 = pipelined_realization(i,5);
    l2 = pipelined_realization(i,8);
    if(l1 == l2)
      r=abs(l1);
      l1=0;
      l2=0;
    else
      r = 0;
    end
%    assert((l1 == 0) || (l2 == 0));
    
    the_oper = [coeff,r,u_sgn,u,l1,v_sgn,v,l2];
    
    assert(coeff == (u_sgn*u*2^l1+v_sgn*v*2^l2)*2^(-r));
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
end
%icost = adder_cost+reg_cost;