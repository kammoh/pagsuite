function [pipelined_adder_graph, feasible] = run_rpag_cmm_decomposition(M,decompose_until_stage,rpag_arguments,depth,start_row,end_row,start_col,end_col)

if ~exist('start_row','var')
  start_row=1;
end
if ~exist('start_col','var')
  start_col=1;
end
if ~exist('end_row','var')
  end_row=size(M,1);
end
if ~exist('end_col','var')
  end_col=size(M,2);
end
if ~exist('rpag_arguments','var')
  rpag_arguments = '';
end
if ~exist('depth','var')
  depth = adder_depth_mat(M);
end

feasible=true;

M_partial = M(start_row:end_row,start_col:end_col);

disp(['Optimizing ',num2str(size(M_partial,1)),'x',num2str(size(M_partial,2)),' depth=',num2str(depth),' at position(',num2str(start_row),',',num2str(start_col),')',' partial matrix M_partial=',strrep(mat2str(M_partial), ' ', ',')]);

global verbose;

if depth <= decompose_until_stage
  if depth > 0
    %stop decomposing and directly solve CMM using RPAG:
    extra_stages = depth - adder_depth_mat(M);
    if verbose>=0
      disp(['Solving CMM on a ',num2str(size(M_partial,1)),'x',num2str(size(M_partial,2)),' matrix using RPAG using ',num2str(extra_stages),' extra stages']);
    end
    [pipelined_adder_graph, pag_cost] = run_rpag_cmm(M,['--verbose=',num2str(verbose-2),' --no_of_extra_stages=',num2str(extra_stages),rpag_arguments]);
  else
    pipelined_adder_graph={};
  end
else
  %decomposing matrix into four smaller matrices and start next recursion:
  
  rows_full = size(M,1);
  cols_full = size(M,2);

  rows=size(M_partial,1);
  cols=size(M_partial,2);

  rows1 = ceil(rows/2);
  rows2 = rows-rows1;

  cols1 = ceil(cols/2);
  cols2 = cols-cols1;

  if verbose>0
    disp(['Decomposing ',num2str(rows),'x',num2str(cols),' matrix into ',num2str(rows1),'x',num2str(cols1),' and ',num2str(rows2),'x',num2str(cols2),' matrices']);
  end

  M1 = M_partial(1:rows1,1:cols1);
  M2 = M_partial(rows1+1:end,1:cols1);

  M3 = M_partial(1:rows1,cols1+1:end);
  M4 = M_partial(rows1+1:end,cols1+1:end);

  depth_next_iter = depth-1;
  
  depth1 = adder_depth_mat(M1);
  depth2 = adder_depth_mat(M2);
  depth3 = adder_depth_mat(M3);
  depth4 = adder_depth_mat(M4);
  
  if depth_next_iter < max([depth1,depth2,depth3,depth4])
    feasible=false;
    pipelined_adder_graph={};
    pag_cost=-1;
    adder_cost=-1;
    reg_cost=-1;
    return;
  end

  assert(all(all([M1,M3;M2,M4]==M_partial))); 

  M1_ext = [M1,zeros(rows1,cols2);zeros(rows2,cols)];
  M2_ext = [zeros(rows1,cols);M2,zeros(rows2,cols2)];
  M3_ext = [zeros(rows1,cols2),M3;zeros(rows2,cols)];
  M4_ext = [zeros(rows1,cols);zeros(rows2,cols2),M4];

  zero_rows_first = zeros(start_row-1,cols_full);  
  zero_rows_last = zeros(rows_full-end_row,cols_full);
  
  M1_ext = [zero_rows_first;zeros(end_row-start_row+1,start_col-1),M1_ext,zeros(end_row-start_row+1,cols_full-end_col);zero_rows_last];
  M2_ext = [zero_rows_first;zeros(end_row-start_row+1,start_col-1),M2_ext,zeros(end_row-start_row+1,cols_full-end_col);zero_rows_last];
  M3_ext = [zero_rows_first;zeros(end_row-start_row+1,start_col-1),M3_ext,zeros(end_row-start_row+1,cols_full-end_col);zero_rows_last];
  M4_ext = [zero_rows_first;zeros(end_row-start_row+1,start_col-1),M4_ext,zeros(end_row-start_row+1,cols_full-end_col);zero_rows_last];
  
  %perform partial CMMs
  if ~isempty(M1) && ~all(all(M1==0))
    [pipelined_adder_graph1, feasible1] = run_rpag_cmm_decomposition(M1_ext,decompose_until_stage,rpag_arguments,depth_next_iter,start_row,start_row+rows1-1,start_col,start_col+cols1-1);
  else
    pipelined_adder_graph1={};
    feasible1=true;
  end
  if ~isempty(M2) && ~all(all(M2==0))
    [pipelined_adder_graph2, feasible2] = run_rpag_cmm_decomposition(M2_ext,decompose_until_stage,rpag_arguments,depth_next_iter,start_row+rows1,start_row+rows-1,start_col,start_col+cols1-1);
  else
    pipelined_adder_graph2={};
    feasible2=true;
  end
  if ~isempty(M3) && ~all(all(M3==0))
    [pipelined_adder_graph3, feasible3] = run_rpag_cmm_decomposition(M3_ext,decompose_until_stage,rpag_arguments,depth_next_iter,start_row,start_row+rows1-1,start_col+cols1,start_col+cols-1);
  else
    pipelined_adder_graph3={};
    feasible3=true;
  end
  if ~isempty(M4) && ~all(all(M4==0))
    [pipelined_adder_graph4, feasible4] = run_rpag_cmm_decomposition(M4_ext,decompose_until_stage,rpag_arguments,depth_next_iter,start_row+rows1,start_row+rows-1,start_col+cols1,start_col+cols-1);
  else
    pipelined_adder_graph4={};
    feasible4=true;
  end

  feasible = feasible1 || feasible2 || feasible3 || feasible4;
  if ~feasible
    pipelined_adder_graph={};
    pag_cost=-1;
    adder_cost=-1;
    reg_cost=-1;
    return;
  end
  
  pipelined_adder_graph = {pipelined_adder_graph1{:},pipelined_adder_graph2{:},pipelined_adder_graph3{:},pipelined_adder_graph4{:}};

%  pipelined_adder_graph = remove_redundant_pipelined_realizations(pipelined_adder_graph);
  pipelined_adder_graph = remove_outputs(pipelined_adder_graph);
  
  %add adders in last stage to merge results from partial CMMs:
  k=length(pipelined_adder_graph)+1;
  for r=1:rows_full
    row_vec = M(r,:);
    
    row_vec = norm_vec(row_vec);
    
    if ~all(row_vec==0)
      row_vec1 = [zeros(1,start_col-1),row_vec(start_col:start_col+cols1-1),zeros(1,cols2),zeros(1,cols_full-end_col)];
      row_vec2 = [zeros(1,start_col-1),zeros(1,cols1),row_vec(start_col+cols1:end_col),zeros(1,cols_full-end_col)];

      assert(all(row_vec1 + row_vec2 == row_vec));
      
      if verbose>1
        disp(['computing ',strrep(mat2str(row_vec), ' ', ','),' from ',strrep(mat2str(row_vec1), ' ', ','),' + ',strrep(mat2str(row_vec2), ' ', ',')]);
      end
      
      [row_vec1,shift1,sign1] = norm_vec(row_vec1);
      [row_vec2,shift2,sign2] = norm_vec(row_vec2);
      
%      assert((sign1 == -1) && (sign2 == -1));
      
      if all(row_vec2==0)
        pipelined_adder_graph{k}={'R',row_vec,depth,row_vec1,depth-1,0};
      elseif all(row_vec1==0)
        pipelined_adder_graph{k}={'R',row_vec,depth,row_vec2,depth-1,0};
      else
        pipelined_adder_graph{k}={'A',row_vec,depth,sign1*row_vec1,depth-1,shift1,sign2*row_vec2,depth-1,shift2};
      end
      k=k+1;
    end
  end

  %add outputs:
  for r=1:rows_full
    row_vec = M(r,:);
    pipelined_adder_graph{k}={'O',row_vec,depth,row_vec,depth,0};
    k=k+1;
  end
end

