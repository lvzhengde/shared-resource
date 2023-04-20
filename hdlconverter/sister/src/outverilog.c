/*
  to output as Verilog HDL format .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"


//
//function call(part select)
//
static int overiOutFuncall(FILE*fp,cgrNode node){
    char*name;
    cgrNode args;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    if(strcmp(name,"")) fprintf(fp,"%s",name);
    else overiOutOpe(fp,cgrGetNode(node,cgrKeyLeft));
    fprintf(fp,"[");
    args=cgrGetNode(node,cgrKeyArgs);
    if(args){
        overiOutOpe(fp,args);
        args=args->next;
    }
    cgrMaps(args,fprintf(fp,":");overiOutOpe(fp,args))
    fprintf(fp,"]");
    return 0;
}

//
//to output operator
//
#define outOpeExpr(sw,fp,left,right,str){   \
    fprintf(fp,"(");                        \
    overiOutOpe(fp,left);                   \
    fprintf(fp,str);                        \
    overiOutOpe(fp,right);                  \
    fprintf(fp,")");                        \
    return 0;                               }
#define outOpeAssign(sw,fp,left,right,node){    \
    overiOutOpe(fp,left);                       \
    if(sw){                                     \
        if(cgrGetNum(node,cgrKeyIsNonBlocking)) \
            fprintf(fp,"<=");                   \
        else fprintf(fp,"=");                   \
    }else fprintf(fp,"<=");                     \
    overiOutOpe(fp,right);                      \
    return 0;                                   }
#define outOpeAssignOpe(sw,fp,left,right,str){  \
    osyscOutOpe(fp,left);                       \
    if(sw) fprintf(fp,"=");                     \
    else   fprintf(fp,"<=");                    \
    overiOutOpe(fp,left);                       \
    fprintf(fp,str);                            \
    overiOutOpe(fp,right);                      \
    return 0;                                   }
#define outOpeIncDec(sw,fp,left,right,str){ \
    if(left) osyscOutOpe(fp,left);          \
    else overiOutOpe(fp,right);             \
    if(sw) fprintf(fp,"=");                 \
    else   fprintf(fp,"<=");                \
    if(left) overiOutOpe(fp,left);          \
    else overiOutOpe(fp,right);             \
    fprintf(fp,str);                        \
    return 0;                               }

static int overiOutOperator(FILE*fp,cgrNode node,int sw){
    cgrNode cond,left,right;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    right=cgrGetNode(node,cgrKeyRight);
    cond=cgrGetNode(node,cgrKeyCond);
    //operation
    switch(node->type){
    case '=' : outOpeAssign(sw,fp,left,right,node);
    case Mueq: outOpeAssignOpe(sw,fp,left,right,"*"); 
    case Dieq: outOpeAssignOpe(sw,fp,left,right,"/");
    case Moeq: outOpeAssignOpe(sw,fp,left,right,"%%");
    case Adeq: outOpeAssignOpe(sw,fp,left,right,"+");
    case Sueq: outOpeAssignOpe(sw,fp,left,right,"-");
    case Lseq: outOpeAssignOpe(sw,fp,left,right,"<<");
    case Rseq: outOpeAssignOpe(sw,fp,left,right,">>");
    case Aneq: outOpeAssignOpe(sw,fp,left,right,"&");
    case Xoeq: outOpeAssignOpe(sw,fp,left,right,"^");
    case Oreq: outOpeAssignOpe(sw,fp,left,right,"|");
    case Oror: outOpeExpr(sw,fp,left,right,"||");
    case Anan: outOpeExpr(sw,fp,left,right,"&&");
    case '|': outOpeExpr(sw,fp,left,right,"|");
    case '^': outOpeExpr(sw,fp,left,right,"^");
    case '&': outOpeExpr(sw,fp,left,right,"&");
    case Eq:  outOpeExpr(sw,fp,left,right,"==");
    case Neq: outOpeExpr(sw,fp,left,right,"!=");
    case Gre: outOpeExpr(sw,fp,left,right,">=");
    case Les: outOpeExpr(sw,fp,left,right,"<=");
    case '>': outOpeExpr(sw,fp,left,right,">");
    case '<': outOpeExpr(sw,fp,left,right,"<");
    case Lshif: outOpeExpr(sw,fp,left,right,"<<");
    case Rshif: outOpeExpr(sw,fp,left,right,">>");
    case '+': outOpeExpr(sw,fp,left,right,"+");
    case '-': outOpeExpr(sw,fp,left,right,"-");
    case '*': outOpeExpr(sw,fp,left,right,"*");
    case '/': outOpeExpr(sw,fp,left,right,"/");
    case '%': outOpeExpr(sw,fp,left,right,"%%");
    case Inc: outOpeIncDec(sw,fp,left,right,"+1");
    case Dec: outOpeIncDec(sw,fp,left,right,"-1");
    case '~': outOpeExpr(sw,fp,left,right,"~");
    case '!': outOpeExpr(sw,fp,left,right,"!");
    case Arr: outOpeExpr(sw,fp,left,right,"->");
    case '.': outOpeExpr(sw,fp,left,right,".");
    }
    // A ? B : C
    if(node->type=='?'){
        osyscOutOpe(fp,cond);
        fprintf(fp,"?");
        osyscOutOpe(fp,left);
        fprintf(fp,":");
        osyscOutOpe(fp,right);
    }
    
    //connection
    else if(node->type==Connect){
        fprintf(fp,"{");
        osyscOutOpe(fp,cond);
        cond=cond->next;
        while(cond){
            fprintf(fp,",");
            osyscOutOpe(fp,cond);
            cond=cond->next;
        }
        fprintf(fp,"}");

    //cast (not supported)    
    }else if(node->type==Cast);
    return 0;
}

//
//to output operation with switch
//0 :only non blocking assignment, 1:with blocking assignment
//
int overiOutOpe2(FILE*fp,cgrNode node,int sw){
    if(!node) return 0;
    switch(node->type){
    case Id:fprintf(fp,"%s",cgrGetStr(node,cgrKeyVal));break;
    case Num:fprintf(fp,"%d",cgrGetNum(node,cgrKeyVal));break;
    case Real:fprintf(fp,"%lf",cgrGetReal(node,cgrKeyVal));break;
    case String:fprintf(fp,"\"%s\"",cgrGetStr(node,cgrKeyVal));break;
    case Funcall:overiOutFuncall(fp,node);break;
    case Array :
        overiOutOpe(fp,cgrGetNode(node,cgrKeyLeft));
        fprintf(fp,"[");
        overiOutOpe(fp,cgrGetNode(node,cgrKeyRight));
        fprintf(fp,"]");
        break;
    default : overiOutOperator(fp,node,sw);break;
    }
    return 0;
}


//
//to output operation
//
int overiOutOpe(FILE*fp,cgrNode node){
    return overiOutOpe2(fp,node,0);
}



//
//port
//
#define overiPortDef(function) {        \
    char*name;                          \
    int sig=cgrGetNum(dec,cgrKeySignal);\
    if((sig!=Sc_in) && (sig!=Sc_out)){  \
        dec=dec->next;                  \
        continue; }                     \
    name=cgrGetStr(dec,cgrKeyId);       \
    function;                           \
    osyscIndent(fp,4);                  \
    fprintf(fp,"%s",name);              \
    dec=dec->next;                      }
static int overiPort(fsmdHandle self,cgrNode node){
    cgrNode dec=node;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    while(dec){
        overiPortDef(1);
        break;
    }
    while(dec) overiPortDef(fprintf(fp,",\n"));
    return 0;
}

//
//to output declaration array
//
static int overiOutDecArray(FILE*fp,cgrNode node){
    if(!node) return 0;
    if(node->type==Num)
        return fprintf(fp,"%d",cgrGetNum(node,cgrKeyVal)-1);
    else if(node->type!=Array) return overiOutOpe(fp,node);
    overiOutDecArray(fp,cgrGetNode(node,cgrKeyLeft));
    fprintf(fp,"[0:");
    overiOutDecArray(fp,cgrGetNode(node,cgrKeyRight));
    fprintf(fp,"]");
    return 0;
}

//
//declaration
//
#define overiDecLine(type,size,name,array){ \
    osyscIndent(fp,4);                      \
    fprintf(fp,type);                       \
    if(size>1) fprintf(fp,"[%d:0] ",size-1);\
    else fprintf(fp," ");                   \
    if(array) overiOutDecArray(fp,array);   \
    else fprintf(fp,"%s",name);             \
    fprintf(fp,";\n");                      }
static int overiDec(fsmdHandle self,cgrNode node){
    char*name;
    int size,sig;
    cgrNode array;
    cgrNode dec=node;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    while(dec){
        name=cgrGetStr(dec,cgrKeyId);
        size=cgrGetNum(dec,cgrKeySize);
        sig=cgrGetNum(dec,cgrKeySignal);
        array=cgrGetNode(dec,cgrKeyArray);
        switch(sig){
        case Sc_in:overiDecLine("input",size,name,array);break;
        case Sc_signal:
            if(cgrGetNum(dec,cgrKeyWire)){
                overiDecLine("wire",size,name,array);
            }else overiDecLine("reg",size,name,array);
            break;
        case Sc_out:
            overiDecLine("output",size,name,array);
            if(!cgrGetNum(dec,cgrKeyWire))
                overiDecLine("reg",size,name,array);
            break;
        }
        dec=dec->next;
    }
    fprintf(fp,"\n");
    return 0;
}

//
//port connection
//
#define overiInstPortDef(port) if(port){      \
    char*pname=cgrGetStr(port,cgrKeyId);      \
    char*signal=cgrGetStr(port,cgrKeySignal); \
    osyscIndent(fp,8);                        \
    fprintf(fp,".%s(%s)",signal,pname);       \
    port=port->next;                          }
static int overiInstPort(
    fsmdHandle self,cgrNode node,char*name,char*mname){
    cgrNode port=node;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    osyscIndent(fp,4);
    fprintf(fp,"%s %s(\n",mname,name);
    overiInstPortDef(port);
    while(port){
        fprintf(fp,",\n");
        overiInstPortDef(port);
    }
    fprintf(fp,");\n");
    return 0;
}

//
//instance
//
static int overiInst(fsmdHandle self,cgrNode node){
    FILE*fp=overiProp(self,fp);
    osyscInstDef(overiInstPort);
    fprintf(fp,"\n");
    return 0;
}

//
//to output dfg block
//
static int overiDfgBlock(fsmdHandle self,cgrNode node,int ind){
    cgrNode ope;
    FILE*fp=overiProp(self,fp);
    ope=cgrGetNode(node,cgrKeyVal);
    while(ope){
        osyscIndent(fp,ind);
        overiOutOpe(fp,ope);
        fprintf(fp,";\n");
        ope=ope->next;
    }
    return 0;
}

//
//to output process control
//
#define overiOutIfCond(fp,cond) {   \
    fprintf(fp,"if(");              \
    overiOutOpe(fp,cond);           \
    fprintf(fp,") ");               }
#define overiOutIfBlockEnd(fp,indent){ \
    osyscIndent(fp,indent);            \
    fprintf(fp,"end\n");               \
    break;                             }
static int overiProcCtrl(fsmdHandle self,cgrNode node){
    cgrNode ctr,ope,cond;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    ctr=node;
    osyscIndent(fp,8);
    while(ctr){
        ope=cgrGetNode(ctr,cgrKeyVal);
        cond=cgrGetNode(ctr,cgrKeyCond);
        if(cond) overiOutIfCond(fp,cond);
        if(ope){
            if(ope->next){
                fprintf(fp," begin\n");
                overiDfgBlock(self,ctr,12);
            }else{
                overiOutOpe(fp,ope);
                fprintf(fp,";\n");
            }
        }else{
            overiOutOpe(fp,ope);
            fprintf(fp,";\n");
        }
        if(cond){
            osyscIndent(fp,8);
            if(ope) if(ope->next){
                fprintf(fp,"end\n");
                osyscIndent(fp,8);
            }
            fprintf(fp,"else ");
        }else if(ope) if(ope->next)
            overiOutIfBlockEnd(fp,8);
        ctr=ctr->next;
    }
    return 0;
}

//
//to output control node
//
static int overiCtrl(fsmdHandle self,cgrNode node){
    int num;
    char*varname;
    cgrNode ope,jump,proc,var,ctrl,cond;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    if(node->prev) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    osyscIndent(fp,8);
    fprintf(fp,"%d : begin\n",num);
    proc=overiProp(self,proc);
    var=cgrGetNode(proc,cgrKeyStateVar);
    varname=cgrGetStr(var,cgrKeyId);
    osyscIndent(fp,12);
    ctrl=node;
    while(ctrl){
        jump=ctrl->jump;
        num=cgrGetNum(jump,cgrKeyNumber);
        ope=cgrGetNode(ctrl,cgrKeyVal);
        cond=cgrGetNode(ctrl,cgrKeyCond);
        if(cond) overiOutIfCond(fp,cond);
        if(ope) fprintf(fp,"begin\n");
        overiDfgBlock(self,ctrl,16);
        if(ope) osyscIndent(fp,16);
        fprintf(fp,"%s<=%d;\n",varname,num);
        if(cond){
            osyscIndent(fp,12);
            if(ope){
                fprintf(fp,"end\n");
                osyscIndent(fp,12);
            }
            fprintf(fp,"else ");
        }else if(ope) overiOutIfBlockEnd(fp,12);
        ctrl=ctrl->next;
    }
    osyscIndent(fp,8);
    fprintf(fp,"end\n");
    return 0;
}

//
//to output dfg node
//
static int overiDfg(fsmdHandle self,cgrNode node){
    int num;
    char*varname;
    cgrNode var,jump,proc;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    osyscIndent(fp,8);
    fprintf(fp,"%d : begin\n",num);
    overiDfgBlock(self,node,12);
    jump=node->jump;
    if(!jump) jump=node;
    num=cgrGetNum(jump,cgrKeyNumber);
    osyscIndent(fp,12);
    proc=overiProp(self,proc);
    var=cgrGetNode(proc,cgrKeyStateVar);
    varname=cgrGetStr(var,cgrKeyId);
    fprintf(fp,"%s<=%d;\n",varname,num);
    osyscIndent(fp,8);
    fprintf(fp,"end\n");
    return 0;
}


//
//to output reset process
//
static int overiResetProc(
    fsmdHandle self,cgrNode node,char*varname){
    cgrNode ope;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    osyscIndent(fp,8);
    overiOutIfCond(fp,cgrGetNode(node,cgrKeyCond));
    ope=cgrGetNode(node,cgrKeyVal);
    fprintf(fp,"begin\n");
    if(ope) overiDfgBlock(self,node,12);
    if(strcmp(varname,"")){
        osyscIndent(fp,12);
        fprintf(fp,"%s<=0;\n",varname);
    }
    osyscIndent(fp,8);
    fprintf(fp,"end\n");
    osyscIndent(fp,8);
    fprintf(fp,"else begin\n");
    return 0;
}


//
//process
//
static int overiProc(fsmdHandle self,cgrNode node){
    cgrNode proc=node;
    cgrNode var,state;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    while(proc){
        int f=0;
        char* varname;
        cgrNode rproc=cgrGetNode(proc,cgrKeyResetProc);
        overiProp(self,proc)=proc;
        osyscIndent(fp,4);
        fprintf(fp,"always@(");
        overiSens(fp,proc,cgrKeySensitive,"",f);
        overiSens(fp,proc,cgrKeySensitivePos,"posedge",f);
        overiSens(fp,proc,cgrKeySensitiveNeg,"negedge",f);
        fprintf(fp,") begin\n");
        state=cgrGetNode(proc,cgrKeyVal);
        if(!state){
            osyscIndent(fp,4);
            fprintf(fp,"end\n\n");
            proc=proc->next;
            continue;
        }
        var=cgrGetNode(proc,cgrKeyStateVar);
        varname=cgrGetStr(var,cgrKeyId);
        overiResetProc(self,rproc,varname);
        if(!var){
            int pind=8;
            if(rproc) pind=12;
            if(state->type==Ctrl){
                overiProcCtrl(self,state);
            }else if(state->type==Dfg){
                overiDfgBlock(self,state,pind);
            }
        }else{
            osyscIndent(fp,8);
            fprintf(fp,"case(%s)\n",varname);
            fsmdRouteReset();
            self->state(self,state);
            osyscIndent(fp,8);
            fprintf(fp,"endcase\n");
        }
        if(rproc){
            osyscIndent(fp,8);
            fprintf(fp,"end\n");
        }
        osyscIndent(fp,4);
        fprintf(fp,"end\n\n");
        proc=proc->next;
    }
    return 0;
}

//
//module
//
int overiModule(fsmdHandle self,cgrNode node){
    char*name;
    cgrNode dec,tproc,proc,inst;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    dec=cgrGetNode(node,cgrKeySignal);
    proc=cgrGetNode(node,cgrKeyProcess);
    inst=cgrGetNode(node,cgrKeyInstance);
    fprintf(fp,"module %s(\n",name);
    overiPort(self,dec);
    fprintf(fp,");\n\n",name);
    overiDec(self,dec);
    overiInst(self,inst);
    self->proc(self,proc);
    fprintf(fp,"endmodule\n\n");
    return 0;
}
//
// output option string
//
int outOpString(FILE*fp,int sw,cgrNode node){
    int type=cgrGetNum(node,cgrKeyFormSw);
    char*str=cgrGetStr(node,cgrKeyVal);
    if(!node) return 0;
    if(type==0) fprintf(fp,"%s",str);//both
    else if(sw==type) fprintf(fp,"%s",str);
    return 0;
}
//
// run
//
static int overiRun(fsmdHandle self){
    cgrNode mo;
    FILE*fp=overiProp(self,fp);
    mo=self->top;
    while(mo){
        switch(mo->type){
        case Sc_module : //module
            self->module(self,mo);
            break;
        case OpString :// option string
            outOpString(fp,2,mo);
            break;
        }
        mo=mo->next;
    }
    fsmdRouteClear();
}

//
//init 
//
fsmdHandle overiIni(char*outfile,cgrNode top){
    FILE*fp;
    overiWork* prop;
    fsmdHandle fsmd;
    if(!(fp=fopen(outfile,"w"))){
        cmsgEcho(__sis_msg,0,outfile);
        return NULL;
    }
    prop=(overiWork*)malloc(sizeof(overiWork));
    prop->fp=fp;
    prop->indent=0;
    fsmd=fsmdIni(top);
    fsmdNameReset();
    fsmd->work=prop;
    fsmd->run=(*overiRun);
    fsmd->dfg=(*overiDfg);
    fsmd->ctrl=(*overiCtrl);
    fsmd->proc=(*overiProc);
    fsmd->module=(*overiModule);
    return fsmd;
}
//
// destruct
//
void overiDes(fsmdHandle fsmd){
    fclose(overiProp(fsmd,fp));
    fsmdDes(fsmd);
    free((overiWork*)(fsmd->work));
}

//
//to output Verilog HDL
//
int outverilog(char*outfile,cgrNode top){
    fsmdHandle fsmd;
    fsmd=overiIni(outfile,top);
    if(!fsmd) return -1;
    fsmd->run(fsmd);
    overiDes(fsmd); 
    return 0;
}

