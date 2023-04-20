#include "xmodem_bca.cc"

SC_MODULE(XODEM_test){
    
    sc_in<bool > clk,rst;

    sc_signal<bool > data_in_ready;
    sc_signal<bool > data_in_strobe;
    sc_signal<sc_uint<8> > data_in;
    sc_signal<sc_uint<8> > data_size;

    sc_signal<bool > data_sig_ready;

    sc_signal<sc_uint<8> > data_sig;
    sc_signal<sc_uint<8> > receive_sig;

    sc_signal<bool > data_out_ready;
    sc_signal<bool > data_out_strobe;
    sc_signal<sc_uint<8> > data_out;

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
        s0->data_in_ready(data_in_ready);
        s0->data_in_strobe(data_in_strobe);
        s0->data_size(data_size);

        s0->receive_in(receive_sig);
        s0->data_out(data_sig);
        s0->data_out_ready(data_sig_ready);

        r0=new XODEM_receiver("r0");
        r0->clk(clk);
        r0->rst(rst);
        r0->data_in(data_sig);
        r0->data_in_ready(data_sig_ready);
        r0->receive_out(receive_sig);
        r0->data_out(data_out);
        r0->data_out_ready(data_out_ready);
        r0->data_out_strobe(data_out_strobe);
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
        //setting data size
        data_size.write(dsize);

        //writing data
        data_in_ready.write(1);
        while(!data_in_strobe.read()) wait(1);
        for(j=0;j<16;j++){
            data_in.write(data[i*16+j]);
            wait(1);
        }
        data_in_ready.write(0);
        while(data_in_strobe.read()) wait(1);

        //getting data
        while(!data_out_ready.read()) wait(1);
        data_out_strobe.write(1);
        wait(1);
        for(j=0;j<16;j++){
            wait(1);
            sc_uint<8> d=data_out.read();
            cout<<d<<endl;
        }
        while(data_out_ready.read()) wait(1);
        data_out_strobe.write(0);
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

    sc_start(sc_time(40,SC_NS)*500);
    return 0;
}


