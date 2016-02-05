function validate_pipelined_realization(pipelined_realization,check_sign)

if ~exist('check_sign')
  check_sign=true;
end

pipelined_realization_size=size(pipelined_realization);

for i=1:pipelined_realization_size(1)
  assert((pipelined_realization(i,6) == 0) || (pipelined_realization(i,2) == pipelined_realization(i,4)+1),['coefficient ',num2str(pipelined_realization(i,1)),' at stage ',num2str(pipelined_realization(i,2)),' depends on ',num2str(pipelined_realization(i,3)),' of stage ',num2str(pipelined_realization(i,4))]);
  assert((pipelined_realization(i,6) == 0) || (pipelined_realization(i,2) == pipelined_realization(i,7)+1),['coefficient ',num2str(pipelined_realization(i,1)),' at stage ',num2str(pipelined_realization(i,2)),' depends on ',num2str(pipelined_realization(i,6)),' of stage ',num2str(pipelined_realization(i,7))]);

  if check_sign == true
    assert(pipelined_realization(i,1) == pipelined_realization(i,3)*2^pipelined_realization(i,5)+pipelined_realization(i,6)*2^pipelined_realization(i,8),['Realization inconsistent: ',num2str(pipelined_realization(i,1)),' ~= ',num2str(pipelined_realization(i,3)),'*2^',num2str(pipelined_realization(i,5)),'+',num2str(pipelined_realization(i,6)),'*2^',num2str(pipelined_realization(i,8))]);
  else
    assert(abs(pipelined_realization(i,1)) == abs(pipelined_realization(i,3)*2^pipelined_realization(i,5)+pipelined_realization(i,6)*2^pipelined_realization(i,8)),['Realization inconsistent: ',num2str(pipelined_realization(i,1)),' ~= ',num2str(pipelined_realization(i,3)),'*2^',num2str(pipelined_realization(i,5)),'+',num2str(pipelined_realization(i,6)),'*2^',num2str(pipelined_realization(i,8))]);
  end

end