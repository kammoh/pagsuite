namespace simple_math {
    int pow(int b,int exp)
    {
        int d=b;
        while(exp>1){
            d*=b;
            exp--;
        }
        return d;
    }

    double pow(double b,double exp)
    {
        double d=b;
        while(exp>1){
            d*=b;
            exp-=1;
        }
        return d;
    }

    int fak(int x)
    {
        int res=1;
        for(int i=2;i<=x;++i)
        {
            res*=i;
        }
        return res;
    }

    double fak(double x)
    {
        double res=1;
        for(int i=2;i<=x;++i)
        {
            res*=(double)i;
        }
        return res;
    }
}
