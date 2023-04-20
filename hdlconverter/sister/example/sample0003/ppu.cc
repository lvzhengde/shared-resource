/*
    poor processer unit
*/

#include <systemc.h>
#include "ppu.h"

#define WORD_SIZE 8
#define RAM_SIZE 256

#define IN_PORT(SIZE) sc_in<sc_uint<SIZE> >
#define OUT_PORT(SIZE) sc_out<sc_uint<SIZE> >
#define SIGNAL(SIZE) sc_signal<sc_uint<SIZE> >

// -----------------------------------------------------------------
//RAM UNIT (I/O delay 1)
//
SC_MODULE(pr_ram){
    //reset and clock
    sc_in<bool> rst,clk;

    //I/O ports
    sc_in<bool>  we;
    IN_PORT(WORD_SIZE)  data_in;
    IN_PORT(WORD_SIZE)  addr_in;
    OUT_PORT(WORD_SIZE) data_out;

    //RAM
    SIGNAL(WORD_SIZE) ram[RAM_SIZE];

    //main process
    void proc(void){
        if(!rst.read()) data_out=0;
        else if(we.read()) ram[addr_in.read()]=data_in;
        else data_out=ram[addr_in.read()].read();
    }

    //constructor
    SC_CTOR(pr_ram){
        SC_METHOD(proc);
        sensitive_pos<<clk;
        sensitive_neg<<rst;
    }
};

// -----------------------------------------------------------------
//poor processer unit
//
SC_MODULE(ppu){
    //I/O ports
    sc_in<bool> rst,clk;

    SIGNAL(WORD_SIZE) pc,ir;
    SIGNAL(WORD_SIZE) din,dadr,dout;
    
    //registers
    sc_signal<bool> we,ZERO_BOOL;
    SIGNAL(WORD_SIZE) ax,bx,cx,dx;
    SIGNAL(WORD_SIZE) ZERO_WORD;

    //sub modules
    pr_ram* iram;
    pr_ram* dram;
   
    //next instruction
    void next_pc(sc_uint<WORD_SIZE> offset){
        pc=pc.read()+offset;
        wait(2);
    }
    //data loading
    sc_uint<WORD_SIZE> data_ld(sc_uint<WORD_SIZE> addr){
        switch(addr){
        case AX: return ax.read();
        case BX: return bx.read();
        case CX: return cx.read();
        case DX: return dx.read();
        case PC: return pc.read();
        default :
            if(addr<BASE_DATA_ADDR){
                sc_uint<WORD_SIZE> val;
                sc_uint<WORD_SIZE> tmp=pc;
                pc=addr-__BASE_ADDRESS__;
                wait(2);
                val=ir.read();
                pc=tmp;
                wait(2);
                return val;
            }else{
                dadr=addr-BASE_DATA_ADDR;
                wait(2);
                return dout.read();
            }
        }
    }

    void i_add(void);  //add
    void i_sub(void);  //sub
    void i_mul(void);  //mul
    void i_lsf(void);  //lshift
    void i_rsf(void);  //rshift

    void i_goto(void); //godo
    void i_jump(void); //jump
    void i_move(void); //move

    void proc(void); //main process
    void rst_proc(void); //main process
    
    //constructor
    SC_CTOR(ppu){
        iram=new pr_ram("IRAM_0");
        iram->rst(rst);
        iram->clk(clk);
        iram->we(ZERO_BOOL);
        iram->data_in(ZERO_WORD);
        iram->addr_in(pc);
        iram->data_out(ir);

        dram=new pr_ram("DRAM_0");
        dram->rst(rst);
        dram->clk(clk);
        dram->we(we);
        dram->data_in(din);
        dram->addr_in(dadr);
        dram->data_out(dout);
        
        SC_THREAD(proc);
        sensitive_pos<<clk;
        sensitive_neg<<rst;
    }
};
//reset process
void ppu::rst_proc(void){
    ax=0;
    bx=0;
    cx=0;
    dx=0;
    we=0;
    pc=0;
    din=0;
    dadr=0;
    ZERO_BOOL=0;
    ZERO_WORD=0;
}

//goto
void ppu::i_goto(void){
    next_pc(1);
    pc=ir.read()-__BASE_ADDRESS__;
}
//jump
void ppu::i_jump(void){
    sc_uint<WORD_SIZE> cond;
    next_pc(1);
    cond=data_ld(ir.read());
    if(cond) next_pc(1);
    else next_pc(2);
    pc=ir.read()-__BASE_ADDRESS__;
}
//move
void ppu::i_move(void){
    sc_uint<WORD_SIZE> addr;
    next_pc(1);
    addr=data_ld(ir.read());
    next_pc(1);
    switch(addr){
    case AX: ax=data_ld(ir.read());break;
    case BX: bx=data_ld(ir.read());break;
    case CX: cx=data_ld(ir.read());break;
    case DX: dx=data_ld(ir.read());break;
    case PC: pc=data_ld(ir.read());break;
    default :
        if(addr>=BASE_DATA_ADDR){
            dadr=addr-BASE_DATA_ADDR;
            din=data_ld(ir.read());
            we=1;
            wait(1);
            we=0;
        } break;
    }
}
//operation
#define OPERATION(ope) {      \
    sc_uint<WORD_SIZE> a,b;   \
    next_pc(1);               \
    a=data_ld(ir.read());     \
    next_pc(1);               \
    b=data_ld(ir.read());     \
    ax=a ope b;               }
void ppu::i_add(void){ OPERATION(+);  } //add
void ppu::i_sub(void){ OPERATION(-);  } //sub
void ppu::i_mul(void){ OPERATION(*);  } //mul
void ppu::i_lsf(void){ OPERATION(<<); } //lshift
void ppu::i_rsf(void){ OPERATION(>>); } //rshift

//main process
void ppu::proc(void){
    while(1){
        if(!rst.read()){
            rst_proc();
            wait(1);
            continue;
        }
        wait(2);
        switch(ir.read()){
        case HLT : pc=pc.read();continue;
        case JMP : i_jump(); continue;
        case GOT : i_goto(); continue;
        case MOV : i_move(); break;
        case ADD : i_add(); break;
        case SUB : i_sub(); break;
        case MUL : i_mul(); break;
        case LSF : i_lsf(); break;
        case RSF : i_rsf(); break;
        default  :break;
        }
        next_pc(1);
    }
}

// -----------------------------------------------------------------
//simulation main
//
int sc_main(int argc,char**argv){
    sc_clock clk;
    sc_signal<bool> rst;
    sc_signal<bool> wake;

    ppu* u0=new ppu("UNIT0");
    u0->clk(clk);
    u0->rst(rst);
    
    int i,val;
    for(i=0;cin>>hex>>val;i++)
        u0->iram->ram[i]=val;

    rst=0;
    wake=1;
    sc_start(0);
    rst=1;
    sc_start(1024);
    
    for(i=0;i<10;i++)
        cout<<hex<<u0->dram->ram[i].read()<<endl;
    return 0;
}

