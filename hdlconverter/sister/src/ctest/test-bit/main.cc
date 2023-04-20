/*
    test enum
*/

#include "systemc.h"

SC_MODULE(sample){
    
    sc_in<bool> rst,clk;
    sc_in<sc_int<24> > in;
    sc_out<sc_int<10> > out;
    void proc(void);

    SC_CTOR(sample){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();
    }
};

void sample::proc(void){
    while(1){
        out.write(in.read()(23,16)+in.read()(15,8)+in.read()(7,0));
        wait(1);
    }
}

//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}




