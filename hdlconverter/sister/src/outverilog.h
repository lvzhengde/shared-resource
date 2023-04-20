/*
  to output as Verilog HDL format .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __OUT_VERILOG_HEAD
#define __OUT_VERILOG_HEAD

//
// handler for to output verilog
//
typedef struct overiWork{
    FILE*fp;
    cgrNode proc;
    int indent;
}overiWork;
//handler accesser
#define overiProp(self,item) (((overiWork*)(self->work))->item)

//init
extern fsmdHandle overiIni(char*outfile,cgrNode top);
//destruct
extern void overiDes(fsmdHandle fsmd);

//to output Verilog HDL
extern int outverilog(char*outfile,cgrNode top);

//to output operator
extern int overiOutOpe(FILE*fp,cgrNode node);

//to output operation with switch
//0 :only non blocking assignment, 1:with blocking assignment
int overiOutOpe2(FILE*fp,cgrNode node,int sw);

//functions
extern int overiModule(fsmdHandle self,cgrNode node);

// output option string
extern int outOpString(FILE*fp,int sw,cgrNode node);

//sensetive
#define overiSens(fp,node,key,str,f) {       \
    cgrNode sen=cgrGetNode(node,key);        \
    if(sen){                                 \
        char*name;                           \
        if(!f){                              \
            name=cgrGetStr(sen,cgrKeyVal);   \
            fprintf(fp,"%s %s",str,name);    \
            sen=sen->next;                   \
            f=1;                             \
        }while(sen){                         \
            name=cgrGetStr(sen,cgrKeyVal);   \
            fprintf(fp," or %s %s",str,name);\
            sen=sen->next;                   \
            }}}

#endif

