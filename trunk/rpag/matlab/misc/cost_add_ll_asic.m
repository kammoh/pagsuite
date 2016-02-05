% A low level ASIC cost model:
function cost = cost_add_ll_asic(x,u,v,bw,c_max,signed)
  cost_HA = 26;
  cost_FA = 62;
  cost_HAb = 26;
  cost_INV = 6;

  realization = compute_exponents(x,u,v,c_max,false);

  [N_FA,N_HA,N_HAb,N_INV] = add_sub_ll_components(realization,bw,signed);
  
  cost = N_FA*cost_FA+N_HA*cost_HA+N_HAb*cost_HAb+N_INV*cost_INV;
  disp(['N_FA=',num2str(N_FA),' N_HA=',num2str(N_HA),' N_HAb=',num2str(N_HAb),' N_INV=',num2str(N_INV)]);
  disp(['cost=',num2str(cost)]);
  