clear
clc

coeffs=[47,99]

b_max=ceil(log2(max(coeffs)))+1;
k=1:b_max;
c_max=2^b_max

V=cell(1,length(coeffs));

l=1;
for u=3:2:c_max
  for i=1:length(coeffs)
    w = coeffs(i);
    V{i} = [fundamental(w+u),fundamental(abs(w-u)),2.^k.*w+u,abs(2.^k.*w-u),w+2.^k.*u,abs(w-2.^k.*u)];
    V{i} = unique(nonzeros(V{i}.*((V{i} > 0) & (V{i} < c_max)))');
  end
  V12 = intersect(V{1},V{2});
  if ~isempty(V12)
    for j=1:length(V12)
      disp(['[',num2str(u),',',num2str(V12(j)),']']);
      VU1{l} = [u,V12(j)];
      l = l + 1;
    end
  end
end

VUx = [VU1{:}];
VU1=unique([VUx(1:2:length(VUx))',VUx(2:2:length(VUx))'],'rows')

k=2:b_max;
C1=[2.^k+1,2.^k-1];
C1=unique(C1(C1 < 2^b_max));

for m=0:10
  X = fundamental((coeffs(1)-2^m*coeffs(2))*2^b_max)./C1
end


return
i=1;
for l11=-b_max:b_max
  for l12=-b_max:b_max
    for l21=-b_max:b_max
      for l22=-b_max:b_max
        v=(coeffs(1)-2^(l11-l21)*coeffs(2))/(2^(l11+l22-l21)-2^l12);
        v = abs(v);
        if v < c_max
          if mod(v,1)==0
            u=abs((coeffs(2)-2^l22*v)*2^(-l12));
            if u < c_max
              if mod(u,1)==0
                VU2{i} = [v,u];
                i = i + 1;
              end
            end
          end
        end
        if l11==2 && l12==0 && l21==3 && l22==0
          v
          u
          return
        end
      end
    end
  end
end
disp(' ');

%for i=1:length(VU2)
%  disp(['[',num2str(VU2{i}(1)),',',num2str(VU2{i}(2)),']']);
%end

VUx = [VU2{:}];
VU2=unique([VUx(1:2:length(VUx))',VUx(2:2:length(VUx))'],'rows')
%unique([V{:}])
%C1=[2.^k+1,2.^k-1];
%C1=unique(C1(C1<c_max));

%X(mod(X,1)==0)