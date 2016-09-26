#ifndef MT_MATRIX_CALC_H
#define MT_MATRIX_CALC_H

#include <vector>
using namespace std;

class mt_matrix_calc{

private:
    int* offsets;
    void* data;
    int c,n;

    void calc_offsets()
    {
        //offsets = (int*)calloc(c,sizeof(int));

        int t=1;
        for(int i=c-1;i>0;--i)
        {
            offsets[i]=t;
            t *= n;
        }
    }

public:
    mt_matrix_calc(int cfg_count,int node_count)
    {
        c = cfg_count;
        n = node_count;
        data = calloc(c,sizeof(int));
        offsets = (int*)data;
        calc_offsets();
    }

    ~mt_matrix_calc()
    {
        free(data);
    }

    void calc_int_to_vec(int val,int* vec)
    {
        for(int i=1;i<c;++i)
        {
            vec[i]=0;
            while(val>=offsets[i]){ val -= offsets[i]; vec[i]++;}
        }
    }

    template<typename T>
    void calc_int_to_vec(int val,vector<T>& vec,int row)
    {
        vec[0] = row;
        for(int i=1;i<c;++i)
        {
            vec[i] = 0;
            while(val>=offsets[i]){ val -= offsets[i]; vec[i]++;}
        }
    }

    template<typename T>
    int calc_vec_to_int(vector<T>& vec)
    {
        int res=0;
        for(int i=1;i<c;++i)
        {
            res += vec[i] * offsets[i];
        }
        return res;
    }

    template<typename T>
    static void make_vec_from_column(vector<vector<T> >& mat,int col,vector<T>& dest){
        dest.reserve(mat.size());
        for(uint i=0, i_end = mat.size();i<i_end;++i)
            dest.push_back( mat[i][col] );

    }
};



#endif // MT_MATRIX_CALC_H
