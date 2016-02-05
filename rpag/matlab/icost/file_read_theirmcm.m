function [is_checked,say_list,say_partial,say_oper,coef_list,partial_list,oper_list] = file_read_theirmcm(file_solution,file_coef)

say_list=0;
coef_list=[];
fid_coef=fopen(file_coef,'r');

while 1
    tline=fgetl(fid_coef);
    
    if tline==-1
        break
    else
        the_coef=str2num(char(tline(1:1,1:length(tline))));
        [power,posodd_coef]=make_number_posodd(the_coef);
        
        if the_coef
            if not(is_inside(posodd_coef,say_list,coef_list))
                say_list=say_list+1;
                coef_list(1,say_list)=posodd_coef;
            end
        end
    end
end

fclose(fid_coef);

say_oper=0;
is_checked=1;
oper_list=[];
say_partial=1;
partial_list=[1];

fid_solution=fopen(file_solution,'r');

while 1
    the_line=fgetl(fid_solution);

    if the_line==-1
        break
    else
        the_start=1;
        the_initial=1;
        say_oper=say_oper+1;

        while the_line(1,the_start)~='='
            the_start=the_start+1;
        end

        the_coef=str2num(char(the_line(1:1,the_initial:the_start-1)));

        say_partial=say_partial+1;
        partial_list(1,say_partial)=the_coef;
        oper_list(say_oper,1)=the_coef;

        the_start=the_start+8;
        the_initial=the_start;

        while the_line(1,the_start)~=','
            the_start=the_start+1;
        end

        first_partial=str2num(char(the_line(1:1,the_initial:the_start-1)));
        oper_list(say_oper,4)=first_partial;

        the_start=the_start+2;
        the_initial=the_start;

        while the_line(1,the_start)~=','
            the_start=the_start+1;
        end

        second_partial=str2num(char(the_line(1:1,the_initial:the_start-1)));
        oper_list(say_oper,7)=second_partial;
        
        the_start=the_start+2;
        the_initial=the_start;
        
        while the_line(1,the_start)~='/'
            the_start=the_start+1;
        end

        first_shift=str2num(char(the_line(1:1,the_initial:the_start-1)));
        if first_shift<0
            oper_list(say_oper,3)=-1;
        else
            oper_list(say_oper,3)=1;
        end
        oper_list(say_oper,5)=abs(first_shift)-1;
        
        the_start=the_start+1;
        the_initial=the_start;
        
        while the_line(1,the_start)~=','
            the_start=the_start+1;
        end

        second_shift=str2num(char(the_line(1:1,the_initial:the_start-1)));
        if second_shift<0
            oper_list(say_oper,6)=-1;
        else
            oper_list(say_oper,6)=1;
        end
        oper_list(say_oper,8)=abs(second_shift)-1;
        
        if and(abs(first_shift)-1==0,abs(second_shift)-1==0)
            [oper_list(say_oper,2),the_coef]=make_number_posodd(oper_list(say_oper,3)*oper_list(say_oper,4)+oper_list(say_oper,6)*oper_list(say_oper,7));
        else
            oper_list(say_oper,2)=0;
        end
        
        if oper_list(say_oper,1)*(2^oper_list(say_oper,2))~=oper_list(say_oper,3)*oper_list(say_oper,4)*(2^oper_list(say_oper,5))+oper_list(say_oper,6)*oper_list(say_oper,7)*(2^oper_list(say_oper,8))
            is_checked=0;
            fprintf(' \n');
            fprintf('Error: The %d. operation computes a wrong number. Quiting... \n', say_oper);
            fprintf(' \n');
            break
        end
    end
end

[say_partial,partial_list,say_oper,oper_list]=remove_redundant(say_list,coef_list,say_partial,partial_list,say_oper,oper_list);

fclose(fid_solution);
