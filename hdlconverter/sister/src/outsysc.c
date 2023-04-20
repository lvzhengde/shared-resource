/*
  to output as SystemC format .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"

/*------------------------------------------------------------------
   
   numbering and statement vaiable creation 
    
------------------------------------------------------------------*/
//
//numbering handler
//
typedef struct{
    int num;
    cgrNode module;
}numbFsmdHandle;

#define numbFsmdProp(self,item) (((numbFsmdHandle*)(self->work))->item)

//
//dfg
//
static int numbFsmdDfg(fsmdHandle self,cgrNode node){
    int num;
    if(!node) return 0;
    num=numbFsmdProp(self,num);
    cgrSetNum(node,cgrKeyNumber,num);
    ((numbFsmdHandle*)self->work)->num+=1;
    return 0;
}

//
//Control
//
static int numbFsmdCtrl(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    if(!(node->prev)) numbFsmdDfg(self,node);
    return 0;
}
//
//getting bit size
//
static int numbFsmdGetBitSize(int num){
    int i,n;
    n=num>>1;
    for(i=1;n>0;i++) n>>=1;
    return i;
}

//
//removing dummy node
//
static int numbFsmdRemDummy(fsmdHandle self,cgrNode node){
    cgrNode jump,state;
    if(!node) return 0;
    state=cgrGetNode(node,cgrKeyVal);
    if(!state) return 0;
    if(state->jump==state) return 0;
    if(cgrGetNode(state,cgrKeyVal)) return 0;
    jump=state->jump;
    fsmdRemoveNode(state);
    cgrSetNode(node,cgrKeyVal,jump);
    return 0;
}

//
//checking jumo node from control node
//
static int numbFsmdCheckCtrl(cgrNode state){
    cgrNode ctrl;
    ctrl=state;
    while(ctrl){
        if(ctrl->jump!=state) return 0;
        ctrl=ctrl->next;
    }
    return 1;
}

//
//process
//
static int numbFsmdProc(fsmdHandle self,cgrNode node){
    int num;
    char*name;
    char buf[SIS_BUF_SIZE];
    cgrNode mo,state,dec;
    if(!node) return 0;
    numbFsmdRemDummy(self,node);
    state=cgrGetNode(node,cgrKeyVal);
    if(!state) return 0;
    numbFsmdProp(self,num)=0;
    fsmdRouteReset();
    if((state->type==Dfg) && (state->jump==state)) return 0;
    if(numbFsmdCheckCtrl(state)) return 0;
    state=cgrGetNode(node,cgrKeyVal);
    self->state(self,state);
    name=cgrGetStr(node,cgrKeyId);
    fsmdNameSet(name);
    sprintf(buf,"%s_state_",name);
    num=((numbFsmdHandle*)self->work)->num;
    dec=fsmdNameCreate(buf,node->file,node->line,
        numbFsmdGetBitSize(num-1),Sc_uint,Sc_signal,"");
    mo=((numbFsmdHandle*)self->work)->module;
    cgrAddNode(mo,cgrKeySignal,dec);
    cgrSetNode(node,cgrKeyStateVar,dec);
    return 0;
}
//
//signal
//
static int numbFsmdSignal(fsmdHandle self,cgrNode node){
    char*name;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    fsmdNameSet(name);
    return 0;
}

//
//module
//
static int numbFsmdModule(fsmdHandle self,cgrNode node){
    char*name;
    cgrNode dec,proc; 
    if(!node) return 0;
    fsmdNameReset();
    numbFsmdProp(self,module)=node;
    dec=cgrGetNode(node,cgrKeySignal);
    self->sigdec(self,dec);
    name=cgrGetStr(node,cgrKeyId);
    fsmdNameSet(name);
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        self->proc(self,proc);
        proc=proc->next;
    }
    fsmdNameClear();
    return 0;
}

//
//nubering
//
int numbFsmd(cgrNode top){
    numbFsmdHandle fsh={0};
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&fsh;
    fsmd->dfg=(*numbFsmdDfg);
    fsmd->ctrl=(*numbFsmdCtrl);
    fsmd->proc=(*numbFsmdProc);
    fsmd->module=(*numbFsmdModule);
    fsmd->signal=(*numbFsmdSignal);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}


/*------------------------------------------------------------------
   
   to output as SystemC format
    
------------------------------------------------------------------*/
//
//output work space
//
typedef struct{
    FILE* fp;
    cgrNode proc;
}osyscWork;
#define osyscProp(self,item) (((osyscWork*)(self->work))->item)

//
// output pointer
//
void osyscOutPointer(FILE*fp,cgrNode node){
    cgrNode ptr=node;
    while(ptr){
        switch(ptr->type){
        case '*' : 
        case '&' : fprintf(fp,"%c",ptr->type);break;
        }
        ptr=ptr->next;
    }
}
//
// output data type2
//
void osyscDataTypeOut2(FILE*fp,int type,int size){
    switch(type){
    case Sc_int     : fprintf(fp,"sc_int<%d>",size);break;
    case Sc_uint    : fprintf(fp,"sc_uint<%d>",size);break;
    case Sc_bigint  : fprintf(fp,"sc_bigint<%d>",size);break;
    case Sc_biguint : fprintf(fp,"sc_biguint<%d>",size);break;
    case Void       : fprintf(fp,"void");break;
    case Bool       : fprintf(fp,"bool");break;
    case Schar      : fprintf(fp,"char");break;
    case Uchar      : fprintf(fp,"unsigned char");break;
    case Sshort     : fprintf(fp,"short");break;
    case Ushort     : fprintf(fp,"unsigned short");break;
    case Sint       : fprintf(fp,"int");break;
    case Uint       : fprintf(fp,"unsigned int");break;
    case Slong      : fprintf(fp,"long");break;
    case Ulong      : fprintf(fp,"unsigned long");break;
    default : break;
    }
}
//
// output data type
//
void osyscDataTypeOut(FILE*fp,cgrNode node){
    int size=cgrGetNum(node,cgrKeySize);
    int type=cgrGetNum(node,cgrKeyType);
    osyscDataTypeOut2(fp,type,size);
}

//
// output signal2
//
void osyscSignalOut2(FILE*fp,int signal){
    switch(signal){
    case Sc_in       : fprintf(fp,"sc_in<")      ;break;
    case Sc_out      : fprintf(fp,"sc_out<")     ;break;
    case Sc_signal   : fprintf(fp,"sc_signal<")  ;break;
    case Sc_fifo     : fprintf(fp,"sc_fifo<")    ;break;
    case Sc_fifo_in  : fprintf(fp,"sc_fifo_in<") ;break;
    case Sc_fifo_out : fprintf(fp,"sc_fifo_out<");break;
    default : break;
    }
}

//
// output signal
//
void osyscSignalOut(FILE*fp,cgrNode node){
    int signal=cgrGetNum(node,cgrKeySignal);
    osyscSignalOut2(fp,signal);
    osyscDataTypeOut(fp,node);
    fprintf(fp," > ");
}

//
//output signal3
//
void osyscSignalOut3(FILE*fp,int signal,int type,int size){
    osyscSignalOut2(fp,signal);
    osyscDataTypeOut2(fp,type,size);
    fprintf(fp," > ");
}


//
//signal
//
static int osyscSignal(fsmdHandle self,cgrNode node){
    char*name;
    cgrNode array;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    array=cgrGetNode(node,cgrKeyArray);
    osyscIndent(fp,4);
    osyscSignalOut(fp,node);
    if(array) osyscOutOpe(fp,array);
    else fprintf(fp,"%s",name);
    fprintf(fp,";\n");
    fsmdNameSet(name);
    return 0;
}

//
//to output input variables
//
static int osyscOutRead(FILE*fp,cgrNode node,int fflg);
static int osyscOutInVar(FILE*fp,cgrNode node,int fflg){
    int cnt,arrsize;
    cgrNode array,dec,par,prev;
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    array=cgrGetNode(dec,cgrKeyArray);
    par=cgrGetNode(node,cgrKeyPar);
    fprintf(fp,"%s",cgrGetStr(node,cgrKeyVal));
    arrsize=cgrGetNum(dec,cgrKeyArraySize);
    prev=node;
    for(cnt=0;par;cnt++){
        if(cnt==arrsize){
            if(fflg){
                fprintf(fp,".read()");
            }
        }
        if(par->type!=Array) break;
        if(cgrGetNode(par,cgrKeyRight)==prev) break;
        fprintf(fp,"[");
        osyscOutRead(fp,cgrGetNode(par,cgrKeyRight),1);
        fprintf(fp,"]");
        prev=par;
        par=cgrGetNode(par,cgrKeyPar);
    }
    if((!cnt) && (!par)){
        if(fflg) fprintf(fp,".read()");
    }
    return 0;
}

//
//to output function call
// 0:nomal,1:with read function
//
static int osyscOutOpeBase(FILE*fp,cgrNode node,int fflg);
static int osyscOutFuncall(FILE*fp,cgrNode node,int sw){
    char*name;
    cgrNode args;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    if(strcmp(name,"")) fprintf(fp,"%s",name);
    else osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyLeft),sw);
    if(sw) fprintf(fp,".read()"); 
    fprintf(fp,"(");
    args=cgrGetNode(node,cgrKeyArgs);
    if(args){
        osyscOutOpeBase(fp,args,sw);
        args=args->next;
        while(args){
            fprintf(fp,",");
            osyscOutOpeBase(fp,args,sw);
            args=args->next;
        }
    }
    fprintf(fp,")");
    return 0;
}

//
// checking array
//
int osyscCheckArrayDec(cgrNode array){
    cgrNode left=cgrGetNode(array,cgrKeyLeft);
    cgrNode dec=cgrGetNode(left,cgrKeySignal);
    if(!left) return 0;
    if(left->type!=Id) return 0;
    if(!dec) return 0;
    if(dec->type!=Declare) return 0;
    if(cgrGetNum(dec,cgrKeyArraySize)>0) return 1;
    if(cgrGetNum(dec,cgrKeyPointer)>0) return 1;
    return 0;
}

//
//to output operator with read function
//
static int osyscOutRead(FILE*fp,cgrNode node,int fflg){
    if(!node) return 0;
    if(!fflg) return osyscOutOpe2(fp,node);
    switch(node->type){
    case Id : osyscOutInVar(fp,node,fflg); break;
    case Funcall : osyscOutFuncall(fp,node,fflg);break;
    case Array : 
        osyscOutRead(fp,cgrGetNode(node,cgrKeyLeft),fflg);
        break;
    default : osyscOutOpe(fp,node);break;    
    }
    return 0;
}

//
//to output operator
//
#define outOpeExpr(fp,left,right,str,fflg){  \
    fprintf(fp,"(");                         \
    osyscOutRead(fp,left,fflg);              \
    fprintf(fp,str);                         \
    osyscOutRead(fp,right,fflg);             \
    fprintf(fp,")");                         \
    return 0;}
#define outOpeAssign(fp,left,right,fflg){ \
    osyscOutOpeBase(fp,left,0);           \
    if(!fflg){                            \
        fprintf(fp,"=");                  \
        osyscOutRead(fp,right,fflg);      \
    }else{                                \
        fprintf(fp,".write(");            \
        osyscOutRead(fp,right,fflg);      \
        fprintf(fp,")");}                 \
    return 0;}
#define outOpeAssignOpe(fp,left,right,str,fflg){  \
    osyscOutOpeBase(fp,left,0);                   \
    if(!fflg){                                    \
        fprintf(fp,"=");                          \
        osyscOutRead(fp,left,fflg);               \
        fprintf(fp,str);                          \
        osyscOutRead(fp,right,fflg);              \
    }else{                                         \
        fprintf(fp,".write(");                    \
        osyscOutRead(fp,left,fflg);               \
        fprintf(fp,str);                          \
        osyscOutRead(fp,right,fflg);              \
        fprintf(fp,")");    }                     \
    return 0;}
#define outOpeIncDec(fp,left,right,str,fflg){    \
    if(left) osyscOutOpeBase(fp,left,0);         \
    else osyscOutOpeBase(fp,right,0);            \
    if(!fflg){                                   \
        fprintf(fp,"=");                         \
        if(left) osyscOutRead(fp,left,fflg);     \
        else osyscOutRead(fp,right,fflg);        \
        fprintf(fp,str);                         \
    }else{                                       \
        fprintf(fp,".write(");                   \
        if(left) osyscOutRead(fp,left,fflg);     \
        else osyscOutRead(fp,right,fflg);        \
        fprintf(fp,str);                         \
        fprintf(fp,")"); }                       \
    return 0;}

//
// output operators
// 0: nomal
// 1: add write/read
//
static int osyscOutOperator(FILE*fp,cgrNode node,int fflg){
    cgrNode cond,left,right;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    right=cgrGetNode(node,cgrKeyRight);
    cond=cgrGetNode(node,cgrKeyCond);
    //operation
    switch(node->type){
    case '=' : outOpeAssign(fp,left,right,fflg);
    case Mueq: outOpeAssignOpe(fp,left,right,"*",fflg); 
    case Dieq: outOpeAssignOpe(fp,left,right,"/",fflg);
    case Moeq: outOpeAssignOpe(fp,left,right,"%%",fflg);
    case Adeq: outOpeAssignOpe(fp,left,right,"+",fflg);
    case Sueq: outOpeAssignOpe(fp,left,right,"-",fflg);
    case Lseq: outOpeAssignOpe(fp,left,right,"<<",fflg);
    case Rseq: outOpeAssignOpe(fp,left,right,">>",fflg);
    case Aneq: outOpeAssignOpe(fp,left,right,"&",fflg);
    case Xoeq: outOpeAssignOpe(fp,left,right,"^",fflg);
    case Oreq: outOpeAssignOpe(fp,left,right,"|",fflg);
    case Oror: outOpeExpr(fp,left,right,"||",fflg);
    case Anan: outOpeExpr(fp,left,right,"&&",fflg);
    case '|': outOpeExpr(fp,left,right,"|",fflg);
    case '^': outOpeExpr(fp,left,right,"^",fflg);
    case '&': outOpeExpr(fp,left,right,"&",fflg);
    case Eq:  outOpeExpr(fp,left,right,"==",fflg);
    case Neq: outOpeExpr(fp,left,right,"!=",fflg);
    case Gre: outOpeExpr(fp,left,right,">=",fflg);
    case Les: outOpeExpr(fp,left,right,"<=",fflg);
    case '>': outOpeExpr(fp,left,right,">",fflg);
    case '<': outOpeExpr(fp,left,right,"<",fflg);
    case Lshif: outOpeExpr(fp,left,right,"<<",fflg);
    case Rshif: outOpeExpr(fp,left,right,">>",fflg);
    case '+': outOpeExpr(fp,left,right,"+",fflg);
    case '-': outOpeExpr(fp,left,right,"-",fflg);
    case '*': outOpeExpr(fp,left,right,"*",fflg);
    case '/': outOpeExpr(fp,left,right,"/",fflg);
    case '%': outOpeExpr(fp,left,right,"%%",fflg);
    case Inc: outOpeIncDec(fp,left,right,"+1",fflg);
    case Dec: outOpeIncDec(fp,left,right,"-1",fflg);
    case '~': outOpeExpr(fp,left,right,"~",fflg);
    case '!': outOpeExpr(fp,left,right,"!",fflg);
    case New: outOpeExpr(fp,left,right,"new ",fflg);
    case Del: outOpeExpr(fp,left,right,"delete ",fflg);
    case Arr: outOpeExpr(fp,left,right,"->",fflg);
    case '.': outOpeExpr(fp,left,right,".",fflg);
    }
    // A ? B : C
    if(node->type=='?'){
        osyscOutOpeBase(fp,cond,fflg);
        fprintf(fp,"?");
        osyscOutOpeBase(fp,left,fflg);
        fprintf(fp,":");
        osyscOutOpeBase(fp,right,fflg);
    }
    
    //connection
    else if(node->type==Connect){
        fprintf(fp,"(");
        osyscOutOpeBase(fp,cond,fflg);
        cond=cond->next;
        while(cond){
            fprintf(fp,",");
            osyscOutOpeBase(fp,cond,fflg);
            cond=cond->next;
        }
        fprintf(fp,")");

    //cast (not supported)    
    }else if(node->type==Cast);
    return 0;
}

//
//to output base
//
static int osyscOutOpeBase(FILE*fp,cgrNode node,int fflg){
    if(!node) return 0;
    switch(node->type){
    case Id:fprintf(fp,"%s",cgrGetStr(node,cgrKeyVal));break;
    case Num:fprintf(fp,"%d",cgrGetNum(node,cgrKeyVal));break;
    case Real:fprintf(fp,"%lf",cgrGetReal(node,cgrKeyVal));break;
    case String:fprintf(fp,"\"%s\"",cgrGetStr(node,cgrKeyVal));break;
    case Funcall:osyscOutFuncall(fp,node,fflg);break;
    case Array :
        if(osyscCheckArrayDec(node))
            osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyLeft),0);
        else osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyLeft),fflg);
        
        fprintf(fp,"[");
        osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyRight),fflg);
        fprintf(fp,"]");
        break;
    case '.' :
        osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyLeft),fflg);
        fprintf(fp,".");
        osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyRight),fflg);
        break;
    case Arr :
        osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyLeft),fflg);
        fprintf(fp,"->");
        osyscOutOpeBase(fp,cgrGetNode(node,cgrKeyRight),fflg);
        break;
    default : osyscOutOperator(fp,node,fflg);break;
    }
    return 0;
}
//
//to output operation2
//
int osyscOutOpe2(FILE*fp,cgrNode node){
    return osyscOutOpeBase(fp,node,0);
}

//
//to output operation
//
int osyscOutOpe(FILE*fp,cgrNode node){
    return osyscOutOpeBase(fp,node,1);
}

//
//to output dfg block
//
static int osyscDfgBlock(fsmdHandle self,cgrNode node,int ind){
    cgrNode ope;
    FILE*fp=osyscProp(self,fp);
    ope=cgrGetNode(node,cgrKeyVal);
    while(ope){
        osyscIndent(fp,ind);
        osyscOutOpe(fp,ope);
        fprintf(fp,";\n");
        ope=ope->next;
    }
    return 0;
}

//
//to output control
//
#define osyscOutIfCond(fp,cond) {   \
    fprintf(fp,"if(");              \
    osyscOutRead(fp,cond,1);        \
    fprintf(fp,") ");               }
#define osyscOutIfBlockEnd(fp,indent){ \
    osyscIndent(fp,indent);            \
    fprintf(fp,"}\n");                 \
    break;                             }
static int osyscCtrl(fsmdHandle self,cgrNode node){
    int num;
    char*varname;
    cgrNode ope,jump,proc,var,ctrl,cond;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    if(node->prev) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    osyscIndent(fp,8);
    fprintf(fp,"case %d :\n",num);
    proc=osyscProp(self,proc);
    var=cgrGetNode(proc,cgrKeyStateVar);
    varname=cgrGetStr(var,cgrKeyId);
    osyscIndent(fp,12);
    ctrl=node;
    while(ctrl){
        jump=ctrl->jump;
        num=cgrGetNum(jump,cgrKeyNumber);
        ope=cgrGetNode(ctrl,cgrKeyVal);
        cond=cgrGetNode(ctrl,cgrKeyCond);
        if(cond) osyscOutIfCond(fp,cond);
        if(ope) fprintf(fp,"{\n");
        osyscDfgBlock(self,ctrl,16);
        if(ope) osyscIndent(fp,16);
        fprintf(fp,"%s.write(%d);\n",varname,num);
        if(cond){
            osyscIndent(fp,12);
            if(ope) fprintf(fp,"}");
            fprintf(fp,"else ");
        }else if(ope) osyscOutIfBlockEnd(fp,12);
        ctrl=ctrl->next;
    }
    osyscIndent(fp,12);
    fprintf(fp,"break;\n");
    return 0;
}

//
//Dfg
//
static int osyscDfg(fsmdHandle self,cgrNode node){
    int num;
    char*varname;
    cgrNode var,jump,proc;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    osyscIndent(fp,8);
    fprintf(fp,"case %d :\n",num);
    osyscDfgBlock(self,node,12);
    jump=node->jump;
    if(!jump) jump=node;
    num=cgrGetNum(jump,cgrKeyNumber);
    osyscIndent(fp,12);
    proc=osyscProp(self,proc);
    var=cgrGetNode(proc,cgrKeyStateVar);
    varname=cgrGetStr(var,cgrKeyId);
    fprintf(fp,"%s.write(%d);\n",varname,num);
    osyscIndent(fp,12);
    fprintf(fp,"break;\n");
    return 0;
}

//
//to output datapath
//
static int osyscProcCtrl(fsmdHandle self,cgrNode node){
    cgrNode ctr,ope,cond;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    ctr=node;
    osyscIndent(fp,8);
    while(ctr){
        ope=cgrGetNode(ctr,cgrKeyVal);
        cond=cgrGetNode(ctr,cgrKeyCond);
        if(cond) osyscOutIfCond(fp,cond);
        if(ope){
            if(ope->next){
                fprintf(fp,"{\n");
                osyscDfgBlock(self,ctr,12);
            }else{
                osyscOutOpe(fp,ope);
                fprintf(fp,";\n");
            }
        }else{
            osyscOutOpe(fp,ope);
            fprintf(fp,";\n");
        }
        if(cond){
            osyscIndent(fp,8);
            if(ope) if(ope->next) fprintf(fp,"}");
            fprintf(fp,"else ");
        }else if(ope) if(ope->next)
            osyscOutIfBlockEnd(fp,8);
        ctr=ctr->next;
    }
    return 0;
}
//
//to output reset process
//
static int osyscResetProc(
    fsmdHandle self,cgrNode node,char*varname){
    cgrNode ope;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    osyscIndent(fp,8);
    osyscOutIfCond(fp,cgrGetNode(node,cgrKeyCond));
    fprintf(fp,"{\n");
    ope=cgrGetNode(node,cgrKeyVal);
    if(ope) osyscDfgBlock(self,node,12);
    if(strcmp(varname,"")){
        osyscIndent(fp,12);
        fprintf(fp,"%s.write(0);\n",varname);
    }
    osyscIndent(fp,8);
    fprintf(fp,"}");
    fprintf(fp,"else{\n");
    return 0;
}

//
//process
//
static int osyscProc(fsmdHandle self,cgrNode node){
    char*name;
    char*varname;
    cgrNode var,state;
    FILE*fp=osyscProp(self,fp);
    cgrNode rproc=cgrGetNode(node,cgrKeyResetProc);
    if(!node) return 0;
    osyscProp(self,proc)=node;
    name=cgrGetStr(node,cgrKeyId);
    state=cgrGetNode(node,cgrKeyVal);
    osyscIndent(fp,4);
    fprintf(fp,"void %s(void){\n",name);
    var=cgrGetNode(node,cgrKeyStateVar);
    varname=cgrGetStr(var,cgrKeyId);
    osyscResetProc(self,rproc,varname);
    if(!var){
        int pind=8;
        if(rproc) pind=12;
        if(state->type==Ctrl) osyscProcCtrl(self,state);
        else if(state->type==Dfg) osyscDfgBlock(self,state,pind);
    }else{
        osyscIndent(fp,8);
        fprintf(fp,"switch(%s.read()){\n",varname);
        fsmdRouteReset();
        self->state(self,state);
        osyscIndent(fp,8);
        fprintf(fp,"}\n");
    }
    osyscIndent(fp,4);
    if(rproc) fprintf(fp,"}");
    fprintf(fp,"}\n");
    return 0;
}

//
//output instance base
//
int osyscOutInstanceBase(
    FILE*fp,int indent,cgrNode inst){
    while(inst){
        char*name =cgrGetStr(inst,cgrKeyId); 
        char*mname=cgrGetStr(inst,cgrKeyType); 
        cgrNode port=cgrGetNode(inst,cgrKeySignal);
        char*signal=cgrGetStr(port,cgrKeySignal);
        osyscIndent(fp,indent);
        //instance creation
        fprintf(fp,"%s = new %s(\"%s\");\n",name,mname,name);
        
        //checking port
        if(!port) return 0;

        //output flat connect
        if(!strcmp("",signal)){
            char*pname=cgrGetStr(port,cgrKeyId);
            osyscIndent(fp,indent);
            fprintf(fp,"(*%s)(%s",name,pname);
            port=port->next;
            while(port){
                signal=cgrGetStr(port,cgrKeySignal);
                if(strcmp("",signal)) break;
                pname=cgrGetStr(port,cgrKeyId);
                fprintf(fp,",%s",pname);
                port=port->next;
            }
            fprintf(fp,");\n");
        }

        //dot(pointer) connect
        while(port){
            char*pname=cgrGetStr(port,cgrKeyId);
            signal=cgrGetStr(port,cgrKeySignal);
            osyscIndent(fp,indent);
            if(strcmp("",signal)){
                fprintf(fp,"%s->%s(%s);\n",name,signal,pname);
            }
            port=port->next;
        }   
        inst=inst->next;
    }
    return 0;
}

//
//instance creation
//
static int osyscInst(fsmdHandle self,cgrNode node){
    osyscOutInstanceBase(osyscProp(self,fp),8,node);
    return 0;
}

//
//sensitive
//

static int osyscSens2(FILE*fp,cgrNode node,char*suffix){
    char*name;
    cgrNode sen;
    if(!node) return 0;
    sen=node;
    while(sen){
        name=cgrGetStr(sen,cgrKeyVal);
        fprintf(fp,"<<%s%s",name,suffix);
        sen=sen->next;
    }
    return 0;
}

//
//sensitive base
//
int osyscSensBase(FILE*fp,int indent,cgrNode node){
    cgrNode sen=cgrGetNode(node,cgrKeySensitive);
    cgrNode pos=cgrGetNode(node,cgrKeySensitivePos);
    cgrNode neg=cgrGetNode(node,cgrKeySensitiveNeg);

    if(!(sen||neg||pos)) return 0;
    osyscIndent(fp,indent);
    fprintf(fp,"sensitive");
    osyscSens2(fp,sen,"");
    osyscSens2(fp,pos,".pos()");
    osyscSens2(fp,neg,".neg()");
    fprintf(fp,";\n");
    return 0;
}

//
//instance declaration
//
static int osyscInstDec(fsmdHandle self,cgrNode node){
    char*name;
    char*type;
    cgrNode inst;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    inst=node;
    while(inst){
        name=cgrGetStr(inst,cgrKeyId);
        type=cgrGetStr(inst,cgrKeyType);
        osyscIndent(fp,4);
        fprintf(fp,"%s* %s;\n",type,name);
        inst=inst->next;
    }
    return 0;
}


//
// process into SC_CTOR base
//
int osyscOutCtorProcBase(FILE*fp,int indent,cgrNode proc){
    //process
    while(proc){
        char*name=cgrGetStr(proc,cgrKeyId);
        osyscIndent(fp,indent);
        switch(cgrGetNum(proc,cgrKeyProcType)){
        case 0  : fprintf(fp,"SC_THREAD(%s);\n",name);break;
        default : fprintf(fp,"SC_METHOD(%s);\n",name);break;
        }
        //sensitive
        osyscSensBase(fp,indent,proc);
        proc=proc->next;
    }
    return 0;
}
//
// process into SC_CTOR
//
static int osyscCtorProc(fsmdHandle self,cgrNode node){
    FILE*fp=osyscProp(self,fp);
    osyscOutCtorProcBase(fp,8,node);
    return 0;
}

//
//module
//
static int osyscModule(fsmdHandle self,cgrNode node){
    char*name;
    cgrNode dec,tproc,proc,inst;
    FILE*fp=osyscProp(self,fp);
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    dec=cgrGetNode(node,cgrKeySignal);
    proc=cgrGetNode(node,cgrKeyProcess);
    inst=cgrGetNode(node,cgrKeyInstance);
    fprintf(fp,"SC_MODULE(%s){\n",name);
    self->sigdec(self,dec);
    osyscInstDec(self,inst);
    tproc=proc;
    while(tproc){
        self->proc(self,tproc);
        tproc=tproc->next;
    }
    osyscIndent(fp,4);
    fprintf(fp,"SC_CTOR(%s){\n",name);
    osyscInst(self,inst);
    osyscCtorProc(self,proc);
    osyscIndent(fp,4);
    fprintf(fp,"}\n");
    fprintf(fp,"};\n\n");
    return 0;
}
//
// run
//
static int osyscRun(fsmdHandle self){
    cgrNode mo;
    FILE*fp=overiProp(self,fp);
    mo=self->top;
    while(mo){
        switch(mo->type){
        case Sc_module : //module
            self->module(self,mo);
            break;
        case OpString :// option string
            outOpString(fp,1,mo);
            break;
        }
        mo=mo->next;
    }
    fsmdRouteClear();
}


//
//to output SystemC format
//
int outsysc(char*outfile,cgrNode top){
    FILE*fp;
    osyscWork prop;
    fsmdHandle fsmd;
    if(!(fp=fopen(outfile,"w"))){
        cmsgEcho(__sis_msg,0,outfile);
        return -1;
    }
    prop.fp=fp;
    fsmd=fsmdIni(top);
    fsmdNameReset();
    fsmd->work=&prop;
    fsmd->run=(*osyscRun);
    fsmd->module=(*osyscModule);
    fsmd->signal=(*osyscSignal);
    fsmd->proc=(*osyscProc);
    fsmd->dfg=(*osyscDfg);
    fsmd->ctrl=(*osyscCtrl);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    fclose(fp);
    return 0;
}

