%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    PhD Project of Mathias Faust
%
%Universities:  NTU: Nanyang Technological University Singapore
%Filename:    	find_potentials.m
%Authors:     	Mathias Faust mfaust@mfnet.ch
%Supervisor:  	Assoc. Prof. Chang Chip Hong
%Creation:    	28.07.2011 (forked from MinLD)
%Last Change: 	28.07.2011
%Description: 	Calculate potential weight 2,4 coefficients from SearchSet.
%Input:       	SearchSet -> Set with weight 8 fundamentals
%Output:		PotentialSet2 -> potential weight 2 fundamentals
%             	PotentialSet4 -> potential weight 3 fundamentals
%Changes:		28.07.2011 creadted new.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [PotentialSet2, PotentialSet4] = find_potentials(SearchSet)

addpath('../common_algorithms/rep_libMF'); 

% make sure input is a row vector
if(size(SearchSet,2) > size(SearchSet,1))
    SearchSet = SearchSet';
end;

% make sure input is of type double
SearchSet = double(SearchSet);

% create array for CSD values
CSDvalues = zeros(size(SearchSet,1)*2,ceil(log2(max(SearchSet)))+1);

% Get all MSD representations
CSDvaluesPtr = 1;
for i=1:size(SearchSet,1)
    workval = dec2csd(SearchSet(i));
    CSDvaluesTmp = checkfor3(workval);
    CSDvalues(CSDvaluesPtr:CSDvaluesPtr+size(CSDvaluesTmp,1)-1,end-size(workval,2)+1:end) = CSDvaluesTmp;
    CSDvaluesPtr = CSDvaluesPtr + size(CSDvaluesTmp,1);
end;

% truncate array to maximal size
CSDvalues = CSDvalues(1:CSDvaluesPtr-1,:);

% create arrays for potential fundamentals
twoweights(28*size(CSDvalues,1),2) = 0;
twoweightsptr = 1;
fourweights(60*size(CSDvalues,1),2) = 0;
fourweightsptr = 1;
poweroftwo = 2.^(size(CSDvalues,2)-1:-1:0);

% iterate over all MSD representations and get potentials
for i = 1:size(CSDvalues,1) % i -> representation under consideration
    TargetValue = sum(poweroftwo.*CSDvalues(i,:));
    LengthOfValue = size(CSDvalues,2)-find(CSDvalues(i,:) ~= 0,1)+1;
    for j = 1:size(CSDvalues,2)-2 % j -> first digit
        if(CSDvalues(i,j) == 0)
            continue;
        end;
        for k=j+1:size(CSDvalues,2) % k -> second digit
            if(CSDvalues(i,k) ~= 0)
                twoweights(twoweightsptr,:) = [abs(sum(2.^(k-j:-1:0).*[CSDvalues(i,j) zeros(1,k-j-1) CSDvalues(i,k)])) TargetValue];
                twoweightsptr = twoweightsptr+1;
                for m=k+1:size(CSDvalues,2) % m -> third digit
                    if(CSDvalues(i,m) ~= 0) % && m-j+3 < LengthOfValue)
                        for n=m+1:size(CSDvalues,2) % n -> fourth digit
%                             if(n-j+1 < LengthOfValue)
                                if(CSDvalues(i,n) ~= 0)
                                    fourweights(fourweightsptr,:) = [abs(sum(2.^(n-j:-1:0).*[CSDvalues(i,j) zeros(1,k-j-1) CSDvalues(i,k) zeros(1,m-k-1) CSDvalues(i,m) zeros(1,n-m-1) CSDvalues(i,n)])) TargetValue];
                                    fourweightsptr = fourweightsptr+1;
%                                 end;
                            end;
                        end;                        
                    end;
                end;
            end;
        end;
    end;
end;

% truncate arrays to filled size and uniquify them for return
twoweights = twoweights(1:twoweightsptr-1,:);
PotentialSet2 = unique(twoweights,'rows');
fourweights = fourweights(1:fourweightsptr-1,:);
PotentialSet4 = unique(fourweights,'rows');


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% HELPER FUNCTIONS %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% Check CSD value recursively for 3 or -3
function [returnarray, posfound] = checkfor3(workval)
returnarray = workval;
posfound = 0;
for j = 1:size(workval,2)-2
    if(workval(j:j+2) == [1 0 -1])
        [tmp1, tmp2]  = checkfor3([1 workval(j+3:end)]);
        for k = 1:size(tmp1,1)
            if(tmp2 > 1)
                returnarray(end+1,:) = [workval(1:j+2) tmp1(k,2:end)];
            end;
            returnarray(end+1,:) = [workval(1:j-1) 0 1 tmp1(k,:)];
        end;
        if(posfound == 0)
            posfound = j;
        end;
    elseif(workval(j:j+2) == [-1 0 1])
        [tmp1, tmp2]  = checkfor3([-1 workval(j+3:end)]);
        for k = 1:size(tmp1,1)
            if(tmp2 > 1)
                returnarray(end+1,:) = [workval(1:j+2) tmp1(k,2:end)];
            end;
            returnarray(end+1,:) = [workval(1:j-1) 0 -1 tmp1(k,:)];
        end;
        if(posfound == 0)
            posfound = j;
        end;
    end;
end;
returnarray = unique(returnarray,'rows'); 