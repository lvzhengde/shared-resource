/*
  Message control.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "generic.h"

cMsg __sis_msg=NULL; // messages handler

//
// messages initialization
// 0x1.. : system
// 0x2.. : parsing
// 0x3.. : scheduling
//
void conMsg(void){
    cMsg mg;
    static cMsgTbl tbl[]={
    {0,__CMSG_ERROR,0x101,"'%s':no such file or directory.",NULL},
    {1,__CMSG_ERROR,0x102,"'%s' is not option for Sister.",NULL},
    {2,__CMSG_ERROR,0x201,"%s:%d:'%s':parse error.",NULL},
    {3,__CMSG_ERROR,0x103,"could not get mempry space when %s.",NULL},
    {4,__CMSG_ERROR,0x202,"%s:%d:process '%s' was not declared.",NULL},
    {5,__CMSG_WARNING,0x203,"%s:%d:SC_THREAD '%s' was ignored.",NULL},
    {6,__CMSG_WARNING,0x204,"%s:%d:SC_CTHREAD '%s' was ignored.",NULL},
    {7,__CMSG_ERROR,0x205,"%s:%d:SC_CTOR '%s' should be '%s'.",NULL},
    {8,__CMSG_ERROR,0x206,"%s:%d:Signal-key can not use pointer.",NULL},
    {9,__CMSG_ERROR,0x207,"%s:%d:Signak-key can not use initializer.",NULL},
    {10,__CMSG_ERROR,0x208,"%s:%d:SC_METHOD process arguments should be void %s.",NULL},
    {11,__CMSG_WARNING,0x209,"%s:%d:SC_METHOD process data type should be void.",NULL},
    {12,__CMSG_ERROR,0x210,"%s:%d:variables in the process can not use pointer.",NULL},
    {13,__CMSG_ERROR,0x211,"%s:%d:'%s' was not declared.",NULL},
    {14,__CMSG_ERROR,0x212,"%s:%d:'%s' instance declare should node use pointers.",NULL},
    {15,__CMSG_ERROR,0x213,"%s:%d:instance creation '%s' arguments should be a signal-name.",NULL},
    {16,__CMSG_ERROR,0x214,"%s:%d:sensitive should be signal-name.",NULL},
    {17,__CMSG_WARNING,0x215,"%s:%d:'%s%s' is out of pointer.",NULL},
    {18,__CMSG_ERROR,0x216,"%s:%d:'%s' arguments should be a signal-name.",NULL},
    {19,__CMSG_ERROR,0x217,"%s:%d:instance creation 'new %s' arguments should be a text.",NULL},
    {20,__CMSG_ERROR,0x218,"%s:%d:floating point can not be used in here.",NULL},
    {21,__CMSG_ERROR,0x219,"%s:%d:character strings can not be used in here.",NULL},
    {22,__CMSG_WARNING,0x220,"%s:%d:function '%s' number of argument should be one or zero.",NULL},
    {23,__CMSG_ERROR,0x221,"%s:%d:function '%s' number of argument should be one.",NULL},
    {24,__CMSG_ERROR,0x222,"%s:%d:can not trace structure declaration.",NULL},
    {25,__CMSG_ERROR,0x223,"%s:%d:data type casting is not supported.",NULL},
    {26,__CMSG_ERROR,0x224,"%s:%d:function '%s' is out of scope.",NULL},
    {27,__CMSG_ERROR,0x225,"%s:%d:should not use '%s' statement if SC_CTOR.",NULL},
    {28,__CMSG_ERROR,0x226,"%s:%d:'break' is on out of block.",NULL},
    {29,__CMSG_ERROR,0x227,"%s:%d:'continue' is on out of block.",NULL},
    {30,__CMSG_WARNING,0x228,"%s:%d:'return' should be void.",NULL},
    {31,__CMSG_WARNING,0x229,"%s:%d:in module '%s' reset signal(rst/reset) was not found.",NULL},
    {32,__CMSG_ERROR,0x230,"%s:%d:'%s' to output to the part selected variable is not supported",NULL},
    {33,__CMSG_ERROR,0x231,"%s:%d:'%s' is not a variable.",NULL},
    {34,__CMSG_ERROR,0x232,"%s:%d:'%s' to output to the bit selected variable is not supported",NULL},
    {35,__CMSG_ERROR,0x233,"%s:%d:'%s' array size is not match",NULL},
    {36,__CMSG_WARNING,0x234,"%s:%d:declaration '%s' is ignored",NULL},
    {37,__CMSG_ERROR,0x235,"%s:%d:instance name is too short.",NULL},
    {38,__CMSG_ERROR,0x236,"%s:%d:function '%s' is called as recursive.",NULL},
    {39,__CMSG_ERROR,0x237,"%s:%d:function '%s' the number of arguments is not match.",NULL},
    {40,__CMSG_ERROR,0x238,"%s:%d:'%s' array size should be constant value.",NULL},
    {41,__CMSG_ERROR,0x239,"%s:%d:zero division was found.",NULL},
    {42,__CMSG_ERROR,0x240,"%s:%d:function 'wait' the number of arguments should be zero or one.",NULL},
    {43,__CMSG_ERROR,0x241,"%s:%d:function 'wait' argument should be void or constant value.",NULL},
    {44,__CMSG_ERROR,0x242,"%s:%d:'%s' only single assigned pointer is supported.",NULL},
    {45,__CMSG_ERROR,0x243,"%s:%d:'%s' the pointer for a pointer is not supported.",NULL},
    {46,__CMSG_ERROR,0x104,"no such sister include directory.",NULL},
    {47,__CMSG_ERROR,0x244,"%s:%d:'%s' no such class/struct declaration.",NULL},
    {48,__CMSG_ERROR,0x245,"%s:%d:not constant value.",NULL},
    {49,__CMSG_ERROR,0x246,"%s:%d:'%s' can not trace structure variable.",NULL},
    {50,__CMSG_ERROR,0x247,"%s:%d:'range' function should be given 2 arguments.",NULL},
    {51,__CMSG_ERROR,0x248,"%s:%d:'range' can not trace the pointer.",NULL},
    {52,__CMSG_ERROR,0x301,"%s:%d:'%s' can not trace dependency.",NULL},
    {53,__CMSG_ERROR,0x249,"%s:%d:'%s' function was not declared.",NULL},
    {54,__CMSG_ERROR,0x250,"%s:%d:can not use 'sc_fifo'.",NULL},
    {55,__CMSG_ERROR,0x251,"%s:%d:can not use 'sc_fifo_in'.",NULL},
    {56,__CMSG_ERROR,0x252,"%s:%d:can not use 'sc_fifo_out'.",NULL},
    {57,__CMSG_WARNING,0x253,"%s:%d:'%s' can not synthesize the function in class defenition.",NULL},
    {58,__CMSG_WARNING,0x254,"%s:%d:'%s' can not use array to sc_fifo.",NULL},
    {59,__CMSG_WARNING,0x255,"%s:%d:'%s' wrong use to sc_fifo.",NULL},
    {60,__CMSG_WARNING,0x256,"%s:%d:'%s' ready/strobe signal should be used as loop condition.",NULL},
    {61,__CMSG_ERROR,0x257,"%s:%d:invalid syntax of if-else.",NULL},
    {62,__CMSG_WARNING,0x258,"%s:%d:can not expect ready/strobe loop on the switch state.",NULL},
    {63,__CMSG_ERROR,0x259,"invalid type of interface '%s'.",NULL},
    {64,__CMSG_ERROR,0x260,"%s:%d:signal '%s' invalid use.",NULL},
    {65,__CMSG_ERROR,0x261,"%s:%d:invalid declaration of port '%s'.",NULL},
    {66,__CMSG_ERROR,0x262,"%s:%d:tag '%s' is not defined.",NULL},
    };
    mg=cmsgIni(tbl,cmsgTblSize(tbl));
    __sis_msg= mg;
}

//
// message destruction
//

void desMsg(void){
    if(!__sis_msg) return;
    cmsgDes(__sis_msg);
}
//
// memory error hendling
//
void memError(char*s){
    cmsgEcho(__sis_msg,3,s);
    exit(-1);
}

