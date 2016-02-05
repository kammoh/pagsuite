%clc
%dec=211;
function msd_set = dec2msd(dec)

msd = dec2csd(dec);

msd_start_set=msd;
msd_next_set=[];
msd_set=[];
msd_found=true;

while ~isempty(msd_start_set)
  msd_start_set_size = size(msd_start_set);
  for k=1:msd_start_set_size(1)
    msd = msd_start_set(k,:);
    for i=1:length(msd)-2
      if (msd(i)==-1) && (msd(i+1)==0) && (msd(i+2)==1)
        msd_next = msd;
        msd_next(i)=1;
        msd_next(i+1)=1;
        msd_next(i+2)=0;
        msd_next_set = [msd_next_set;msd_next];
        msd_found=true;
      elseif (msd(i)==1) && (msd(i+1)==0) && (msd(i+2)==-1) 
        msd_next = msd;
        msd_next(i)=-1;
        msd_next(i+1)=-1;
        msd_next(i+2)=0;
        msd_next_set = [msd_next_set;msd_next];
        msd_found=true;
      end
    end
  end
  msd_set = [msd_set;msd_start_set];
  msd_start_set = msd_next_set;
  msd_next_set=[];
end

msd_set = unique(msd_set,'rows');
return

%check solution (optional):
msd_start_set_size=size(msd_set);
for i=1:msd_start_set_size(1)
  disp(['msd=[',num2str(msd_set(i,:)),'] -> dec=',num2str(csd2dec(msd_set(i,:)))]);
  assert(dec == csd2dec(msd_set(i,:)));
end