/*
  to output as SystemC format .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __OUT_SYSTEMC_HEAD
#define __OUT_SYSTEMC_HEAD

//FSMD state numbering
extern int numbFsmd(cgrNode top);

//to output SystemC format
extern int outsysc(char*outfile,cgrNode top);

//to output operation
extern int osyscOutOpe(FILE*fp,cgrNode node);

// to output signal
extern void osyscSignalOut(FILE*fp,cgrNode node);
extern void osyscSignalOut2(FILE*fp,int signal);
extern void osyscSignalOut3(FILE*fp,int signal,int type,int size);

// to output data type
extern void osyscDataTypeOut(FILE*fp,cgrNode node);
extern void osyscDataTypeOut2(FILE*fp,int type,int size);

//output instance base
extern int osyscOutInstanceBase(FILE*fp,int indent,cgrNode inst);
// output pointer
extern void osyscOutPointer(FILE*fp,cgrNode node);
// process into SC_CTOR base
extern int osyscOutCtorProcBase(FILE*fp,int indent,cgrNode proc);
//sensitive base
extern int osyscSensBase(FILE*fp,int indent,cgrNode node);

// checking array
extern int osyscCheckArrayDec(cgrNode array);

//print indent
#define osyscIndent(fp,size){ \
    int _i_;for(_i_=0;_i_<size;_i_++) fprintf(fp," ");}

//to output instance
#define osyscInstDef(function) {            \
    char*name;                              \
    char*mname;                             \
    cgrNode inst,port;                      \
    if(!node) return 0;                     \
    inst=node;                              \
    while(inst){                            \
        mname=cgrGetStr(inst,cgrKeyType);   \
        name=cgrGetStr(inst,cgrKeyId);      \
        port=cgrGetNode(inst,cgrKeySignal); \
        function(self,port,name,mname);     \
        inst=inst->next; }}


#endif

