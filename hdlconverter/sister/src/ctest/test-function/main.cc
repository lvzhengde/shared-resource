/*
    test enum
*/

#include "systemc.h"
int ftest1(int a,int b);
extern int ftest3(int a,int b);
int ftest2(int a,int b){
    return a-b;
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


void test_function::proc(void){
    int x=10;
    int y=20;
    int n,m,o;
    m=ftest1(x,y);
    n=ftest2(x,y);
    o=ftest3(x,y);
    out.write(n+m+o);
}

int ftest1(int a,int b){
    return a+b;
}
/*
//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}
*/
