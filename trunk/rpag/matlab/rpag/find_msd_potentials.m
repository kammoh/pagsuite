% [XY] = find_msd_potentials(C,max_adder_depth) computes all possible (x,y)
% pairs (abs fundamentals) from the MSD presentations of the elements in C 
% which have an adder distance <= max_adder_depth.
% Each pair can be used to compute one or more Elements of C with one
% A-Op.
%
% The return value XY is a 2xN matrix, where the (x,y)-pairs are the
% columns of the matrix and N is the number of possible pairs.
% The XY matrix is not unique. The occurence of equal rows in XY is equal 
% to the number of Elements in C that can be realized.
% Each row is sorted such that x < y.
%
% All possible bit combinations are evaluated using the recursive method
% bit_combinations()

function [XY] = find_msd_potentials(C,max_adder_depth)

%exclude 1 from MSD coefficients
C =setdiff(C,[0,1]);
global verbose; 

XY=[];
for j=1:length(C)
  XY_tmp=[];
  msd_set = dec2msd(C(j));

  msd_set_size=size(msd_set);

  one_indices=[];

  XY_new=[];
  for i=1:msd_set_size(1)
    msd = msd_set(i,:);
  
    [x,y] = bit_combinations(msd,[],[],max_adder_depth);
    XY_tmp=[XY_tmp;x,y];
  end
  XY_tmp = unique(sortrows(XY_tmp),'rows');
  if verbose >= 3
    disp(['************* coefficient ',num2str(C(j)),' *************']);
    XY_tmp
  end
  XY = [XY;XY_tmp];
end

%XY_size=size(XY);
%for i=1:XY_size(1)
%  disp(['(x,y)=(',num2str(XY(i,1)),',',num2str(XY(i,2)),') => AD(x)=',num2str(adder_depth(XY(i,1))),' AD(y)=',num2str(adder_depth(XY(i,2)))]);
%end
