#include "systemc.h"

struct _inout{
    char a;
    char b;
};


//
//top module
//
SC_MODULE(sample){
    sc_in <bool > clk,rst;
    sc_in<bool  >  sw;
    sc_in<sc_uint<6>  >  sw2;
    sc_in<struct _inout >  in1;
    sc_in<struct _inout  >  in2;
    sc_out<struct _inout >  out;
    
    void proc(void);
    SC_CTOR(sample){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();
    }
};


void sample::proc(void){
    while(1){
        struct _inout x=10;
        do{
            for(int i=0;i<100;i++){
                if(sw.read()){
                    switch(sw2.read()){
                        case 0  :out.write(in1.read());break;
                        case 1  :out.write(in2.read());break;
                        case 2  :out.write(in1.read());break;
                        default :out.write(in2.read());break;
                    }
                }else out.write(in2.read());
            }
        }while(1);
    }
}

