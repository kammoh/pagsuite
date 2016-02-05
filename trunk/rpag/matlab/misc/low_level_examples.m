realization1 = [5,3,0,1,1;];
realization2 = [5,7,-1,3,-1;];
realization3 = [5,3,2,-7,0;];
realization4 = [5,11,0,-3,1;];
realization5 = [5,13,-1,-3,-1;];

realization = realization5;

global verbose;
verbose = 5;

bw=10;
x_bitwidth=bw;

gate_cost=[13 6 10 26 13];

cost_HA = 26;
cost_FA = 62;
cost_HAb = 26;
cost_INV = 6;

disp(' ');
disp(['realization=',num2str(realization)]);

x = realization(1);
u = abs(realization(2));
v = abs(realization(4));
u_sgn = sign(realization(2));
v_sgn = sign(realization(4));
l1 = realization(3);
l2 = realization(5);

if(l1 == l2)
  r=abs(l1);
  l1=0;
  l2=0;
else
  r = 0;
end

the_oper = [x,r,u_sgn,u,l1,v_sgn,v,l2];

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
disp(['Aksoy''s oper cost=',num2str(oper_cost)]);


disp(['signed result:']);
[N_FA,N_HA,N_HAb,N_INV] = add_sub_ll_components(realization,bw,true);

cost = N_FA*cost_FA+N_HA*cost_HA+N_HAb*cost_HAb+N_INV*cost_INV;
disp(['N_FA=',num2str(N_FA),' N_HA=',num2str(N_HA),' N_HAb=',num2str(N_HAb),' N_INV=',num2str(N_INV)]);
disp(['cost=',num2str(cost)]);

disp(['unsigned result:']);
[N_FA,N_HA,N_HAb,N_INV] = add_sub_ll_components(realization,bw,false);

cost = N_FA*cost_FA+N_HA*cost_HA+N_HAb*cost_HAb+N_INV*cost_INV;
disp(['N_FA=',num2str(N_FA),' N_HA=',num2str(N_HA),' N_HAb=',num2str(N_HAb),' N_INV=',num2str(N_INV)]);
disp(['cost=',num2str(cost)]);
