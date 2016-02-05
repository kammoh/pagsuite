% generates a pdf from dot-file
% parameter 'name' must be a filename wihout '.dot' extension

function dot2pdf(name)

if strcmp(computer,'MACI') || strcmp(computer,'MACI64')
  system(['dot -o',name,'.ps -Tps2 ',name,'.dot']);
  system(['ps2pdf14 ',name,'.ps ',name,'.pdf']);
  system(['rm ',name,'.ps']);
elseif strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
  system(['dot -o',name,'.ps -Tps2 ',name,'.dot']);
  system(['epstopdf ',name,'.ps']);
  system(['del ',name,'.ps']);
elseif strcmp(computer,'GLNXA') || strcmp(computer,'GLNXA64')
  system(['dot -o',name,'.ps -Tps2 ',name,'.dot']);
  system(['ps2ps ',name,'.ps ',name,'_b.ps']);
  system(['ps2pdf14 ',name,'_b.ps ',name,'.pdf']);
  system(['rm ',name,'.ps']);
  system(['rm ',name,'_b.ps']);
end

