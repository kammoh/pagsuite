% add_sub_icost(realization,bw)
% computes the low-level implementation cost for a *single* realization
% using the methods provided by Aksoy.

function icost = add_sub_icost(realization,bw)

global verbose;

gate_cost=[13 6 10 26 13];

x_bitwidth=bw;
x = realization(1);
u = abs(realization(2));
v = abs(realization(4));
u_sgn = sign(realization(2));
v_sgn = sign(realization(4));
l1 = realization(3);
l2 = realization(5);

if (l1 < 0) && (l2 < 0)
  assert(l1 == l2);
  r = abs(l1);
  l1 = 0;
  l2 = 0;
else
  r = 0;
end

the_oper = [x,r,u_sgn,u,l1,v_sgn,v,l2];

assert(x == (u_sgn*u*2^l1+v_sgn*v*2^l2)*2^(-r));
assert(the_oper(1) == (the_oper(3)*the_oper(4)*2^the_oper(5)+the_oper(6)*the_oper(7)*2^the_oper(8))*2^(-the_oper(2)));

if and(the_oper(1,3)==1,the_oper(1,6)==1)
    if the_oper(1,2)==0
        icost=adder_type_one(x_bitwidth,gate_cost,the_oper);
    else
        icost=adder_type_two(x_bitwidth,gate_cost,the_oper);
    end
else
    if or(and(the_oper(1,3)==1,the_oper(1,5)),and(the_oper(1,6)==1,the_oper(1,8)))
        icost=subtractor_type_one(x_bitwidth,gate_cost,the_oper);
    elseif or(and(the_oper(1,3)==-1,the_oper(1,5)),and(the_oper(1,6)==-1,the_oper(1,8)))
        icost=subtractor_type_two(x_bitwidth,gate_cost,the_oper);
    elseif the_oper(1,2)
        icost=subtractor_type_three(x_bitwidth,gate_cost,the_oper);
    end
end
