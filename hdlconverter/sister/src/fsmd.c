/*
  FSMD access utiliteis.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"

/*------------------------------------------------------------------
  FSMD accessing class
------------------------------------------------------------------*/
//
//  Ope
//
static int fsmdOpe(fsmdHandle self,cgrNode node){
    int ret=0;
    cgrNode ope;
    if(!node) return ret;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :ret=self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case Funcall : ret=self->funcall(self,ope);break;
        case '=' : fsmdOpeLinkEq(ope,ret); break;
        default  : fsmdOpeLink(ope,ret) break;
        }
        ope=ope->next;
    }
    return ret;
}

//
//state
//
static int fsmdState(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    if(fsmdRouteCheck(node)) return 0;
    fsmdRouteSet(node);
    switch(node->type){
    case Dfg :
        self->dfg(self,node);
        self->state(self,node->jump);
        break;
    case Ctrl:
        self->ctrl(self,node);
        self->state(self,node->jump);
        self->state(self,node->next);
        break;
    }
    return 0;
}

//
//process
//
static int fsmdProc(fsmdHandle self,cgrNode node){
    cgrNode sen,state,rst;
    if(!node) return 0;
    fsmdRouteReset();
    sen=cgrGetNode(node,cgrKeySensitive);
    self->sen(self,sen);
    sen=cgrGetNode(node,cgrKeySensitivePos);
    self->pos(self,sen);
    sen=cgrGetNode(node,cgrKeySensitiveNeg);
    self->neg(self,sen);
    rst=cgrGetNode(node,cgrKeyResetProc);
    self->state(self,rst);
    state=cgrGetNode(node,cgrKeyVal);
    self->state(self,state);
    return 0;
}

//
//signal declaration
//
static int fsmdSigDec(fsmdHandle self,cgrNode node){
    cgrNode sig;
    sig=node;
    while(sig){
        self->signal(self,sig);
        sig=sig->next;
    }
    return 0;
}

//
//module
//
static int fsmdModule(fsmdHandle self,cgrNode node){
    cgrNode dec,proc; 
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    self->sigdec(self,dec);
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        self->proc(self,proc);
        proc=proc->next;
    }
    return 0;
}

//
//dummy
//
int fsmdDummy(fsmdHandle self,cgrNode node){
    return 0;
}

//
//to run
//
static int fsmdRun(fsmdHandle self){
    cgrNode mo;
    mo=self->top;
    while(mo){
        switch(mo->type){
        case Sc_module :
            self->module(self,mo);
            break;
        }
        mo=mo->next;
    }
    fsmdRouteClear();
    return 0;
}

//
//distruction
//
void fsmdDes(fsmdHandle fsmd){
    if(fsmd) free(fsmd);
}

//
//initialization
//
fsmdHandle fsmdIni(cgrNode top){
    fsmdHandle fsmd;
    cstrMakeBuf(fsmd,1,struct _fsmdHandle,
        memError("creating FSMD handler"));
    fsmd->top=top;    
    fsmd->work=NULL;    
    fsmd->run=(*fsmdRun);    
    fsmd->module=(*fsmdModule);    
    fsmd->inst=(*fsmdDummy);    
    fsmd->sigdec=(*fsmdSigDec);    
    fsmd->signal=(*fsmdDummy);    
    fsmd->proc=(*fsmdProc);    
    fsmd->state=(*fsmdState);    
    fsmd->dfg=(*fsmdDummy);    
    fsmd->ctrl=(*fsmdDummy);    
    fsmd->ope=(*fsmdOpe);    
    fsmd->id=(*fsmdDummy);    
    fsmd->sen=(*fsmdDummy);    
    fsmd->pos=(*fsmdDummy);    
    fsmd->neg=(*fsmdDummy);    
    fsmd->funcall=(*fsmdDummy);    
    return fsmd;
}

/*------------------------------------------------------------------
  unique name cleation
------------------------------------------------------------------*/
//name map
static cMap __fsmd_name_map=NULL;

//clear
void fsmdNameClear(void){
    if(__fsmd_name_map) cmapDes(__fsmd_name_map);
    __fsmd_name_map=NULL;
}
//reset
cMap fsmdNameReset(void){
    if(__fsmd_name_map) cmapDes(__fsmd_name_map);
    __fsmd_name_map=cmapIni(sizeof(cgrNode),
        sizeof(uchar),(*cmapStrKey),(*cmapStrCmp));
    return __fsmd_name_map;
}
//setting node
void fsmdNameSet(char*name){
    uchar t=1;
    cmapUpdate(__fsmd_name_map,&name,&t);
}
//new name cleation
#define __MAX_NAME_NUMBER 1000000
cgrNode fsmdNameCreate(char*prefix,char*file,
    int line,int size,int type,int signal,char*opt){
    int i;
    char*str;
    static char buf[SIS_BUF_SIZE];
    static char pbuf[SIS_BUF_SIZE/2];
    cgrNode def=cgrNew(Declare,file,line);
    cgrSetNum(def,cgrKeySize,size);
    cgrSetNum(def,cgrKeyType,type);
    cgrSetNum(def,cgrKeySignal,signal);
    cgrSetStr(def,cgrKeyOpt,opt);
    strncpy(pbuf,prefix,SIS_BUF_SIZE/2);
    while(1){
        for(i=0;i<__MAX_NAME_NUMBER;i++){
            sprintf(buf,"%s%.6d",pbuf,i);
            str=buf;
            if(!cmapFind(__fsmd_name_map,&str)){
                cgrSetStr(def,cgrKeyId,str);
                fsmdNameSet(cgrGetStr(def,cgrKeyId));
                return def;
            }
        }
        sprintf(pbuf,"_%s",pbuf);
    }
}
/*------------------------------------------------------------------
  checking FSMD search route
------------------------------------------------------------------*/
//search route checking map
static cMap __fsmd_route_check=NULL;
//search route stack
static cVector __fsmd_route_stack=NULL;


//to push to stack and new route creation
cMap fsmdRoutePush(void){
    if(!__fsmd_route_stack)
        __fsmd_route_stack=cvectIni(sizeof(cMap),8);
    cvectAppend(__fsmd_route_stack,__fsmd_route_check);
    __fsmd_route_check=NULL;
    return fsmdRouteReset();
}
//to pop from stack 
cMap fsmdRoutePop(void){
    cmapDes(__fsmd_route_check);
    __fsmd_route_check=
        cioVoid(cvectPop(__fsmd_route_stack),cMap);
    if(!cvectSize(__fsmd_route_stack)){
        cvectDes(__fsmd_route_stack);
        __fsmd_route_stack=NULL;
    }
    return __fsmd_route_check;
}

//clear
void fsmdRouteClear(void){
    if(__fsmd_route_check) cmapDes(__fsmd_route_check);
    __fsmd_route_check=NULL;
}
//key
static int __fsmdSearchKey(void* a){
    int* node=(int*)a;
    return *node;
}
//compare
static int __fsmdSearchCmp(void* a,void* b){
    cgrNode node0=cioVoid(a,cgrNode);
    cgrNode node1=cioVoid(b,cgrNode);
    return node0==node1;
}
//reset
cMap fsmdRouteReset(void){
    if(__fsmd_route_check) cmapDes(__fsmd_route_check);
    __fsmd_route_check=cmapIni(sizeof(cgrNode),
        sizeof(uchar),(*__fsmdSearchKey),(*__fsmdSearchCmp));
    return __fsmd_route_check;
}
//setting node
void fsmdRouteSet(cgrNode node){
    uchar t=1;
    cmapUpdate(__fsmd_route_check,&node,&t);
}
//checking node
int fsmdRouteCheck(cgrNode node){
    if(cmapFind(__fsmd_route_check,&node)) return 1;
    return 0;
}
/*------------------------------------------------------------------
  setting back node
------------------------------------------------------------------*/
void fsmdSetJump(cgrNode node0,cgrNode node1){
    cgrNode back,sback;
    if(!node0) return ;
    node0->jump=node1;
    if(!node1) return ;
    sback=cgrGetNode(node1,cgrKeyBack);
    while(sback){
        if(sback->jump==node0) return;
        sback=sback->next;
    }
    back=cgrNew(Back,node0->file,node0->line);
    back->jump=node0;
    cgrAddNode(node1,cgrKeyBack,back);
}

/*------------------------------------------------------------------
  removing jump node
------------------------------------------------------------------*/
int fsmdRemoveJump(cgrNode node0,cgrNode node1){
    cgrNode back,prev,next;
    back=cgrGetNode(node0,cgrKeyBack);
    while(back){
        next=back->next;
        prev=back->prev;
        if(back->jump==node1){
            if(prev) cgrSetContext(prev,next);
            else{
                if(next) next->prev=NULL;
                cgrSetNode(node0,cgrKeyBack,next);
            }
        }
        back=next;
    }
    return 0;
}
/*------------------------------------------------------------------
  removing jump node
------------------------------------------------------------------*/
int fsmdRemoveNode(cgrNode node){
    cgrNode jump,back,prev,next;
    if(!node) return 0;
    prev=node->prev;
    next=node->next;
    jump=node->jump;
    cgrSetContext(prev,next);
    back=cgrGetNode(node,cgrKeyBack);
    while(back){
        prev=back->jump;
        fsmdSetJump(prev,jump);
        fsmdRemoveJump(prev,node);
        back=back->next;
    }
    fsmdRemoveJump(jump,node);
    return 0;
}
/*------------------------------------------------------------------
  removing state node
------------------------------------------------------------------*/
int fsmdRemoveState(cgrNode proc,cgrNode node){
    cgrNode jump=node->jump;
    if(!node) return 0;
    if(cgrGetNode(proc,cgrKeyVal)==node)
        cgrSetNode(proc,cgrKeyVal,jump);
    fsmdRemoveNode(node);   
    return 0;
}

/*------------------------------------------------------------------
  copying operator node
------------------------------------------------------------------*/
//
//copying one node
//
#define fsmdCopyOpeAttr(cpn,node,key) {  \
        cgrNode val=cgrGetNode(node,key);\
        val=fsmdCopyOpe(val);            \
        cgrSetNode(cpn,key,val);         }
cgrNode fsmdCopyOpeOne(cgrNode node){
    cgrNode cpn;
    if(!node) return NULL;
    cpn=cgrNew(node->type,node->file,node->line);
    if(node->type==Num) fsmdCopyAttr(Num,cpn,node,cgrKeyVal);
    else if(node->type==Real)fsmdCopyAttr(Real,cpn,node,cgrKeyVal);
    else if(node->type==String)fsmdCopyAttr(Str,cpn,node,cgrKeyVal);
    else if(node->type==Id){
        fsmdCopyAttr(Str,cpn,node,cgrKeyVal);
        fsmdCopyAttr(Num,cpn,node,cgrKeyVarType);
    }else if(node->type==Data_type){
        fsmdCopyOpeAttr(cpn,node,cgrKeyVal);
        fsmdCopyOpeAttr(cpn,node,cgrKeyPointer);
        fsmdCopyAttr(Num,cpn,node,cgrKeySize);
        fsmdCopyAttr(Node,cpn,node,cgrKeyOpt);
        fsmdCopyAttr(Node,cpn,node,cgrKeyType);
    }
    fsmdCopyAttr(Node,cpn,node,cgrKeySignal);
    return cpn;
}

//
//copying operator node tree
//
cgrNode fsmdCopyOpe(cgrNode node){
    cgrNode args,cpn,left,right,cond,next;
    if(!node) return NULL;
    cpn=fsmdCopyOpeOne(node);
    cond=fsmdCopyOpe(cgrGetNode(node,cgrKeyCond));
    cgrSetNode(cpn,cgrKeyCond,cond);
    cgrSetNode(cond,cgrKeyPar,cpn);
    left=fsmdCopyOpe(cgrGetNode(node,cgrKeyLeft));
    cgrSetNode(cpn,cgrKeyLeft,left);
    cgrSetNode(left,cgrKeyPar,cpn);
    right=fsmdCopyOpe(cgrGetNode(node,cgrKeyRight));
    cgrSetNode(cpn,cgrKeyRight,right);
    cgrSetNode(right,cgrKeyPar,cpn);
    next=fsmdCopyOpe(node->next);
    cgrSetContext(cpn,next);
    args=fsmdCopyOpe(cgrGetNode(node,cgrKeyArgs));
    cgrSetNode(cpn,cgrKeyArgs,args);
    fsmdCopyAttr(Str,cpn,node,cgrKeyId);
    return cpn;
}


