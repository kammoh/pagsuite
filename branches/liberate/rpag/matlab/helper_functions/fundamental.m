% fundamental(x) reduces an integer vector x 
% to an odd integer vector 'fun' with corresponding
% shift factor vector 'exp' shuch that
% x(i) = fun(i) * 2^exp(i)

function [fun,exp] = fundamental(x)
fun = zeros(1,length(x));
exp = zeros(1,length(x));
for i=1:length(x)
    if x(i)==0
        fun(i)=0;
    else
        fun(i)=x(i);
        while(mod(fun(i),2) == 0)
            fun(i) = fun(i)/2;
            exp(i) = exp(i) + 1;
        end
    end
end

