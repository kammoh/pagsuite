function predecessor_pair_list = find_all_predecessor_pairs(coeff,c_max)

global verbose;


coeff_unique = setdiff(unique(abs(coeff)),[0,1]);
coeff_fun = unique(fundamental(coeff_unique));
if ~exist('c_max')
  c_max = 2^(ceil(log2(max(coeff_fun))));
end

%analyze max. adder depth (=max no of stages) and store in s_max
s_max=0;
for i=1:length(coeff_fun)
  s=adder_depth(coeff_fun(i));
  if s > s_max
    s_max = s;
  end
end

if(verbose >= 1)
  disp(['number of pipeline stages: ',num2str(s_max)]);
end

%precalculate the A-sets and store in cell array
A = cell(1,s_max);
A{1}=[1];
for i=2:s_max
  A{i} = succ_set(A{i-1},c_max);
  if(verbose >= 1)
    disp(['no. of possibilities of pipeline stage ',num2str(i),': ',num2str(length(A{i}))]);
  end
end

As=A{s_max};
predecessor_pair_list=[];

for i=1:length(As)
  x=As(i);
  for j=1:length(coeff)
    y=succ(x,coeff(j),c_max,false,false);
    y=intersect(y,As); % filter y values by AD(y) <= s_max-1
    for k=1:length(y)
      if(verbose >= 2)
        disp(['coefficient ',num2str(coeff(j)),' can be realized with successors x=',num2str(x),' and y=',num2str(y(k))]);
      end
      predecessor_pair_list = [predecessor_pair_list; coeff(j),x, y(k)];
    end
  end  
end 

predecessor_pair_list = unique(predecessor_pair_list, 'rows');
