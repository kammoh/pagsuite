%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%	PhD	Project of Mathias Faust
%
%Universities:  NTU: Nanyang Technological University Singapore
%Filename:      dec2csd.m    
%Authors:       ?
%Modified by:   Markus Raehle mraehle@bluewin.ch
%Supervisor:    Assoc. Prof. Chang Chip Hong
%Creation:      10.10.2006
%Last Change:   31.10.2006
%MatLab vers.:  7.2.0.232.(R2006a)
%Description:   this code transforms a signed integer value to the csd form
%                 of it.
%Input:         h -> integer value
%Output:        CSD -> integer in csd form
%     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%------------------------included by Markus Raehle-------------------------
function [CSD] = dec2csd(h) %CSD function

% Function requires at least one input argument
if(nargin < 1)
    error(' Please use function in the following way:   CSD = dec2csd(h)');
end;

if(h == 0)
    CSD = 0;
    return;
elseif(h == 1)
    CSD = 1;
    return;
elseif(h == -1)
    CSD = -1;
    return;
end

negative = 1;
if h < 0
    h = -h;
    negative = -1;
end
%--------------------end of Markus Raehle----------------------------------

binstr = dec2bin(h);
CSD = [0 str2num(binstr')'];
csdLength = length(CSD);

save = 0;
i = csdLength;
while i >= 1
    if(save == 1 && CSD(i) == 1)
        CSD(i) = 0;
    elseif(save == 1 && CSD(i) == 0)
        CSD(i) = 1;
        save = 0;
    elseif(i < csdLength & CSD(i:i+1) == [1 1])
        save = 1;
        CSD(i:i+1) = [0 -1];
    end;
    i=i-1;
    continue;
end;

if(CSD(1) == 0)
    CSD = CSD(2:end);
end;
%------------------------included by Markus Raehle-------------------------
CSD = negative * CSD;
%--------------------end of Markus Raehle----------------------------------