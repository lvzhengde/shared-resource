/*
    test enum
*/

#include "systemc.h"

//
//definition
//
struct aaa{ };
struct { }bbb;
struct bbb{
    int a;
    int b;
}ddd;
struct ccc{ 
    char a[1+1][3];
    char b;
};
struct ddd{
    sc_uint<15> a;
    char b;
    struct aaa{
        sc_uint<8> e[2];
        struct {
            char g;
            sc_uint<8> h;
            sc_uint<8> i[2];
        }f[2];
    }c;
    struct ccc d,e;
};
    
SC_MODULE(test_struct){
    
    sc_in<bool> in;
    sc_out<struct ddd> out;
    struct ddd ftest(struct ddd a,struct ddd b);
    void proc(void);

    SC_CTOR(test_struct){
        SC_THREAD(proc);
        sensitive<<in;
    }
};


void test_struct::proc(void){
    struct ddd a,b[2],c;
    struct ccc{ 
        char a[2+3][4];
        char b;
    }d,e;
    
    a.a=0;
    a.b=1;
    a.c.e[0]=2;
    a.c.e[1]=3;
    a.c.f[0].g=4;
    a.c.f[0].h=5;
    a.c.f[0].i[0]=6;
    a.c.f[0].i[1]=7;
    a.c.f[1].g=8;  //78:71
    a.c.f[1].h=9;
    a.c.f[1].i[0]=10;
    a.c.f[1].i[1]=11;
    a.d.a[0][0]=12;
    a.d.a[0][1]=13;
    a.d.a[0][2]=14;
    a.d.a[1][0]=15;
    a.d.a[1][1]=16;
    a.d.a[1][2]=17;
    a.d.b=18;
    a.e.a[0][0]=19;
    a.e.a[0][1]=20;
    a.e.a[0][2]=21;
    a.e.a[1][0]=22;
    a.e.a[1][1]=23;
    a.e.a[1][2]=24;
    a.e.b=25;
    c=ftest(a,b[0]);
    out.write(c);
}


struct ddd test_struct::ftest(struct ddd x,struct ddd y){
       x.c.f[1].i[1](4,1)=26;
       x.c.e[1].range(5,2)=27;
       y=x;
       return x;
}

//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}

