% csd2dec(csd) converts a csd vector to a decimal number
% csd(1) is the lsb

function dec = csd2dec(csd)

N=length(csd);
dec = sum(csd .* 2.^(0:N-1));

