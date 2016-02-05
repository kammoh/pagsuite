function [power,deger] = make_number_posdd(deger)

power=0;

if deger<0
    deger=(-1)*deger;
end

while 1
    if deger~=0
        if mod(deger,2)==0
            deger=deger/2;
            power=power+1;
        else
            return
        end
    else
        return
    end
end
