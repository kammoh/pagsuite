function is_member = is_member_of(set, value)

is_member = nnz(value==set)>0;