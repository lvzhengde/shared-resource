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
    sc_in<sc_uint<3> > sel;
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
    sc_uint<8>  x;
    sc_uint<16> y;
    x=10;
    if(!sel.read())   y=in1.read()+in2.read()+in3.read();
    else if(sel.read()==1) y=in1.read()+in2.read()*in3.read();
    else if(sel.read()==2) y=in1.read()*in2.read()+in3.read();
    else{
        if(sel.read()==3) y=in1.read()-in2.read()-in3.read();
        else if(sel.read()==4) y=in1.read()-in2.read()+in3.read();
        else y=in1.read()+in2.read()-in3.read();
    }
    out.write(x+y);
}



//
//sc_main
//
int sc_main(int argc,char**argv){


}

