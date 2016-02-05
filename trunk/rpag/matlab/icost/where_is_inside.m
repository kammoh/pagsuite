function [value] = where_is_inside(aranan,yer_say,yer)

value=0;

if yer_say~=0
    for i=yer_say:-1:1
        if yer(1,i)==aranan
            value=i;
            return
        end
    end
end
