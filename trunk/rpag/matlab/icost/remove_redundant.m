function [say_ready,ready_list,say_oper,oper_list] = remove_redundant(say_coef,coef_list,say_ready,ready_list,say_oper,oper_list)

say_remove=0;
remove_list=[];

ready_list(:,1)=[];
say_ready=say_ready-1;

for i=1:say_ready
    if not(is_inside(ready_list(1,i),say_coef,coef_list))
        is_redundant=1;
        
        for j=1:say_oper
            if oper_list(j,1)==ready_list(1,i)
                the_pos=j;
            end
            
            if or(oper_list(j,4)==ready_list(1,i),oper_list(j,7)==ready_list(1,i))
                is_redundant=0;
                break;
            end
        end
        
        if is_redundant
            say_remove=say_remove+1;
            remove_list(say_remove,1)=i;
            remove_list(say_remove,2)=the_pos;
        end
    end
end

if say_remove
    for i=1:say_remove
        say_ready=say_ready-1;
        ready_list(:,remove_list(i,1)-i+1)=[];

        say_oper=say_oper-1;
        oper_list(remove_list(i,2)-i+1,:)=[];
    end
end

new_readylist=[1];
for i=1:say_ready
    new_readylist(1,i+1)=ready_list(1,i);
end

say_ready=say_ready+1;
ready_list=new_readylist;
