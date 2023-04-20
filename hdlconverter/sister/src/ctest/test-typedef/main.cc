/*
    test enum
*/

#include "systemc.h"

typedef struct nnn{
    char aaa;
    char bbb,ccc;
}xxx;
typedef struct yyy{
    char aaa;
    char bbb;
    struct{
        char ccc;
        char ddd;
    }eee,fff;
}zzz;
typedef struct yyy www;
typedef sc_int<6> mmm;

//
//definition
//

SC_MODULE(test_typedef){

    sc_in<bool> in;
    sc_out<bool> out;
    sc_signal<mmm> sig;

    mmm ftest(mmm a,mmm b);
    xxx ftest2(xxx a,xxx b);
    void proc(void);

    SC_CTOR(test_typedef){
        SC_THREAD(proc);
        sensitive<<in;
    }
};


void test_typedef::proc(void){
    typedef int nnn;
    nnn a;
    www b;
    xxx c;
    zzz d;
    mmm e,f,g;
    xxx h,i,j;
    nnn k[2],l[2],m,n,o;
    
    a=10;
    b.aaa=20;
    c.bbb=30;
    c.ccc=40;
    d.fff.ddd=50;
    e=60;
    f=70;
    g=ftest(e,f);
    h.aaa=80;
    h.bbb=90;
    h.ccc=100;
    i.aaa=110;
    i.bbb=120;
    i.ccc=130;
    j=ftest2(h,i);
    k[0]=140;
    k[1]=150;
    l[0]=160;
    l[1]=170;
    k[1]=180;
    l[1](5,3)=190;
    m=k[0]+l[1](2,1);
    l[1](4,2)=190;
    m=k[0]+l[1](2,1);
    n=k[1]+l[1];
    sig.write(e);
    
}
mmm test_typedef::ftest(mmm a,mmm b){
    return a+b;
}
xxx test_typedef::ftest2(xxx a,xxx b){
    xxx c;
    c.aaa=a.aaa+b.aaa;
    c.bbb=a.bbb+b.bbb;
    c.ccc=a.ccc+b.ccc;
    return c;
}

//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}

