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
    while(1){
        int i[3];
        out.write((sc_int<2>)in.read());
        i[(int)in.read()]=10;
        wait(1);
    }
}


