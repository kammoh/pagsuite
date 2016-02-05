% add_sub_ll_components(realization,bw,signed)
% computes the low-level components of a addition or subtraction for a
% given *single* realization vector (no matrix!)
%
% The components are:
% N_FA:  No of full adders 
% N_HA:  No of half adders 
% N_HAb: No of full adders with one input stuck to '1'
% N_INV: No of inverters

function [N_FA,N_HA,N_HAb,N_INV] = add_sub_ll_components(realization,bw,signed)

  global verbose;
  
  u = realization(2);
  v = realization(4);
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
  
  n_u = bw+ceil(log2(abs(u)));
  n_v = bw+ceil(log2(abs(v)));
  n_m = min(n_u+l1,n_v+l2);
  n_M = max(n_u+l1,n_v+l2);
  
  if verbose >= 5
    disp(['n_u=',num2str(n_u),' n_v=',num2str(n_v)]);
    disp(['n_m=',num2str(n_m),' n_M=',num2str(n_M)]);
  end

  if (u > 0) && (v > 0)
    %addition
    if r == 0
      %addition case 1:
      if verbose >= 4
        disp('addition case 1');
      end
      if signed
        N_FA = n_M - l2 - 1;
        N_HA = 1;
      else
        N_FA = n_m - l2 - 1;
        N_HA = n_M - n_m + 1;
      end
      N_HAb = 0;
      N_INV = 0;
    else
      %addition case 2:
      if verbose >= 4
        disp('addition case 2');
      end
      if signed
        N_FA = n_M - r;
        N_HA = 0;
      else
        N_FA = n_m - r;
        N_HA = n_M - n_m;
      end
      N_HAb = 0;
      N_INV = 0;
    end
  else
    %subtraction
    if l1 > 0
      %subtraction case 1:
      if verbose >= 4
        disp('subtraction case 1');
      end
      if signed
        N_FA = n_u;
        N_HA = l1-1;
        N_HAb = 0;
        N_INV = n_v;
      else
        N_FA = max(l1 , n_v) - l1;
        N_HA = min(l1 , n_v) - 1;
        N_HAb = n_u + min(l1-n_v,0);
        N_INV = max(l1,n_v);
      end
    elseif l2 > 0
      %subtraction case 2:
      if verbose >= 4
        disp('subtraction case 2');
      end
      if signed
        N_FA = n_u - l2 - 1;
        N_HA = 0;
        N_HAb = 1;
        N_INV = n_v;
      else
        N_FA = n_v - 1;
        N_HA = 0;
        N_HAb = n_u - n_v - l2 + 1;
        N_INV = n_v;
      end
    elseif r > 0
      %subtraction case 3:
      if verbose >= 4
        disp('subtraction case 3');
      end
      if signed
        N_FA = n_u - r - 1;
        N_HA = 0;
        N_HAb = 1;
        N_INV = n_v - r;
      else
        N_FA = n_v - r - 1;
        N_HA = 0;
        N_HAb = n_u - n_v + 1;
        N_INV = n_v - r;
      end
    else
      error('This case does not exist!');
    end
  end