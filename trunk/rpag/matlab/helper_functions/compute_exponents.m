% compute_exponents(A,B,C) computes the missing exponents m and n
% from fundamentals A,B and C such that
% A = B*2^m +/- C*2^n
%
% the sign of C is corrected to match above equation

function realization = compute_exponents(A,B,C,c_max,all_combinations,all_realizations)

m=0; n=0; %reset exponents
assert(B ~= 0);
assert(C ~= 0);

if ~exist('all_combinations')
  all_combinations = false;
end
if ~exist('all_realizations')
  all_realizations = false;
end
realization = [];

%check the trivial cases A=B or A=C:
if A==B
  realization = [realization; sort_realization(A,B,0,0,0,all_combinations)];
  if ~all_realizations
    return;
  end
end
if A==C
  realization = [realization; sort_realization(A,C,0,0,0,all_combinations)];
  if ~all_realizations
    return;
  end
end

l_max = ceil(log2((c_max+max([A,B,C]))/min([A,B,C])));

%find concrete value for l, begining with all realizations based on left shifts:
%check l>0:
for l=1:l_max
  if(A == B*2^l + C)
    m=l; n=0;
    %disp(['case 3:  realization: ',num2str(A),'=',num2str(B),'*2^',num2str(l),'+',num2str(C)]);
    realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
    if ~all_realizations
      return;
    end
  end
  if (A == abs(B*2^l - C))
    if B*2^l > C
      %disp(['case 4a: realization: ',num2str(A),'=',num2str(B),'*2^',num2str(l),'-',num2str(C)]);
      assert(A == B*2^l - C)
      m=l; n=0; C=-C;
    else
      %disp(['case 4b: realization: ',num2str(A),'=',num2str(C),'-',num2str(B),'*2^',num2str(l)]);
      assert(A == C - B*2^l)
      m=l; n=0; B=-B;
    end
    realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
    if ~all_realizations
      return;
    end
  end
  if (A == B + C*2^l)
    %disp(['case 5:  realization: ',num2str(A),'=',num2str(B),'+',num2str(C),'*2^',num2str(l)]);
    m=0; n=l;
    realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
    if ~all_realizations
      return;
    end
  end
  if (A == abs(B - C*2^l))
    if B > C*2^l
      %disp(['case 6a: realization: ',num2str(A),'=',num2str(B),'-',num2str(C),'*2^',num2str(l)]);
      assert(A == B - C*2^l)
      m=0; n=l; C=-C;
    else
      %disp(['case 6b: realization: ',num2str(A),'=',num2str(C),'*2^',num2str(l),'-',num2str(B)]);
      assert(A == C*2^l - B)
      m=0; n=l; B=-B;
    end
    realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
    return;
  end
end
%error(['No realization found for A=',num2str(A),', B=',num2str(B),' and C=',num2str(C)])

%No left shift was found, search for right shift:
[fun,exp] = fundamental(B + C);
if(A == fun)
  %disp(['case 1:  realization: ',num2str(A),'=',num2str(B),'*2^-',num2str(exp),'+',num2str(C),'*2^-',num2str(exp)]);
  assert(A == B*2^-exp + C*2^-exp)
  m=-exp; n=-exp;
  realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
  if ~all_realizations
    return;
  end
end
[fun,exp] = fundamental(B - C);
if(A == fun)
  %disp(['case 2a:  realization: ',num2str(A),'=',num2str(B),'*2^-',num2str(exp),'-',num2str(C),'*2^-',num2str(exp)]);
  assert(A == B*2^-exp - C*2^-exp)
  m=-exp; n=-exp;
  C=-C;
  realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
  if ~all_realizations
    return;
  end
end
[fun,exp] = fundamental(C - B);
if(A == fun)
  %disp(['case 2b:  realization: ',num2str(A),'=',num2str(B),'*2^-',num2str(exp),'-',num2str(C),'*2^-',num2str(exp)]);
  assert(A == C*2^-exp - B*2^-exp)
  m=-exp; n=-exp;
  B=-B;
  realization = [realization; sort_realization(A,B,m,C,n,all_combinations)];
  if ~all_realizations
    return;
  end
end



function realization = sort_realization(A,B,m,C,n,all_combinations)
realization=[];
% A:
if B>C
  realization = [realization;A,B,m,C,n];
else
  realization = [realization;A,C,n,B,m];
end
assert(realization(1) == realization(2)*2^realization(3)+realization(4)*2^realization(5),'realization invalid');
if all_combinations == false
  return; %break here if a single solution is sufficient
end
% B:
if B>0
  if A>C
    realization = [realization;B,A,-m,-C,n-m];
  else
    realization = [realization;B,A,-m,-C,n-m];
  end
else
  if A>C
    realization = [realization;-B,C,n-m,-A,-m];
  else
    realization = [realization;-B,C,n-m,-A,-m];
  end
end      
assert(realization(2,1) == realization(2,2)*2^realization(2,3)+realization(2,4)*2^realization(2,5),'realization invalid');
% C:
if C>0
  if A>B
    realization = [realization;C,A,-n,-B,m-n];
  else
    realization = [realization;C,A,-n,-B,m-n];
  end
else
  if A>B
    realization = [realization;-C,B,m-n,-A,-n];
  else
    realization = [realization;-C,B,m-n,-A,-n];
  end
end
assert(realization(3,1) == realization(3,2)*2^realization(3,3)+realization(3,4)*2^realization(3,5),'realization invalid');
