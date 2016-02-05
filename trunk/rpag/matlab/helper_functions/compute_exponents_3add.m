% compute_exponents_3add(A,B,C,D) computes the missing exponents k,l and m
% from fundamentals B, C and D such that
% A = B*2^k +/- C*2^l +/- D*2^m
%
function realization = compute_exponents_3add(A,B,C,D,c_max,all_combinations)

global verbose;

realization = [];

%check the trivial cases A=B, A=C or A=D:
if A==B
  realization = [realization; A,B,0,0,0,0,0];
  return;
end
if A==C
  realization = [realization; A,C,0,0,0,0,0];
  return;
end
if A==D
  realization = [realization; A,D,0,0,0,0,0];
  return;
end

exp_max = ceil(log2((c_max+max([A,B,C,D]))/max(min([A,B,C,D]),1)));

%find concrete value for l, begining with all realizations based on left shifts:
%check n,o>0:
for n=0:exp_max
  for o=0:exp_max
    for cases=1:12
      switch cases
        %cases 1-4: A = B*2^n +/- C*2^o +/- D
        case 1
          B_cur=B; C_cur=C; D_cur=D;
        case 2 
          B_cur=B; C_cur=C; D_cur=-D;
        case 3 
          B_cur=B; C_cur=-C; D_cur=D;
        case 4 
          B_cur=B; C_cur=-C; D_cur=-D;
        %cases 5-8: A = D*2^n +/- C*2^o +/- B
        case 5
          B_cur=D; C_cur=C; D_cur=B;
        case 6 
          B_cur=D; C_cur=C; D_cur=-B;
        case 7 
          B_cur=D; C_cur=-C; D_cur=B;
        case 8 
          B_cur=D; C_cur=-C; D_cur=-B;
        %cases 9-12: A = B*2^n +/- D*2^o +/- C
        case 9
          B_cur=B; C_cur=D; D_cur=C;
        case 10 
          B_cur=B; C_cur=D; D_cur=-C;
        case 11
          B_cur=B; C_cur=-D; D_cur=C;
        case 12
          B_cur=B; C_cur=-D; D_cur=-C;
      end
      A_cur = abs(B_cur*2^n + C_cur*2^o + D_cur);
      [A_cur,r] = fundamental(A_cur);
      if(A == A_cur)
        %correct sign if necesarry:
        if (B_cur*2^n + C_cur*2^o + D_cur)*2^-r == -A
          B_cur=-B_cur; C_cur=-C_cur; D_cur=-D_cur;
        end
        %sort result such that B > C > D
        B_re = B_cur; C_re = C_cur; D_re = D_cur;
        B_sh = n-r; C_sh = o-r; D_sh = -r;
        if C_re < D_re
          tmp = D_re; D_re = C_re; C_re = tmp;
          tmp = D_sh; D_sh = C_sh; C_sh = tmp;
        end
        if B_re < C_re
          tmp = C_re; C_re = B_re; B_re = tmp;
          tmp = C_sh; C_sh = B_sh; B_sh = tmp;
        end
        if C_re < D_re
          tmp = D_re; D_re = C_re; C_re = tmp;
          tmp = D_sh; D_sh = C_sh; C_sh = tmp;
        end
        
        realization = [realization; A,B_re,B_sh,C_re,C_sh,D_re,D_sh];
        if verbose >= 4
          disp(['case ',num2str(cases),':  realization: ',num2str(realization(1)),'=',num2str(realization(2)),'*2^',num2str(realization(3)),' + ',num2str(realization(4)),'*2^',num2str(realization(5)),' + ',num2str(realization(6)),' *2^ ',num2str(realization(7)),' = ',num2str(realization(2)*2^realization(3)+realization(4)*2^realization(5)+realization(6)*2^realization(7))]);
        end
        assert((B_re >= C_re) && (C_re >= D_re));
        assert(realization(1) == abs(realization(2)*2^realization(3)+realization(4)*2^realization(5)+realization(6)*2^realization(7)));
        return;
      end
    end    
  end
end
%error('realization not found!');