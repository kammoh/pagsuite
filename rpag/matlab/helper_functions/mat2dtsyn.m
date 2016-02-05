function [string] = mat2dtsyn(realisation)
string = '{';
for i=size(realisation,1):-1:2
    string =[string,'{[',num2str(realisation(i,1)),'],',num2str(realisation(i,2)),',','[',num2str(realisation(i,3)),']',',',num2str(realisation(i,4)),',',num2str(realisation(i,5)),',','[',num2str(realisation(i,6)),']',',',num2str(realisation(i,7)),',',num2str(realisation(i,8)),'},'];
end
i=1;
string =[string,'{[',num2str(realisation(i,1)),'],',num2str(realisation(i,2)),',','[',num2str(realisation(i,3)),']',',',num2str(realisation(i,4)),',',num2str(realisation(i,5)),',','[',num2str(realisation(i,6)),']',',',num2str(realisation(i,7)),',',num2str(realisation(i,8)),'}'];
string =[string,'}'];
end