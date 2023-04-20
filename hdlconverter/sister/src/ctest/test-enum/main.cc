/*
    test enum
*/

#include "systemc.h"

//
//definition
//
enum     { };
enum Tag0{ };
enum     { }Var0;
enum Tag1{ }Var1;

enum     { e0,    };
enum Tag2{ e1, e2 };
enum     { e4     }Var2;

enum { e7   , e8=10            };
enum { e9=4 , e10,             };
enum { e11=5, e12  ,e13=3, e14 };

SC_MODULE(test_enum){
    enum Tag3{ e11=1, e12  ,e13=15, e14 };
    enum {e20=10};
    sc_in<bool> in;
    sc_out<bool> out;
    void proc(void);

    SC_CTOR(test_enum){
        SC_THREAD(proc);
        sensitive<<in;
    }
};

void test_enum::proc(void){
    enum Tag2 { e11=30 , e12  ,e13=40, e14 }a,b,c;
    enum Tag3{ e5, e6,};
    enum {};
    Tag3 v0;
    int x;
    v0=e6;
    a=e11;
    b=e14;
    x=e20;
    out.write(a);
}

//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}

