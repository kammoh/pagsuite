% dec2csd(dec) converts a decimal 'dec' into its CSD representation
% csd(1) is LSB

function csd = dec2csd(dec)
binstr = dec2bin(dec);

csd = fliplr(binstr-48);

%check MSB, if one, enlarge the vector
if csd(length(csd)) == 1
  csd = [csd,0];
end

one_cnt=0;
%for i=1:length(csd)
i=1;
while i <= length(csd)
  if csd(i) == 1
    if one_cnt == 0
      csd(i) = 1;
    end
    one_cnt = one_cnt + 1;
  elseif csd(i) == 0
    if one_cnt > 1
      csd(i) = 1;
      for j=i-one_cnt+1:i-1
        csd(j) = 0;
      end
      csd(i-one_cnt) = -1;
      i = i - 1;
%      disp(['csd: ',num2str(csd)]);
    else
      csd(i) = 0;
    end
    one_cnt=0;
  end
  i = i + 1;
end

