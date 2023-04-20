/*
    test enum
*/

#include "systemc.h"
void ftest1(int &a,int b){
    a+=b;
}
void ftest2(int *a,int b){
    *a+=b;
}
typedef int nnn;
void ftest3(nnn &a,int b){
    a+=b;
}
void ftest4(nnn *a,int b){
    *a+=b;
}

typedef struct _mmm{
    int s;
}ooo ;

void ftest5(ooo &a,int b){
    a.s+=b;
}
void ftest6(ooo *a,int b){
    a->s+=b;
}



SC_MODULE(test_function){
    
    sc_in<char> in;
    sc_out<char> out;
    void proc(void);

    SC_CTOR(test_function){
        SC_THREAD(proc);
        sensitive<<in;
    }
};


//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}

void test_function::proc(void){
    int x=1;
    int y=2;
    ooo z;
    int v[2];
    ooo w[2];
    z.s=3;
    v[0]=4;
    w[0].s=5;

    ftest1(x,y);

    ftest2(&x,y);
    ftest3(x,y);
    ftest4(&x,y);
    ftest5(z,y);
    ftest6(&z,y);
    ftest1(v[0],y);
    ftest2(&v[0],y);
    ftest5(w[0],y);
    ftest6(&w[0],y);
    out.write(x);
}



