function [add_count,adder_depth_avg,adder_depth_max,GPC] = analyze_realization(realization)

global verbose;
unique_coeffs = unique(setdiff(fundamental(abs(realization(:,1))),1));
add_count = length(unique_coeffs);
adder_depth_avg = 0;
adder_depth_max = 0;

%compute avg. AD:
for i=1:add_count
  adder_depth_current = adder_depth(unique_coeffs(i));
  adder_depth_avg = adder_depth_avg + adder_depth_current;
  if(adder_depth_current > adder_depth_max)
    adder_depth_max = adder_depth_current;
  end
end

%compute GPC:
GPC = 0;
gpcMap = containers.Map(1,0);

allFound = false;

while ~allFound
  allFound = true;
  for i=1:size(realization,1)
    if realization(i,4) ~= 0 %ignore output nodes
      if ~gpcMap.isKey(realization(i,1)) && gpcMap.isKey(abs(realization(i,2))) && gpcMap.isKey(abs(realization(i,4)))
        gpcMap(realization(i,1)) = gpcMap(abs(realization(i,2))) + gpcMap(abs(realization(i,4))) + 1;
        if verbose >= 3
          disp(['GPC of node ',num2str(realization(i,1)),' is ',num2str(gpcMap(realization(i,1)))]);
        end
        GPC = GPC + gpcMap(realization(i,1));
        allFound = false;
      end
    end
  end
end
  
adder_depth_avg=adder_depth_avg/add_count;
