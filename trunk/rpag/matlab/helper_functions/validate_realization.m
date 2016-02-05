function validate_realization(realization,check_sign)

if ~exist('check_sign')
  check_sign=true;
end

realization_size=size(realization);

for i=1:realization_size(1)
  if check_sign == true
    assert(realization(i,1) == realization(i,2)*2^realization(i,3)+realization(i,4)*2^realization(i,5),['Realization inconsistent: ',num2str(realization(i,1)),' ~= ',num2str(realization(i,2)),'*2^',num2str(realization(i,3)),'+',num2str(realization(i,4)),'*2^',num2str(realization(i,5))]);
  else
    assert(abs(realization(i,1)) == abs(realization(i,2)*2^realization(i,3)+realization(i,4)*2^realization(i,5)),['Realization inconsistent: ',num2str(realization(i,1)),' ~= ',num2str(realization(i,2)),'*2^',num2str(realization(i,3)),'+',num2str(realization(i,4)),'*2^',num2str(realization(i,5))]);
  end
  assert((abs(realization(i,2)) == 1) || (realization(i,2) == 0) || (sum(abs(realization(i,2)) == abs(realization(:,1))') > 0),['There no realization for ',num2str(realization(i,2)),' is given which is needed by ',num2str(realization(i,1))]);
  assert((abs(realization(i,4)) == 1) || (realization(i,4) == 0) || (sum(abs(realization(i,4)) == abs(realization(:,1))') > 0),['There no realization for ',num2str(realization(i,4)),' is given which is needed by ',num2str(realization(i,1))]);
  
%  if (realization(i,3) < 0) || (realization(i,5) < 0)
%    warning(['Realization depends on negative exponents: ',num2str(realization(i,1)),' = ',num2str(realization(i,2)),'*2^',num2str(realization(i,3)),'+',num2str(realization(i,4)),'*2^',num2str(realization(i,5))]);
%  end
end