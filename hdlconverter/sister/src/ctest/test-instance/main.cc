#include "systemc.h"

//
//dummy
//
SC_MODULE(sub1){
    sc_in<bool> clk;
    sc_in<unsigned char > in;
    void proc(void){ unsigned char x=in.read(); }
    SC_CTOR(sub1){
        SC_METHOD(proc);
        sensitive<<clk.pos();
    }
};
SC_MODULE(sub2){
    sc_in<bool> clk;
    sc_out<unsigned char > out;
    void proc(void){ out.write(1); }
    SC_CTOR(sub2){
        SC_METHOD(proc);
        sensitive<<clk.pos();
    }
};
//
//top module
//
SC_MODULE(sample){
    sc_in <bool > clk,rst;
    sc_signal<unsigned char >  sig;
    
    sub1* u0;
    sub2* u1;

    void proc(void);
    SC_CTOR(sample){
        u0=new sub1("u0");
        u0->in(sig);
        u0->clk(clk);

        u1=new sub2("u1");
        u1->out(sig);
        u1->clk(clk);

        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();
    }
};


void sample::proc(void){

}

//
//main
//
int sc_main(int argc,char**argv){

    return 0;
}
