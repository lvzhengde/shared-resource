/*
    instruction table
*/

#ifndef __SAMPLE_ASM_HEAD 
#define __SAMPLE_ASM_HEAD 

/*$INSTRUCTION>> from here.DONT REMOVE THIS COMMENT*/
#define HLT 0x000  //OPERAND 0
#define JMP 0x001  //OPERAND 3
#define GOT 0x002  //OPERAND 1
#define MOV 0x003  //OPERAND 2
#define ADD 0x004  //OPERAND 2
#define SUB 0x005  //OPERAND 2
#define MUL 0x006  //OPERAND 2
#define LSF 0x007  //OPERAND 2
#define RSF 0x008  //OPERAND 2
/*$INSTRUCTION<< to here DONT REMOVE THIS COMMENT*/

/*$REGISTER>> from here.DONT REMOVE THIS COMMENT*/
#define AX 0x000
#define BX 0x001
#define CX 0x002
#define DX 0x003
#define PC 0x004
/*$REGISTER<< to here DONT REMOVE THIS COMMENT*/

/*$BASEADDRESS>> from here.DONT REMOVE THIS COMMENT*/
#define __BASE_ADDRESS__ 64
/*$BASEADDRESS<< to here DONT REMOVE THIS COMMENT*/

#define BASE_DATA_ADDR 128
#endif

