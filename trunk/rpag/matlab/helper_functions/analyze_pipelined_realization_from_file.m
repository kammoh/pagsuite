function [add_count,register_count,mux_count,pag_cost,no_of_stages] = analyze_pipelined_realization_from_file(filename)

pipelined_adder_graph = eval(fileread(filename));
[add_count,register_count,mux_count,pag_cost,no_of_stages] = analyze_pipelined_realization(pipelined_adder_graph);