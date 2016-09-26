clear
clc

csd_min2=3;
csd_min3=11;
csd_min4=43; 
csd_min5=171; 
csd_min6=683; 
csd_min7=2731; 
csd_min8=10923; 
csd_min9=43691; 
csd_min10=174763; 
csd_min11=699051; 
csd_min12=2796203; 
csd_min13=11184811; 
csd_min14=44739243; 
csd_min15=178956971; 
csd_min16=715827883;

%c=csd_min8;
%c=csd_min4;
c=csd_min5;

dec2csd(c)

global verbose;
verbose = 2;

%[a,b] = find_potentials(c);
%potentials = unique([a(:,1)',b(:,1)'])';
[AB] = find_potentials2(c);
potentials = unique(fundamental(abs(AB(:))));

%filter potentials that are greater than c:
potentials = potentials(potentials < c);

predecessor_pair_list = find_all_predecessor_pairs(c);
all = unique([predecessor_pair_list(:,2)',predecessor_pair_list(:,3)'])';
%filter 'all' that are greater than c:
all = all(all < c);


diff=setdiff(all,potentials);

% %diff2 stores only nz2 and nz4 digit numbers:
% diff2=[];
% for i=1:length(diff)
%   nonzero_digits = nnz(dec2csd(diff(i)));
%   if (nonzero_digits == 2) || (nonzero_digits == 4)
%     diff2 = [diff2,diff(i)];
%   end
% end

% diff3=setdiff(potentials,all)';

disp(['length all: ',num2str(length(all))]);
disp(['length potentials: ',num2str(length(potentials))]);
disp(['length diff: ',num2str(length(diff))]);
% disp(['length diff2: ',num2str(length(diff2))]);
% disp(['length diff3: ',num2str(length(diff3))]);
disp(['diff: ',num2str(diff)]);
% disp(['diff2: ',num2str(diff2)]);
% disp(['diff3: ',num2str(diff3)]);
