/*
   No0002
   Gray scale.

*/

#include <systemc.h>

#define R_WAI 306
#define G_WAI 600
#define B_WAI 117

//sample module
SC_MODULE(sample){
    sc_in<bool> clk,rst;
    sc_in<bool> we;
    sc_in<sc_uint<8> > r,g,b;
    sc_out<bool> re;
    sc_out<sc_uint<8> > out;

    
    sc_signal< sc_uint<18> >y;
    sc_signal<sc_uint<2> > state;

    //reset
    #define RESET {    \
        y.write(0);    \
        re.write(0);   \
        out.write(0);  \
        state.write(0);\
        return ;       \
    }
    //to gray scale
    sc_uint<18> gscale(
        sc_uint<8> r,sc_uint<8> g,sc_uint<8> b){
        return (r*R_WAI)+(g*G_WAI)+(b*B_WAI);
    }
    //main process
    void proc(void){
        if(!rst.read()) RESET;
        switch(state.read()){
        case 0 : //waiting data
            if(we.read()){
                re.write(1);
                y.write(gscale(r.read(),g.read(),b.read()));
                state.write(1);
            }
            break;
        case 1 : //output data
            re.write(0);
            out.write(y.read()/1024);
            state.write(0);
            break;
        }
    }
    //constructor
    SC_CTOR(sample){
        SC_METHOD(proc);
        sensitive_pos<<clk;
        sensitive_neg<<rst;
    }
};


//
//main
//
int sc_main(int argc,char**argv){
    int rv,gv,bv;

    sc_clock clk("CLK",1);
    sc_signal<bool> rst;
    sc_signal<bool> we;
    sc_signal<sc_uint<8> > r,g,b;
    sc_signal<bool> re;
    sc_signal<sc_uint<8> > out;
    
    sample* u0=new sample("u0");
    u0->clk(clk);
    u0->rst(rst);
    u0->we(we);
    u0->re(re);
    u0->r(r);
    u0->g(g);
    u0->b(b);
    u0->out(out);

    rst=0;
    sc_start(1);

    rst=1;
    we=1;

    while(cin>>hex>>rv){
        cin>>hex>>gv;
        cin>>hex>>bv;
        r=rv;
        g=gv;
        b=bv;
        sc_start(1);
        while(re) sc_start(1);
        cout<<hex<<out<<endl;
        cout<<hex<<out<<endl;
        cout<<hex<<out<<endl;
    }

    delete u0;
    return 0;
}



