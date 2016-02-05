function str = node2str(m)

str = mat2str(m);
str = strrep(str,'-', 'm'); %replace '-' by 'm'
str = strrep(str,' ', '_'); %replace ' ' by '_'
str = strrep(str,'[', ''); %replace '[' by ''
str = strrep(str,']', ''); %replace ']' by ''
