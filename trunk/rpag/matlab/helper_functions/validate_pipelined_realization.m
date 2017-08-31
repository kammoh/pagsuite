function validate_pipelined_realization(pipelined_realization,varargin)


check_sign=true;
coeff=[];
for k=1:2:size(varargin,2)
  switch(varargin{k})
    case 'checksign'
      checksign = varargin{k+1};
    case 'coeff'
      coeff = varargin{k+1};
  end
end


if iscell(pipelined_realization)
  pipeline_depth=0;
  for i=1:length(pipelined_realization)
    pipeline_depth = max(pipelined_realization{i}{3},pipeline_depth);
  end
  
  %collect pipeline sets to be able to check if a predecessor was computed
  pipeline_sets = cell(1,pipeline_depth);
  for i=1:length(pipelined_realization)
    if pipelined_realization{i}{3} > 0
      pipeline_sets{pipelined_realization{i}{3}} = [pipeline_sets{pipelined_realization{i}{3}};pipelined_realization{i}{2}];
    end
  end
  
  for i=1:length(pipelined_realization)
    row = pipelined_realization{i};
    if row{1} == 'A'
      %check operation:
      assert(all(row{2}==row{4}*2.^row{6}+row{7}*2.^row{9}),['Realization inconsistent (adder): ',mat2str(row{2}),' ~= ',mat2str(row{4}),'*2^',num2str(row{6}),'+',mat2str(row{7}),'*2^',num2str(row{9})]);
      
      %check existance of inputs:
      assert((row{5} == 0) || vector_is_member_of_matrix(norm(row{4}),pipeline_sets{row{5}}),['Realization inconsistent (adder): Input ',mat2str(row{4}),' not found in stage ',num2str(row{5})]);
      assert((row{8} == 0) || vector_is_member_of_matrix(norm(row{7}),pipeline_sets{row{8}}) || vector_is_member_of_matrix((-1)*row{7},pipeline_sets{row{8}}),['Realization inconsistent (adder): Input ',mat2str(row{7}),' not found in stage ',num2str(row{8})]);
      
      %check for valid pipeline:
      assert(all(row{3}==row{5}+1),['Realization inconsistent (adder): ',mat2str(row{2}),' in stage ',row{3},' depends on ',mat2str(row{4}),' of stage ',num2str(row{5})]);
      assert(all(row{3}==row{8}+1),['Realization inconsistent (adder): ',mat2str(row{2}),' in stage ',row{3},' depends on ',mat2str(row{4}),' of stage ',num2str(row{8})]);
      
    elseif row{1} == 'R'
      %check operation:
      assert(all(row{2}==row{4}),['Realization inconsistent (register): ',mat2str(row{2}),' ~= ',mat2str(row{4})]);

      %check existance of input:
      assert((row{5} == 0) || vector_is_member_of_matrix(row{4},pipeline_sets{row{5}}),['Realization inconsistent (register): Input ',mat2str(row{4}),' not found in stage ',num2str(row{5})]);
    
      %check for valid pipeline:
      assert(all(row{3}==row{5}+1),['Realization inconsistent (register): ',mat2str(row{2}),' in stage ',row{3},' depends on ',mat2str(row{4}),' of stage ',num2str(row{5})]);
    elseif row{1} == 'O'      
      %check operation:
      assert(all(row{2}==row{4}.*2.^row{6}),['Realization inconsistent (output): ',mat2str(row{2}),' ~= ',mat2str(row{4}),'*2^',num2str(row{6})]);

      %check existance of input:
      assert((row{5} == 0) || vector_is_member_of_matrix(row{4},pipeline_sets{row{5}}),['Realization inconsistent (output): Input ',mat2str(row{4}),' not found in stage ',num2str(row{5})]);      

      %check for valid pipeline:
      assert(all(row{3}==row{5}),['Realization inconsistent (output): ',mat2str(row{2}),' in stage ',row{3},' depends on ',mat2str(row{4}),' of stage ',num2str(row{5})]);
    end
  end
else
  pipelined_realization_size=size(pipelined_realization);

  for i=1:pipelined_realization_size(1)
    assert((pipelined_realization(i,6) == 0) || (pipelined_realization(i,2) == pipelined_realization(i,4)+1),['coefficient ',num2str(pipelined_realization(i,1)),' at stage ',num2str(pipelined_realization(i,2)),' depends on ',num2str(pipelined_realization(i,3)),' of stage ',num2str(pipelined_realization(i,4))]);
    assert((pipelined_realization(i,6) == 0) || (pipelined_realization(i,2) == pipelined_realization(i,7)+1),['coefficient ',num2str(pipelined_realization(i,1)),' at stage ',num2str(pipelined_realization(i,2)),' depends on ',num2str(pipelined_realization(i,6)),' of stage ',num2str(pipelined_realization(i,7))]);

    if check_sign == true
      assert(pipelined_realization(i,1) == pipelined_realization(i,3)*2^pipelined_realization(i,5)+pipelined_realization(i,6)*2^pipelined_realization(i,8),['Realization inconsistent: ',num2str(pipelined_realization(i,1)),' ~= ',num2str(pipelined_realization(i,3)),'*2^',num2str(pipelined_realization(i,5)),'+',num2str(pipelined_realization(i,6)),'*2^',num2str(pipelined_realization(i,8))]);
    else
      assert(abs(pipelined_realization(i,1)) == abs(pipelined_realization(i,3)*2^pipelined_realization(i,5)+pipelined_realization(i,6)*2^pipelined_realization(i,8)),['Realization inconsistent: ',num2str(pipelined_realization(i,1)),' ~= ',num2str(pipelined_realization(i,3)),'*2^',num2str(pipelined_realization(i,5)),'+',num2str(pipelined_realization(i,6)),'*2^',num2str(pipelined_realization(i,8))]);
    end

  end
end

if ~isempty(coeff)
  %check output coefficients:
  for i=1:length(pipelined_realization)
    row = pipelined_realization{i};
    if row{1} == 'O'
      assert(vector_is_member_of_matrix_abs(row{2},coeff),['output coefficient ',mat2str(row{2}),' does not appear in specified coefficients: ',mat2str(coeff)])
%      assert(vector_is_member_of_matrix_normed(row{2},coeff),['output coefficient ',mat2str(row{2}),' does not appear in specified coefficients: ',mat2str(coeff)])     
    end
  end
end

function vr_norm = norm(vr)
  if vr(1) < 0
    vr_norm = -vr;
  else
    vr_norm = vr;
end

%checks is a row vector vr is contained in any row of matrix M and returns
%true, if yes
function is_member = vector_is_member_of_matrix(vr,M)
  for i=1:size(M,1)
    if all(vr==M(i,:))
      is_member=true;
      return;
    end
  end
  is_member=false;

%checks is a normed row vector vr is contained in any normed row of matrix M and returns
%true, if yes
function is_member = vector_is_member_of_matrix_normed(vr,M)
  for i=1:size(M,1)
    if all(norm(vr)==norm(M(i,:)))
      is_member=true;
      return;
    end
  end
  is_member=false;

%checks is a absolute row vector vr is contained in any absolute row of matrix M and returns
%true, if yes
function is_member = vector_is_member_of_matrix_abs(vr,M)
  for i=1:size(M,1)
    if all(abs(vr)==abs(M(i,:)))
      is_member=true;
      return;
    end
  end
  is_member=false;
  
  