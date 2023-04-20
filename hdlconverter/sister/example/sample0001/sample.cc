/*
    No.0001
    moving average
*/

#include <systemc.h>

//
//main module
//
#define TAP_SIZE 4
#define COUNTER_SIZE 3
#define BASE_BIT_SIZE 8
#define MAX_BIT_SIZE 12

#ifdef SISTER_SYNTHESIS
SC_MODULE(sample){
    //input
    sc_in<bool> clk,rst; //clock,reset
    sc_in<bool> w;//writing singal
    sc_in<sc_uint<BASE_BIT_SIZE> > in;//data input port
    
    //output
    sc_out<bool> r;//waiting data signal
    sc_out<sc_uint<BASE_BIT_SIZE> > out;//data output port
    
    //registers
    sc_signal<sc_uint<COUNTER_SIZE> > cnt; //counter
    sc_signal<sc_uint<BASE_BIT_SIZE> > bf[TAP_SIZE]; //buffer
    
    //functions
    void proc(void); //main process
    sc_uint<MAX_BIT_SIZE> totaler(void);//total calculation
    
    //constructor
    SC_CTOR(sample){
        SC_METHOD(proc);
        sensitive_pos<<clk;
        sensitive_neg<<rst;
    }
};

//
//total calculation
//
sc_uint<MAX_BIT_SIZE> sample::totaler(void){
    sc_uint<COUNTER_SIZE> i;
    sc_uint<MAX_BIT_SIZE> sum;
    for(i=0,sum=0;i<TAP_SIZE;i++) sum+=bf[i].read();
    return sum;
}

//
//main process
//
void sample::proc(void){
    sc_uint<MAX_BIT_SIZE> sum;

    //reset process
    if(!rst.read()){
        cnt=0;
        out=0;
        r=0;
        return ;
    }

    //wating input
    if(!w.read()) return ;
    
    //loading input data
    r=1;
    bf[cnt.read()]=in.read();
    if(cnt.read()==TAP_SIZE-1) cnt=0;
    else cnt=cnt.read()+1;

    //getting total value
    sum=totaler();
    out=sum/TAP_SIZE;
    r=sum[0]&0;
}

#else

#include "debug.cc"

#endif


//
//test bench
//
int sc_main(int argc,char**argv){
    int inval;

    sc_clock clk("CLOCK",1);
    sc_signal<bool> rst;
    sc_signal<bool> w;
    sc_signal<bool> r;
    sc_signal<sc_uint<BASE_BIT_SIZE> > in;
    sc_signal<sc_uint<BASE_BIT_SIZE> > out;
   
    sample* u0=new sample("u1");
    
    u0->clk(clk);
    u0->rst(rst);
    u0->w(w);
    u0->r(r);
    u0->in(in);
    u0->out(out);
    
    rst=0;
    sc_start(1);
    w=1;
    rst=1;

    while(cin>>hex>>inval){
       while(r.read()) sc_start(1);
       cout<<hex<<out<<endl;
       in=inval;
       sc_start(1);
    }

    return 0;
}

