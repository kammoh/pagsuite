function showpdf(filename)

if strcmp(computer,'MACI') || strcmp(computer,'MACI64')
  pdf_viewer = 'open /Applications/Preview.app';
  system([pdf_viewer,' ',filename]);
elseif strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
  pdf_viewer = 'path to win pdf viewer exe';
  system([pdf_viewer,' ',filename]);
elseif strcmp(computer,'GLNXA') || strcmp(computer,'GLNXA64')
  pdf_viewer = 'xpdf';
  system([pdf_viewer,' ',filename,' & ']);
end
