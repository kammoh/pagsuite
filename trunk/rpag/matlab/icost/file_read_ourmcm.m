function [is_checked,say_list,say_partial,say_oper,coef_list,partial_list,oper_list] = file_read_ourmcm(file_solution)

fid_solution=fopen(file_solution,'r');

while 1
    tline= fgetl(fid_solution);
    if or(strcmp(tline, '*** Implementation of Coefficients ***'),strcmp(tline, '*** Implementation of Filter Coefficients ***'))
        break
    end
end

say_list=0;
coef_list=[];

fgetl(fid_solution);

while 1
    tline=fgetl(fid_solution);
    
    if not(isempty(tline))
        the_initial=1;
        while tline(1,the_initial)~='='
            the_initial=the_initial+1;
        end

        the_coef=str2num(char(tline(1:1,1:the_initial-1)));
        [power,posodd_coef]=make_number_posodd(the_coef);
        
        if the_coef
            if not(is_inside(posodd_coef,say_list,coef_list))
                say_list=say_list+1;
                coef_list(1,say_list)=posodd_coef;
            end
        end
    else
        break
    end
end

while 1
    tline= fgetl(fid_solution);
    if strcmp( tline, '*** Implementation of Partial Terms ***' )
        break
    end
end

is_checked=1;
first_line=1;
say_partial=1;
partial_list=[1];

say_oper=0;
oper_list=[];

while 1
    the_line=fgetl(fid_solution);

    if and(strcmp(the_line, '')==1,first_line==0)
        break
    elseif the_line==-1
        break
    elseif and(strcmp(the_line, '')==1,first_line==1)
        the_line=fgetl(fid_solution);
    end
    
    first_line=0;

    the_start=1;
    the_initial=1;
    say_oper=say_oper+1;

    while the_line(1,the_start)~='>'
        the_start=the_start+1;
    end

    the_coef=str2num(char(the_line(1:1,the_initial:the_start-1)));

    say_partial=say_partial+1;
    partial_list(1,say_partial)=the_coef;
    oper_list(say_oper,1)=the_coef;

    the_start=the_start+2;
    the_initial=the_start;

    while the_line(1,the_start)~='='
        the_start=the_start+1;
    end

    shift_coef=str2num(char(the_line(1:1,the_initial:the_start-1)));
    oper_list(say_oper,2)=shift_coef;

    the_start=the_start+2;

    if the_line(1,the_start)=='+'
        oper_list(say_oper,3)=1;
    elseif the_line(1,the_start)=='-'
        oper_list(say_oper,3)=-1;
    end

    the_start=the_start+1;
    the_initial=the_start;

    while the_line(1,the_start)~='<'
        the_start=the_start+1;
    end

    first_partial=str2num(char(the_line(1:1,the_initial:the_start-1)));
    oper_list(say_oper,4)=first_partial;

    the_start=the_start+2;
    the_initial=the_start;

    while and(the_line(1,the_start)~='+',the_line(1,the_start)~='-')
        the_start=the_start+1;
    end

    first_shift=str2num(char(the_line(1:1,the_initial:the_start-1)));
    oper_list(say_oper,5)=first_shift;

    if the_line(1,the_start)=='+'
        oper_list(say_oper,6)=1;
    elseif the_line(1,the_start)=='-'
        oper_list(say_oper,6)=-1;
    end

    the_start=the_start+1;
    the_initial=the_start;
    while the_line(1,the_start)~='<'
        the_start=the_start+1;
    end

    second_partial=str2num(char(the_line(1:1,the_initial:the_start-1)));
    oper_list(say_oper,7)=second_partial;

    the_start=the_start+2;
    second_shift=str2num(char(the_line(1:1,the_start:length(the_line))));
    oper_list(say_oper,8)=second_shift;
    
    if oper_list(say_oper,1)*(2^oper_list(say_oper,2))~=oper_list(say_oper,3)*oper_list(say_oper,4)*(2^oper_list(say_oper,5))+oper_list(say_oper,6)*oper_list(say_oper,7)*(2^oper_list(say_oper,8))
        is_checked=0;
        fpritnf(' \n');
        fprintf('Error: The %d. operation computes a wrong number. Quiting... \n', say_oper);
        fpritnf(' \n');
        break
    end
end

fclose(fid_solution);
