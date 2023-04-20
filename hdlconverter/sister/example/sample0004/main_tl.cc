#include "xmodem_tl.cc"

SC_MODULE(XODEM_test){
    
    sc_in<bool > clk,rst;
    sc_fifo<sc_uint<8> > data_in;
    sc_fifo<sc_uint<8> > data_out;
    sc_fifo<sc_uint<8> > data_sig;
    sc_fifo<sc_uint<8> > data_size;
    sc_fifo<sc_uint<8> > receive_sig;

    XODEM_sender*   s0;
    XODEM_receiver* r0;

    void proc(void);

    SC_CTOR(XODEM_test){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();

        s0=new XODEM_sender("s0");
        s0->clk(clk);
        s0->rst(rst);
        s0->data_in(data_in);
        s0->data_size(data_size);
        s0->receive_in(receive_sig);
        s0->data_out(data_sig);

        r0=new XODEM_receiver("r0");
        r0->clk(clk);
        r0->rst(rst);
        r0->data_in(data_sig);
        r0->data_out(data_out);
        r0->receive_out(receive_sig);
    }

};

void XODEM_test::proc(void){
    int i,j;
    sc_uint<8> dsize=2;
    sc_uint<8> data[]={
         0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
        16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        };
    for(i=0;i<dsize;i++){
        data_size.write(dsize);
        for(j=0;j<16;j++) data_in.write(data[i*16+j]);
        for(j=0;j<16;j++){
            sc_uint<8> d=data_out.read();
            cout<<d<<endl;
        }
    }
}

int sc_main(int argc,char**argv){
    sc_signal<bool> rst;
    sc_clock clk("CLOCK",sc_time(40,SC_NS));

    XODEM_test* t0=new XODEM_test("t0");
    t0->rst(rst);
    t0->clk(clk);

    rst.write(0);
    rst.write(1);

    sc_start(sc_time(40,SC_NS)*10);
    return 0;
}


