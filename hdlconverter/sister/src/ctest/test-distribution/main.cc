/*
    No.0001
    moving average
*/

#include <systemc.h>

//
//main module
//

SC_MODULE(sample){
    //input
    sc_in<sc_uint<8> > in1,in2,in3;
    sc_out<sc_uint<16> > out;

    void proc(void);
    SC_CTOR(sample){
        SC_METHOD(proc);
        sensitive<<in1<<in2<<in3;
    }
};

//
//process
//
void sample::proc(void){
    sc_uint<16> tmp;
    sc_uint<8> x=in1.read(),y=in2.read(),z=in3.read();
    tmp=x*z+y*z;
    tmp=x*z-z*y;
    tmp=z*x-z*y;
    tmp=z*x-y*z;
    tmp=z/x-y/z;
    tmp=x/z-y/z;
    out.write(tmp);
}



//
//sc_main
//
int sc_main(int argc,char**argv){


}

