/*
    test enum
*/

#include "systemc.h"

SC_MODULE(sample){
    
    sc_in<bool> rst,clk;
    sc_in<char> in;
    sc_out<char> out;
    void proc(void);

    SC_CTOR(sample){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();
    }
};

void sample::proc(void){
    out.write(in.read());
}

//
//test bench
//
int sc_main(int argc,char**argv){
    
    return 0;
}




