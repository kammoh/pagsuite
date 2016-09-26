% [X,Y] = bit_combinations(msd,x_indices,y_indices,max_adder_depth)
% is a helper function for find_msd_potentials() that recursively iterates
% all possible combinations of bits in msd.
% x_indices and y_indices can be empty vectors []
% max_adder_depth is the maximum adder depth of x and y

function [X,Y] = bit_combinations(msd,x_indices,y_indices,max_adder_depth)

global verbose;

%the first call of the recursion is detected by an empty x_indices set
X=0;
Y=0;
if isempty(x_indices)
  x_indices_max = 0;

  y_indices=[];
  for k=1:length(msd)
    if abs(msd(k)) == 1
      y_indices=[y_indices,k];
    end
  end
else
  x_indices_max = max(x_indices);
end  

no_of_total_bits = length(x_indices)+length(y_indices);

X=[]; Y=[];

for i=1:length(y_indices)
  if y_indices(i) > x_indices_max
    x_indices_new = [x_indices, y_indices(i)];
    if i==1
      y_indices_new = y_indices(2:length(y_indices));
    elseif i == length(y_indices)
      y_indices_new = y_indices(1:i-1);
    else
      y_indices_new = [y_indices(1:i-1),y_indices(i+1:length(y_indices))];
    end

    %check adder depth
    if (max_adder_depth < 0) || ((ceil(log2(length(x_indices_new))) <= max_adder_depth) && (ceil(log2(length(y_indices_new))) <= max_adder_depth))
    
      %calculate X:
      x=0;
      for k=1:length(x_indices_new)
        x = x + msd(x_indices_new(k))*2^(x_indices_new(k)-1);
      end
      %calculate Y:
      y=0;
      for k=1:length(y_indices_new)
        y = y + msd(y_indices_new(k))*2^(y_indices_new(k)-1);
      end
      if verbose >= 3
        disp([num2str(x_indices_new),' <-> ',num2str(y_indices_new),' : ',num2str(x+y),' = ',num2str(x),' + ',num2str(y)]);
      end
      %insert result (as abs fundamental):
      x=fundamental(abs(x));
      y=fundamental(abs(y));
      %sort result:
      if x > y
        t=x; x=y; y=t;
      end
      X=[X;x];
      Y=[Y;y];
    end
    
    %start next recursion?
    if length(x_indices_new) < floor(no_of_total_bits/2)
      [x_new,y_new] = bit_combinations(msd,x_indices_new,y_indices_new,max_adder_depth);
      X = [X;x_new];
      Y = [Y;y_new];
    end
  end
end
