function fanout = analyze_pipelined_realization_fanout(pipelined_adder_graph)

fanout = zeros(1,size(pipelined_adder_graph,2));

%take each node...
for i=1:size(pipelined_adder_graph,2)
  if cell2mat(pipelined_adder_graph{i}(1)) ~= 'O'
    %... and count how many times it is used in the following stage:
    for j=1:size(pipelined_adder_graph,2)
      if (cell2mat(pipelined_adder_graph{j}(3)) == cell2mat(pipelined_adder_graph{i}(3))+1) || (cell2mat(pipelined_adder_graph{j}(1)) == 'O')
        if cell2mat(pipelined_adder_graph{j}(4)) == cell2mat(pipelined_adder_graph{i}(2))
          fanout(i) = fanout(i)+1;
        end
        if cell2mat(pipelined_adder_graph{j}(1)) == 'A'
          if norm_vec(cell2mat(pipelined_adder_graph{j}(7))) == norm_vec(cell2mat(pipelined_adder_graph{i}(2)))
            fanout(i) = fanout(i)+1;
          end
        end
      end
    end
  end
end