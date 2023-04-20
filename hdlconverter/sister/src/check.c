/*
  checking syntax.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"
#include <time.h>
#include <ctype.h>
#include <regex.h>

/*------------------------------------------------------------------
  
                        process checking
(module).cgrKeyProcess=(SC_METHOD)-->(SC_THREAD)-->(task) ... 
(module_0)-->(module_1) ... --> (module_N)

------------------------------------------------------------------*/
//
//setting process
//
static int chkProcSetProc(cgrNode mo,cgrNode proc){
    cgrNode prev,next;
    if(!mo) return -1;
    if(!proc) return -1;
    prev=proc->prev;
    next=proc->next;
    if(prev)cgrSetContext(prev,next);
    else cgrSetNode(mo,cgrKeyVal,next);
    proc->next=NULL;
    proc->prev=NULL;
    cgrAddNode(mo,cgrKeyProcess,proc);
    return 0;
}

//
//module
//
static int chkProcModule(gsHandle self,cgrNode node){
    cgrNode proc,mo,cfg,next;
    if(!node) return 0;
    mo=gsPeek(self,Sc_module);
    proc=node;
    while(proc){
        next=proc->next;
        switch(proc->type){
        case Function:
            cfg=cgrGetNode(proc,cgrKeyVal);
            if(!cfg) break;
            chkProcSetProc(mo,proc);
            break;
        }
        proc=next;
    }
    return 0;
}

//
//searching parent into the stack
//
static cgrNode chkProcSearchPar(cVector stack,char*str){
    int i;
    char*name;
    cgrNode node;
    if(!str) return NULL;
    if(!stack) return NULL;
    if(!strcmp(str,"")) return NULL;
    for(i=cvectSize(stack)-1;i>=0;i--){
        node=cvectElt(stack,i,cgrNode);
        if(!node) continue;
        if(node->type!=Sc_module) continue;
        name=cgrGetStr(node,cgrKeyId);
        if(!strcmp(name,str)) return node;
    }
    return NULL;
}

//
//function
//
static int chkProcFunction(gsHandle self,cgrNode node){
    char*parname;
    cgrNode cfg,par;
    if(!node) return 0;
    cfg=cgrGetNode(node,cgrKeyVal);
    if(!cfg && (cgrGetNum(node,cgrKeyIsFuncBody)!=1)) return 0;
    parname=cgrGetStr(node,cgrKeyPar);
    par=chkProcSearchPar(self->stack,parname);
    if(!par) return -1;
    chkProcSetProc(par,node);
    return 0;
}
//
//checking module declaration
//
static chkProcModuleDec(gsHandle self,cgrNode node){
    int i;
    char*name;
    char*iname;
    cgrNode item;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    setJumpForStack(self,item,i){
        if(item->type!=Sc_module) continue;
        iname=cgrGetStr(item,cgrKeyId);
        if(!strcmp(name,iname)) return -1;
    }
    return 0;
}

//
//root
//
static int chkProcRoot(gsHandle self,cgrNode node){
    cgrNode dec,predec,next;
    dec=node;
    predec=NULL;
    while(dec){
        next=dec->next;
        switch(dec->type){
        case Sc_module:
            if(chkProcModuleDec(self,dec)) break;
            gsPush(self,dec);
            self->module(self,cgrGetNode(dec,cgrKeyVal));
            cgrSetContext(predec,dec);
            predec=dec;
            break;
        case Function:
            self->function(self,dec);
            break;
        }
        dec=next;
    }
    cgrSetContext(predec,NULL);
    return 0;
}

//
//initialization
//
gsHandle chkProc(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*chkProcRoot);
    gs->module=(*chkProcModule);
    gs->function=(*chkProcFunction);
    return gs;
}

/*------------------------------------------------------------------
  
      to ignore THREAD and task registration
(module).cgrKeyTask=(task_0)-->(task_1) ...    
(module).cgrKeyProcess=(SC_METHOD_0)-->(SC_METHOD_1) ...    

------------------------------------------------------------------*/
//
//checking SC_METHOD
//
static int ignThreadMethod(cgrNode par,char*id,int sw){
    char*name;
    cgrNode proc;
    proc=cgrGetNode(par,cgrKeyProcess);
    while(proc){
        name=cgrGetStr(proc,cgrKeyId);
        if(!strcmp(id,name)){
            cgrSetNum(proc,cgrKeyProcType,sw);
            cgrSetNum(proc,cgrKeyMethFlg,1);
            return 0;
        }
        proc=proc->next;
    }
    return -1;
}

//
//to ignore process
//
static cgrNode ignThreadProcess(cgrNode par,char*id){
    char*name;
    cgrNode proc,prev,next;
    proc=cgrGetNode(par,cgrKeyProcess);
    while(proc){
        name=cgrGetStr(proc,cgrKeyId);
        if(!strcmp(id,name)){
            prev=proc->prev;
            next=proc->next;
            cgrSetContext(prev,next);
            if(!prev) cgrSetNode(par,cgrKeyProcess,next);
            return proc;
        }
        proc=proc->next;
    }
    return NULL;
}

//
//function call
//
static int ignThreadFuncall(gsHandle self,cgrNode node){
    int ret;
    char*id;
    char*pname;
    cgrNode par,args;
    if(!node) return 0;
    //process key
    id=cgrGetStr(node,cgrKeyId);
    if(strcmp(id,"SC_METHOD") &&
       strcmp(id,"SC_THREAD") && 
       strcmp(id,"SC_CTHREAD")) return 0;
    //checking args
    args=cgrGetNode(node,cgrKeyArgs);
    if(!args){
        cmsgEcho(__sis_msg,18,node->file,node->line,id);
        return -1; }
    if(args->type!=Id){
        cmsgEcho(__sis_msg,18,node->file,node->line,id);
        return -1; }
    //process name
    pname=cgrGetStr(args,cgrKeyVal);
    //to search parent
    par=gsPeek(self,Sc_module);
    //SC_METHOD
    if(!strcmp(id,"SC_METHOD")){
        ret=ignThreadMethod(par,pname,1);
        if(ret<0){
            cmsgEcho(__sis_msg,4,node->file,node->line,pname);
            return -1;
        }
    //SC_THREAD   
    }else if(!strcmp(id,"SC_THREAD")){
        ret=ignThreadMethod(par,pname,0);
        if(ret<0){
            cmsgEcho(__sis_msg,4,node->file,node->line,pname);
            return -1;
        }
    //SC_CTHREAD   
    }else if(!strcmp(id,"SC_CTHREAD")){
       cmsgEcho(__sis_msg,6,node->file,node->line,pname);
       ignThreadProcess(par,pname);
    }
    return 0;
}

//
//checking cfg
//
static int ignThreadCfg(struct _gsHandle* self,cgrNode node){
    cgrNode cfg;
    char*statename;
    if(!node) return 0;
    cfg=node;
    while(cfg){
        switch(cfg->type){
        case Dfg :
            self->dfg(self,cfg);break;
        case Block : self->cfgblock(self,cfg);break;
        case Data_type : statename="declare-variable";
        case Break     : statename="break";
        case Continue  : statename="continue";
        case Return    : statename="return";
        case If        : statename="if";
        case Else      : statename="else";
        case For       : statename="for";
        case Do        : statename="do";
        case While     : statename="while";
        case Switch    : statename="switch";
          cmsgEcho(__sis_msg,27,cfg->file,cfg->line,statename);
          break;
        }
        cfg=cfg->next;
    }
    return 0;
}


//
//checking sc_ctor
//
static int ignThreadScCtor(gsHandle self,cgrNode ctr){
    char*id;
    char*name;
    cgrNode mo;
    if(!ctr) return 0;
    id=cgrGetStr(ctr,cgrKeyId);
    mo=gsPeek(self,Sc_module);
    if(!mo) return 0;
    name=cgrGetStr(mo,cgrKeyId);
    if(strcmp(id,name))
       cmsgEcho(__sis_msg,7,ctr->file,ctr->line,id,name);
    cgrSetNode(mo,cgrKeyCtor,mo);   
    return 0;
}

//
//module
//
static int ignThreadModule(gsHandle self,cgrNode node){
    char*id;
    cgrNode mo;
    if(!node) return 0;
    mo=node;
    while(mo){
        switch(mo->type){
        case Sc_ctor :
            ignThreadScCtor(self,mo);
            self->cfg(self,cgrGetNode(mo,cgrKeyVal));
            break;
        }
        mo=mo->next;
    }
    return 0;
}

//
//task resigtration
//
static int ignThreadTasks(cgrNode mo){
    int flg;
    cgrNode proc,prev,next;
    if(!mo) return 0;
    proc=cgrGetNode(mo,cgrKeyProcess);
    while(proc){
        next=proc->next;
        flg=cgrGetNum(proc,cgrKeyMethFlg);
        if(!flg){
            prev=proc->prev;
            cgrSetContext(prev,next);
            proc->prev=NULL;
            if(!prev) cgrSetNode(mo,cgrKeyProcess,next);
            cgrAddNode(mo,cgrKeyTask,proc);
        }
        proc=next;
    }
    return 0;
}

//
//root
//
static int ignThreadRoot(gsHandle self,cgrNode node){
    cgrNode dec;
    dec=node;
    while(dec){
        switch(dec->type){
        case Sc_module :
            gsPush(self,dec);
            self->module(self,cgrGetNode(dec,cgrKeyVal));
            ignThreadTasks(dec);
            gsPop(self);
            break;
        }
        dec=dec->next;
    }
    return 0;
}

//
//initialization
//
gsHandle ignThread(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*ignThreadRoot);
    gs->module=(*ignThreadModule);
    gs->funcall=(*ignThreadFuncall);
    gs->cfg=(*ignThreadCfg);
    return gs;
}

/*------------------------------------------------------------------
  
  checking variable declaration
(module).cgrKeySignal=(dec_0)-->(dec_1)--> ... 

------------------------------------------------------------------*/
//
//setting stack pointer
//
//push
static void chkVarSpPush(gsHandle self){
    static cgrNode esp=NULL;
    if(!esp) esp=cgrNew(Esp,"",0);
    gsPush(self,esp);
}
//pop
static cgrNode chkVarSpPop(gsHandle self){
    cgrNode dec;
    while(dec=gsPop(self))
        if(dec->type==Esp) break;
    return dec;  
}

//
//setting declaration
//
static cgrNode chkVarSetDec(cgrNode var,
    int size,int type,int signal,char*opt){
    int ptr=0;
    int addr=0;
    int arrsize=0;
    cgrNode dec;
    cgrNode ini=NULL;
    cgrNode array=NULL;
    if(!var) return NULL;
    dec=cgrNew(Declare,var->file,var->line);
    switch(type){
    case Bool :size=1;break;    
    case Uchar :size=8;break;    
    case Schar :size=8;break;    
    case Ushort :size=16;break;    
    case Sshort :size=16;break;    
    case Uint :size=64;break;    
    case Sint :size=64;break;    
    case Ulong :size=64;break;    
    case Slong :size=64;break;    
    }
    cgrSetNum(dec,cgrKeySize,size);
    cgrSetNum(dec,cgrKeyType,type);
    cgrSetNum(dec,cgrKeySignal,signal);
    cgrSetStr(dec,cgrKeyOpt,opt);
    while(var){
        //initializer
        if(var->type=='='){
            ini=cgrGetNode(var,cgrKeyRight);
            var=cgrGetNode(var,cgrKeyLeft);
        //array
        }else if(var->type==Array){
            arrsize++;
            if(!array) array=fsmdCopyOpe(var);
            var=cgrGetNode(var,cgrKeyLeft);
        //pointer
        }else if(var->type=='*'){
            ptr++;
            var=cgrGetNode(var,cgrKeyRight);
        //address
        }else if(var->type=='&'){
            addr++;
            var=cgrGetNode(var,cgrKeyRight);
        //normally
        }else if(var->type==Id){
            cgrSetStr(dec,cgrKeyId,cgrGetStr(var,cgrKeyVal));
            break;
        //error
        }else{
            cmsgEcho(__sis_msg,2,var->file,var->line,"declaration");
            return NULL;
        }
    }
    cgrSetNode(dec,cgrKeyIni,ini);
    cgrSetNode(dec,cgrKeyArray,array);
    cgrSetNum(dec,cgrKeyPointer,ptr);
    cgrSetNum(dec,cgrKeyAddress,addr);
    cgrSetNum(dec,cgrKeyArraySize,arrsize);
    cgrSetNode(var,cgrKeySignal,dec);
    return dec;
}

//
//variable declaration
//
static int chkVarDataType(gsHandle self,cgrNode node){
    cgrNode dec,mo;
    cgrNode var,opt_val,tnode,pnode;
    int signal,type,size,ptr,addr;
    char*opt;
    //checking node
    if(!node) return -1;
    //checking module
    mo=gsPeek(self,Sc_module);
    if(!mo) return -1;
    //checking variable
    var=cgrGetNode(node,cgrKeyVal);
    if(!var) return -1;
    //checking data type
    tnode=cgrGetNode(node,cgrKeyType);
    if(!tnode) return -1;
    type=tnode->type;
    size=cgrGetNum(node,cgrKeySize);
    //checking signal key
    signal=Sc_signal;
    //checking option
    opt_val=cgrGetNode(node,cgrKeyOpt);
    if(opt_val) opt=cgrGetStr(opt_val,cgrKeyVal);
    else opt="";
    //getting pointer node
    ptr=0;
    addr=0;
    pnode=cgrGetNode(node,cgrKeyPointer);
    cgrMaps(pnode,
        if(pnode->type=='*') ptr++;
        else if(pnode->type=='&') addr++);
    //creating variables
    while(var){ 
        cgrNode next=var->next;
        if(var->type=='='){
            self->ope(self,cgrGetNode(var,cgrKeyRight));
            var=cgrGetNode(var,cgrKeyLeft);
        }
        dec=chkVarSetDec(var,size,type,signal,opt);
        if(!dec) break;
        //checking pointer
        if(ptr) cgrSetNum(dec,cgrKeyPointer,ptr);
        if(addr) cgrSetNum(dec,cgrKeyAddress,addr);
        //normally
        cgrAddNode(mo,cgrKeySignal,dec);
        gsPush(self,dec);
        var=next; 
    }
    return 0;
}

//
//making signal declaration from sc_signal_key
//
static int chkVarMakeSigDec(gsHandle self,
    cgrNode mo,cgrNode node){
    cgrNode tmp;
    cgrNode dec;
    cgrNode var,opt_val,data_type,sig_key;
    int signal,type,size;
    char*opt;
    regex_t*r=self->work;
    regmatch_t t;

    //checking node
    if(!node) return -1;
    //checking module
    mo=gsPeek(self,Sc_module);
    if(!mo) return -1;
    //checking variable
    var=cgrGetNode(node,cgrKeyVal);
    if(!var) return -1;
    //checking data type
    data_type=cgrGetNode(node,cgrKeySize);
    if(!data_type) return -1;
    tmp=cgrGetNode(data_type,cgrKeyType);
    if(!tmp) return -1;
    type=tmp->type;
    size=cgrGetNum(data_type,cgrKeySize);
    //checking signal key
    sig_key=cgrGetNode(node,cgrKeyType);
    if(!sig_key) return -1;
    signal=sig_key->type;
    //checking I/O type
    switch(signal){
    case Sc_fifo     :
        cmsgEcho(__sis_msg,54,node->file,node->line);
        return -1;
    case Sc_fifo_in  :
        cmsgEcho(__sis_msg,55,node->file,node->line);
        return -1;
    case Sc_fifo_out :
        cmsgEcho(__sis_msg,56,node->file,node->line);
        return -1;
    default : break;
    }

    //checking option
    opt_val=cgrGetNode(node,cgrKeyOpt);
    if(opt_val) opt=cgrGetStr(opt_val,cgrKeyVal);
    else opt="";
    //creating variables
    while(var){
        char*varname;
        cgrNode tmp;
        dec=chkVarSetDec(var,size,type,signal,opt);
        if(!dec) break;
        //checking pointer
        if(cgrGetNum(dec,cgrKeyPointer)||
            cgrGetNum(dec,cgrKeyAddress))
            cmsgEcho(__sis_msg,8,var->file,var->line);
        //checking address
        if(cgrGetNode(dec,cgrKeyIni))
            cmsgEcho(__sis_msg,9,var->file,var->line);
        //normally
        cgrAddNode(mo,cgrKeySignal,dec);
        gsPush(self,dec);
        tmp=var->next;
        var->next=NULL;
        self->ope(self,var);
        varname=cgrGetStr(dec,cgrKeyId);
        if(!regexec(r,varname,1,&t,REG_EXTENDED))
            cgrSetNode(mo,cgrKeyReset,dec);
        var->next=tmp;
        var=var->next; 
    }
    return 0;
}

//
//fucntion
//
static int chkVarFunction(gsHandle self,cgrNode node){
    cgrNode args,datatype;
    if(!node) return -1;
    if(cgrGetNum(node,cgrKeyIsOutOfModule)) return 0;
    args=cgrGetNode(node,cgrKeyArgs);
    if(args){
        cgrNode type=cgrGetNode(args,cgrKeyType);
        if(args->next){
            cmsgEcho(__sis_msg,10,node->file,node->line);
            return -1;}
        if(type->type!=Void){
            cmsgEcho(__sis_msg,10,node->file,node->line);
            return -1;}
    }
    datatype=cgrGetNode(node,cgrKeyType);
    if(datatype){ 
        cgrNode type=cgrGetNode(datatype,cgrKeyType);
        if(type->type!=Void){
            cmsgEcho(__sis_msg,11,node->file,node->line);
            return -1;}
    }
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
//
//checking function call (part selected variable)
//
static chkVarFuncall(gsHandle self,cgrNode node){
    int i;
    char*id;
    char*name;
    cgrNode dec;
    cgrNode left;

    if(!node) return 0;
    if(cgrGetNum(node,cgrKeyIsPartSelec)){
        left=cgrGetNode(node,cgrKeyLeft);
        self->ope(self,left);
        self->args(self,cgrGetNode(node,cgrKeyArgs));
        return 0;
    }
    left=cgrGetMaxLeft(node);
    if(left->type==Id) name=cgrGetStr(left,cgrKeyVal);
    else name=cgrGetStr(left,cgrKeyId);
    if(!strcmp(name,"wait")) return 0;
    setJumpForStack(self,dec,i){    
        if(dec->type!=Declare) continue;
        id=cgrGetStr(dec,cgrKeyId);
        if(!strcmp(name,id)){
            cgrSetNode(node,cgrKeySignal,dec);
            cgrSetNode(left,cgrKeySignal,dec);
            cgrSetNum(node,cgrKeyIsPartSelec,1);
            cgrSetNum(left,cgrKeyIsPartSelec,1);
            return 0;
        }
    }
    cmsgEcho(__sis_msg,53,node->file,node->line,name);
    return -1;
}

//
//checking variable
//
static chkVarCheckId(gsHandle self,cgrNode node){
    int i;
    char*id;
    char*name;
    cgrNode dec;
    if(!node) return 0;
    if(cgrGetNum(node,cgrKeyVarType)==1) return 0;
    name=cgrGetStr(node,cgrKeyVal);
    setJumpForStack(self,dec,i){    
        if(dec->type!=Declare) continue;
        id=cgrGetStr(dec,cgrKeyId);
        if(!strcmp(name,id)){
            cgrSetNode(node,cgrKeySignal,dec);
            return 0;
        }
    }
    cmsgEcho(__sis_msg,13,node->file,node->line,name);
    return -1;
}

//
//operation
//
#define chkSubGetFuncallName(node,name) {          \
    name=cgrGetStr(node,cgrKeyId);                 \
    if(!strcmp(name,"")){                          \
        cgrNode left=cgrGetNode(node,cgrKeyLeft);  \
        while(left){                               \
            if(left->type==Id) break;              \
            left=cgrGetNode(left,cgrKeyLeft);      \
        }if(!left) name="";                        \
        name=cgrGetStr(left,cgrKeyVal);           }}
static int chkVarOpe(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id:chkVarCheckId(self,ope); break;
        case Real:cmsgEcho(__sis_msg,20,ope->file,ope->line);break;
        case String:cmsgEcho(__sis_msg,21,ope->file,ope->line);break;
        case Funcall:{
            char*name;
            chkSubGetFuncallName(ope,name);
            if(strcmp(name,"SC_METHOD")&&
               strcmp(name,"SC_THREAD")&&
               strcmp(name,"SC_CTHREADD")) self->funcall(self,ope);
            }break;
        case Data_type:self->datatype(self,ope);break;
        case '=' :gsOpeLinkEq(ope); break;
        default  :gsOpeLink(ope); break;
        }
        ope=ope->next;
    }
    return 0;
}

//
//CFG
//
static int chkVarCfg(struct _gsHandle* self,cgrNode node){
    cgrNode cfg;
    if(!node) return 0;
    cfg=node;
    while(cfg){
        switch(cfg->type){
        case Dfg : self->dfg(self,cfg);break;
        case Data_type : self->vardec(self,cfg);break;
        case Break : self->cfgbreak(self,cfg);break;
        case Continue : self->cfgcontinue(self,cfg);break;
        case Return : self->cfgreturn(self,cfg);break;
        case If : self->cfgif(self,cfg);break;
        case Else : self->cfgelse(self,cfg);break;
        case Switch : self->cfgswitch(self,cfg);break;
        case For : 
            chkVarSpPush(self);
            self->cfgfor(self,cfg);
            chkVarSpPop(self);
            break;
        case Do : 
            chkVarSpPush(self);
            self->cfgdo(self,cfg);
            chkVarSpPop(self);
            break;
        case While :
            chkVarSpPush(self);
            self->cfgwhile(self,cfg);
            chkVarSpPop(self);
            break;
        case Block :
            chkVarSpPush(self);
            self->cfgblock(self,cfg);
            chkVarSpPop(self);
            break;
        }
        cfg=cfg->next;
    }
    return 0;
}


//
//instance port
//
static int chkVarInstancePort(gsHandle self,cgrNode mo,cgrNode node){
    cgrNode port=node;
    while(port){
        char*name=cgrGetStr(port,cgrKeyId);
        cgrNode sig=cgrGetNode(mo,cgrKeySignal);
        while(sig){   
            char* sname=cgrGetStr(sig,cgrKeyId); 
            if(!strcmp(sname,name)){            
                cgrSetNode(port,cgrKeyCheckVar,sig); 
                cgrSetNum(sig,cgrKeyWire,1);
                break; 
            }
            sig=sig->next; 
        }
        port=port->next;
    }
    return 0;
}

//
//module
//
static int chkVarModule(struct _gsHandle* self,cgrNode node){
    cgrNode inst; //instance node
    cgrNode mo,proc,dec;
    if(!node) return 0;
    mo=cgrGetNode(node,cgrKeyVal);
    chkVarSpPush(self);
    gsPush(self,node);
    while(mo){
        switch(mo->type){
        case Sc_signal_key:
            chkVarMakeSigDec(self,node,mo);
            break;
        }
        mo=mo->next;
    }
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        self->function(self,proc);
        proc=proc->next;
    }

    //checking instans port
    inst=cgrGetNode(node,cgrKeyInstance);
    while(inst){
        chkVarInstancePort(self
            ,node,cgrGetNode(inst,cgrKeySignal));
        inst=inst->next;
    }

    chkVarSpPop(self);
    return 0;
}

//
//destruction
//
void chkVarDes(gsHandle self){
    regex_t* r=self->work;
    regfree(r);
    free(r);
}
//
//initialization
//
gsHandle chkVar(chk2OptHandle opt,cgrNode top){
    gsHandle gs;
    regex_t* r;

    //regex
    cstrMakeBuf(r,1,regex_t,
        memError("compiling regular expression of reset name"));
    if(regcomp(r,opt.rstname,REG_EXTENDED))
        memError("compiling regular expression of reset name");

    //init
    gs=gsIni(top);
    gs->work=r;
    gs->module=(*chkVarModule);
    gs->cfg=(*chkVarCfg);
    gs->function=(*chkVarFunction);
    gs->datatype=(*chkVarDataType);
    gs->ope=(*chkVarOpe);
    gs->funcall=(*chkVarFuncall);
    return gs;
}


/*------------------------------------------------------------------
  
  variable name creation

------------------------------------------------------------------*/
//
// variable name creation
//
static int creVarCreateName(cgrNode node){
    int i;
    cMap map;
    char*name;
    char*tmpstr;
    char ctrue=1;
    cgrNode var;
    cVector memvect;
    static char buf[SIS_BUF_SIZE];
    if(!node) return 0;
    //name table initialization
    memvect=cvectIni(sizeof(char*),20);
    map=cmapIni(sizeof(char*),
        sizeof(char),(*cmapStrKey),(*cmapStrCmp));
    var=cgrGetNode(node,cgrKeySignal);
    var=cgrGetTail(var);
    //name creation
    while(var){
        name=cgrGetStr(var,cgrKeyId);
        cstrSet(tmpstr,name,memError("checking syntax"));
        for(i=1;cmapFind(map,&name);i++){
            sprintf(buf,"%s__%.6d",tmpstr,i);
            name=buf;
        }
        cstrFree(tmpstr);
        cstrSet(tmpstr,name,memError("checking syntax"));
        cvectAppend(memvect,tmpstr);
        cmapUpdate(map,&tmpstr,&ctrue);
        cgrSetStr(var,cgrKeyId,tmpstr);
        var=var->prev;
    }
    //memory free
    cmapDes(map);
    for(i=0;i<cvectSize(memvect);i++)
        cstrFree(cvectElt(memvect,i,char*));
    cvectDes(memvect);
    return 0;
}

//
// changing variable name
//
static int creVarChangeVar(cgrNode node){
    char*name;
    char*varname;
    cgrNode dec;
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    if(!dec)
        return 0;
    name=cgrGetStr(dec,cgrKeyId);
    if(node->type==Id){ //x
        varname=cgrGetStr(node,cgrKeyVal);
        if(strcmp(name,varname)) cgrSetStr(node,cgrKeyVal,name);
    }else{  //x(...)
        cgrNode left;
        left=cgrGetMaxLeft(node);
        if(left->type==Id){ //x(...)
            varname=cgrGetStr(left,cgrKeyVal);
            if(strcmp(name,varname))
                cgrSetStr(left,cgrKeyVal,name);
        }else{ //x[...](...)
            varname=cgrGetStr(left,cgrKeyId);
            if(strcmp(name,varname)) 
            cgrSetStr(left,cgrKeyId,name);
        }
    }
    return 0;    
}

//
// id
//
static int creVarId(gsHandle self,cgrNode node){
    if(!node) return 0;
    creVarChangeVar(node);
    return 0;
}

//
//funcall
//
static int creVarFuncall(gsHandle self,cgrNode node){
    if(!node) return 0;
    creVarChangeVar(node);
    return 0;
}

//
//module
//
static int creVarModule(gsHandle self,cgrNode node){
    cgrNode proc;
    if(!node) return 0;
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        self->function(self,proc);
        proc=proc->next;
    }
    return 0;
}

//
//root
//
static int creVarRoot(struct _gsHandle* self,cgrNode node){
    cgrNode dec;
    dec=node;
    while(dec){
        switch(dec->type){
        case Sc_module :
           creVarCreateName(dec);
           self->module(self,dec);
           break;
        }
        dec=dec->next;
    }
    return 0;
}

//
//initialization
//
gsHandle creVar(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*creVarRoot);
    gs->module=(*creVarModule);
    gs->id=(*creVarId);
    gs->funcall=(*creVarFuncall);
    return gs;
}

/*------------------------------------------------------------------
  
  getting sensitive

------------------------------------------------------------------*/

//
//setting sensitive (pos(),neg())
//
static int getSenSetSensitive( gsHandle self,cgrNode node,int sw);
static int getSenSetSensitivePosNeg( gsHandle self,cgrNode node,int sw){
    char*name;
    char*posneg;
    cgrNode left;
    cgrNode right;

    left=cgrGetNode(node,cgrKeyLeft);
    right=cgrGetNode(node,cgrKeyRight);
    //check node
    if((!left)||(!right)){
        cmsgEcho(__sis_msg,16,node->file,node->line);
        return -1;
    }
    if((left->type!=Id)||(right->type!=Funcall)){
        cmsgEcho(__sis_msg,16,node->file,node->line);
        return -1;
    }
    //pos()
    posneg=cgrGetStr(right,cgrKeyId);
    if(!strcmp(posneg,"pos")) 
        getSenSetSensitive(self,left,cgrKeySensitivePos);
    //neg()
    else if(!strcmp(posneg,"neg")) 
        getSenSetSensitive(self,left,cgrKeySensitiveNeg);
    //error
    else{
        cmsgEcho(__sis_msg,16,node->file,node->line);
        return -1;
    }
    return 0;
}

//
//setting sensitive
//
static int getSenSetSensitive( gsHandle self,cgrNode node,int sw){
    cgrNode dec,proc,id;
    if(!node) return 0;
    //sensitive<<clk
    if(node->type==Id){
        proc=gsPeek(self,Function);
        if(!proc) return 0;
        dec=cgrGetNode(node,cgrKeyDeclared); 
        if(!dec) return -1;
        id=cgrNew(Id,node->file,node->line);
        cgrSetStr(id,cgrKeyVal,cgrGetStr(node,cgrKeyVal));
        cgrSetNode(id,cgrKeySignal,dec);
        cgrAddNode(proc,sw,id);
        return 0;
    //sensitive<<clk.pos();
    }else if((node->type=='.')||(node->type==Arr))
        return getSenSetSensitivePosNeg(self,node,sw);

    cmsgEcho(__sis_msg,16,node->file,node->line);
    return -1;
}

//
//searching node
//
static int getSenSearchNode(gsHandle self,cgrNode node,int sw){
    if(!node) return 0;
    if(node->type !=Lshif) return 0;
    getSenSetSensitive(self,cgrGetNode(node,cgrKeyRight),sw);
    getSenSearchNode(self,cgrGetNode(node,cgrKeyPar),sw);
    return 0;
}

//
//checking sensitive
//
static int getSenCheckSensitive(gsHandle self,cgrNode node){
    int sw;
    char*name;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyVal);
    if(!strcmp(name,"sensitive")) sw=cgrKeySensitive;
    else if(!strcmp(name,"sensitive_pos")) sw=cgrKeySensitivePos;
    else if(!strcmp(name,"sensitive_neg")) sw=cgrKeySensitiveNeg;
    else return 0;
    getSenSearchNode(self,cgrGetNode(node,cgrKeyPar),sw);
    return 0;
}
//
//function call
//
static int getSenFuncall(gsHandle self,cgrNode node){
    char*name;
    char*pname;
    cgrNode proc,mo,args;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    if(strcmp(name,"SC_METHOD") &&
       strcmp(name,"SC_THREAD")) return 0;
    args=cgrGetNode(node,cgrKeyArgs);
    if(args->type!=Id) return 0;
    name=cgrGetStr(args,cgrKeyVal);
    mo=gsPeek(self,Sc_module);
    if(!mo) return 0;
    proc=cgrGetNode(mo,cgrKeyProcess);
    while(proc){
        pname=cgrGetStr(proc,cgrKeyId);
        if(!strcmp(pname,name)) gsPush(self,proc);
        proc=proc->next;
    }
    return 0;
}

//
// id
//
static int getSenId(gsHandle self,cgrNode node){
    if(!node) return 0;
    getSenCheckSensitive(self,node);
    return 0;
}

//
//module
//
static int getSenModule(gsHandle self,cgrNode node){
    cgrNode proc;
    if(!node) return 0;
    proc=cgrGetNode(node,cgrKeyVal);
    chkVarSpPush(self);
    gsPush(self,node);
    while(proc){
        if(proc->type==Sc_ctor)
            self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        proc=proc->next;
    }
    chkVarSpPop(self);
    return 0;
}

//
//initialization
//
gsHandle getSen(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*getSenModule);
    gs->funcall=(*getSenFuncall);
    gs->id=(*getSenId);
    return gs;
}


/*------------------------------------------------------------------
  
  instance creation

------------------------------------------------------------------*/
//
//checking variable declaration
//
static int creInsCheckVarDec(gsHandle self,cgrNode node){
    char*name;
    char*dname;
    cgrNode mo,dec;
    if(!node) return 0;
    if(node->type!=Id) return 0;
    mo=gsPeek(self,Sc_module);
    if(!mo) return 0;
    name=cgrGetStr(node,cgrKeyVal);
    if(!strcmp(name,"sensitive")) return 0;
    if(!strcmp(name,"sensitive_pos")) return 0;
    if(!strcmp(name,"sensitive_neg")) return 0;
    dec=cgrGetNode(node,cgrKeyDeclared);
    while(dec){
        cgrNode var=cgrGetNode(dec,cgrKeyVal);
        dname=cgrGetStr(var,cgrKeyVal);
        if(!strcmp(dname,name)) return 0;
        dec=dec->next;
    }
    dec=cgrGetNode(mo,cgrKeyInstance);
    while(dec){
        dname=cgrGetStr(dec,cgrKeyIni);
        if(!strcmp(dname,name)) return 0;
        dec=dec->next;
    }
    cmsgEcho(__sis_msg,13,node->file,node->line,name);
    return -1;
}

//
//instance addition
//
static int creInsAddInstance(gsHandle self,
    char*dec,char*type,char*name,int ptr,char*file,int line){
    int i,len;
    cgrNode mo,node;
    mo=gsPeek(self,Sc_module);
    if(!mo) return -1;
    len=strlen(name);
    if(len<=0) cmsgEcho(__sis_msg,37,file,line);
    for(i=0;i<len;i++)
        if(isspace(name[i])) name[i]='_';
    node=cgrNew(Instance,file,line);
    cgrSetStr(node,cgrKeyId,name);
    cgrSetStr(node,cgrKeyIni,dec);
    cgrSetStr(node,cgrKeyType,type);
    cgrSetNum(node,cgrKeyPointer,ptr);
    cgrAddNode(mo,cgrKeyInstance,node);
    return 0;
}

//
//checking instance declaration
//
static int creInsCheckDeclare(gsHandle self,cgrNode node){
    char*name;
    cgrNode ptr;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    cgrSetStr(node,cgrKeyVal,name);
    gsPush(self,node);
    ptr=cgrGetNode(node,cgrKeyPointer);
    if(!ptr) cmsgEcho(__sis_msg,36,node->file,node->line,name);
    else if(ptr->next || (ptr->type != '*'))
        cmsgEcho(__sis_msg,14,node->file,node->line,name);
    return 0;
}

//
//finding instance declaration from module
//
static cgrNode creInsFindDec(gsHandle self,char*name){
    char*tname;
    cgrNode mo,dec;
    mo=gsPeek(self,Sc_module);
    if(!mo) return NULL;
    dec=cgrGetNode(mo,cgrKeyInstance);
    while(dec){
        tname=cgrGetStr(dec,cgrKeyId);
        if(!strcmp(tname,name)) return dec;
        dec=dec->next;
    }
    return NULL;
}

//
//finding instance declaration from name
//
static cgrNode creInsFindName(gsHandle self,char*name){
    int i;
    char*iname;
    cgrNode item;
    setJumpForStack(self,item,i){
        if(item->type!=Instance)continue;
        iname=cgrGetStr(item,cgrKeyId);
        if(!strcmp(name,iname)) return item;
    }
    return NULL;
}

//
//flat port connection
//
#define creInsCreatePort(port,node,name,signal,deced){ \
    port=cgrNew(Id,node->file,node->line);             \
    cgrSetNode(port,cgrKeyDeclared,deced);             \
    cgrSetStr(port,cgrKeyId,name);                     \
    cgrSetNum(port,cgrKeyIsInstancePort,1);            \
    cgrSetStr(port,cgrKeySignal,signal);}

static int creInsFlatConnect(gsHandle self,cgrNode node){
    char*name;
    char*argname;
    cgrNode port,dec,args;
    if(!node) return 0;
    name=cgrGetStr(node,cgrKeyId);
    dec=creInsFindName(self,name);
    if(!dec) return 0;
    name=cgrGetStr(dec,cgrKeyVal);
    dec=creInsFindDec(self,name);
    if(!dec) return 0;
    args=cgrGetTail(cgrGetNode(node,cgrKeyArgs));
    while(args){
        if(args->type != Id)
            cmsgEcho(__sis_msg,15,node->file,node->line,name);
        else{
            cgrNode deced=cgrGetNode(args,cgrKeyDeclared); //declared node
            argname=cgrGetStr(args,cgrKeyVal);
            creInsCreatePort(port,node,argname,"",deced);
            cgrAddNode(dec,cgrKeySignal,port);
        }
        args=args->prev;
    }
}

//
//port connection with dot
//
static int creInsDotConnect(
    gsHandle self,cgrNode par,cgrNode node){
    char*name;
    char*signal;
    cgrNode left,dec,args,port;
    cgrNode deced; //declared node
    left=cgrGetNode(par,cgrKeyLeft);
    if(!left) return 0;
    if(left->type!=Id) return 0;
    name=cgrGetStr(left,cgrKeyVal);
    dec=creInsFindName(self,name);
    if(!dec) return 0;
    name=cgrGetStr(dec,cgrKeyVal);
    dec=creInsFindDec(self,name);
    if(!dec) return 0;
    if((par->type =='.') && cgrGetNum(dec,cgrKeyPointer))
        cmsgEcho(__sis_msg,17,node->file,node->line,name,".");
    if((par->type ==Arr) && !cgrGetNum(dec,cgrKeyPointer))
        cmsgEcho(__sis_msg,17,node->file,node->line,name,"->");
    args=cgrGetNode(node,cgrKeyArgs);
    if((args->type != Id ) || args->next)
        cmsgEcho(__sis_msg,15,node->file,node->line,name);
    deced=cgrGetNode(args,cgrKeyDeclared);
    name=cgrGetStr(args,cgrKeyVal);
    signal=cgrGetStr(node,cgrKeyId);
    creInsCreatePort(port,node,name,signal,deced);
    cgrAddNode(dec,cgrKeySignal,port);
    return 0;
}

//
//new instance creation
//
static int creInsNewInstance(
  gsHandle self,cgrNode par,cgrNode node){
    char*name;
    char*mname;
    char*iname;
    cgrNode dec,eq,var,args;
    if(!node) return 0;
    eq=cgrGetNode(par,cgrKeyPar);
    if(!eq) return 0;
    if(eq->type != '=') return 0;
    var=cgrGetNode(eq,cgrKeyLeft);
    if(!var) return 0;
    if(var->type != Id) return 0;
    name=cgrGetStr(var,cgrKeyVal);
    dec=creInsFindName(self,name);
    if(!dec) return 0;
    mname=cgrGetStr(node,cgrKeyId);
    args=cgrGetNode(node,cgrKeyArgs);
    if(!args){
        cmsgEcho(__sis_msg,19,node->file,node->line,name);
        return -1;
    }else if((args->type != String)||args->next){
        cmsgEcho(__sis_msg,19,node->file,node->line,name);
        return -1;
    }
    iname=cgrGetStr(args,cgrKeyVal);
    creInsAddInstance(self,name,mname,iname,1,node->file,node->line);
    cgrSetStr(dec,cgrKeyVal,iname);
    return 0;
}

//
//function call
//
static int creInsFuncall(gsHandle self,cgrNode node){
    char*name;
    cgrNode par;
    if(!node) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    //checking function name
    name=cgrGetStr(node,cgrKeyId);
    if(!strcmp(name,"SC_METHOD"))  return 0;
    if(!strcmp(name,"SC_THREAD"))  return 0;
    if(!strcmp(name,"SC_CTHREAD")) return 0;
    //arguments checking
    self->args(self,cgrGetNode(node,cgrKeyArgs));
    //port connection
    if(!par) creInsFlatConnect(self,node);
    else switch(par->type){
    case '.' :
    case Arr :
        creInsDotConnect(self,par,node);
        break;
    case New :
      creInsNewInstance(self,par,node);
      break;
    }
    return 0;
}

//
//id
//
static int creInsId(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    creInsCheckVarDec(self,node); 
    return 0;
}

//
//module
//
static int creInsModule(gsHandle self,cgrNode node){
    cgrNode proc;
    if(!node) return 0;
    proc=cgrGetNode(node,cgrKeyVal);
    chkVarSpPush(self);
    gsPush(self,node);
    while(proc){
        switch(proc->type){
        case Sc_ctor :
            self->cfg(self,cgrGetNode(proc,cgrKeyVal));
            break;
        case Instance :
            creInsCheckDeclare(self,proc);
            break;
        }    
        proc=proc->next;
    }
    
    
    chkVarSpPop(self);
    return 0;
}

//
//initialization
//
gsHandle creIns(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*creInsModule);
    gs->funcall=(*creInsFuncall);
    gs->id=(*creInsId);
    return gs;
}


/*------------------------------------------------------------------
  
  translation read/write to blocking assingment

------------------------------------------------------------------*/
//
//x.read()(10,0) -> x(10,0)
//
static int traRWReadSpan(cgrNode node){
    cgrNode left,par,args;
    if(!node) return 0;
    if(node->type!=Funcall) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    if(par->type!='.') return 0;
    par->type=Funcall;
    args=cgrGetNode(node,cgrKeyArgs);
    cgrSetNode(par,cgrKeyArgs,args);
    cgrSetNode(par,cgrKeyRight,NULL);
    return 0;
}

//
//translation read function
//
static int traRWRead(gsHandle self,cgrNode node){
    cgrNode left,args,par;
    if(!node) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    if(!par) return 0;
    //x.read()(10,0) -> x(10,0)
    if(par->type==Funcall)
        return traRWReadSpan(par);
    if(par->type!='.') return 0;
    left=cgrGetNode(par,cgrKeyLeft);
    if(!left) return 0;
    args=cgrGetNode(node,cgrKeyArgs);
    //x.read() --> x
    if(!args){
        remDeadSetPar(self,par,left);
        return 0;
    }
    //error
    if(args->next){
        cmsgEcho(__sis_msg,22,node->file,node->line,"read");
        args->next=NULL;
    }
    //x.read(y) --> y=x
    par->type='=';
    cgrSetNode(args,cgrKeyPar,par);
    cgrSetNode(par,cgrKeyLeft,args);
    cgrSetNode(par,cgrKeyRight,left);
    return 0;
}
//
//translation write function
//
static int traRWWrite(cgrNode node){
    cgrNode left,args,par;
    if(!node) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    if(!par) return 0;
    if(par->type!='.') return 0;
    left=cgrGetNode(par,cgrKeyLeft);
    if(!left) return 0;
    args=cgrGetNode(node,cgrKeyArgs);
    //error
    if(!args)
      cmsgEcho(__sis_msg,23,node->file,node->line,"write");
    else if(args->next)
        cmsgEcho(__sis_msg,23,node->file,node->line,"write");
   
    //x.write(y) --> x=y
    par->type='=';
    cgrSetNode(args,cgrKeyPar,par);
    cgrSetNode(par,cgrKeyRight,args);
    cgrSetNum(par,cgrKeyIsNonBlocking,1);
    return 0;
}

//
//function call
//
int traRWFuncall(gsHandle self,cgrNode node){
    char*name;
    cgrNode left,task;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    self->ope(self,left);
    self->args(self,cgrGetNode(node,cgrKeyArgs));
    chkSubGetFuncallName(node,name);
    if(!strcmp(name,"read")) traRWRead(self,node);
    else if(!strcmp(name,"write")) traRWWrite(node);
    task=gsCheckTask(self,node);
    if(task) self->cfg(self,cgrGetNode(task,cgrKeyVal));
    return 0;
}

//
//initialization
//
gsHandle traRW(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->cfg=(*gsCfg2);
    gs->module=(*gsModule2);
    gs->funcall=(*traRWFuncall);
    return gs;
}


/*------------------------------------------------------------------
  
  checking synthesizable subset syntax
  # cast
  # pointer
  # structure
  # bit/part selected variable
  # function call that from out of module
   
------------------------------------------------------------------*/
//
//function call
//
#define chkSubFuncallCheckOut(node,msgFunction){\
    cgrNode par=cgrGetNode(node,cgrKeyPar);     \
    if(par) switch(par->type){                  \
    case Inc :                                  \
    case Dec :                                  \
        msgFunction;                            \
        break;                                  \
    case '=' :                                  \
    case Mueq:                                  \
    case Dieq:                                  \
    case Moeq:                                  \
    case Adeq:                                  \
    case Sueq:                                  \
    case Aneq:                                  \
    case Xoeq:                                  \
    case Oreq:                                  \
    case Lseq:                                  \
    case Rseq:                                  \
        if(node==cgrGetNode(par,cgrKeyLeft))    \
            msgFunction;                        \
        break; }                                } 
static int chkSubFuncall(gsHandle self,cgrNode node){
    char*name;
    char*tname;
    cgrNode mo,task,dec;
    if(!node) return 0;
    chkSubGetFuncallName(node,name);
    if(!strcmp(name,"")) return 0;
    if(!strcmp(name,"SC_METHOD") ||
       !strcmp(name,"SC_THREAD") ||
       !strcmp(name,"SC_CTHREAD")) return 0;
    self->args(self,cgrGetNode(node,cgrKeyArgs));
    mo=gsPeek(self,Sc_module);
    if(!mo) return 0;
    task=cgrGetNode(mo,cgrKeyTask);
    while(task){
        cgrNode next=task->next;
        //is function out of module
        if(task->type==Id)
            task=cgrGetNode(task,cgrKeyIdLinkTask);
        //get function declaration
        tname=cgrGetStr(task,cgrKeyId);
        if(!strcmp(tname,name)){
            chkSubFuncallCheckOut(node,
                cmsgEcho(__sis_msg,33,node->file,node->line,name));
            cgrSetNode(node,cgrKeySignal,task);    
            return 0;
        }
        task=next;
    }
    dec=cgrGetNode(mo,cgrKeySignal);
    while(dec){
        tname=cgrGetStr(dec,cgrKeyId);
        if(!strcmp(tname,name)){
            chkSubFuncallCheckOut(node,
                cgrSetNum(node,cgrKeyIsPartSelec,1));
            cgrSetNode(node,cgrKeySignal,dec);    
        }
        dec=dec->next;
    }
    if(!cgrGetNum(node,cgrKeyIsPartSelec)){
        cmsgEcho(__sis_msg,26,node->file,node->line,name);
    }
    return 0;
}
//
//checking bit selection
//
static int chkSubOpeCheckBit(gsHandle self,cgrNode node){
    char*name;
    int cnt,asize;
    cgrNode prev,par,dec;
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    if(!dec) return 0;
    asize=cgrGetNum(dec,cgrKeyArraySize);
    par=cgrGetNode(node,cgrKeyPar);
    if(cgrGetNode(par,cgrKeyRight)==node) return 0;
    for(cnt=0;par;cnt++){
        if(par->type!=Array) break;
        prev=par;
        par=cgrGetNode(par,cgrKeyPar);
    }
    if(asize==cnt) return 0;
    name=cgrGetStr(node,cgrKeyVal);
    if(asize>cnt){
        cmsgEcho(__sis_msg,35,node->file,node->line,name);
        return 0;
    }
    chkSubFuncallCheckOut(prev,
        cmsgEcho(__sis_msg,34,node->file,node->line,name));
    return 0;
}

//
//checking operator
//
static int chkSubOperator(cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case '=':{
        cgrNode left=cgrGetNode(node,cgrKeyLeft);
        if(left->type==Id){
            cgrNode dec=cgrGetNode(left,cgrKeySignal);
            cgrSetNum(dec,cgrKeyWire,0);
        }}break;
    case '.':
    case Arr:cmsgEcho(__sis_msg,24,node->file,node->line);break;
    case Cast:cmsgEcho(__sis_msg,25,node->file,node->line);break;
    case '*':    
        if(!cgrGetNode(node,cgrKeyLeft))    
            cmsgEcho(__sis_msg,12,node->file,node->line);
        break;
    case '&':    
        if(!cgrGetNode(node,cgrKeyLeft))    
            cmsgEcho(__sis_msg,12,node->file,node->line);
        break;
    }
    return 0;
}

//
//operation
//
static int chkSubOpe(struct _gsHandle* self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Funcall : self->funcall(self,ope); break;
        case Data_type : self->datatype(self,ope); break;
        case Id : chkSubOpeCheckBit(self,ope); break;
        default :
            chkSubOperator(ope);
            gsOpeLink(ope);
            break;
        }
        ope=ope->next;
    }
    return 0;
}
//
//initialization
//
gsHandle chkSub(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->ope=(*chkSubOpe);
    gs->module=(*gsModule2);
    gs->funcall=(*chkSubFuncall);
    return gs;
}


/*------------------------------------------------------------------
  
  removing declaration
  int x,y=1  --> x; y=1;

------------------------------------------------------------------*/
//
//inserting statement
//
static int remDecInstState(
    gsHandle self,cgrNode prev,cgrNode next){
    cgrNode proc;
    proc=gsPeek(self,Function);
    if(!proc) return -1;
    if(!prev) cgrAddNode(proc,cgrKeyVal,next);
    else cgrInstNode(prev,next);
    return 0;
}

//
//removing statement
//
static int remDecRemoveState(gsHandle self,cgrNode node){
    int i,key;
    cgrNode state;
    setJumpForStack(self,state,i){
        switch(state->type){
        case While  :
        case For    :
            key=cgrGetNum(state,cgrKeyProcess);
            cgrRemoveNode(state,key,node);
            return 0;
        case Do       :
        case Block    :
        case Function :
        case Case     :
        case Default  :
            cgrRemoveNode(state,cgrKeyVal,node);
            return 0;
        }
    } 
    return 0;
}

//
//for
//
static int remDecFor(gsHandle self,cgrNode node){
    if(!node) return 0;
    gsPush(self,node);
    cgrSetNum(node,cgrKeyProcess,cgrKeyIni);
    self->dfg(self,cgrGetNode(node,cgrKeyIni));
    cgrSetNum(node,cgrKeyProcess,cgrKeyCond);
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    cgrSetNum(node,cgrKeyProcess,cgrKeyStep);
    self->dfg(self,cgrGetNode(node,cgrKeyStep));
    cgrSetNum(node,cgrKeyProcess,cgrKeyVal);
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    cgrSetNum(node,cgrKeyProcess,0);
    gsPop(self);
    return 0;
}

//
//while
//
static int remDecWhile(gsHandle self,cgrNode node){
    if(!node) return 0;
    gsPush(self,node);
    cgrSetNum(node,cgrKeyProcess,cgrKeyCond);
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    cgrSetNum(node,cgrKeyProcess,cgrKeyVal);
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    cgrSetNum(node,cgrKeyProcess,0);
    gsPop(self);
    return 0;
}

//
//default statement action
//
static int remDecStateDef(gsHandle self,cgrNode node){
    if(!node) return 0;
    gsPush(self,node);
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    gsPop(self);
    return 0;
}


//
//variable declaration
//
static int remDecVarDec(gsHandle self,cgrNode node){
    cgrNode var,dec,prev,next;
    if(!node) return 0;
    prev=node;
    dec=cgrGetNode(node,cgrKeyVal);
    while(dec){
        next=dec->next;
        dec->prev=NULL;
        dec->next=NULL;
        var=cgrNew(Dfg,dec->file,dec->line);
        cgrSetNode(var,cgrKeyVal,dec);
        remDecInstState(self,prev,var);
        prev=var;
        dec=next;
    }
    remDecRemoveState(self,node);
    return 0;
}

//
//initialization
//
gsHandle remDec(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*gsModule2);
    gs->vardec=(*remDecVarDec);
    gs->cfgfor=(*remDecFor);
    gs->cfgwhile=(*remDecWhile);
    gs->cfgdo=(*remDecStateDef);
    gs->cfgblock=(*remDecStateDef);
    return gs;
}

/*------------------------------------------------------------------
  
  translation from control graph to FSMD

------------------------------------------------------------------*/
//
//setting back node
//
#define setJumpBack fsmdSetJump

//
//settind default jump node
//
static int setJumpDefNode(gsHandle self,cgrNode node){
    int i;
    cgrNode back,ifel,snode,state;

    setJumpForStack(self,state,i){
        switch(state->type){
        case Dfg:
            if(state->next){
                setJumpBack(node,state->next);
                return 0;
            }break;
        case If :
        case Else :
            ifel=state->next;
            while(ifel){
                if(ifel->type!=Else){
                    setJumpBack(node,ifel);
                    return 0;
                }
                ifel=ifel->next;
            }break;
        case For     :
        case While   :
            back=cgrGetNode(state,cgrKeyLoopFront);
            if(back){
                setJumpBack(node,back);
                return 0;
            }
        case Do      : 
            setJumpBack(node,state);
            return 0;
        case Case    :
            ifel=state->next;
            while(ifel){
                snode=cgrGetNode(ifel,cgrKeyVal);
                if(snode){
                    setJumpBack(node,snode);
                    return 0;
                }
                ifel=ifel->next;
            }break;
        case Switch  :
        case Block   :
        case Default :
            if(state->next){
                setJumpBack(node,state->next);
                return 0;
            } 
            break;
        case Function:
            snode=cgrGetNode(state,cgrKeyVal);
            setJumpBack(node,snode);
            return 0;
        }
    }
    return 0;
}

//
//setting jump node
//
int exIfAddJump(cgrNode node,cgrNode back){
    cgrNode next,prev;
    if(!node) return 0;
    while(back){
        next=back->next;
        back->prev=NULL;
        cgrAddNode(node,cgrKeyBack,back);
        prev=back->jump;
        prev->jump=node;
        back=next;
    }
    return 0;
}

//
//setting back node
//
static int setJumpStealBack(cgrNode ctr,cgrNode cfg,cgrNode node){
    cgrNode back;
    if(!ctr) return 0;
    if(!cfg) return 0;
    if(!node) return 0;
    back=cgrGetNode(cfg,cgrKeyBack);
    while(back){
        if(back->jump==node){
            ctr->jump=cfg;
            back->jump=ctr;
            return 0;
        }
        back=back->next;
    }
    return 0;
}



//
//creating contrall node
//
static cgrNode exIfCreateCtr(cgrNode node){
    cgrNode cfg,ctr;
    ctr=cgrNew(Ctrl,node->file,node->line);
    cgrSetNode(ctr,cgrKeyCond,cgrGetNode(node,cgrKeyCond));
    cfg=cgrGetNode(node,cgrKeyVal);
    if(cfg) setJumpStealBack(ctr,cfg,node); 
    ctr->jump=node->jump;
    return ctr;
}
#define setJumpNewCtr exIfCreateCtr

//
//checking sequence
//
static cgrNode exIfSetDef(gsHandle self,cgrNode node){
    cgrNode def,back;
    if(!node) return NULL;
    def=cgrNew(Def,node->file,node->line);
    back=cgrGetNode(node,cgrKeyBack);
    exIfAddJump(def,back);
    cgrSetNode(node,cgrKeyBack,NULL);
    return def;
}
#define setJumpNewDef exIfSetDef
#define setJumpNewDefSet(self,node,def){ \
    def=setJumpNewDef(self,node);        \
    setJumpDefNode(self,def);}
    
//
//if statement
//
#define exIfSetControl(NODE,CFG) {\
        next=setJumpNewCtr(NODE); \
        setJumpBack(next,CFG);    \
        self->cfg(self,CFG); } 
static int exIfIf(gsHandle self,cgrNode node){
    cgrNode cfg,def,nstate,item,ifs,back,prev,next;
    if(!node) return 0;
    setJumpNewDefSet(self,node,def);
    cfg=cgrGetNode(node,cgrKeyVal);
    if(cfg){ //if(...) ... ;
        exIfSetControl(node,cfg);
        cgrSetContext(def,next);
    } else next=def; //if(...) ;     
    prev=next;
    for(ifs=node->next;ifs;ifs=ifs->next){
        if(ifs->type!=Else) break;
        item=cgrGetNode(ifs,cgrKeyVal);
        if(item){
            if(item->type==If){
                cfg=cgrGetNode(item,cgrKeyVal);
                if(!cfg) continue; //else if(...) ;
                else exIfSetControl(item,cfg);//else if(...)...;
            }else exIfSetControl(ifs,item); //else ... ;
        }else continue; //else ;    
        cgrSetContext(prev,next);
        prev=next;
    }
    return 0;
}

//
//switch statement condition creation
//
static cgrNode exSwitchCreateCond(cgrNode left,cgrNode right){
    cgrNode eq,cleft,cright;
    if(!left) return NULL;
    if(!right) return NULL;
    eq=cgrNew(Eq,right->file,right->line);
    cleft=fsmdCopyOpe(left);
    cgrSetNode(eq,cgrKeyLeft,cleft);
    cgrSetNode(cleft,cgrKeyPar,eq);
    cright=fsmdCopyOpe(right);
    cgrSetNode(eq,cgrKeyRight,cright);
    cgrSetNode(cright,cgrKeyPar,eq);
    return eq;
}

//
//switch
//
static int exSwitch(gsHandle self,cgrNode node){
    cgrNode jump,back,ctr,cfg,def,cas,left,right,cnd,prev;
    if(!node) return 0;
    setJumpNewDefSet(self,node,def);//switch(...){...}
    left=cgrGetNode(node,cgrKeyCond);
    cas=cgrGetNode(node,cgrKeyVal);
    prev=def;
    while(cas){
        gsPush(self,cas);
        ctr=setJumpNewCtr(cas);
        cfg=cgrGetNode(cas,cgrKeyVal);
        if(cfg) setJumpBack(ctr,cfg);
        else setJumpDefNode(self,ctr);
        self->cfg(self,cfg);
        switch(cas->type){
        case Case ://case ... : ...
            right=cgrGetNode(cas,cgrKeyCond);
            cnd=exSwitchCreateCond(left,right);
            cgrSetNode(ctr,cgrKeyCond,cnd);
            break;
        case Default : break;//default: ...
        }
        cgrSetContext(prev,ctr);
        prev=ctr;
        gsPop(self);
        cas=cas->next;
    }
    return 0;
}

//
//for statement initializer extraction
//
static int exForIni(cgrNode ini,cgrNode def){
    cgrNode prev,back;
    if(!ini) return 0;
    back=cgrGetNode(def,cgrKeyBack);
    setJumpChangeBack(ini,back);
    prev=ini;
    while(prev->next){
        setJumpBack(prev,prev->next);
        prev=prev->next;
    }
    cgrSetNode(def,cgrKeyBack,NULL);
    setJumpBack(prev,def);
    return 0;
}

//
//for
//
#define setJumpNewDefLoop(self,node,def){       \
    def=setJumpNewDef(self,node);               \
    if(node->next) setJumpBack(def,node->next); \
    else { gsPop(self);                         \
           setJumpDefNode(self,def);            \
           gsPush(self,node);}}
static int exFor(gsHandle self,cgrNode node){
    cgrNode def,ini,cfg,step,ctr;
    if(!node) return 0;
    setJumpNewDefLoop(self,node,def);
    ini=cgrGetNode(node,cgrKeyIni);
    if(ini) exForIni(ini,def); //for(...;;)..
    else ini=def; //for(;;)
    cfg=cgrGetNode(node,cgrKeyVal);
    if(cgrGetNode(node,cgrKeyCond)){
        ctr=setJumpNewCtr(node);
        cgrSetContext(def,ctr);
    }else ctr=def;
    if(cfg) setJumpBack(ctr,cfg);
    self->cfg(self,cfg);
    step=cgrGetNode(node,cgrKeyStep);
    if((!cfg)&& (!step)){ //for(;;);
        setJumpBack(ctr,def);
    }else if(!step){//for(;;){...}
        cgrNode back=cgrGetNode(node,cgrKeyBack);
        setJumpChangeBack(def,back);
    }else if(!cfg){//for(;;...) ;
        setJumpBack(ctr,step);
        setJumpBack(cgrGetTail(step),def);
    }else{//for(;;...){...}
        cgrNode back=cgrGetNode(node,cgrKeyBack);
        setJumpChangeBack(step,back);
        self->cfg(self,step);
        setJumpBack(cgrGetTail(step),def);
    }
    return 0;
}

//
//while
//
static int exWhile(gsHandle self,cgrNode node){
    cgrNode def,ctr,cfg;
    if(!node) return 0;
    setJumpNewDefLoop(self,node,def);
    cfg=cgrGetNode(node,cgrKeyVal);
    if(cgrGetNode(node,cgrKeyCond)){ //while(...)...
        ctr=setJumpNewCtr(node);
        cgrSetContext(def,ctr);
    }else ctr=def; //while() ...
    if(cfg){
        cgrNode back;
        setJumpBack(ctr,cfg);
        self->cfg(self,cfg);
        back=cgrGetNode(node,cgrKeyBack);
        setJumpChangeBack(def,back);
    }else //while() ;
        setJumpBack(ctr,def);
    return 0;
}

//
//do-while
//
static int exDo(gsHandle self,cgrNode node){
    cgrNode scfg,back,ctr,def,cfg;
    if(!node) return 0;
    cfg=cgrGetNode(node,cgrKeyVal);
    if(!cfg) return self->cfgwhile(self,node);
    setJumpNewDefLoop(self,node,def);
    ctr=setJumpNewCtr(node);
    cgrSetContext(def,ctr);
    setJumpBack(ctr,cfg);
    self->cfg(self,cfg);
    back=cgrGetNode(def,cgrKeyBack);
    setJumpChangeBack(ctr->jump,back);
    cgrSetNode(def,cgrKeyBack,NULL);
    back=cgrGetNode(node,cgrKeyBack);
    setJumpChangeBack(def,back);
    return 0;
}

//
//Dfg
//
static int setJumpDfg(gsHandle self,cgrNode node){
    if(!node) return 0;
    setJumpDefNode(self,node);
    return 0;
}

//
//Block
//
static int setJumpBlock(gsHandle self,cgrNode node){
    cgrNode back,cfg,def;
    if(!node) return 0;
    def=setJumpNewDef(self,node);
    setJumpDefNode(self,def);
    cfg=cgrGetNode(node,cgrKeyVal);
    if(cfg){ // {...}
        cgrNode ctr=setJumpNewCtr(node);
        setJumpBack(ctr,cfg); 
        self->cfg(self,cfg);
        cgrSetContext(def,ctr);
    }else ;// { }
    return 0;
}


//
//Break
//
static int setJumpBreak(gsHandle self,cgrNode node){
    int flg,i;
    cgrNode def,snode,state;
    if(!node) return 0;
    node->next=NULL;
    setJumpForStack(self,state,i){
        switch(state->type){
        case For     :
        case Do      : 
        case While   :
        case Switch  :
            flg=1;
            if(state->next){
                def=setJumpNewDef(self,node);
                setJumpBack(def,state->next);
                return 0;
            } break;
        case Function:
            if(flg){
                def=setJumpNewDef(self,node);
                snode=cgrGetNode(state,cgrKeyVal);
                setJumpBack(def,snode);
            }else cmsgEcho(__sis_msg,28,node->file,node->line);
            return 0;
        }
    }
    return 0;
}

//
//Continue
//
static int setJumpContinue(gsHandle self,cgrNode node){
    int i;
    cgrNode back,def,state;
    if(!node) return 0;
    node->next=NULL;
    setJumpForStack(self,state,i){
        switch(state->type){
        case For     :
        case While   :
            def=setJumpNewDef(self,node);
            back=cgrGetNode(state,cgrKeyLoopFront);
            if(back) setJumpBack(def,back);
            else setJumpBack(def,state);
            return 0;
        case Do      : 
            def=setJumpNewDef(self,node);
            setJumpBack(def,state);
            return 0;
        }
    }
    cmsgEcho(__sis_msg,29,node->file,node->line);
    return 0;
}

//
//Return
//
static int setJumpReturn(gsHandle self,cgrNode node){
    int i;
    cgrNode def,state,snode;
    if(!node) return 0;
    node->next=NULL;
    if(cgrGetNode(node,cgrKeyVal)){
        cmsgEcho(__sis_msg,30,node->file,node->line);
        cgrSetNode(node,cgrKeyVal,NULL);
    }
    def=setJumpNewDef(self,node);
    setJumpForStack(self,state,i){
        if(state->type==Function){
            state=gsPeek(self,Function);
            snode=cgrGetNode(state,cgrKeyVal);
            setJumpBack(def,snode);
            break;
        }else if(state->type==Block){
            if(!cgrGetNum(state,cgrKeyFuncInter)) continue;
            int tmp=cvectSize(self->stack);
            cvectSize(self->stack)=i+1;
            setJumpDefNode(self,def);
            cvectSize(self->stack)=tmp;
            break;
        }
    }
    return 0;
}


//
//module
//
static int setJumpModule(gsHandle self,cgrNode node){
    cgrNode dummy,proc;
    if(!node) return 0;
    chkVarSpPush(self);
    gsPush(self,node);
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        cgrSetNum(proc,cgrKeyProcType,1);//setting process type
        gsPush(self,proc);
        dummy=cgrGetNode(proc,cgrKeyVal);
        if(!dummy){
          dummy=cgrNew(Dfg,proc->file,proc->line);
          cgrAddNode(proc,cgrKeyVal,dummy);
        }else if(dummy->type !=Dfg){
          dummy=cgrNew(Dfg,proc->file,proc->line);
          cgrAddNode(proc,cgrKeyVal,dummy);
        }
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        proc=proc->next;
    }
    chkVarSpPop(self);
    return 0;
}

//
//initialization
//
gsHandle setJump(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*setJumpModule);
    gs->cfg=(*gsCfg2);
    gs->dfg=(*setJumpDfg);
    gs->cfgdo=(*exDo);
    gs->cfgfor=(*exFor);
    gs->cfgwhile=(*exWhile);
    gs->cfgswitch=(*exSwitch);
    gs->cfgblock=(*setJumpBlock);
    gs->cfgif=(*exIfIf);
    gs->cfgelse=(*gsDummy);
    gs->cfgbreak=(*setJumpBreak);
    gs->cfgreturn=(*setJumpReturn);
    gs->cfgcontinue=(*setJumpContinue);
    return gs;
}


/*------------------------------------------------------------------
  
  translation NFA to DFA

------------------------------------------------------------------*/
//
//translation pattern A
//(dfg)-(def)_  ==> (dfg)-(dfg)_  
//        |___|             |___|
//
static int n2dDef(gsHandle self,cgrNode node);
static int n2dDef_A(gsHandle self,cgrNode node){
    cgrNode jump;
    if(!node) return 0;
    if(node->next) return 0;
    jump=node->jump;
    if(jump!=node) return 0;
    node->type=Dfg;
    return 1;
}

//
//translation pattern B
//(dfg)-(def)-(dfg)  ==> (dfg)-(dfg)  
//
static int n2dDef_B(gsHandle self,cgrNode node){
    cgrNode jump,back,prev;
    if(!node) return 0;
    if(node->next) return 0;
    if(node->prev) return 0;
    if(cgrGetNode(node,cgrKeyCond))return 0;
    fsmdRemoveNode(node);
    return 1;
}

//
//translation pattern C
//(dfg)-(def)-(dfg1)  ==>   (dfg)-(if)-(dfg2) 
//        |                        | 
//       (if)-(dfg2)            (else)-(dfg1)
//
static int n2dDef_C(gsHandle self,cgrNode node){
    cgrNode jump,tail,back,next;
    if(!node) return 0;
    if(node->type!=Def) return 0;
    next=node->next;
    while(next){
        if(!cgrGetNode(next,cgrKeyCond)) return 0;
        next=next->next;
    }
    next=node->next;
    node->type=Ctrl;
    next->prev=NULL;
    back=cgrGetNode(node,cgrKeyBack);
    cgrSetNode(next,cgrKeyBack,back);
    cgrSetNode(node,cgrKeyBack,NULL);
    while(back){
        jump=back->jump;
        if(jump) jump->jump=next;
        back=back->next;
    }
    node->next=NULL;
    tail=cgrGetTail(next);
    cgrSetContext(tail,node);
    n2dDef(self,next);
    return 1;
}

//
//translation pattern D
//(dfg)-(def)-(dfg1)  ==>   (dfg)-(if)-(dfg2) 
//        |                        | 
//       (if)-(dfg2)            (else)-(dfg3)
//        |   
//     (else)-(dfg3) 
//
static int n2dDef_D(gsHandle self,cgrNode node){
    cgrNode jump,back,next;
    if(!node) return 0;
    if(node->type!=Def) return 0;
    next=node->next;
    while(next){
        if(!cgrGetNode(next,cgrKeyCond)) break;
        next=next->next;
    }
    if(!next) return 0;
    next=node->next;
    back=cgrGetNode(node,cgrKeyBack);
    cgrSetNode(next,cgrKeyBack,back);
    next->prev=NULL;
    while(back){
        jump=back->jump;
        if(jump) jump->jump=next;
        back=back->next;
    }
    fsmdRemoveJump(node->jump,node);
    n2dDef(self,next);
    return 1;
}

//
//translation NFA control node to DFA
//
static int n2dDef(gsHandle self,cgrNode node){
    if(!node) return 0;
    if(n2dDef_A(self,node)) return 0;
    if(n2dDef_B(self,node)) return 0;
    if(n2dDef_C(self,node)) return 0;
    if(n2dDef_D(self,node)) return 0;
    return 0;
}
//
//dfg
//
static int n2dDfg(gsHandle self,cgrNode node){
    cgrNode jump;
    if(!node) return 0;
    node->next=NULL;
    node->prev=NULL;
    jump=node->jump;
    if(!cgrGetNode(node,cgrKeyVal) &&
       !cgrGetNum(node,cgrKeyWait)){
        cgrNode fun=gsPeek(self,Function);
        if(cgrGetNode(fun,cgrKeyVal)!=node)
            fsmdRemoveNode(node);
    }
    self->cfg(self,jump);
    return 0;
}

//
//state
//
static int n2dState(gsHandle self,cgrNode node){
    if(!node) return 0;
    if(fsmdRouteCheck(node)) return 0;
    fsmdRouteSet(node);
    switch(node->type){
    case Dfg:
        self->dfg(self,node);
        break;
    case Def:
        node->prev=NULL;
        self->cfg(self,node->jump);
        self->cfg(self,node->next);
        n2dDef(self,node);
        break;
    case Ctrl:
        self->cfg(self,node->jump);
        self->cfg(self,node->next);
        break;
    }
    return 0;
}

//
//module
//
static int n2dModule(gsHandle self,cgrNode node){
    cgrNode proc,state;
    if(!node) return 0;
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        gsPush(self,proc);
        fsmdRouteReset();
        state=cgrGetNode(proc,cgrKeyVal);
        self->cfg(self,state);
        proc=proc->next;
    }
    fsmdRouteClear();
    return 0;
}
//
//initialization
//
gsHandle n2d(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->dfg=(*n2dDfg);
    gs->cfg=(*n2dState);
    gs->module=(*n2dModule);
    return gs;
}

/*------------------------------------------------------------------
  
  translation to SSA form

------------------------------------------------------------------*/
//
//function argument to variable declaration
//
#define toSsaSetPar(node,par,key) { \
    cgrSetNode(node,cgrKeyPar,par); \
    cgrSetNode(par,key,node);       }
#define toSsaGetCfg(self) \
    (cvectElt(self->stack,(cvectSize(self->stack)-1),cgrNode))
#define toSsaGetCfg2(self) \
    (cvectElt(self->stack,(cvectSize(self->stack)-2),cgrNode))
#define toSsaSetOpe(self,node,key) {                   \
    cgrNode nope=toSsaOpe2(self,cgrGetNode(node,key)); \
    toSsaSetPar(nope,node,key);                        }
#define exFuncSetNull(node) node->next=NULL;node->prev=NULL;
static cgrNode exFuncArg2Dec(cgrNode node){
    cgrNode val,next,ptr;
    if(!node) return NULL;
    val=cgrGetNode(node,cgrKeyVal);
    ptr=cgrGetNode(node,cgrKeyPointer);
    while(ptr){
        int type=ptr->type;
        cgrNode tptr=cgrNew(type,node->file,node->line);
        next=ptr->next;
        toSsaSetPar(val,tptr,cgrKeyRight);
        val=tptr;
        ptr=next;
    }
    cgrSetNode(node,cgrKeyVal,val);
    return node;
}

//
//adding dfg node
//
static cgrNode exFuncAddDfg(gsHandle self,cgrNode node){
    cgrNode dfg,state,prev,next;
    state=toSsaGetCfg(self);
    dfg=cgrNew(Dfg,state->file,state->line);
    cgrSetNode(dfg,cgrKeyVal,node);
    //do
    if(state->type==Do){
        cgrNode cfg;
        cfg=cgrGetNode(state,cgrKeyVal);
        if(!cfg) cgrSetNode(state,cgrKeyVal,dfg);
        else{
            state=cgrGetTail(cfg);
            cgrSetContext(state,dfg);
        }
        return dfg;
    //while || for
    }else if((state->type==While) ||
             (state->type==For)){
        if(!cgrGetNode(state,cgrKeyLoopFront)){
            cgrSetNode(dfg,cgrKeyLoopBack,state);
            cgrSetNode(state,cgrKeyLoopFront,dfg);
        }
    }else{
        cgrNode back=cgrGetNode(state,cgrKeyLoopBack);
        if(back){
            cgrSetNode(dfg,cgrKeyLoopBack,back);
            cgrSetNode(back,cgrKeyLoopFront,dfg);
            cgrSetNode(state,cgrKeyLoopBack,NULL);
        }
    }
    //other
    prev=state->prev;
    if(prev){
        cgrSetContext(prev,dfg);
        cgrSetContext(dfg,state);
    }else{
        state=toSsaGetCfg2(self);
        cgrAddNode(state,cgrKeyVal,dfg);
    }
    return dfg;
}

//
//getting id node from declaration node
//
static cgrNode exFuncGetId(cgrNode node){
    cgrNode val;
    if(!node) return NULL;
    val=cgrGetNode(node,cgrKeyVal);
    while(val){
        switch(val->type){
        case Id  : return val;
        case '*' :
        case '&' :val=cgrGetNode(val,cgrKeyRight);break;
        case Array:val=cgrGetNode(val,cgrKeyLeft);break;
        default : return NULL;
        }
    }
    return val;
}

//
//function arguments expansion
//
static int exFuncArgs(gsHandle self,
    cgrNode block,cgrNode dec,cgrNode arg,char*fname){
    cgrNode dnext,anext,dfg;
    while(dec && arg){
        dnext=dec->next;
        anext=arg->next;
        exFuncSetNull(dec);
        exFuncSetNull(arg);
        exFuncArg2Dec(dec);
        dfg=exFuncAddDfg(self,arg);
        cgrAddNode(block,cgrKeyVal,dec);
        cgrSetNode(exFuncGetId(dec),cgrKeyFuncInter,dfg);
        dec=dnext;
        arg=anext;
    }
    if(dec||arg){
        cgrNode type=cgrGetNode(dec,cgrKeyType);
        if(type) if(type->type!=Void)
            cmsgEcho(__sis_msg,39,block->file,block->next,fname);
    }
    return 0;
}

//
//function expansion
//
static int exFunc(gsHandle self,
    cgrNode dfg,cgrNode task,cgrNode fnc,cgrNode ret){
    //local declaration
    char*fname;
    cgrNode cfg,dec,arg;
    //inline block creation
    dfg->type=Block;
    cgrSetNum(dfg,cgrKeyFuncInter,1);
    cfg=cgrCopyCfg(cgrGetNode(task,cgrKeyVal));
    cgrSetNode(dfg,cgrKeyVal,cfg);
    //arguments expansion
    dec=fsmdCopyOpe(cgrGetNode(task,cgrKeyArgs));
    arg=cgrGetNode(fnc,cgrKeyArgs);
    fname=cgrGetStr(task,cgrKeyId);
    exFuncArgs(self,dfg,dec,arg,fname);
    //return statement expansion
    exFuncAddDfg(self,ret);
    return 0;
}

//
//checking function call
//
static int exFunctionCheck(gsHandle self,cgrNode node){
    cgrNode dfg,ret,fnc,task;
    if(!node) return 0;
    fnc=node;
    if(fnc->type=='='){
        ret=cgrGetNode(fnc,cgrKeyLeft);
        fnc=cgrGetNode(fnc,cgrKeyRight);
    }else ret=NULL;
    if(fnc->type!=Funcall) return 0;
    task=gsCheckTask(self,fnc);
    if(!task) return 0;
    dfg=toSsaGetCfg(self);
    if(dfg->type!=Dfg) return 0;
    exFunc(self,dfg,task,fnc,ret);
    self->cfg(self,cgrGetNode(dfg,cgrKeyVal));
    return 0;
}
//
//dfg
//
static int toSsaDfg(gsHandle self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    exFunctionCheck(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}

//
//assignment operator expansion
//
static int toSsaOpe(gsHandle self,cgrNode node);
static int toSsaExEq(gsHandle self,cgrNode node, int type){
    cgrNode var,ope;
    cgrDecLR(node,0);
    var=fsmdCopyOpe(left);
    ope=cgrNew(type,node->file,node->line);
    node->type='=';
    toSsaSetPar(ope,node,cgrKeyRight);
    toSsaSetPar(right,ope,cgrKeyRight);
    toSsaSetPar(var,ope,cgrKeyLeft);
    toSsaOpe(self,node);
    return 0;
}

//
//new DFG node creation
//
static cgrNode toSsaOpeCreateDfg(gsHandle self,cgrNode node){
    cgrNode mo,prev,dfg,dec,invar,outvar,eq;
    if(!node) return NULL;
    //temporary variable creation
    dec=fsmdNameCreate("__tmp_",
        node->file,node->line,1,Sc_uint,Sc_signal,"");
    mo=gsPeek(self,Sc_module);
    cgrAddNode(mo,cgrKeySignal,dec);
    outvar=cgrNew(Id,node->file,node->line);
    cgrSetNum(outvar,cgrKeyVarType,1);
    cgrSetStr(outvar,cgrKeyVal,cgrGetStr(dec,cgrKeyId));
    cgrSetNode(outvar,cgrKeySignal,dec);
    invar=fsmdCopyOpe(outvar);
    //SSA creation
    eq=cgrNew('=',node->file,node->line);
    toSsaSetPar(outvar,eq,cgrKeyLeft);
    toSsaSetPar(node,eq,cgrKeyRight);
    //setting dfg
    dfg=exFuncAddDfg(self,eq);
    prev=gsPop(self);
    gsPush(self,dfg);
    self->dfg(self,dfg);
    gsPop(self);
    gsPush(self,prev);
    return invar;
}

//
//operation depth2
//
static cgrNode toSsaOpe2(gsHandle self,cgrNode node){
    cgrNode invar=node;
    chk2OptHandle* opt=self->work;

    if(!node) return NULL;
    switch(node->type){
    case Id :fsmdNameSet(cgrGetStr(node,cgrKeyVal));
    case Num     :
    case Real    :
    case String  : break;
    case '.' :
    case Arr :
    case Array :
        toSsaSetOpe(self,node,cgrKeyLeft);
        toSsaSetOpe(self,node,cgrKeyRight);
        break;
    case Funcall: 
        if(!gsCheckTask(self,node)){
            self->funcall(self,node);
        }else invar=toSsaOpeCreateDfg(self,node);
        break; 
    case '*' :
    case '&' :
        if(!cgrGetNode(node,cgrKeyLeft)){
            toSsaSetOpe(self,node,cgrKeyRight);
            break;
        }
    default:
        if(opt->chain){ //chaining //----
            toSsaSetOpe(self,node,cgrKeyCond);
            toSsaSetOpe(self,node,cgrKeyLeft);
            toSsaSetOpe(self,node,cgrKeyRight);
        }else //not chaining ---- 
            invar=toSsaOpeCreateDfg(self,node);
        break;
    }
    return invar;
}
//
//operation depth1
//
static cgrNode toSsaOpe1(gsHandle self,cgrNode node){
    chk2OptHandle* opt=self->work;

    if(!(opt->chain)){ //not chaining --- 
        cgrNode ope=node;
        cgrNode invar=node;
        cgrNode next,prev;
        if(!node) return NULL;
        prev=NULL;
        while(ope){
            next=ope->next;
            invar=toSsaOpe2(self,ope);
            cgrSetContext(prev,invar);
            prev=invar;
            ope=next;
        }
        return cgrGetHead(invar);
    
    
    }else{ //chainning  ------------
        cgrNode ope=node;
        while(ope){
            cgrNode next=ope->next;
            toSsaOpe(self,ope);
            ope=next;
        }
    }
    return node;
}

//
//function call
//
static int toSsaFuncall(gsHandle self,cgrNode node){
    cgrNode left,invar,cfg;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    self->ope(self,left);
    invar=toSsaOpe1(self,cgrGetNode(node,cgrKeyArgs));
    cgrSetNode(node,cgrKeyArgs,invar);
    return 0;
}
//
//assignment into condition
//
static int toSsaOpeAss(gsHandle self,cgrNode node){
    cgrNode var;
    cgrNode cfg=toSsaGetCfg(self);
    if(!node) return 0;
    if(cfg->type==Dfg) return 0;
    if(cgrGetNode(cfg,cgrKeyCond)!=node) return 0;
    var=fsmdCopyOpe(cgrGetNode(node,cgrKeyLeft));
    cgrSetNode(cfg,cgrKeyCond,var);
    exFuncAddDfg(self,node);
    return 0;
}

//
//operation
//
static int toSsaOpe(gsHandle self,cgrNode node){
    int type;
    cgrNode ope,nope,cfg;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :fsmdNameSet(cgrGetStr(node,cgrKeyVal));
        case Num     :
        case Real    :
        case String  : break;
        case Data_type:self->datatype(self,ope);break;
        case Funcall: 
            cfg=toSsaGetCfg(self);
            if((cfg->type!=Dfg) &&
                (gsCheckTask(self,node))){
                cgrNode par=cgrGetNode(ope,cgrKeyPar);
                nope=toSsaOpe2(self,ope);
                if(!par){
                    if(cfg->type==Return)
                        cgrSetNode(cfg,cgrKeyVal,nope);
                    else cgrSetNode(cfg,cgrKeyCond,nope);
               }else toSsaSetPar(nope,par,cgrKeyRight);
            }else self->funcall(self,ope);
            break;
        case '=' :
            gsOpeLinkEq(ope);
            toSsaOpeAss(self,ope);
            break;
        case Mueq:toSsaExEq(self,ope,'*');break;
        case Dieq:toSsaExEq(self,ope,'/');break;
        case Moeq:toSsaExEq(self,ope,'%');break;
        case Adeq:toSsaExEq(self,ope,'+');break;
        case Sueq:toSsaExEq(self,ope,'-');break;
        case Aneq:toSsaExEq(self,ope,'&');break;
        case Xoeq:toSsaExEq(self,ope,'^');break;
        case Oreq:toSsaExEq(self,ope,'|');break;
        case Lseq:toSsaExEq(self,ope,Lshif);break;
        case Rseq:toSsaExEq(self,ope,Rshif);break;
        case Connect:
            nope=toSsaOpe1(self,cgrGetNode(ope,cgrKeyCond));
            cgrSetNode(ope,cgrKeyCond,nope);
            break;
        default:
            toSsaSetOpe(self,ope,cgrKeyCond);
            toSsaSetOpe(self,ope,cgrKeyLeft);
            toSsaSetOpe(self,ope,cgrKeyRight);
            break;
        }
    ope=ope->next;
    }
    return 0;
}
//
// for
//
static int toSsaFor(gsHandle self,cgrNode node){
    cgrNode prev,ini;
    if(!node) return 0;
    prev=node->prev;
    ini=cgrGetNode(node,cgrKeyIni);
    if(ini){
        if(prev) cgrSetContext(prev,cgrGetHead(ini));
        else{
            cgrNode par=toSsaGetCfg2(self);
            cgrSetNode(par,cgrKeyVal,ini);
        }
        prev=gsPop(self);
        self->cfg(self,ini);
        gsPush(self,prev);
        cgrSetContext(cgrGetTail(ini),node);
        cgrSetNode(node,cgrKeyIni,NULL);
    }
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfg(self,cgrGetNode(node,cgrKeyStep));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}

static int toSsaSig(gsHandle self,cgrNode node){
    if(!node) return 0;
    fsmdNameSet(cgrGetStr(node,cgrKeyId));
    return 0;
}

//
//driver
//
static int toSsaRoot(struct _gsHandle* self,cgrNode node){
    cgrNode dec;
    dec=node;
    fsmdNameReset();
    while(dec){
        if(dec->type==Sc_module){
            cgrNode sig=cgrGetNode(dec,cgrKeySignal);
            cgrMaps(sig,self->sig_dec(self,sig));
            self->module(self,dec);
        }
        dec=dec->next;
    }
    fsmdNameClear();
    return 0;
}

//
//initialization
//
gsHandle toSsa(chk2OptHandle opt,cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->work=&opt;
    gs->ope=(*toSsaOpe);
    gs->dfg=(*toSsaDfg);
    gs->root=(*toSsaRoot);
    gs->cfg=(*gsCfg2);
    gs->cfgfor=(*toSsaFor);
    gs->sig_dec=(*toSsaSig);
    gs->module=(*gsModule2);
    gs->funcall=(*toSsaFuncall);
    return gs;
}

/*------------------------------------------------------------------
    
  deciding bitwidth for temporaly variable

------------------------------------------------------------------*/
//
//temporaly variable
//
static int tmpBitVar(gsHandle self,cgrNode node){
    int num,need;
    cgrNode dec,par,right;
    if(!node) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    if(!par) return 0;
    if(par->type!='=') return 0;
    if(cgrGetNode(par,cgrKeyLeft)!=node) return 0;
    right=cgrGetNode(par,cgrKeyRight);
    dec=cgrGetNode(node,cgrKeySignal);
    num=cgrGetNum(dec,cgrKeySize);
    need=cgrBitWidth(right);
    if(need>num){
        cgrSetNum(dec,cgrKeySize,need);
        if(need>64) cgrSetNum(dec,cgrKeyType,Sc_biguint);
    }
    return 0;
}

//
//id
//
static int tmpBitId(gsHandle self,cgrNode node){
    if(!node) return 0;
    if(cgrGetNum(node,cgrKeyVarType)==1)tmpBitVar(self,node);
    return 0;
}

//
//initialization
//
gsHandle tmpBit(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->id=(*tmpBitId);
    gs->module=(*gsModule2);
    return gs;
}

/*------------------------------------------------------------------
  
  jointing function call interface

------------------------------------------------------------------*/
//
//ID
//
#define fnInteMakeEq(eq,left,right)   \
    eq=cgrCreateOpe('=',left,right,NULL);   
static int fnInteId(gsHandle self,cgrNode node){
    cgrNode vardfg,var,dfg,eq,invar;
    if(!node) return 0;
    vardfg=cgrGetNode(node,cgrKeyFuncInter);
    if(!vardfg) return 0;
    var=cgrGetNode(vardfg,cgrKeyVal);
    if(!var) return 0;
    dfg=toSsaGetCfg(self);
    if(!dfg) return 0;
    fnInteMakeEq(eq,node,var);
    cgrSetNode(dfg,cgrKeyVal,eq);
    cgrSetNode(vardfg,cgrKeyVal,NULL);
    return 0;
}

//
//return
//
static int fnInteReturn(gsHandle self,cgrNode node){
    int i;
    cgrNode block,eq,val,ret,retvar,outvar;
    if(!node) return 0;
    val=cgrGetNode(node,cgrKeyVal);
    if(!val) return 0;
    setJumpForStack(self,block,i){
        if(block->type==Function) return 0;
        if(block->type!=Block) continue;
        if(!cgrGetNum(block,cgrKeyFuncInter)) continue;
        break;
    }
    if(!block) return 0;
    ret=block->prev;
    if(!ret) return 0;
    retvar=cgrGetNode(ret,cgrKeyVal);
    if(retvar){
        outvar=fsmdCopyOpe(retvar);
        fnInteMakeEq(eq,outvar,val);
    }else eq=val;
    exFuncAddDfg(self,eq);
    cgrSetNode(node,cgrKeyVal,NULL);
    return 0;
}

//
//initialization
//
gsHandle fnInte(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->id=(*fnInteId);
    gs->cfg=(*gsCfg2);
    gs->module=(*gsModule2);
    gs->cfgreturn=(*fnInteReturn);
    return gs;
}

/*------------------------------------------------------------------
  
  removing dead coad

------------------------------------------------------------------*/
//
//constant values conpression
//
#define remDeadConstExp(node,lval,rval,ope) { \
    int _num=lval ope rval;                   \
    node->type=Num;                           \
    cgrSetNode(node,cgrKeyLeft,NULL);         \
    cgrSetNode(node,cgrKeyRight,NULL);        \
    cgrSetNum(node,cgrKeyVal,_num);           \
    return 0;                                 }
#define remDeadConstVal(node,val) {   \
    if(!node) return 0;              \
    if(node->type!=Num) return 0;    \
    val=cgrGetNum(node,cgrKeyVal);   }
#define remDeadConstMin(node,val,ope) {      \
    if(cgrGetNode(node,cgrKeyLeft)) break;   \
    remDeadConstExp(node,ope,val,);          }
static int remDeadConst(gsHandle self,cgrNode node){
    int lval,rval;
    cgrDecLR(node,0);
    remDeadConstVal(right,rval);
    switch(node->type){
    case '~': remDeadConstExp(node,,rval,~);
    case '!': remDeadConstExp(node,,rval,!);
    case '+': remDeadConstMin(node,rval,+);
    case '-': remDeadConstMin(node,rval,-);
    }
    remDeadConstVal(left,lval);
    switch(node->type){
    case '|': remDeadConstExp(node,lval,rval,|);
    case '^': remDeadConstExp(node,lval,rval,^);
    case '&': remDeadConstExp(node,lval,rval,&);
    case Eq:  remDeadConstExp(node,lval,rval,==);
    case Neq: remDeadConstExp(node,lval,rval,!=);
    case Gre: remDeadConstExp(node,lval,rval,>=);
    case Les: remDeadConstExp(node,lval,rval,<=);
    case '>': remDeadConstExp(node,lval,rval,>);
    case '<': remDeadConstExp(node,lval,rval,<);
    case Lshif: remDeadConstExp(node,lval,rval,<<);
    case Rshif: remDeadConstExp(node,lval,rval,>>);
    case '+': remDeadConstExp(node,lval,rval,+);
    case '-': remDeadConstExp(node,lval,rval,-);
    case '*': remDeadConstExp(node,lval,rval,*);
    case '/': remDeadConstExp(node,lval,rval,/);
    case '%': remDeadConstExp(node,lval,rval,%);
    }
    return 0;
}
//
//setting parent node
//
int remDeadSetPar(
    gsHandle self,cgrNode node,cgrNode ope){
    cgrNode par;
    if(!node) return 0;
    par=cgrGetNode(node,cgrKeyPar);
    if(par){
        if(cgrGetNode(par,cgrKeyLeft)==node){
            toSsaSetPar(ope,par,cgrKeyLeft);
        }else if(cgrGetNode(par,cgrKeyRight)==node){
            toSsaSetPar(ope,par,cgrKeyRight);
        }else if(cgrGetNode(par,cgrKeyCond)==node){
            toSsaSetPar(ope,par,cgrKeyCond);
        }else if(cgrGetNode(par,cgrKeyArgs)==node){
            toSsaSetPar(ope,par,cgrKeyArgs);
        }    
    }else{
        cgrNode cfg=toSsaGetCfg(self);
        if(node->prev)
            cgrSetContext(node->prev,ope);
        else if(cgrGetNode(cfg,cgrKeyCond)==node)
            cgrSetNode(cfg,cgrKeyCond,ope);
        else if(cgrGetNode(cfg,cgrKeyVal)==node)
            cgrSetNode(cfg,cgrKeyVal,ope);
        cgrSetNode(ope,cgrKeyPar,NULL);
    }
    cgrSetContext(ope,node->next);
    return 0;
}
//
//multiplier to shifter
//
#define remDeadMulCheck(node,flg,num){  \
    num=cgrGetNum(node,cgrKeyVal);      \
    if(num<0){                          \
        num=-num;                       \
        flg=1;                          \
    }else flg=0;                        }
#define remDeadMulChange(node,left,right,num,flg,operation) \
    if(num==1){                                             \
        if(flg){                                            \
            node->type='-';                                 \
            toSsaSetPar(left,node,cgrKeyRight);             \
            cgrSetNode(node,cgrKeyLeft,NULL);               \
        }else remDeadSetPar(self,node,left);                \
    }else{                                                  \
        int i;                                              \
        for(i=0;num>(1<<i);i++);                            \
        if(num!=(1<<i)) return 0;                           \
        if(flg){                                            \
            cgrNode min=cgrNew('-',node->file,node->line);  \
            remDeadSetPar(self,node,min);                   \
            toSsaSetPar(node,min,cgrKeyRight); }            \
        node->type=operation;                               \
        cgrSetNum(right,cgrKeyVal,i);                       }
static int remDeadMulShif(gsHandle self,cgrNode node){
    int num,flg;
    cgrDecLR(node,0);
    if(node->type!='*') return 0;
    if((!left)||(!right)) return 0;
    if(right->type!=Num){
        cgrNode tmp=right;
        right=left;
        left=tmp;
    }
    if(right->type!=Num) return 0;
    toSsaSetPar(left,node,cgrKeyLeft);
    toSsaSetPar(right,node,cgrKeyRight);
    remDeadMulCheck(right,flg,num);
    if(num==0){ 
        remDeadConstExp(node,num,,);
    }else remDeadMulChange(node,left,right,num,flg,Lshif);
    return 0;
}
//
//divider to shifter
//
static int remDeadDivShif(gsHandle self,cgrNode node){
    int num,flg=0;
    cgrDecLR(node,0);
    if(node->type!='/') return 0;
    if((!left)||(!right)) return 0;
    if(right->type!=Num) return 0;
    remDeadMulCheck(right,flg,num);
    if(num==0){ 
        cmsgEcho(__sis_msg,41,node->file,node->line);
        remDeadConstExp(node,0,,);
    }else remDeadMulChange(node,left,right,num,flg,Rshif);
    return 0;
}

//
//operation
//
static int remDeadConstOpe(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case Funcall : self->funcall(self,ope);break;
        case Data_type : self->datatype(self,ope);break;
        case '=':gsOpeLinkEq(ope);break;
        case '*':
            gsOpeLink(ope);
            remDeadMulShif(self,ope);
            break;
        case '/':
            gsOpeLink(ope);
            remDeadDivShif(self,ope);
            break;
        default : gsOpeLink(ope); break;
        }
        remDeadConst(self,ope);
        ope=ope->next;
    }
    return 0;
}

//
//operator
//
static int remDeadOpe(gsHandle self,cgrNode node,int flg){
    int i,ret=1;
    gsHandle map=self;
    if(!node) return 1;
    if(node->type==Funcall){
        cgrNode task=gsCheckTask(self,node);
        if(task) self->cfg(self,cgrGetNode(node,cgrKeyVal));
        return 0;
    }
    schSwitchFlg(node,flg,remDeadOpe,ret);
    if(flg) return 0;
    return ret;
}

//
//dfg
//
static int remDeadDfg(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=cgrGetNode(node,cgrKeyVal);
    if(remDeadOpe(self,ope,0)){
        cgrSetNode(node,cgrKeyVal,NULL);
        return 0;
    }
    self->ope(self,ope);
    return 0;
}
//
//signal
//
static int remDeadSig(gsHandle self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyArray));
    return 0;
}

//
//module
//
static int remDeadModule(gsHandle self,cgrNode node){
    cgrNode proc,sig,task;
    if(!node) return 0;
    chkVarSpPush(self);
    gsPush(self,node);
    proc=cgrGetNode(node,cgrKeyProcess);
    //process
    cgrMaps(proc,gsPush(self,proc);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal)));
    //task
    task=cgrGetNode(node,cgrKeyTask);
    cgrMaps(task,
        //is function out of module
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        gsPush(self,task);
        self->cfg(self,cgrGetNode(task,cgrKeyVal)));
    //signal
    sig=cgrGetNode(node,cgrKeySignal);
    cgrMaps(sig,self->sig_dec(self,sig));
    chkVarSpPop(self);
    return 0;
}

//
//initialization
//
gsHandle remDead(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->dfg=(*remDeadDfg);
    gs->cfg=(*gsCfg2);
    gs->sig_dec=(*remDeadSig);
    gs->module=(*remDeadModule);
    gs->ope=(*remDeadConstOpe);
    return gs;
}

/*------------------------------------------------------------------
  
  getting reset process

------------------------------------------------------------------*/
//
//checking reset signal
//
static int rstProcId(gsHandle self,cgrNode node){
    char*name;
    char*rname;
    cgrNode mo,rst;
    if(!node) return 0;
    mo=gsPeek(self,Sc_module);
    rst=cgrGetNode(mo,cgrKeyReset);
    if(!rst) return 0;
    rname=cgrGetStr(rst,cgrKeyId);
    if(!strcmp(rname,"")) return 0;
    name=cgrGetStr(node,cgrKeyVal);
    if(!strcmp(name,rname)) gsPush(self,node);
    return 0;
}

//
//checking reset process
//
static cgrNode rstProcCheck(
    gsHandle self,cgrNode node){
    char*rname;
    char*rvname;
    cgrNode pobj,obj,start,state,rst,next,cond;
    cgrNode mo=gsPeek(self,Sc_module);
    if(!node) return NULL;
    rst=cgrGetNode(mo,cgrKeyReset);
    if(!rst){
        char*mname=cgrGetStr(mo,cgrKeyId);
        cmsgEcho(__sis_msg,31,mo->file,mo->line,mname);
        return NULL;
    }
    state=cgrGetNode(node,cgrKeyVal);
    if(!state) return NULL;
    start=state;
    if((state->type==Dfg)&&
        (!cgrGetNode(state,cgrKeyVal)))
        state=state->jump;
    if(state->type!=Ctrl) return NULL;
    pobj=start;
    obj=state->jump;
    while((obj!=start) && (obj!=state)){
        if(obj->type!=Dfg) return NULL;
        pobj=obj;
        obj=obj->jump;
    }
    next=state->next;
    if(!next) return NULL;
    if(next->next) return NULL;
    if(cgrGetNode(next,cgrKeyCond)) return NULL;
    
    rname=cgrGetStr(rst,cgrKeyId);
    cond=cgrGetNode(state,cgrKeyCond);
    self->ope(self,cond);
    rst=gsPeek(self,Id);
    if(!rst) return NULL;
    rvname=cgrGetStr(rst,cgrKeyVal);
    if(strcmp(rname,rvname)) return NULL;
    pobj->jump=state;
    return state;
}

//
//reset process creation
//
static int rstProcProc(gsHandle self,cgrNode node){
    cgrNode state,next;
    cgrNode mo=gsPeek(self,Sc_module);
    if(!node) return 0;
    
    state=rstProcCheck(self,node);
    if(!state) return 0;
    next=state->next;
    setJumpChangeBack(next,cgrGetNode(state,cgrKeyBack));
    cgrSetNode(state,cgrKeyBack,NULL);
    next->prev=NULL;
    state->next=NULL;
    state->type=Dfg;
    if(!cgrGetNode(next,cgrKeyVal)){
        cgrNode jump=next->jump;
        fsmdRemoveNode(next);
        next=state;
        jump=next;
    }
    if(cgrGetNode(node,cgrKeyVal)==state) 
        cgrSetNode(node,cgrKeyVal,next);
    cgrSetNode(node,cgrKeyResetProc,state);
    return 0;
}

//
//module
//
static int rstProcModule(gsHandle self,cgrNode node){
    cgrNode proc;
    if(!node) return 0;
    chkVarSpPush(self);
    gsPush(self,node);
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        chkVarSpPush(self);
        gsPush(self,proc);
        rstProcProc(self,proc);
        chkVarSpPop(self);
        proc=proc->next;
    }
    chkVarSpPop(self);
    return 0;
}
//
//destruction
//
static void rstProcDes(gsHandle gs){
    //pass
}

//
//initialization
//
gsHandle rstProc(chk2OptHandle opt,cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->id=(*rstProcId);
    gs->module=(*rstProcModule);
    return gs;
}


/*------------------------------------------------------------------
  
  checking recursive call

------------------------------------------------------------------*/
//
//function call
//
static int chkRecFuncall(gsHandle self,cgrNode node){
    int i;
    cgrNode task,tobj;
    if(!node) return 0;
    task=gsCheckTask(self,node);
    if(!task) return 0;
    setJumpForStack(self,tobj,i){
        if(task==tobj){
            char*fname=cgrGetStr(task,cgrKeyId);
            cgrNode mo=gsPeek(self,Sc_module);
            cmsgEcho(__sis_msg,38,node->file,node->line,fname);
            cgrRemoveNode(mo,cgrKeyTask,task);
            return 0;
        }
    }
    gsPush(self,task);
    self->cfg(self,cgrGetNode(task,cgrKeyVal));
    gsPop(self);
    return 0;
}

//
//initialization
//
gsHandle chkRec(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*gsModule2);
    gs->funcall=(*chkRecFuncall);
    return gs;
}


/*------------------------------------------------------------------
  
  addr and multiplier commutation

------------------------------------------------------------------*/
//
//to swap right node
//
static int commuMoveSwapRight(cgrNode node1,cgrNode node2){
    cgrNode r1,r2;
    if(!node1) return 0;
    if(!node2) return 0;
    r1=cgrGetNode(node1,cgrKeyRight);
    if(!r1) return 0;
    r2=cgrGetNode(node2,cgrKeyRight);
    if(!r2) return 0;
    toSsaSetPar(r1,node2,cgrKeyRight);
    toSsaSetPar(r2,node1,cgrKeyRight);
    return 0;
}
//
//moving right const node
//
#define commuMoveLoop(node,key,ret,FUNCTION){       \
    cgrNode left=node;                              \
    while(left){                                    \
        cgrNode lleft=cgrGetNode(left,cgrKeyLeft);  \
        cgrNode right=cgrGetNode(left,cgrKeyRight); \
        if(!lleft) ret;                             \
        if(!right) ret;                             \
        if(left->type!=node->type) ret;             \
        FUNCTION;                                   \
        left=key; }                                 }
static cgrNode commuMoveConstRight(cgrNode node){
    if(!node) return NULL;
    commuMoveLoop(node,cgrGetNode(left,cgrKeyPar),
        return NULL,if((right->type)!=Num) return left);
    return NULL;   
}
//
//moving const
//
#define commuMoveBlock                          \
    if(right->type==Num){                       \
        cgrNode next=commuMoveConstRight(gen);  \
        if(!next) return 0;                     \
        if(next!=cgrGetNode(left,cgrKeyPar))    \
            commuMoveSwapRight(left,next);      }
static int commuMoveConst(gsHandle self,cgrNode node){
    cgrNode ope;
    cgrNode gen=NULL;
    if(!node) return 0;
    commuMoveLoop(node,lleft,break,gen=left);
    ope=cgrGetNode(gen,cgrKeyRight);
    if(!ope) return 0;
    if(ope->type==Num){
        cgrNode right=cgrGetNode(gen,cgrKeyLeft);
        toSsaSetPar(ope,gen,cgrKeyLeft);
        toSsaSetPar(right,gen,cgrKeyRight);
    }
    commuMoveLoop(node,lleft,return 0,commuMoveBlock);
    return 0;
}
//
//pattern A
//y=a+b+c+d --> y=(a+b)+(c+d)
//
#define commuDecLLR(left,node,ret)        \
    cgrNode lleft,lright; {               \
    if(!left) return ret;                 \
    if(left->type!=node->type) return ret;\
    lleft=cgrGetNode(left,cgrKeyLeft);    \
    if(!lleft)  return ret;               \
    lright=cgrGetNode(left,cgrKeyRight);  \
    if(!lright) return ret;               }
static int commuOpeCheckA(gsHandle self,cgrNode node){
    cgrDecLR(node,0);
    commuDecLLR(left,node,0);
    if(!right) return 0;
    if(lleft->type!=node->type)  return 0;
    toSsaSetPar(lleft,node,cgrKeyLeft);
    toSsaSetPar(lright,left,cgrKeyLeft);
    toSsaSetPar(right,left,cgrKeyRight);
    toSsaSetPar(left,node,cgrKeyRight);
    return 0;
}
//
//pattern B
//y=(a*b)+(c*d)+e --> y=(a*b)+e+(c*d)
//
static int commuOpeCheckB(gsHandle self,cgrNode node){
    cgrDecLR(node,0);
    commuDecLLR(left,node,0);
    if(!right) return 0;
    if((right->type!=Id) &&
       (right->type!=Num)&&
       (right->type!=Arr)&&
       (right->type!='.')&&
       (right->type!=Array)) return 0;
    toSsaSetPar(right,left,cgrKeyRight);
    toSsaSetPar(lright,node,cgrKeyRight);
    return 0;
}

//
//operation
//
static int commuOpe(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case Funcall : self->funcall(self,ope);break;
        case Data_type : self->datatype(self,ope);break;
        case '=':gsOpeLinkEq(ope);break;
        case '*':
        case '+':
        case '|':
        case '^':
        case '&':
        case Oror:
        case Anan:
            commuMoveConst(self,node);
            commuOpeCheckB(self,node);
            commuOpeCheckA(self,node);
        default : gsOpeLink(ope); break;
        }
        ope=ope->next;
    }
    return 0;
}
//
//function call
//
static int commuFuncall(gsHandle self,cgrNode node){
    cgrNode left,task;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    self->ope(self,left);
    self->args(self,cgrGetNode(node,cgrKeyArgs));
    task=gsCheckTask(self,node);
    if(task) self->cfg(self,cgrGetNode(task,cgrKeyVal));
    return 0;
}
//
//initialization
//
gsHandle commu(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->ope=(*commuOpe);
    gs->module=(*gsModule2);
    gs->funcall=(*commuFuncall);
    return gs;
}


/*------------------------------------------------------------------
  
  checking wait

------------------------------------------------------------------*/
//
//function call
//
static chkWaitFuncall(gsHandle self,cgrNode node){
    int i,num;
    char*name;
    cgrNode val,ndfg;
    cgrNode dfg=toSsaGetCfg(self);
    if(!node) return 0;
    if(!dfg) return 0;
    if(dfg->type!=Dfg) return 0;
    name=cgrGetStr(node,cgrKeyId);
    if(strcmp(name,"wait")) return 0;
    if(cgrGetNode(node,cgrKeyPar)) return 0;
    val=cgrGetNode(node,cgrKeyArgs);
    if(val){
        if(val->next){
            cmsgEcho(__sis_msg,42,val->file,val->line);
            return 0;
        }
        if(val->type!=Num){
            cmsgEcho(__sis_msg,43,val->file,val->line);
            return 0;
        }
        num=cgrGetNum(val,cgrKeyVal);
    }else num=1;
    cgrSetNum(dfg,cgrKeyWait,1);
    cgrSetNode(dfg,cgrKeyVal,NULL);
    for(i=0,ndfg=dfg;i<(num-1);i++){
        cgrNode next=ndfg->next;
        cgrNode sdfg=cgrNew(Dfg,dfg->file,dfg->line);
        cgrSetNum(sdfg,cgrKeyWait,1);
        cgrSetContext(ndfg,sdfg);
        cgrSetContext(sdfg,next);
        ndfg=sdfg;
    }
    return 0;
}

//
//initialization
//
gsHandle chkWait(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->cfg=(*gsCfg2);
    gs->funcall=(*chkWaitFuncall);
    gs->module=(*gsModule2);
    return gs;
}
/*------------------------------------------------------------------
  
  pointer translation

------------------------------------------------------------------*/
//
//pointer test
//
static int traPtrIdTest(cgrNode node){
    int array,addr,ptr;
    cgrNode dec;
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    if(!dec) return 0;
    ptr=cgrGetNum(dec,cgrKeyPointer);
    addr=cgrGetNum(dec,cgrKeyAddress);
    array=cgrGetNum(dec,cgrKeyArraySize);
    if(!ptr && !addr && !array) return 0;
    else if((ptr==1) && !addr && !array) return ptr;
    else if(!ptr && (addr==1) && !array) return -addr;
    else if(!ptr && !addr && (array==1)) return array;
    else if(!(ptr+array-addr)) return 0;
    else{
        char*name=cgrGetStr(dec,cgrKeyId);
        cmsgEcho(__sis_msg,45,node->file,node->line,name);
    }
    return 0;
}
//
//change base
//
static cgrNode traPtrChangeBase(gsHandle self,cgrNode node){
    int depth;
    cgrNode sig,val,state;
    if(!node) return 0;
    sig=cgrGetNode(node,cgrKeySignal);
    val=cgrGetNode(sig,cgrKeyMemMap);
    if(!val) return 0;
    depth=traPtrIdTest(node);
    if(!depth) return 0;
    val=fsmdCopyOpe(val);
    state=node;
    if(depth>0){
        cgrNode par=cgrGetNode(state,cgrKeyPar);
        if(!par) return 0;
        else if((par->type=='*') &&
            !cgrGetNode(par,cgrKeyLeft)) state=par;
        else if(par->type!=Array) return 0;
    }
    remDeadSetPar(self,state,val);
    return val;
}

//
//translation id
//
static int traPtrIdChange(gsHandle self,cgrNode node){
    traPtrChangeBase(self,node);
    return 0;
}
//
//id
//
static int traPtrId(gsHandle self,cgrNode node){
    int depth;
    cgrNode par,sig,right,dfg;
    if(!node) return 0;
    traPtrIdChange(self,node);
    par=cgrGetNode(node,cgrKeyPar);
    if(!par) return 0;
    if(par->type!='=') return 0;
    if(cgrGetNode(par,cgrKeyLeft)!=node) return 0;
    depth=traPtrIdTest(node);
    if(!depth) return 0;
    right=cgrGetNode(par,cgrKeyRight);
    if(!right) return 0;
    if(right->type=='&'){
        if(cgrGetNode(right,cgrKeyLeft)) return 0;
        right=cgrGetNode(right,cgrKeyRight);
    }
    dfg=toSsaGetCfg(self);
    if(!dfg) return 0;
    if(dfg->type!=Dfg) return 0;
    sig=cgrGetNode(node,cgrKeySignal);
    if(cgrGetNode(sig,cgrKeyMemMap)){
        char*name=cgrGetStr(sig,cgrKeyId);
        cmsgEcho(__sis_msg,44,node->file,node->line,name);
    }
    cgrSetNode(sig,cgrKeyMemMap,right);
    cgrSetNode(dfg,cgrKeyVal,NULL);
    return 0;
}
//
//translation funcall
//
static int traPtrFuncallChange(gsHandle self,cgrNode node){
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    cgrNode val=traPtrChangeBase(self,node);
    if(!val) return 0;
    if(val->type==Id){
        val->type=Funcall;
        cgrSetStr(val,cgrKeyId,cgrGetStr(val,cgrKeyVal));
        cgrSetStr(val,cgrKeyVal,"");
    }else{
        cgrNode par=cgrGetNode(val,cgrKeyPar);
        cgrNode fcall=cgrNew(Funcall,node->file,node->line);
        //set left
        cgrSetNode(fcall,cgrKeyLeft,val);
        cgrSetNode(val,cgrKeyPar,fcall);
        //set par
        if(cgrGetNode(par,cgrKeyLeft)==val) 
            cgrSetNode(par,cgrKeyLeft,fcall);
        if(cgrGetNode(par,cgrKeyRight)==val)
            cgrSetNode(par,cgrKeyRight,fcall);
        if(cgrGetNode(par,cgrKeyCond) ==val) 
            cgrSetNode(par,cgrKeyCond,fcall);
        cgrSetNode(fcall,cgrKeyPar,par);
        
        val=fcall;
    }
    cgrSetNum(val,cgrKeyIsPartSelec,1);
    cgrSetNode(val,cgrKeyArgs,args);
    cgrSetNode(args,cgrKeyPar,val);
    return 0;
}

//
//translation funcall
//
static int traPtrFuncall(gsHandle self,cgrNode node){
    traPtrFuncallChange(self,node);
    return 0;
}
//
//initialization
//
gsHandle traPtr(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->id=(*traPtrId);
    gs->funcall=(*traPtrFuncall);
    gs->cfg=(*gsCfg2);
    gs->module=(*gsModule2);
    return gs;
}

/*------------------------------------------------------------------
  
  multi dimension array expansion

------------------------------------------------------------------*/
//
//signal
//
static int exArrSig(gsHandle self,cgrNode node){
    cgrNode n,array;
    int arrsize,num=1;
    if(!node) return 0;
    arrsize=cgrGetNum(node,cgrKeyArraySize);
    if(arrsize<2) return 0;
    array=cgrGetNode(node,cgrKeyArray);
    while(array){
        cgrDecLR(array,0);
        if(array->type!=Array) break;
        if(right->type!=Num){
            char*name=cgrGetStr(node,cgrKeyId);
            cmsgEcho(__sis_msg,40,node->file,node->line,name);
        }else num*=cgrGetNum(right,cgrKeyVal);
        array=left;
    }
    if(!array) return 0;
    array=cgrGetNode(array,cgrKeyPar);
    n=cgrNew(Num,node->file,node->line);
    cgrSetNum(n,cgrKeyVal,num);
    toSsaSetPar(n,array,cgrKeyRight);
    cgrSetNode(array,cgrKeyPar,NULL);
    cgrSetNode(node,cgrKeyArray,array);
    cgrSetNum(node,cgrKeyArraySize,1);
    return 0;
}

//
//getting base array size
//
static cgrNode exArrGetBase(cgrNode node){
    int num=1;
    cgrNode par,n;
    if(!node) return NULL;
    par=cgrGetNode(node,cgrKeyPar);
    while(par){
        cgrNode right=cgrGetNode(par,cgrKeyRight);
        if(right->type!=Num) return NULL;
        num*=cgrGetNum(right,cgrKeyVal);
        par=cgrGetNode(par,cgrKeyPar);
    }
    n=cgrNew(Num,node->file,node->line);
    cgrSetNum(n,cgrKeyVal,num);
    return n;
}

//
//id
//
static int exArrId(gsHandle self,cgrNode node){
    cgrNode sig,sp,si,sd,bnum,prev;
    if(!node) return 0;
    sig=cgrGetNode(node,cgrKeySignal);
    if(!sig) return 0;
    if(cgrGetNum(sig,cgrKeyArraySize)<2) return 0;
    sd=cgrGetNode(sig,cgrKeyArray);
    while(sd){
        cgrNode left=cgrGetNode(sd,cgrKeyLeft);
        if(!left) break;
        if(left->type!=Array) break;
        sd=left;
    }
    if(!sd) return 0;
    si=cgrGetNode(node,cgrKeyPar);
    if(!si) return 0;
    if(si->type!=Array) return 0;
    sp=si;
    prev=NULL;
    while(bnum=exArrGetBase(sd)){
        cgrNode right=cgrGetNode(si,cgrKeyRight);
        cgrNode mul=cgrCreateOpe('*',bnum,right,NULL);
        if(prev) prev=cgrCreateOpe('+',prev,mul,NULL);
        else prev=mul;
        sp=si;
        sd=cgrGetNode(sd,cgrKeyPar);
        si=cgrGetNode(si,cgrKeyPar);
    }
    toSsaSetPar(node,sp,cgrKeyLeft);
    toSsaSetPar(prev,sp,cgrKeyRight);
    return 0;
}
//
//module
//
static int exArrModule(gsHandle self,cgrNode node){
    cgrNode proc,sig;
    if(!node) return 0;
    chkVarSpPush(self);
    gsPush(self,node);
    proc=cgrGetNode(node,cgrKeyProcess);
    cgrMaps(proc,gsPush(self,proc);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal)));
    sig=cgrGetNode(node,cgrKeySignal);
    cgrMaps(sig,self->sig_dec(self,sig));
    chkVarSpPop(self);
    return 0;
}
//
//initialization
//
gsHandle exArr(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->id=(*exArrId);
    gs->cfg=(*gsCfg2);
    gs->sig_dec=(*exArrSig);
    gs->module=(*exArrModule);
    return gs;
}
/*------------------------------------------------------------------
  
  checking enum declaration

------------------------------------------------------------------*/
//
//macros
//
#define chkEnumTagTable(gs) ((cMstk*)gs->work)[0]
#define chkEnumNumTable(gs) ((cMstk*)gs->work)[1]

//
// id
//
static int chkEnumId(gsHandle self,cgrNode node){
    char*id;
    cgrNode* decid;
    id=cgrGetStr(node,cgrKeyVal);
    decid=cmstkFind(chkEnumNumTable(self),id);
    if(decid){
        int num;
        num=cgrGetNum(*decid,cgrKeyIni);
        node->type=Num;
        cgrSetNum(node,cgrKeyVal,num);
    }
    return 0;
}

//
// get pointer node
//
static cgrNode tagChangePtr(cgrNode node){
    cgrNode ptr=cgrGetNode(node,cgrKeyPointer);
    if(!ptr) return NULL;
    return fsmdCopyOpe(ptr);
}
//
// change tag to variable declaration 2 (base)
//
static cgrNode tagChange2(gsHandle self,cgrNode node,int size){
    cgrNode dt;  //data type node
    cgrNode scdt;//systemc data type
    dt=cgrNew(Data_type,node->file,node->line);
    cgrSetNode(dt,cgrKeyPointer,tagChangePtr(node));
    if(size>64) scdt=cgrNew(Sc_biguint,node->file,node->line);
    else scdt=cgrNew(Sc_uint,node->file,node->line);
    cgrSetNode(dt,cgrKeyType,scdt);
    cgrSetNum(dt,cgrKeySize,size);
    return dt;
}
//
// change tag to variable declaration 3 (args)
//
static cgrNode tagChange3(gsHandle self,cgrNode node,int size){
    cgrNode dt; //declaration
    cgrNode var;//variable
    var=cgrGetNode(node,cgrKeyVal);
    dt=tagChange2(self,node,size);
    cgrSetNode(dt,cgrKeyVal,var);
    return dt;
}
//
// change tag to variable declaration
//
static cgrNode tagChange(gsHandle self,cgrNode node,int size){
    cgrNode var; //enum variable
    cgrNode par; //parent node;
    cgrNode dt;  //data type node
    if(!node) return NULL;
    var=cgrGetNode(node,cgrKeyVal);
    par=gsTop(self);
    if(par==node){
        cgrNode tmp;//temporary;
        tmp=gsPop(self);
        par=gsTop(self);
        gsPush(self,tmp);
    }
    dt=NULL;
    if(var){
        dt=tagChange2(self,node,size);
        cgrReplaceNode(node,dt,par,cgrKeyVal);
        cgrSetNode(dt,cgrKeyVal,var);
    }else cgrRemoveNode(par,cgrKeyVal,node);
    return dt;
}
//
// enum
//
static int chkEnumEnum(gsHandle self,cgrNode node){
    char*tag;     //tag name
    int num;      //enum number
    int max;      //max enum number
    cgrNode body; //enum body
    cgrNode vars; //enum variables
    //setting tag table
    tag=cgrGetStr(node,cgrKeyTag);
    if(tag) cmstkSet(chkEnumTagTable(self),tag,node);
    
    //set number table
    num=0;
    max=0;
    body=cgrGetNode(node,cgrKeyEnumBody);
    while(body){
        char*id;
        if(cgrGetNum(body,cgrKeyIsEnumSet)) num=cgrGetNum(body,cgrKeyIni);
        else cgrSetNum(body,cgrKeyIni,num);    
        id=cgrGetStr(body,cgrKeyId);
        cmstkSet(chkEnumNumTable(self),id,body);
        body=body->next;
        if(num>max) max=num;
        num++;
    }
    cgrSetNum(node,cgrKeyEnumMax,max);
    cgrSetNum(node,cgrKeySize,cgrBitWidthNum(max));

    //make integer variable
    if(cmstkSize(chkEnumTagTable(self))>3) 
        tagChange(self,node,cgrGetNum(node,cgrKeySize));
    return 0;
}

//
//tag
//
static int chkEnumTag(gsHandle self,cgrNode node){
    char*id;
    cgrNode* decenum;

    id=cgrGetStr(node,cgrKeyId);
    decenum=cmstkFind(chkEnumTagTable(self),id);
    if(decenum) tagChange(self,node,cgrGetNum(*decenum,cgrKeySize));
    else cmsgEcho(__sis_msg,66,node->file,node->line,id);
    return 0;
}

//
//module
//
static int chkEnumModule(gsHandle self,cgrNode node){
    cgrNode dec; //declaration
    cgrNode proc;//process
    cgrNode task;//task
    if(!node) return 0;

    //in module declaration
    dec=cgrGetNode(node,cgrKeyVal);
    while(dec){
        switch(dec->type){
        case Enum :
            self->dec_enum(self,dec);
            break;
        }
        dec=dec->next;
    }
    //into process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        cmstkPush(chkEnumTagTable(self));
        cmstkPush(chkEnumNumTable(self));
        gsPush(self,proc);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        cmstkPop(chkEnumTagTable(self));
        cmstkPop(chkEnumNumTable(self));
        gsPop(self);
        proc=proc->next;
    }
    //into task
    task=cgrGetNode(node,cgrKeyTask);
    cgrMaps(task,
        //is function out of module
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        cmstkPush(chkEnumTagTable(self));
        cmstkPush(chkEnumNumTable(self));
        gsPush(self,task);
        self->cfg(self,cgrGetNode(task,cgrKeyVal));
        cmstkPop(chkEnumTagTable(self));
        cmstkPop(chkEnumNumTable(self));
        gsPop(self));

    return 0;
}

//
//  Root  
//
static int chkEnumRoot(gsHandle self,cgrNode node){
    cgrNode dec;
    cMstk table[2]; //0:tab table,1:enum table
    char*fname="";

    dec=node;
    self->work=(void*)table;
    chkEnumTagTable(self)=cmstkNew(sizeof(cgrNode));
    chkEnumNumTable(self)=cmstkNew(sizeof(cgrNode));
    cmstkPush(chkEnumTagTable(self));
    cmstkPush(chkEnumNumTable(self));
    
    while(dec){
        if(strcmp(fname,dec->file)){
            cmstkPop(chkEnumTagTable(self));
            cmstkPop(chkEnumNumTable(self));
            cmstkPush(chkEnumTagTable(self));
            cmstkPush(chkEnumNumTable(self));
            fname=dec->file;
        }

        switch(dec->type){
        case Sc_module :
           cmstkPush(chkEnumTagTable(self));
           cmstkPush(chkEnumNumTable(self));
           self->module(self,dec);
           cmstkPop(chkEnumTagTable(self));
           cmstkPop(chkEnumNumTable(self));
           break;
        case Enum:
            self->dec_enum(self,dec);
           break;
        }
        dec=dec->next;
    }
    cmstkPop(chkEnumTagTable(self));
    cmstkPop(chkEnumNumTable(self));
    cmstkDes(chkEnumTagTable(self));
    cmstkDes(chkEnumNumTable(self));
    return 0;
}

//
//initialization
//
gsHandle chkEnum(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*chkEnumRoot);
    gs->dec_enum=(*chkEnumEnum);
    gs->module=(*chkEnumModule);
    gs->cfg=(*gsCfg2);
    gs->tag=(*chkEnumTag);
    gs->id=(*chkEnumId);
    return gs;
}

/*------------------------------------------------------------------
  
  checking class declaration

------------------------------------------------------------------*/
//
//macros
//
#define chkClassTagTable(gs) ((cMstk*)gs->work)[0]
#define chkClassVarTable(gs) ((cMstk*)gs->work)[1]
//
// get const value
//
static int getConstValue(gsHandle self,cgrNode node){
    remDeadConstOpe(self,node);
    if(node->type!=Num){
        cmsgEcho(__sis_msg,48,node->file,node->line);
        return 0;
    }
    return cgrGetNum(node,cgrKeyVal);
}

//
// get all array size 
//
static int getAllArraySize(gsHandle self,cgrNode node){
    int c;
    cgrNode par;
    c=1;
    par=cgrGetMaxLeft(node);
    par=cgrGetNode(par,cgrKeyPar);
    while(par){
        int size;
        if(par->type!=Array) break;
        size=getConstValue(self,cgrGetNode(par,cgrKeyRight));
        c*=size;
        par=cgrGetNode(par,cgrKeyPar);
    }
    return c;
}


//
//get start node
// x->y.z   get  x
//
static cgrNode chkClassUseGetStart(gsHandle self,cgrNode node){
    cgrNode n;
    n=node;
    while(n){
        cgrNode l;
        l=cgrGetNode(n,cgrKeyLeft);
        if(!l) return NULL;
        if((l->type!='.')&&(l->type!=Arr)) return l;
        n=l;
    }
    return NULL;
}

//
// x->y.z   is  '->'
//
static int chkClassUseIsMiddle(gsHandle self,cgrNode node){
    cgrNode p;
    p=cgrGetNode(node,cgrKeyPar);
    if(!p) return 0;
    if((p->type!='.')&&(p->type!=Arr)) return 0;
    return 1;
}
//
// find declaration A
//
static cgrNode chkClassUseFindDec_A(
    gsHandle self,cgrNode dec,cgrNode node){
    char* key;
    cgrNode body;
    if(!node) return NULL;
    if(node->type==Id) key=cgrGetStr(node,cgrKeyVal);
    if(node->type==Funcall) key=cgrGetStr(node,cgrKeyId);
    body=cgrGetNode(dec,cgrKeyClassBody);
    while(body){
        cgrNode var;
        var=cgrGetNode(body,cgrKeyVal);
        while(var){
            char*name;
            cgrNode left;
            left=cgrGetMaxLeft(var);
            name=cgrGetStr(left,cgrKeyVal);
            if(!strcmp(name,key)) return body;
            var=var->next;
        }
        body=body->next;
    }
    return NULL;
}
//
// find declaration B
//
static cgrNode chkClassUseFindDec_B(
    gsHandle self,cgrNode dec,cgrNode node){
    cgrNode left;
    cgrNode dec2;
    left=cgrGetMaxLeft(node);
    return chkClassUseFindDec_A(self,dec,left);
}

//
// find declaration C
//
static cgrNode chkClassUseFindDec_C(
    gsHandle self,cgrNode dec,cgrNode node){
    return  chkClassUseFindDec_A(self,dec,node);
}    

//
// find declaration E
//
static cgrNode chkClassUseFindDec_E(
    gsHandle self,cgrNode dec,cgrNode node){
    return  chkClassUseFindDec_B(self,dec,node);
}    

//
// get position and offset A
//
static cgrNode chkClassUseGetPos_A(
    gsHandle self,cgrNode dec,cgrNode*offset){
    cgrNode pos;
    int posi,offi;
    if(!dec) return NULL;
    posi=cgrGetNum(dec,cgrKeyClassElmIndex);
    if(posi<0) return NULL;
    offi=cgrGetNum(dec,cgrKeyClassElmOffset);
    if(offi<=0) return NULL;
    *offset=cgrNew(Num,dec->file,dec->line);
    cgrSetNum(*offset,cgrKeyVal,offi-1);
    pos=cgrNew(Num,dec->file,dec->line);
    cgrSetNum(pos,cgrKeyVal,posi);
    return pos;
}

//
// get position and offset B
//
static cgrNode chkClassUseGetPos_B(
    gsHandle self,cgrNode node,cgrNode dec,cgrNode*offset){
    int posi; //position number
    cgrNode pos;//position node
    cgrNode rdec,rvar; //right node declaration
    cgrNode rnode; //node var (x[][][] ... x)
    char* name; //var name

    //check declaration
    if(!dec) return NULL;

    //get node id
    rnode=cgrGetMaxLeft(node);
    if(!rnode) return NULL;
    if(rnode->type!=Id) return NULL;

    name=cgrGetStr(rnode,cgrKeyVal);
    //get right node declaration
    rdec=cgrGetNode(dec,cgrKeyVal);
    while(rdec){
        char*rname;
        rvar=cgrGetMaxLeft(rdec);
        if(!rvar) return NULL;
        if(rvar->type!=Id) return NULL;
        rname=cgrGetStr(rvar,cgrKeyVal);
        if(!strcmp(rname,name)) break;
        rdec=rdec->next;
    }
    //get offset
    if(rdec){
        cgrNode p1,p2; //par
        cgrNode snode; //tempolary array size node;
        cgrNode tmp,tmp2,tmp3; //tempolary
        int sub;  //tempolary array size;
        int base; //base offset size

        //init
        sub=1;
        p1=node;
        p2=rdec;
        tmp3=NULL;
        
        //loop start
        while(p1&&p2){
            int asize;     //array size
            cgrNode r1,r2; //right;

            //get right node
            if(p1->type!=Array) break;
            if(p2->type!=Array) break;
            r1=cgrGetNode(p1,cgrKeyRight);
            r2=cgrGetNode(p2,cgrKeyRight);

            //set offset size
            snode=cgrNew(Num,node->file,node->line);
            cgrSetNum(snode,cgrKeyVal,sub);
            tmp=cgrCreateOpe('*',snode,r1,NULL);
            if(tmp3){
                tmp2=cgrCreateOpe('+',tmp3,tmp,NULL);
                tmp3=tmp2;
            }else tmp3=tmp;
        
            //get array size
            if(r2->type!=Num) return NULL;
            asize=cgrGetNum(r2,cgrKeyVal);
            sub*=asize;

            p1=cgrGetNode(p1,cgrKeyLeft);
            p2=cgrGetNode(p2,cgrKeyLeft);
        }
        //set offset
        if(dec->type==Data_type) base=cgrBitWidth(dec);
        else base=cgrGetNum(dec,cgrKeyClassElmOffsetDef);
        *offset=cgrNew(Num,dec->file,dec->line);
        cgrSetNum(*offset,cgrKeyVal,base-1);

        //get position
        tmp2=cgrNew(Num,dec->file,dec->line);
        cgrSetNum(tmp2,cgrKeyVal,base);
        tmp=cgrCreateOpe('*',tmp2,tmp3,NULL); //base*index
        posi=cgrGetNum(dec,cgrKeyClassElmIndex);
        tmp2=cgrNew(Num,dec->file,dec->line);
        cgrSetNum(tmp2,cgrKeyVal,posi);
        pos=cgrCreateOpe('+',tmp,tmp2,NULL); //pos+(base*index)
        return pos;
    }
}

//
// get position and offset C
//
static cgrNode chkClassUseGetPos_C(
    gsHandle self,cgrNode fnode,cgrNode dec,cgrNode*offset){
    cgrNode pos; //position node
    cgrNode tmp; //tempolary
    cgrNode mnode,nnode; //max node,min node

    //get base position and offset
    pos=chkClassUseGetPos_A(self,dec,offset);
    
    //get range
    mnode=cgrGetNode(fnode,cgrKeyArgs);
    if(!mnode) return NULL;
    nnode=mnode->next;
    if(!nnode) return NULL;
    
    //set operation
    *offset=cgrCreateOpe('-',mnode,nnode,NULL);
    tmp=cgrCreateOpe('+',nnode,pos,NULL);
    pos=tmp;
    return pos;
}

//
// get position and offset E
//
static cgrNode chkClassUseGetPos_E(
    gsHandle self,cgrNode fnode,cgrNode dec,cgrNode*offset){
    cgrNode pos;  //position node
    cgrNode tmp;  //tempolary
    cgrNode left; //left node
    cgrNode mnode,nnode; //max node,min node

    //get base position and offset
    left=cgrGetNode(fnode,cgrKeyLeft);
    pos=chkClassUseGetPos_B(self,left,dec,offset);
    
    //get range
    mnode=cgrGetNode(fnode,cgrKeyArgs);
    if(!mnode) return NULL;
    nnode=mnode->next;
    if(!nnode) return NULL;
    
    //set operation
    *offset=cgrCreateOpe('-',mnode,nnode,NULL);
    tmp=cgrCreateOpe('+',nnode,pos,NULL);
    pos=tmp;
    return pos;
}



//
// get declare node from start node
//
static cgrNode chkClassUseGetDec(
    gsHandle self,cgrNode node,int pattern){
    
    cMstk table;   //table
    cgrNode* dect=NULL; //declare node

    table=chkClassVarTable(self);

    //get declar node
    switch(pattern){
    case ptnutilType_A :
        dect=(cgrNode*)cmstkFind(table,cgrGetStr(node,cgrKeyVal));
        break;
    case ptnutilType_B :{
        cgrNode left=cgrGetMaxLeft(node);
        dect=(cgrNode*)cmstkFind(table,cgrGetStr(left,cgrKeyVal));
        }break;
    case ptnutilType_C : {
        char*name;
        name=cgrGetStr(node,cgrKeyId);
        cmsgEcho(__sis_msg,49,node->file,node->line,name);
        }break;
    case ptnutilType_D :{
        char*name;
        cgrNode left;
        left=cgrGetMaxLeft(node);
        name=cgrGetStr(left,cgrKeyId);
        cmsgEcho(__sis_msg,49,node->file,node->line,name);
        }break;
    case ptnutilType_E :{
        char*name;
        cgrNode left;
        left=cgrGetMaxLeft(node);
        name=cgrGetStr(left,cgrKeyVal);
        cmsgEcho(__sis_msg,49,node->file,node->line,name);
        }break;
    default : return NULL;
    }
    if(!dect) return NULL;
    return *dect;

}
//
// middle node loop
//
static cgrNode chkClassUseMiddle(
    gsHandle self, cgrNode*dec,cgrNode*p,cgrNode*r){
    cgrNode ps;
    cgrNode pos;
    cgrNode offset;

    //middle node
    pos=cgrNew(Num,(*dec)->file,(*dec)->line);
    cgrSetNum(pos,cgrKeyVal,0);
    while(chkClassUseIsMiddle(self,*p)){
        switch(ptnutilMatch(*r)){
        case ptnutilType_A :
            *dec=chkClassUseFindDec_A(self,*dec,*r);
            ps=chkClassUseGetPos_A(self,*dec,&offset);
            break;
        case ptnutilType_B :
            *dec=chkClassUseFindDec_B(self,*dec,*r);
            ps=chkClassUseGetPos_B(self,*r,*dec,&offset);
            break;
        default : return NULL;
        }
        if(!ps) return NULL;
        pos=cgrCreateOpe('+',pos,ps,NULL);
        *p=cgrGetNode(*p,cgrKeyPar);
        *r=cgrGetNode(*p,cgrKeyRight);
    }
    return pos;
}

//
// terminal node
//
static cgrNode chkClassUseTerminal(gsHandle self,
    cgrNode dec,cgrNode pos,
    cgrNode r,cgrNode*offset){
    cgrNode ps;

    switch(ptnutilMatch(r)){
    case ptnutilType_A :
        dec=chkClassUseFindDec_A(self,dec,r);
        ps=chkClassUseGetPos_A(self,dec,offset);
        break;
    case ptnutilType_B :
        dec=chkClassUseFindDec_B(self,dec,r);
        ps=chkClassUseGetPos_B(self,r,dec,offset);
        break;
    case ptnutilType_C :
        dec=chkClassUseFindDec_C(self,dec,r);
        ps=chkClassUseGetPos_C(self,r,dec,offset);
        break;
    case ptnutilType_E :
        dec=chkClassUseFindDec_E(self,dec,r);
        ps=chkClassUseGetPos_E(self,r,dec,offset);
        break;
    default : return 0;
    }
    if(!ps) return NULL;
    if(!(*offset)) return NULL;
    pos=cgrCreateOpe('+',pos,ps,NULL);
    return pos;
}

//
// set inserted node
//
static cgrNode chkClassUseSetInsert(
    gsHandle self,cgrNode p,cgrNode ps){
    cgrNode bit;

    switch(ps->type){
    case Arr :{
        cgrNode right;
        right=cgrNew(Id,p->file,p->line);
        cgrSetNode(p,cgrKeyLeft,NULL);
        cgrSetNode(p,cgrKeyRight,right);
        cgrSetNode(right,cgrKeyPar,p);
        p->type='*';
        bit=right;
        }break;
    case '.' :
        bit=p;
        break;
    default  :bit=NULL;
    }
    return bit;
}

//
// make bitselect A
//
static cgrNode checkClassUseMakeA(gsHandle self,
    cgrNode node,cgrNode pos,cgrNode offset,cgrNode out){

    //set output
    out->type=Funcall;
    cgrSetNode(out,cgrKeyLeft,NULL);
    cgrSetNode(out,cgrKeyRight,NULL);
    if(node->type==Id){          //id
        char*name;
        name=cgrGetStr(node,cgrKeyVal);
        cgrSetStr(out,cgrKeyId,name);
    }else if(node->type==Array){ //array
        cgrSetNode(out,cgrKeyLeft,node);
        cgrSetNode(node,cgrKeyPar,out);
    }
    cgrSetContext(offset,pos);
    cgrSetNode(offset,cgrKeyPar,out);
    cgrSetNode(out,cgrKeyArgs,offset);
    return out;
}

//
// make bitselect B
//
static cgrNode checkClassUseMakeB(gsHandle self,
    cgrNode node,cgrNode pos,cgrNode offset,cgrNode out){
    return checkClassUseMakeA(self,node,pos,offset,out);
}
//
// set variable node
//
static chkClassUseSetVar(gsHandle self,int pattern,
    cgrNode s,cgrNode pos,cgrNode offset,cgrNode bit){
    cgrNode of;

    of=fsmdCopyOpe(pos);
    offset=cgrCreateOpe('+',of,offset,NULL);
    switch(pattern){
    case ptnutilType_A :
        checkClassUseMakeA(self,s,pos,offset,bit);
        break;
    case ptnutilType_B :
        checkClassUseMakeB(self,s,pos,offset,bit);
        break;
    default : break;
    }
    cgrSetNum(bit,cgrKeyIsPartSelec,1);
    return 0;
}

//
//change to bit select
//
static int chkClassUse(gsHandle self,cgrNode node){
    cgrNode s;    //start   node
    cgrNode p,ps; //parent  node
    cgrNode r;    //right   node
    cgrNode  dec; //declare node
    cgrNode  bit; // bit select node
    cgrNode pos,offset; //position,offset
    int pattern;    //start node pattern flg

    //get start node
    s=chkClassUseGetStart(self,node);
    if(!s) return 0;
    pattern=ptnutilMatch(s);

    //get declare node
    dec=chkClassUseGetDec(self,s,pattern);
    if(!dec) return 0;

    //check node
    p=cgrGetNode(s,cgrKeyPar);
    if(!p) return 0;
    r=cgrGetNode(p,cgrKeyRight);
    if(!r) return 0;
    ps=p;

    //middle node
    pos=chkClassUseMiddle(self,&dec,&p,&r);

    //terminal node
    pos=chkClassUseTerminal(self,dec,pos,r,&offset);
    if(!pos) return 0;
    
    //set inserted node
    bit=chkClassUseSetInsert(self,p,ps);

    if(!bit) return 0;
    //make start node
    chkClassUseSetVar(self,pattern,s,pos,offset,bit);

    return 0;
}

//
//operator
//
static int chkClassOpe(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case Funcall : self->funcall(self,ope); break;
        case Data_type : self->datatype(self,ope); break;
        case '=' : gsOpeLinkEq(ope); break;
        case '.' :
        case Arr :
            chkClassUse(self,ope);
        default  : gsOpeLink(ope); break;
        }
        ope=ope->next;
    }
    return 0;
}

// cfg
static int chkClassCfg(gsHandle self,cgrNode node){
    cgrNode cfg;
    cfg=node;
    cmstkPush(chkClassTagTable(self));
    cmstkPush(chkClassVarTable(self));
    while(cfg){
        gsCfgBody(self,cfg);
        cfg=cfg->next;
    }
    cmstkPop(chkClassTagTable(self));
    cmstkPop(chkClassVarTable(self));
}

//
// class/struct bitwidth (class tag)
//
static int chkClassWidth(gsHandle self,cgrNode node);
static int chkClassWidthTag(gsHandle self,cgrNode node){
    int i;
    char*key;
    cgrNode* c;
    cgrNode var;
    key=cgrGetStr(node,cgrKeyTag);
    c=(cgrNode*)cmstkFind(chkClassTagTable(self),key);
    if(!c){
        cmsgEcho(__sis_msg,47,node->file,node->line,key);
        return 1;
    }
    cgrCopyDataType2(*c,node);
    i=cgrGetNum(*c,cgrKeySize);
    cgrSetNum(node,cgrKeySize,i);
    return i;
}

//
// class/struct bitwidth body
//
static int chkClassWidthBody(gsHandle self,cgrNode body){
    int offset;
    switch(body->type){
    case Class  :
    case Struct :
        offset=chkClassWidth(self,body);
        break;
    case Function :
        cmsgEcho(__sis_msg,57,
            body->file,body->line,cgrGetStr(body,cgrKeyId));
        offset=0;
        break;
    default :
        offset=cgrBitWidth(body);
        break;
    }
    return offset;
}

//
// class/struct bitwidth
//
static int chkClassWidth(gsHandle self,cgrNode node){
    int i,arrsize;
    cgrNode body; //class body
    cgrNode var ; //variable node
    
    //-->push
    cmstkPush(chkClassTagTable(self));

    //is class tag
    if(cgrGetNum(node,cgrKeyIsClassTag)){
        cmstkPop(chkClassTagTable(self));
        return chkClassWidthTag(self,node);
    }
    
    //bit width decision
    body=cgrGetNode(node,cgrKeyClassBody);
    for(i=0;body;body=body->next){
        int offset,offsetdef;
        offsetdef=chkClassWidthBody(self,body);
        var=cgrGetNode(body,cgrKeyVal);
        arrsize=0;
        while(var){
            arrsize+=getAllArraySize(self,var);
            var=var->next;
        }
        if(arrsize>0) offset=offsetdef*arrsize;
        cgrSetNum(body,cgrKeyClassElmIndex,i);
        cgrSetNum(body,cgrKeyClassElmOffset,offset);
        cgrSetNum(body,cgrKeyClassElmOffsetDef,offsetdef);
        i+=offset;
    }
    if(i<=0) i=1;
    
    //set size
    cgrSetNum(node,cgrKeySize,i);

    //<--pop
    cmstkPop(chkClassTagTable(self));

    return i;
}

//
//class/struct to sc_uint/sc_biguint base
//
static int chkClassChangeBase(gsHandle self,cgrNode dec,cgrNode node){
    if(dec){
        cgrNode var;
        cgrSetNode(dec,cgrKeyClassDec,node);
        var=cgrGetNode(dec,cgrKeyVal);
        while(var){
            char*key;
            cgrNode left;
            cgrNode decdef;
            decdef=node;
            //trace struct/class declaration
            if(cgrGetNum(node,cgrKeyIsClassTag)){
                char*tag;
                cgrNode*tmp;
                tag=cgrGetStr(node,cgrKeyTag);
                tmp=(cgrNode*)cmstkFind(chkClassTagTable(self),tag);
                if(tmp) decdef=*tmp;
            } 
            //set declaration
            left=cgrGetMaxLeft(var);
            key=cgrGetStr(left,cgrKeyVal);
            cmstkSet(chkClassVarTable(self),key,decdef);
            var=var->next;
        }
    }
    return 0;   
}
//
//class/struct to sc_uint/sc_biguint
//
static int chkClassChange(gsHandle self,cgrNode node){
    if(cmstkSize(chkClassTagTable(self))>3){
        int size; //bitwidth
        cgrNode dec;//declar node
        //get size
        size=cgrGetNum(node,cgrKeySize);
        dec=tagChange(self,node,size);
        //set var table
        chkClassChangeBase(self,dec,node);
    }
    return 0;
}
//
//class/struct to sc_uint/sc_biguint 2 (function)
//
static int chkClassChange2(gsHandle self,cgrNode func,cgrNode node){
    int size; //bitwidth
    cgrNode dec;//declar node
    //get size
    size=cgrGetNum(node,cgrKeySize);
    dec=tagChange2(self,node,size);
    cgrSetNode(func,cgrKeyType,dec);
    return 0;
}
//
//class/struct to sc_uint/sc_biguint 3 (args)
//
static int chkClassChange3(gsHandle self,cgrNode func,cgrNode node){
    int size; //bitwidth
    cgrNode dec;//declar node
    cgrNode prev,next;//preview,next
    //get size
    prev=node->prev;
    next=node->next;
    size=cgrGetNum(node,cgrKeySize);
    dec=tagChange3(self,node,size);
    if(prev) prev->next=dec;
    else cgrSetNode(func,cgrKeyArgs,dec);
    dec->next=next;
    if(next) next->prev=dec;
    //set declaration
    chkClassChangeBase(self,dec,node);
    return 0;
}
//
//class/struct to sc_uint/sc_biguint 4 (signal)
//
static int chkClassChange4(gsHandle self,cgrNode sig,cgrNode node){
    int size; //bitwidth
    cgrNode dec;//declar node
    //get size
    size=cgrGetNum(node,cgrKeySize);
    dec=tagChange3(self,node,size);
    cgrSetNode(sig,cgrKeySize,dec);
    return 0;
}

//
// class/struct base
//
static int chkClassClassBase(gsHandle self,cgrNode node){
    char* key; //key

    //set to table
    key=cgrGetStr(node,cgrKeyTag);
    if(key){
        if(!cgrGetNum(node,cgrKeyIsClassTag))
            cmstkSet(chkClassTagTable(self),key,node);
    }

    //bit width desition
    chkClassWidth(self,node);

    return 0;
}

//
// class/struct
//
static int chkClassClass(gsHandle self,cgrNode node){
    chkClassClassBase(self,node);
    chkClassChange(self,node);
    return 0;
}

//
// class/struct 2 (function)
//
static int chkClassClass2(gsHandle self,cgrNode func,cgrNode node){
    chkClassClassBase(self,node);
    chkClassChange2(self,func,node);
    return 0;
}

//
// class/struct 3 (function args)
//
static int chkClassClass3(gsHandle self,cgrNode func,cgrNode node){
    chkClassClassBase(self,node);
    chkClassChange3(self,func,node);
    return 0;
}


//
// args declaration
//
static int chkClassArgDec(gsHandle self,cgrNode func,cgrNode node){
    cgrNode arg;
    arg=node;
    while(arg){
        cgrNode next;
        next=arg->next;
        if((arg->type==Class)||(arg->type==Struct))
            chkClassClass3(self,func,arg);
        arg=next;
    }
    return 0;
}

//
// typedef
//
static int chkClassTypedef(gsHandle self,cgrNode node){
    cgrNode dec;
    dec=cgrGetNode(node,cgrKeyVal);
    if(!dec) return 0;
    switch(dec->type){
        case Class  :
        case Struct :
            self->dec_class(self,dec);
            break;
    }
    return 0;
}
//
// task
//
static int chkClassTask(gsHandle self,cgrNode node){
    cgrNode type; //data type

    //-->push
    gsPush(self,node);
    cmstkPush(chkClassTagTable(self));
    cmstkPush(chkClassVarTable(self));

    //set type
    type=cgrGetNode(node,cgrKeyType);
    if(type){
        if((type->type==Class)|| (type->type==Struct)) 
            chkClassClass2(self,node,type);
    }

    //operation
    chkClassArgDec(self,node,cgrGetNode(node,cgrKeyArgs));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    
    //<--pop
    gsPop(self);
    cmstkPop(chkClassTagTable(self));
    cmstkPop(chkClassVarTable(self));
    return 0;
}

//
// signal
//
static int chkClassSigDec(gsHandle self,cgrNode node){
    cgrNode type;
    type=cgrGetNode(node,cgrKeySize);
    if(!type) return 0;
    switch(type->type){
    case Class  :
    case Struct :
        chkClassClassBase(self,type);
        chkClassChange4(self,node,type);
        break;
    }
    return 0;
}

//
// module
//
static int chkClassModule(gsHandle self,cgrNode node){
    cgrNode dec; //declaration
    cgrNode proc;//process
    cgrNode task;//task
    if(!node) return 0;
    
    //-->push
    cmstkPush(chkClassTagTable(self));
    cmstkPush(chkClassVarTable(self));
    //in module declaration
    dec=cgrGetNode(node,cgrKeyVal);
    while(dec){
        switch(dec->type){
        case Class  :
        case Struct :
            self->dec_class(self,dec);
            break;
        case Typedef :    
            self->tdef(self,dec);
            break;
        case Sc_signal_key :
            self->sig_dec(self,dec);
            break;
        }
        dec=dec->next;
    }
    //into process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        //-->push
        gsPush(self,proc);
        cmstkPush(chkClassTagTable(self));
        cmstkPush(chkClassVarTable(self));
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        //<--pop
        gsPop(self);
        cmstkPop(chkClassTagTable(self));
        cmstkPop(chkClassVarTable(self));
        proc=proc->next;
    }
    //into task
    task=cgrGetNode(node,cgrKeyTask);
    cgrMaps(task,
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        chkClassTask(self,task));
    //<--pop
    cmstkPop(chkClassTagTable(self));
    cmstkPop(chkClassVarTable(self));

    return 0;
}

//
//  Root  
//
static int chkClassRoot(gsHandle self,cgrNode node){
    cgrNode dec;
    cMstk table[2];//table buffer
    char*fname="";
    
    //init tagtable
    self->work=(void*)table;
    chkClassTagTable(self)=cmstkNew(sizeof(cgrNode));
    chkClassVarTable(self)=cmstkNew(sizeof(cgrNode));
    cmstkPush(chkClassTagTable(self));
    cmstkPush(chkClassVarTable(self));

    //main loop
    dec=node;
    while(dec){
        if(strcmp(fname,dec->file)){
            cmstkPop(chkClassTagTable(self));
            cmstkPop(chkClassVarTable(self));
            cmstkPush(chkClassTagTable(self));
            cmstkPush(chkClassVarTable(self));
            fname=dec->file;
        }

        switch(dec->type){
        case Sc_module :
            self->module(self,dec);
            break;
        case Class :   
        case Struct:
            self->dec_class(self,dec);
            break;
        case Typedef :
            self->tdef(self,dec);
            break;
        }
        dec=dec->next;
    }
    //tag table destruction
    cmstkPop(chkClassTagTable(self));
    cmstkPop(chkClassVarTable(self));
    cmstkDes(chkClassTagTable(self));
    cmstkDes(chkClassVarTable(self));
    return 0;
}

//
//initialization
//
gsHandle chkClass(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*chkClassRoot);
    gs->module=(*chkClassModule);
    gs->sig_dec=(*chkClassSigDec);
    gs->cfg=(*chkClassCfg);
    gs->dec_class=(*chkClassClass);
    gs->tdef=(*chkClassTypedef);
    gs->ope=(*chkClassOpe);
    return gs;
}

/*------------------------------------------------------------------
  
  checking range function

------------------------------------------------------------------*/
//
// funcall
// x.range(4,3) --> x(4,3)
//
static int chkRangeFuncall(gsHandle self,cgrNode node){
    cgrNode fleft; //function left node

    //check function left node
    fleft=cgrGetNode(node,cgrKeyLeft);
    if(!fleft){
        int type;    //pointer type
        char*name;   //function name
        cgrNode par; //parent node
        cgrNode out; //out node

        //get name
        name=cgrGetStr(node,cgrKeyId);
        if(strcmp(name,"range")) return 0;
        
        //get parent node
        par=cgrGetNode(node,cgrKeyPar);
        if(!par) return 0;
        //get parent type
        type=par->type;

        //set range function
        if((type=='.') || (type==Arr)){
            cgrNode args;  //argument node
            cgrNode left;  //left node

            //check left node
            left=cgrGetNode(par,cgrKeyLeft);
            if(!left) return 0;

            args=cgrGetNode(node,cgrKeyArgs);
            //check args
            if(!args){
                cmsgEcho(__sis_msg,50,node->file,node->line);
                return 0;
            }else if(!(args->next)){
                cmsgEcho(__sis_msg,50,node->file,node->line);
                return 0;
            }

            //set output x.y->range(...)
            if((type==Arr) && 
                ((left->type=='.') || (left->type==Arr))){
                cmsgEcho(__sis_msg,51,node->file,node->line);
                return 0;
            }
            //x.y.range(...)
            else if((left->type=='.') || (left->type==Arr)){
                cgrNode lleft;  //left left node
                cgrNode lright; //left right node
                lleft=cgrGetNode(left,cgrKeyLeft);
                lright=cgrGetNode(left,cgrKeyRight);
                cgrSetNode(par,cgrKeyLeft,lleft);
                cgrSetNode(lleft,cgrKeyPar,par);
                left=lright;
                out=node;
            }
            //x.range(...)
            else if(type=='.'){
                out=par;
                cgrSetNode(par,cgrKeyLeft,NULL);
                cgrSetNode(par,cgrKeyRight,NULL);
            //x->range(...)
            }else if(type==Arr){
                out=node;
                par->type='*';
                cgrSetNode(par,cgrKeyLeft,NULL);
                cgrSetNode(par,cgrKeyRight,NULL);
            }
            out->type=Funcall;

            //if x.range(...)
            if(left->type==Id){
                char* rname;
                rname=cgrGetStr(left,cgrKeyVal);
                cgrSetStr(out,cgrKeyId,rname);
            //if x[10].range(...)
            }else{
                cgrSetNode(out,cgrKeyLeft,left);
                cgrSetNode(left,cgrKeyPar,out);
            }
            cgrSetNode(out,cgrKeyArgs,args);
            cgrSetNode(args,cgrKeyPar,out);
        }
    }
    return 0;
}
//
//initialization
//
gsHandle chkRange(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*gsModule3);
    gs->funcall=(*chkRangeFuncall);
    return gs;
}

/*------------------------------------------------------------------
  
  checking typedef

------------------------------------------------------------------*/
//
// macros
//
#define chkTdefTagTable(gs) ((cMstk*)gs->work)[0]

//
// tag
//
static int chkTdefTag(gsHandle self,cgrNode node){
    char*tag;//tag string
    cgrNode* dec; //declaration node

    tag=cgrGetStr(node,cgrKeyId);
    dec=(cgrNode*)cmstkFind(chkTdefTagTable(self),tag);
    if(!dec){
        return 0;
    }
    switch((*dec)->type){
    case Class  :
    case Struct :
        cgrCopyDataType2(*dec,node);
        break;
    default :
        cgrCopyDataType5(*dec,node);
        break;
    }
    return 0;
}

//
// typedef 
//
static int chkTdefTdef(gsHandle self,cgrNode node){
    char* tag; //tag string
    cgrNode dt;//data type node
    
    //set tag to table
    tag=cgrGetStr(node,cgrKeyTag);
    dt=cgrGetNode(node,cgrKeyVal);
    cmstkSet(chkTdefTagTable(self),tag,dt);
    
    //remove node
    if(cmstkSize(chkTdefTagTable(self))>3){
        cgrNode par;
        par=gsTop(self);
        if(par==node){
            cgrNode tmp;
            tmp=gsPop(self);
            par=gsTop(self);
            gsPush(self,tmp);
        }
        cgrRemoveNode(par,cgrKeyVal,node);
    }
    return 0;
}
//
// signal key
//
static int chkTdefSignal(gsHandle self,cgrNode node){
    cgrNode size=cgrGetNode(node,cgrKeySize); //port type
    if(!size) return 0;
    if(size->type==Tag) chkTdefTag(self,size);
    return 0;
}

//
// module 
//
static int chkTdefModule(gsHandle self,cgrNode node){
    cgrNode proc;
    cgrNode task;
    cgrNode dec;

    if(!node) return 0;

    //push
    gsPush(self,node);
    cmstkPush(chkTdefTagTable(self));
    
    //module block
    dec=cgrGetNode(node,cgrKeyVal);
    while(dec){
        cgrNode next=dec->next;
        switch(dec->type){
        case Typedef:
            self->tdef(self,dec);
        case Sc_signal_key :
            self->sig_dec(self,dec);
            break;
        }
        dec=next;
    }

    //process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        cgrNode next=proc->next;
        gsPush(self,proc);
        cmstkPush(chkTdefTagTable(self));

        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        
        gsPop(self);
        cmstkPop(chkTdefTagTable(self));
        proc=next;
    }
    //task
    task=cgrGetNode(node,cgrKeyTask);
    while(task){
        cgrNode type,args;
        cgrNode next=task->next;
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);

        type=cgrGetNode(task,cgrKeyType);
        args=cgrGetNode(task,cgrKeyArgs);

        gsPush(self,task);
        cmstkPush(chkTdefTagTable(self));
        //function type
        if(type){
            if(type->type==Tag) chkTdefTag(self,type);
        }
        //function args
        while(args){
            if(args->type==Tag) chkTdefTag(self,args);
            args=args->next;
        }
        //function body
        self->cfg(self,cgrGetNode(task,cgrKeyVal));

        gsPop(self);
        cmstkPop(chkTdefTagTable(self));
        task=next;
    }
    //pop
    gsPop(self);
    cmstkPop(chkTdefTagTable(self));
    return 0;
}


//
// Root  
//
static int chkTdefRoot(gsHandle self,cgrNode node){
    cgrNode dec;
    cMstk table[1];//table buffer
    char* fname="";
    
    //init tagtable
    self->work=(void*)table;
    chkTdefTagTable(self)=cmstkNew(sizeof(cgrNode));
    cmstkPush(chkTdefTagTable(self));

    //main loop
    dec=node;
    while(dec){
        if(strcmp(fname,dec->file)){
            cmstkPop(chkTdefTagTable(self));
            cmstkPush(chkTdefTagTable(self));
            fname=dec->file;
        }
        switch(dec->type){
        case Sc_module :
            self->module(self,dec);
            break;
        case Typedef :
            self->tdef(self,dec);
            break;
        }
        dec=dec->next;
    }
    //tag table destruction
    cmstkPop(chkTdefTagTable(self));
    cmstkDes(chkTdefTagTable(self));
    return 0;
}

//
//initialization
//
gsHandle chkTdef(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*chkTdefRoot);
    gs->module=(*chkTdefModule);
    gs->tdef=(*chkTdefTdef);
    gs->sig_dec=(*chkTdefSignal);
    gs->tag=(*chkTdefTag);
    gs->cfg=(*gsCfg2);
    return gs;
}


/*------------------------------------------------------------------
  
  division variable declaration

------------------------------------------------------------------*/
//
// division base
//
static int divVarDiv(gsHandle self,cgrNode node);
static int divVarDivBase(gsHandle self,cgrNode node,
    cgrNode (*copyf)(cgrNode)){

    cgrNode var;   //variable node
    cgrNode next;  //next variable node
    cgrNode newdec;//new declaration node
    var=cgrGetNode(node,cgrKeyVal);
    if(!var) return 0;
    next=var->next;
    if(!next) return 0;
    newdec=copyf(node);
    cgrSetNode(newdec,cgrKeyVal,next);
    var->next=NULL;
    next->prev=NULL;
    cgrInstNode(node,newdec);
    divVarDiv(self,newdec);
    return 0;
}

//
// division
//
static int divVarDiv(gsHandle self,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case Sc_signal_key :
        divVarDivBase(self,node,(*cgrCopySignalKey));
        break;
    case Function: 
    case Public  :
    case Private : break;
    case Class   : //class/struct
    case Struct  :{
        cgrNode body;
        body=cgrGetNode(node,cgrKeyClassBody);
        while(body){
            divVarDiv(self,body);
            body=body->next;
        }
    } //should no break!
    default : //normally
        divVarDivBase(self,node,(*cgrCopyDataType3));
        break;
    }
    return 0;
}


//
// data type
//
static int divVarDataType(gsHandle self,cgrNode node){
    divVarDiv(self,node);
    return 0;
}

//
// class
//
static int divVarClass(gsHandle self,cgrNode node){
    divVarDiv(self,node);
    return 0;
}

//
// typedef
//
static int divVarTdef(gsHandle self,cgrNode node){
    cgrNode val;
    val=cgrGetNode(node,cgrKeyVal);
    switch(val->type){
        case Class  :
        case Struct :
            self->dec_class(self,val);
            break;
    }
    return 0;
}

//
// module block
//
int divVarModuleBlock(gsHandle self,cgrNode node){
    switch(node->type){
    case Typedef:
        self->tdef(self,node);
        break;
    case Sc_signal_key :
        divVarDiv(self,node);
        break;
    case Class :
    case Struct:
        self->dec_class(self,node);
        break;    
    }
    return 0;
}

//
// module 
//
int divVarModule(gsHandle self,cgrNode node){
    cgrNode proc;
    cgrNode task;
    cgrNode dec;

    if(!node) return 0;

    //module block
    dec=cgrGetNode(node,cgrKeyVal);
    while(dec){
        cgrNode next=dec->next;
        divVarModuleBlock(self,dec);
        dec=next;
    }

    //process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        cgrNode next=proc->next;
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        proc=next;
    }
    //task
    task=cgrGetNode(node,cgrKeyTask);
    while(task){
        cgrNode next=task->next;
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        self->cfg(self,cgrGetNode(task,cgrKeyVal));
        task=next;
    }
    //pop
    return 0;
}

//
// Root  
//
static int divVarRoot(gsHandle self,cgrNode node){
    cgrNode dec;

    //main loop
    dec=node;
    while(dec){
        switch(dec->type){
        case Sc_module :
            self->module(self,dec); 
            break;
        case Typedef : 
            self->tdef(self,dec);
            break;
        case Class  : 
        case Struct : 
            self->dec_class(self,dec);
            break;
        }
        dec=dec->next;
    }
    return 0;
}

//
//initialization
//
gsHandle divVar(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*divVarRoot);
    gs->dec_class=(*divVarClass);
    gs->datatype=(*divVarDataType);
    gs->module=(*divVarModule);
    gs->tdef=(*divVarTdef);
    gs->cfg=(*gsCfg2);
    return gs;
}
/*------------------------------------------------------------------
  
  checking function out of module
  module[cgrKeyTask]->Id->Id ...

------------------------------------------------------------------*/

#define chkFuncTable(gs) ((cMstk*)gs->work)[0]
#define chkFuncExTable(gs) ((cMstk*)gs->work)[1]

//
//remove function node
//
static int chkFuncRemove(cgrNode node){
    cgrNode prev;
    if(!node) return 0;
    prev=node->prev;
    if(!prev){
        cgrNode next=node->next;
        cgrSetRoot(next);
        if(next) next->prev=NULL;
    }else{
        cgrNode next=node->next;
        prev->next=next;
        if(next) next->prev=prev;
    }
    return 0;
}

//
//Function
//  
static int chkFuncFunction(gsHandle self,cgrNode node){
    char*name;
    cgrNode opt;
    cgrNode type;
    cgrNode* dec;
    int staticflg; //0:not ,1:is static
    
    //check function out of module
    if(!cgrGetNum(node,cgrKeyIsOutOfModule)) return 0;

    //get values
    staticflg=0; 
    name=cgrGetStr(node,cgrKeyId);
    type=cgrGetNode(node,cgrKeyType);
    opt=cgrGetNode(node,cgrKeyOpt);
    dec=cmstkFind(chkFuncTable(self),name);

    if(opt){
        if(!strcmp(cgrGetStr(opt,cgrKeyVal),"static"))
            staticflg=1;
    }

    if(dec){//exists in stack
        cgrNode cfg=cgrGetNode(node,cgrKeyVal);
        if(cfg){
            cmstkSet(chkFuncTable(self),name,node);
            if(!staticflg) 
                cmstkSet(chkFuncExTable(self),name,node);
        }    
    }else{//not exits in stack
        cmstkSet(chkFuncTable(self),name,node);
        if(!staticflg) 
            cmstkSet(chkFuncExTable(self),name,node);
    }
    chkFuncRemove(node);
    return 0;
}
//
//make task(id) node to module
//
static int chkFuncMakeTaskDec(
    gsHandle self,cgrNode mo,char*key,cgrNode dec){
    cgrNode id=cgrNew(Id,dec->file,dec->line);
    cgrNode cfg=cgrGetNode(dec,cgrKeyVal);
    cgrNode next=cgrGetNode(mo,cgrKeyTask);
    cgrSetStr(id,cgrKeyVal,key);
    if(cfg) cgrSetNode(id,cgrKeyIdLinkTask,dec);
    id->next=next;
    if(next) next->prev=id;
    cgrSetNode(mo,cgrKeyTask,id);
    return 0;
}

//
//make task(id) node to module
//
static int chkFuncMakeTask(gsHandle self,cMap map,cgrNode mo){
    cmapMaps(map,char*,cgrNode,
        chkFuncMakeTaskDec(self,mo,key,val));
    return 0;    
}
//
//set task to Module in a file
//
static int chkFuncFileModule(
    gsHandle self,cgrNode node,cMstk table){
    cgrNode task=cgrGetNode(node,cgrKeyTask);
    cgrMaps(task,
        char*name=cgrGetStr(task,cgrKeyVal);
        cgrNode dec=cgrGetNode(task,cgrKeyIdLinkTask);
        cgrNode*fdec=cmstkFind(table,name);
        if((!dec) && fdec) 
            cgrSetNode(task,cgrKeyIdLinkTask,*fdec));
    return 0;
}
//
//Module
//
static int chkFuncModule1(gsHandle self,cgrNode node){
    cvectMaps(chkFuncTable(self),cMap,
        chkFuncMakeTask(self,val,node));
    return 0;
}
//
//Module (extern)
//
static int chkFuncModule2(gsHandle self,cgrNode node){
    chkFuncFileModule(self,node,chkFuncExTable(self));
    return 0;
}

//
//File
//
static int chkFuncFile(gsHandle self,cgrNode node){
    cgrNode dec;
    dec=node;
    cgrMaps(dec,
        if(dec->type==Sc_module)
            chkFuncFileModule(self,dec,chkFuncTable(self)));
    return 0;
}

//
//Root
//  
static int chkFuncRoot(gsHandle self,cgrNode node){
    char*fname;
    cgrNode dec; //declaration
    cgrNode ftop;//top of file node
    cMstk table[2];

    //table initialization
    self->work=(void*)table;
    chkFuncTable(self)=cmstkNew(sizeof(cgrNode));
    chkFuncExTable(self)=cmstkNew(sizeof(cgrNode));
    cmstkPush(chkFuncTable(self));

    fname="";
    dec=node;
    ftop=dec;
    //file scope
    while(dec){
        cgrNode next=dec->next;
        //move on file
        if(strcmp(fname,dec->file)){
            chkFuncFile(self,ftop);
            cmstkPop(chkFuncTable(self));
            cmstkPush(chkFuncTable(self));
            fname=dec->file;
            ftop=dec;
        }
        //module
        switch(dec->type){
        case Sc_module :
           chkFuncModule1(self,dec);
           break;
        case Function:
           self->function(self,dec);
           break;
        }
        dec=next;
    }
    chkFuncFile(self,ftop);

    //extern scope
    dec=node;
    cgrMaps(dec,
        if(dec->type==Sc_module) 
        chkFuncModule2(self,dec));

    //table destruction
    cmstkDes(chkFuncTable(self));
    cmstkDes(chkFuncExTable(self));
    return 0;
}


//
//initialization
//
gsHandle chkFunc(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->root=(*chkFuncRoot);
    gs->function=(*chkFuncFunction);
    return gs;
}
/*------------------------------------------------------------------
  
  distribution
 (a*c+b*c)=c*(a+b)

------------------------------------------------------------------*/
//
// set ope distributive low root
// a*n+b*n->(a+b)*n
//
static int distriOpeSet(gsHandle self,
    cgrNode a,cgrNode b,cgrNode n,cgrNode add,cgrNode mul){
    int atype=mul->type;
    int mtype=add->type;
    mul->type=mtype;
    add->type=atype;
    cgrSetNode(mul,cgrKeyRight,n);
    cgrSetNode(n,cgrKeyPar,mul);
    cgrSetNode(add,cgrKeyLeft,a);
    cgrSetNode(a,cgrKeyPar,add);
    cgrSetNode(add,cgrKeyRight,b);
    cgrSetNode(b,cgrKeyPar,add);
    return 0;
}
//
// distributive low root
//
static int distriOpe1(gsHandle self,cgrNode node){
    cgrNode ll,lr,rl,rr;
    cgrDecLR(node,0);
    
    //check LR node
    if(!left) return 0;
    if(!right) return 0;
    if(left->type!=right->type) return 0;
    if((left->type!='*')&&(left->type!='/')) return 0;
    ll=cgrGetNode(left,cgrKeyLeft);
    lr=cgrGetNode(left,cgrKeyRight);
    rl=cgrGetNode(right,cgrKeyLeft);
    rr=cgrGetNode(right,cgrKeyRight);
    if(!(ll&&lr&&rl&&rr)) return 0;
    
    //a*n+b*n or a*n+b*n 
    if(cgrIdentOpe(lr,rr)){
        distriOpeSet(self,ll,rl,lr,left,node);
        return 0;
    }
    if(left->type!='*') return 0;

    //a*n+n*b
    if(cgrIdentOpe(lr,rl))      distriOpeSet(self,ll,rr,rl,left,node);
    //n*a+b*n
    else if(cgrIdentOpe(ll,rr)) distriOpeSet(self,lr,rl,rr,left,node);
    //n*a+n*b
    else if(cgrIdentOpe(ll,rl)) distriOpeSet(self,lr,rr,rl,left,node);
    return 0;
}

//
// operation
//
static int distriOpe(gsHandle self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case Funcall : self->funcall(self,ope); break;
        case '+'     :
        case '-'     :
            gsOpeLink(ope);
            distriOpe1(self,node);
            break;
        case Data_type : self->datatype(self,ope); break;
        case '=' : gsOpeLinkEq(ope); break;
        default  : gsOpeLink(ope); break;
        }
        ope=ope->next;
    }
    return 0;
}

//
//initialization
//
gsHandle distri(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->ope=(*distriOpe);
    gs->module=(*gsModule2);
    gs->funcall=(*commuFuncall);
    return gs;
}

/*------------------------------------------------------------------
  
  prechecking variable declaration before main checking

------------------------------------------------------------------*/
//
//id
//
static int preChkVarIdFuncall(gsHandle self,cgrNode node){
    char*name=cgrGetIdName(node);
    if(!node) return 0;
    //checking args
    if(node->type==Funcall)
        self->ope(self,cgrGetNode(node,cgrKeyArgs));
    //setting declaration
    cgrNode*dec=cmstkFind(self->mstack,name);
    if(!dec) return 0;
    cgrSetNode(node,cgrKeyDeclared,*dec);
    return 0;
}

//
//args/signal/variable declaration
//
static int preChkVarDec(gsHandle self,cgrNode node){
    cgrNode var; //variable node
    char*name;   //variable name

    var=cgrGetNode(node,cgrKeyVal);
    if(!var) return 0;
    name=cgrGetIdName(var);
    if(name[0]=='\0') return -1;
    cmstkSet(self->mstack,name,node);
    self->ope(self,var);
    return 0;
}
//
//module
//
static int preChkVarVarModule(gsHandle self,cgrNode node){
    cgrNode body; //module body
    cgrNode task;
    //checking node
    if(!node) return 0;

    //checking signal declaration
    body=cgrGetNode(node,cgrKeyVal);
    while(body){
        switch(body->type){
        case Sc_signal_key : self->sig_dec(self,body); break;
        case Sc_ctor       : cgrSetNode(node,cgrKeyCtor,body);break;
        default : break;
        }
        body=body->next;
    }
    //checking task
    task=cgrGetNode(node,cgrKeyTask);
    while(task){
        cgrNode next=task->next;
        //checking task
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        if(task){
            cgrNode args=cgrGetNode(task,cgrKeyArgs);
            gsPush(self,task);
            cmstkPush(self->mstack);
            //checking args
            while(args){
                self->datatype(self,args);
                args=args->next;
            }
            //checking cfg
            self->cfg(self,cgrGetNode(task,cgrKeyVal));
            cmstkPop(self->mstack);
            gsPop(self);
        }
        task=next;
    }

    //start checking
    return gsModule5(self,node);
}

//
//initialization
//
gsHandle preChkVar(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module    =(*preChkVarVarModule);
    gs->argdec    =(*preChkVarDec);
    gs->datatype  =(*preChkVarDec);
    gs->sig_dec   =(*preChkVarDec);
    gs->id        =(*preChkVarIdFuncall);
    gs->funcall   =(*preChkVarIdFuncall);
    return gs;
}

/*------------------------------------------------------------------
  
  checking if-else

------------------------------------------------------------------*/
//
//else
//
static int chkIfElseElse(gsHandle self,cgrNode node){
    cgrNode prev=NULL;

    if(!node) return 0;
    if(node->type!=Else){
        self->cfg(self,node);
        return 0;
    }
    chkIfElseElse(self,node->next);

    //searching node
    gsCfgElse(self,node);
    
    prev=node->prev;
    if(!prev){
        cmsgEcho(__sis_msg,61,node->file,node->line);
        return 0;
    }
    if(prev->type==If) return 0;
    else if(prev->type==Else){
        cgrNode cfgif=cgrGetNode(prev,cgrKeyVal);
        cgrNode block=cgrNew(Block,node->file,node->line);
        if(!cfgif){
            cmsgEcho(__sis_msg,61,node->file,node->line);
            return 0;
        }else if((cfgif->type!=If) || (cfgif->next)){
            cmsgEcho(__sis_msg,61,node->file,node->line);
            return 0;
        }
        cgrSetNode(block,cgrKeyVal,cfgif);
        cgrSetNode(prev,cgrKeyVal,block);
        cgrSetContext(prev,node->next);
        cgrSetContext(cfgif,node);
    }else{
        cmsgEcho(__sis_msg,61,node->file,node->line);
        return 0;
    }
    return 0;
}

//
//initialization
//
gsHandle chkIfElse(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->cfg       =(*gsCfg2);
    gs->module    =(*gsModule3);
    gs->cfgelse   =(*chkIfElseElse);
    return gs;
}

/*------------------------------------------------------------------
  
   making header and footer string

------------------------------------------------------------------*/
//
// making string
//
static char* mkHedFotHeadName(char*outname){
    int i=0,len=0;
    char* buf=NULL;
    cstrSet(buf,"__",memError("making output buffer"));
    cstrCat(buf,outname,memError("making output buffer"));
    cstrCat(buf,"__DECLARE",memError("making output buffer"));
    
    len=strlen(buf);
    for(i=0;i<len;i++){
        if(isalpha(buf[i])){//alphabet
            if(islower(buf[i])) buf[i]=toupper(buf[i]); 
        }else if(isdigit(buf[i])) ; //number
        else buf[i]='_';
    }
    return buf;
}
//
// making header
//
static cgrNode mkHedFotHead(
    cgrNode top,char*toolname,char*outname,char*tm){
    char buf[SIS_BUF_SIZE]; //buffer
    char*headname=mkHedFotHeadName(outname); //outname
    char*file="";
    int line=1;

    if(top){
        file=top->file;
        line=top->line;
    }
    cgrNode head =cgrNew(OpString,file,line); //header
    cgrNode head2=cgrNew(OpString,file,line); //ifdef
    cgrNode head3=cgrNew(OpString,file,line); //include

    //header
    sprintf(buf,"/*\n    Created by %s.\n*/\n",toolname);
    cgrSetStr(head,cgrKeyVal,buf);

    //ifdef
    sprintf(buf,"#ifndef %s\n#define %s\n",headname,headname);
    cgrSetNum(head2,cgrKeyFormSw,1);
    cgrSetStr(head2,cgrKeyVal,buf);
    free(headname);

    //include "systemc.h"
    sprintf(buf,"#include \"systemc.h\"\n");
    cgrSetNum(head3,cgrKeyFormSw,1);
    cgrSetStr(head3,cgrKeyVal,buf);

    //setcontext
    cgrSetContext(head ,head2);
    cgrSetContext(head2,head3);
    return head;
}

//
// making footer
//
static cgrNode mkHedFotFoot(cgrNode top){
    char*file="";
    int line=1;
    cgrNode foot;
    if(top){
        file=top->file;
        line=top->line;
    }
    foot=cgrNew(OpString,file,line); //foot
    cgrSetNum(foot,cgrKeyFormSw,1);
    cgrSetStr(foot,cgrKeyVal,"#endif\n");

    return foot;
}

//
//initialization
//
int mkHedFot(cgrNode top,
    char*toolname,char*outname,char*tm){
    cgrNode head=mkHedFotHead(top,toolname,outname,tm); //header
    cgrNode foot=mkHedFotFoot(top); //footer
    cgrNode t1=cgrGetTail(head);
    cgrSetContext(t1,top);
    cgrSetContext(cgrGetTail(top),foot);
    cgrSetRoot(head);
    return 0;
}



/*------------------------------------------------------------------
  
  prechecking signal declaration

------------------------------------------------------------------*/
//
//funcall
//
static int preChkSigIdFuncall(gsHandle self,cgrNode node){
    char*name=cgrGetIdName(node);
    int ptn=ptnutilMatchReadWrite(node);
    cgrNode dec=cgrGetNode(node,cgrKeyDeclared);
    if(!dec) return 0;
    if(dec->type!=Sc_signal_key) return 0;
    switch(ptn){
    case ptnutilType_H :
    case ptnutilType_I :
    case ptnutilType_J :
    case ptnutilType_K : return 0;
    defualt : break;
    }
    cmsgEcho(__sis_msg,64,node->file,node->line,name);
    return 0;
}

//
//signal declaration
//
static int preChkSigSigDec(gsHandle self,cgrNode node){
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode var=cgrGetNode(node,cgrKeyVal);
    cgrNode left=cgrGetMaxLeft(var);
    char*name=cgrGetIdName(left);

    if(!var) return 0;
    if(var->type==Id) return 0;
    if(!type) return 0;
    switch(type->type){
    case Sc_fifo   : return 0;
    case Sc_signal : return 0;
    }

    cmsgEcho(__sis_msg,65,node->file,node->line,name);
    return 0;
}

//
// module
//
static int preChkSigModule(gsHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal); //module body

    while(body){
        switch(body->type){
        case Sc_signal_key :
            self->sig_dec(self,body);
            break;
        }
        body=body->next;
    }
    //start checking
    gsModule3(self,node);
    return 0;
}
//
//initialization
//
gsHandle preChkSig(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->cfg     =(*gsCfg2);
    gs->module  =(*preChkSigModule);
    gs->funcall =(*preChkSigIdFuncall);
    gs->sig_dec =(*preChkSigSigDec);
    return gs;
}


/*------------------------------------------------------------------
  
  setting function out of module

------------------------------------------------------------------*/
//
//module
//
static int setFuncModule(gsHandle self,cgrNode node){
    cgrNode task=cgrGetNode(node,cgrKeyTask);
    while(task){
        cgrNode next=task->next;
        if(task->type==Id){
            cgrNode taskdec=cgrGetNode(task,cgrKeyIdLinkTask);
            if(taskdec){
                cgrNode ntask=cgrCopyFunction(taskdec);
                cgrInstNode(task,ntask);
                cgrRemoveNode(node,cgrKeyTask,task);
                task->next=NULL;
            }
        }
        task=next;
    }
    return 0;
}

//
//initialization
//
gsHandle setFunc(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*setFuncModule);
 
    return gs;
}

/*------------------------------------------------------------------
  
  removing unused task

------------------------------------------------------------------*/
//
// module
//
static int remTaskModule(gsHandle self,cgrNode node){
    cgrNode task=cgrGetNode(node,cgrKeyTask);
    cMap map=cmapNew(
        sizeof(char*),sizeof(cgrNode),(*cmapStrKey),(*cmapStrCmp));

    while(task){
        cgrNode next=task->next;
        char*name="";
        switch(task->type){
        case Id       : name=cgrGetIdName(task); break;
        case Function : name=cgrGetStr(task,cgrKeyId); break;
        default : break;
        }
        if(cmapFind(map,&name)) cgrRemoveNode(node,cgrKeyTask,task);
        else cmapUpdate(map,&name,task);
        task=next;
    }

    cmapDes(map);
    return 0;
}
//
//initialization
//
gsHandle remTask(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module  =(*remTaskModule);
    return gs;
}


/*------------------------------------------------------------------
  
  removing cast

------------------------------------------------------------------*/
//
// removing
//
static int remCastRem(gsHandle self,cgrNode node){
    cgrNode par=cgrGetNode(node,cgrKeyPar);
    cgrNode ope=cgrGetNode(node,cgrKeyCond);
    cgrNode cfg=gsTop(self);
    
    //checking node
    if(!ope) return 0;
    if(!node) return 0;

    //setting next/prev
    ope->next=node->next;
    ope->prev=node->prev;
    if(node->next) node->next->prev=ope;
    if(node->prev) node->prev->next=ope;
    
    //setting parent node
    else if(par){
        cgrSetNode(ope,cgrKeyPar,par);
        if(cgrGetNode(par,cgrKeyLeft)==node) 
            cgrSetNode(par,cgrKeyLeft,ope);
        else if(cgrGetNode(par,cgrKeyRight)==node) 
            cgrSetNode(par,cgrKeyRight,ope);
        else if(cgrGetNode(par,cgrKeyCond)==node) 
            cgrSetNode(par,cgrKeyCond,ope);
        else if(cgrGetNode(par,cgrKeyArgs)==node) 
            cgrSetNode(par,cgrKeyArgs,ope);
    }
    //setting cfg
    else if(cfg){
        if(cgrGetNode(cfg,cgrKeyVal)==node) 
            cgrSetNode(cfg,cgrKeyVal,ope);
        else if(cgrGetNode(cfg,cgrKeyCond)==node) 
            cgrSetNode(cfg,cgrKeyCond,ope);
        else if(cgrGetNode(cfg,cgrKeyIni)==node) 
            cgrSetNode(cfg,cgrKeyIni,ope);
        else if(cgrGetNode(cfg,cgrKeyIni)==node) 
            cgrSetNode(cfg,cgrKeyCond,ope);
    }
    return 0;
}

//
//operator
//
static int remCastOpe(gsHandle self,cgrNode node){
    cgrNode ope=node;
    while(ope){
        cgrNode next=ope->next;
        gsOpeBody(self,ope);
        if(ope->type==Cast) remCastRem(self,ope);
        ope=next;
    }
    return 0;
}
//
//initialization
//
gsHandle remCast(cgrNode top){
    gsHandle gs;
    gs=gsIni(top);
    gs->module=(*gsModule4);
    gs->cfg=(*gsCfg2);
    gs->ope=(*remCastOpe);
    return gs;
}



/*------------------------------------------------------------------
  
  checking syntax and creating graph for synthesizing

------------------------------------------------------------------*/
int check(cgrNode top,chk2OptHandle opt){
    chkRun(chkFunc);  //checking function out of module
    chkRun(chkProc);  //checking processes and tasks
    chkRun(ignThread);//to ignore CTHREAD
    chkRun(chkRec);   //checking recursive call
    chkRun(remTask);  //removing unused task
    chkRun(divVar);   //variable declaration division 
    chkRun(chkTdef);  //checking typedef
    chkRun(chkRange); //checking range function
    chkRun(chkEnum);  //checking enum declaration
    chkRun(chkIfElse);//checking if-else
    chkRun(divVar);   //variable declaration division 
    chkRun(chkClass); //checking class declaration
    chkRun(setFunc);  //setting function out of module
    chkRun(remCast);  //removing cast
    chkRun(preChkVar);//prechecking variable declaration
    chkRun(preChkSig);//prechecking signal declaration
    chkRun(getSen);   //getting sensitive
    chkRun(creIns);   //getting instance
    chkRun(traRW);    //translation read/write
    checkExSet1;
    chkRun3(chkVar,chkVarDes); //checking variables declaration
    chkRun(remDec);   //removing variable declaration
    chkRun(creVar);   //variable name creation
    chkRun(fnInte);   //jointing function call interface
    chkRun(exArr);    //multi dimension array expansion
    checkExSet1;
    chkRun(tmpBit);   //deciding bitwidth for temporaly variable
    chkRun(traPtr);   //pointer translation
    chkRun(chkWait);  //checking wait
    chkRun(chkSub);   //checking synthesizable
    chkRun(setJump);  //translation from control graph to FSMD
    chkRun(n2d);      //translation NFA to DFA

    if(cmsgGetErr(__sis_msg)) return -1;
    return 0;
}


/*------------------------------------------------------------------
  
  checking after fsmd optimization 

------------------------------------------------------------------*/
//
//default option handler
//
chk2OptHandle chk2OptDefault(void){
    chk2OptHandle chk2={0};
    chk2.outfile="out";
    chk2.rstname="([rR][sS][tT])|([rR][eE][sS][eE][tT])";
    chk2.chain=0;
    return chk2;
}

//
//check2
//
int check2(cgrNode top,chk2OptHandle opt){
    //getting reset process
    if(!opt.noreset) chkRun3(rstProc,rstProcDes);

    if(cmsgGetErr(__sis_msg)) return -1;
    return 0;
}    

