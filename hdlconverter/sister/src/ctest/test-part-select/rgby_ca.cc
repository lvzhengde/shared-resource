/*
    RGB->Y CA model.
*/
#ifndef __RGBY_CA_CC__DECLARE
#define __RGBY_CA_CC__DECLARE
#include "systemc.h"

//=================================================================
// module rgb->y
//==================================================================
SC_MODULE(rgby){

    //ports -----------------------------------------------
    sc_in<bool > clk;
    sc_in<bool > rst;

    sc_in<sc_uint<24> >   rgb;
    sc_in<bool >          rgb__ready;

    sc_out<sc_uint<16> >  y;
    sc_out<bool >         y__ready;
    

    //registers -------------------------------------------
    sc_signal<sc_uint<4> >  state;
    sc_signal<sc_uint<24> > inbuf;

    sc_signal<sc_uint<26> > t5;
    sc_signal<sc_uint<29> > t4;
    sc_signal<sc_uint<28> > t3;
    sc_signal<sc_uint<31> > t1_2;
    
    sc_signal<bool> sw;

    //DATAPATH process ------------------------------------
    void datapath(void){
        if(!rst.read()){ //reset
            t1_2.write(0);
            t3.write(0);
            t4.write(0);
            t5.write(0);
        }else{ //process start
            t5.write(inbuf.read()( 7, 0)<<2);    
            t4.write(inbuf.read()(15, 8)*19);    
            t3.write(inbuf.read()(23,16)*10);    
            if(sw.read()) t1_2.write(t3.read()+t4.read());
            else t1_2.write(t1_2.read()+t5.read());
        }
    }
    
    //controler(FSM) process ------------------------------
    void control(void){
        //reset
        if((!rst.read())) {
            y.write(0);
            y__ready.write(0);
            state.write(0);
        //process start
        }else{
            switch(state.read()){
            case 0 :
                if((!rgb__ready.read())) state.write(0);
                else {
                    inbuf.write(rgb.read());
                    state.write(1);
                }
                break;
            case 1 :
                if(rgb__ready.read()) state.write(1);
                else state.write(2);
                break;
            case 2 :
                sw.write(1);
                state.write(3);
                break;
            case 3 :
                sw.write(0);
                state.write(4);
                break;
            case 4 :
                state.write(5);
                break;
            case 5 :
                y__ready.write(1);
                y.write(t1_2.read());
                state.write(6);
                break;
            default :
                y__ready.write(0);
                state.write(0);
                break;
            }
        }
    }

    //constructor -----------------------------------------
    SC_CTOR(rgby){

        SC_METHOD(control);
        sensitive<<clk.pos()<<rst.neg();

        SC_METHOD(datapath);
        sensitive<<clk.pos()<<rst.neg();
    }
};

#endif

