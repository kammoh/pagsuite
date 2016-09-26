function [p1,p2] = evaluate_usefull_predecessor_pairs(w1,w2,l_max,ad_max,l11,l12,l21,l22,r1,r2,s12,s22)

%disp(['w1=',num2str(w1),',w2=',num2str(w2),',l_max=',num2str(l_max),',ad_max=',num2str(ad_max),',l11=',num2str(l11),',l12=',num2str(l12),',l21=',num2str(l21),',l22=',num2str(l22),',r1=',num2str(r1),',r2=',num2str(r2),',s12=',num2str(s12),',s22=',num2str(s22)]);
p1 = abs((w1*2^(r1-l21)-(-1)^s12*(-1)^s22*w2*2^(r2-l22)) / (2^(l11-l21) - (-1)^s22*2^(l12-l22)));
if ceil(p1)==p1 && ~isinf(p1)
  if adder_depth(p1) <= ad_max
    p2 = abs((w1*2^(r1-l11) - (-1)^s12*w2*2^(r2-l12)) / (2^(l21-l11) - (-1)^s22*2^(l22-l12)));
    if ceil(p2)==p2 && ~isinf(p2)
      if adder_depth(p2) <= ad_max
        p1 = fundamental(p1);
        p2 = fundamental(p2);
        if p1 > p2
          tmp = p2;
          p2 = p1;
          p1 = tmp;
        end
        return;
%       disp(['(p1,p2)=(',num2str(p1),',',num2str(p2),')']);
%       assert(length(intersect(succ(p1,p2,2^(3*l_max)),[w1,w2])) == 2)
      end
    end
  end
end
p1=-1;
p2=-1;
