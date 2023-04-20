/*
    XMODEM (only SOH)
*/

#include "systemc.h"
#ifndef XMODEM_HEAD
#define XMODEM_HEAD

/*=====================================================

    global

======================================================*/
//
// control code
//
#define SOH 1
#define STK 2
#define EOT 3

#define ACK 1
#define NAK 2

#define CAN 7

//
//check sum
//
static sc_uint<8> xmodem_check_sum(sc_uint<8> d[16]){
    sc_uint<6>  i;
    sc_uint<10> sum=0;

    for(i=0;i<16;i++){
        sum+=d[i];
    }
    return sum(7,0);
}


/*=====================================================

    sender

======================================================*/
//
//sender module
//
SC_MODULE(XODEM_sender){
    //input
    sc_in<bool> clk,rst;                //from driver
    sc_fifo_in<sc_uint<8> > data_in;    //from driver
    sc_fifo_in<sc_uint<8> > data_size;  //from driver
    sc_fifo_in<sc_uint<8> > receive_in; //from receiver

    //output
    sc_fifo_out<sc_uint<8> > data_out;  //to receiver

    //process
    void proc(void);

    //constructor
    SC_CTOR(XODEM_sender){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();

    }
};

//
// sender process
//
void XODEM_sender::proc(void){
    while(1){
        sc_uint<8> head;
        sc_uint<8> block_num;
        sc_uint<8> re_block_num;
        sc_uint<8> send_buf[16];
        sc_uint<8> chk_sum;

        sc_uint<6> i;     //loop conuter
        sc_uint<8> ct;    //conuter
        sc_uint<8> dsize; //data_size
        sc_uint<8> re;    //recive control

        //reset 
        if(!rst.read()){
            data_out.write(0);
        }

        //waiting NAK
        while(receive_in.read()!=NAK);

        //getting data size
        dsize=data_size.read();

        for(ct=0;ct<dsize;ct++){

            //making data
            head=SOH;
            block_num=ct;
            re_block_num=~ct;
            for(i=0;i<16;i++) send_buf[i]=data_in.read();
            chk_sum=xmodem_check_sum(send_buf);
            
            //checking reciver
            while(1){
                //send data 
                data_out.write(head);
                data_out.write(block_num);
                data_out.write(re_block_num);
                for(i=0;i<16;i++) data_out.write(send_buf[i]);
                data_out.write(chk_sum);

                //receive data
                re=receive_in.read();
                if(re==ACK) break;
                else if(re==NAK) continue;
                else if(re==CAN) break;
            }
            if(re==CAN) break;
        }
        
        //send EOT
        if(re!=CAN) data_out.write(EOT);
    }
}

/*=====================================================

    receiver

======================================================*/
//
//reciver module
//
SC_MODULE(XODEM_receiver){
    //input
    sc_in<bool> clk,rst;                //from driver
    sc_fifo_in<sc_uint<8> > data_in;    //from sender

    //output
    sc_fifo_out<sc_uint<8> > data_out;    //to driver
    sc_fifo_out<sc_uint<8> > receive_out; //to sender

    //process
    void proc(void);

    //constructor
    SC_CTOR(XODEM_receiver){
        SC_THREAD(proc);
        sensitive<<clk.pos()<<rst.neg();

    }
};

//
// receiver process
//
void XODEM_receiver::proc(void){
    while(1){
        sc_uint<8> head;
        sc_uint<8> block_num;
        sc_uint<8> re_block_num;
        sc_uint<8> rece_buf[16];
        sc_uint<8> chk_sum;
        sc_uint<8> chk_sum2;

        sc_uint<6> i;     //loop conuter
       
        //reset
        if(!rst.read()){
            data_out.write(0);    //to driver
            receive_out.write(0); //from sender
        }

        //sending NAK
        receive_out.write(NAK);

        while(1){
            //getting data
            head=data_in.read();
            if(head==SOH){
                block_num=data_in.read();
                re_block_num=data_in.read();
                for(i=0;i<16;i++) rece_buf[i]=data_in.read();
                chk_sum=data_in.read();
                
                //checking data 
                chk_sum2=xmodem_check_sum(rece_buf);
                if(chk_sum2!=chk_sum){
                    receive_out.write(NAK);
                    continue;
                }

                //sending data to driver
                for(i=0;i<16;i++) data_out.write(rece_buf[i]);

                //sending ack
                receive_out.write(ACK);

            }else if(head==EOT) break;
            else if(head==CAN) break;
            else receive_out.write(NAK);
        }
    }
}
#endif

