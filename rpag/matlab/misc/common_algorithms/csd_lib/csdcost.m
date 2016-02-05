% csdcost2(dec) computes the adder costs for a 
% decimal dec in CSD representation without explicitly
% computing the csd representation

function cost = csdcost(dec)

cost = zeros(1,length(dec));

for k=1:length(dec)
  N=ceil(log2(abs(dec(k)+1))); 
%  bin=dec2binvec_2th_cpl(dec(k),N);
  bin = fliplr(dec2bin(dec)-48);

  one_cnt=0;
  for i=1:N
    if bin(i) == 1
      if i < 3
        one_cnt = one_cnt + 1;
      elseif (i == 3) || (i == N)
        if ~((bin(i-1) == 1) && (bin(i-2) == 1))
          one_cnt = one_cnt + 1;
        end
      else % 3 < i < N
        if ~((bin(i-1) == 1) && (bin(i-2) == 1)) && ~((bin(i+1) == 1) && (bin(i-1) == 0) && (bin(i-2) == 1) && (bin(i-3) == 1))
          one_cnt = one_cnt + 1;
        end
      end
  %    disp(['i=',num2str(i),' one_cnt=',num2str(one_cnt)]);
    end
  end
  cost(k) = one_cnt-1;
end