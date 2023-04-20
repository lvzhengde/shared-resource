/*
  control graph searcher.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"
/*------------------------------------------------------------------
  Root  
------------------------------------------------------------------*/
static int gsRoot(struct _gsHandle* self,cgrNode node){
    cgrNode dec;
    dec=node;
    while(dec){
        switch(dec->type){
        case Sc_module :
           cmstkPush(self->mstack);
           self->module(self,dec);
           cmstkPop(self->mstack);
           break;
        case Function:
           cmstkPush(self->mstack);
           self->function(self,dec);
           cmstkPop(self->mstack);
           break;
        case Enum:
            self->dec_enum(self,dec);
           break;
        case Class :
        case Struct:
            self->dec_class(self,dec);
           break;
        }
        dec=dec->next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Module
------------------------------------------------------------------*/
static int gsModule(struct _gsHandle* self,cgrNode node){
    cgrNode moblock;
    if(!node) return 0;
    moblock=cgrGetNode(node,cgrKeyVal);
    while(moblock){
        switch(moblock->type){
        case Public :
        case Private :
            self->visible(self,moblock);
            break;
        case Sc_signal_key:  
            self->sig_dec(self,moblock);
            break;
        case Function :
            cmstkPush(self->mstack);
            self->function(self,moblock);
            cmstkPop(self->mstack);
            break;
        case Sc_ctor:
            cmstkPush(self->mstack);
            self->sc_ctor(self,moblock);
            cmstkPop(self->mstack);
            break;
        case Enum:
            self->dec_enum(self,moblock);
            break;
        case Class :
        case Struct:
            self->dec_class(self,moblock);
            break;
        }
        moblock=moblock->next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Module2
------------------------------------------------------------------*/
int gsModule2(gsHandle self,cgrNode node){
    cgrNode proc;
    if(!node) return 0;
    gsPush(self,node);
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        gsPush(self,proc);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        proc=proc->next;
    }
    gsPop(self);
    return 0;
}

/*------------------------------------------------------------------
  Module3
------------------------------------------------------------------*/
int gsModule3(gsHandle self,cgrNode node){
    cgrNode proc;
    cgrNode task;
    if(!node) return 0;
    proc=cgrGetNode(node,cgrKeyProcess);
    gsPush(self,node);
    while(proc){
        gsPush(self,proc);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        proc=proc->next;
    }
    task=cgrGetNode(node,cgrKeyTask);
    while(task){
        gsPush(self,task);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(task,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        task=task->next;
    }
    gsPop(self);
    return 0;
}
/*------------------------------------------------------------------
  Module4
------------------------------------------------------------------*/
int gsModule4(gsHandle self,cgrNode node){
    cgrNode proc;
    cgrNode task;
    cgrNode ctor;
    if(!node) return 0;
    //start process
    gsPush(self,node);
    
    //task
    task=cgrGetNode(node,cgrKeyTask);
    while(task){
        gsPush(self,task);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(task,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        task=task->next;
    }
    //process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        gsPush(self,proc);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        proc=proc->next;
    }
    //ctor
    ctor=cgrGetNode(node,cgrKeyCtor);
    if(ctor){
        gsPush(self,ctor);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(ctor,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
    }

    //end module
    gsPop(self);
    return 0;
}
/*------------------------------------------------------------------
  Module5
------------------------------------------------------------------*/
int gsModule5(gsHandle self,cgrNode node){
    cgrNode proc;
    cgrNode ctor;
    if(!node) return 0;
    //start process
    gsPush(self,node);
    
    //process
    proc=cgrGetNode(node,cgrKeyProcess);
    while(proc){
        gsPush(self,proc);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(proc,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
        proc=proc->next;
    }
    //ctor
    ctor=cgrGetNode(node,cgrKeyCtor);
    if(ctor){
        gsPush(self,ctor);
        cmstkPush(self->mstack);
        self->cfg(self,cgrGetNode(ctor,cgrKeyVal));
        gsPop(self);
        cmstkPop(self->mstack);
    }

    //end module
    gsPop(self);
    return 0;
}



/*------------------------------------------------------------------
  Visible
------------------------------------------------------------------*/
static int gsVisible(struct _gsHandle* self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
  Signal Declaration
------------------------------------------------------------------*/
static int gsSigDec(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->datatype(self,cgrGetNode(node,cgrKeySize));
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Function
------------------------------------------------------------------*/
static int gsFunction(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->argdec(self,cgrGetNode(node,cgrKeyArgs));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  SC_CTOR
------------------------------------------------------------------*/
static int gsScCtor(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Arguments declaration
------------------------------------------------------------------*/
static int gsArgDec(struct _gsHandle* self,cgrNode node){
    cgrNode argdec;
    argdec=node;
    while(argdec){
        switch(argdec->type){
        case Class  : 
        case Struct : 
            self->dec_class(self,argdec);
        default : self->datatype(self,argdec);
            break;
        }
        self->pointer(self,cgrGetNode(argdec,cgrKeyPointer));
        self->ope(self,cgrGetNode(argdec,cgrKeyVal));
        argdec=argdec->next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Data Type
------------------------------------------------------------------*/
static int gsDataType(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Pointer
------------------------------------------------------------------*/
static int gsPointer(struct _gsHandle* self,cgrNode node){
    return 0;
}

/*------------------------------------------------------------------
  Cfg body
------------------------------------------------------------------*/
//
//body2
//
static int gsCfgBody2(struct _gsHandle*self,cgrNode cfg){
    cmstkPush(self->mstack);
    switch(cfg->type){
    case While    : self->cfgwhile(self,cfg);   break;
    case Switch   : self->cfgswitch(self,cfg);  break;
    case Do       : self->cfgdo(self,cfg);      break;
    case For      : self->cfgfor(self,cfg);     break;
    case If       : self->cfgif(self,cfg);      break;
    case Else     : self->cfgelse(self,cfg);    break;
    case Class    : self->dec_class(self,cfg);  break;
    case Struct   : self->dec_class(self,cfg);  break;
    case Block    : self->cfgblock(self,cfg);   break;
    }
    cmstkPop(self->mstack);
    return 0;
}
//
//body1
//
int gsCfgBody(struct _gsHandle*self,cgrNode cfg){
    if(!cfg) return 0;
    switch(cfg->type){
    case Dfg      : self->dfg(self,cfg);        break;
    case Data_type: self->vardec(self,cfg);     break;
    case Break    : self->cfgbreak(self,cfg);   break;
    case Continue : self->cfgcontinue(self,cfg);break;
    case Return   : self->cfgreturn(self,cfg);  break;
    case Enum     : self->dec_enum(self,cfg);   break;
    case Typedef  : self->tdef(self,cfg);       break;
    case Tag      : self->tag(self,cfg);        break;
    default       : gsCfgBody2(self,cfg);       break;
    }
    return 0;
}
/*------------------------------------------------------------------
  Cfg
------------------------------------------------------------------*/
static int gsCfg(struct _gsHandle* self,cgrNode node){
    cgrNode cfg=node;
    while(cfg){
        cgrNode next=cfg->next;
        gsCfgBody(self,cfg);
        cfg=next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Cfg2
------------------------------------------------------------------*/
int gsCfg2(struct _gsHandle* self,cgrNode node){
    cgrNode cfg=node;
    while(cfg){
        cgrNode next=cfg->next;
        gsPush(self,cfg);
        gsCfgBody(self,cfg);
        gsPop(self);
        cfg=next;
    }
    return 0;
}

/*------------------------------------------------------------------
  Variable declaration
------------------------------------------------------------------*/
static int gsVarDec(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->datatype(self,node);
    return 0;
}
/*------------------------------------------------------------------
  Dfg
------------------------------------------------------------------*/
static int gsDfg(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Break
------------------------------------------------------------------*/
static int gsCfgBreak(struct _gsHandle* self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
  Continue
------------------------------------------------------------------*/
static int gsCfgContinue(struct _gsHandle* self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
  Return
------------------------------------------------------------------*/
static int gsCfgReturn(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  If
------------------------------------------------------------------*/
static int gsCfgIf(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Else
------------------------------------------------------------------*/
int gsCfgElse(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  For
------------------------------------------------------------------*/
static int gsCfgFor(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->cfg(self,cgrGetNode(node,cgrKeyIni));
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfg(self,cgrGetNode(node,cgrKeyStep));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Do-While
------------------------------------------------------------------*/
static int gsCfgDo(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  While
------------------------------------------------------------------*/
static int gsCfgWhile(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Switch
------------------------------------------------------------------*/
static int gsCfgSwitch(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->cfgcase(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Case
------------------------------------------------------------------*/
static int gsCfgCase(struct _gsHandle* self,cgrNode node){
    cgrNode state;
    if(!node) return 0;
    state=node;
    while(state){
        switch(state->type){
        case Case:
            gsPush(self,state);
            self->ope(self,cgrGetNode(state,cgrKeyCond));
            self->cfg(self,cgrGetNode(state,cgrKeyVal));
            gsPop(self);
            break;
        case Default:
            gsPush(self,state);
            self->cfg(self,cgrGetNode(state,cgrKeyVal));
            gsPop(self);
        
        }
        state=state->next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Block
------------------------------------------------------------------*/
static int gsCfgBlock(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->cfg(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
/*------------------------------------------------------------------
  Ope
------------------------------------------------------------------*/
//
//operator body
//
int gsOpeBody(struct _gsHandle* self,cgrNode ope){
    if(!ope) return 0;
    switch(ope->type){
    case Id :self->id(self,ope);break;
    case Num     : break;
    case Real    : break;
    case String  : break;
    case Funcall : self->funcall(self,ope); break;
    case Data_type : self->datatype(self,ope); break;
    case '=' : gsOpeLinkEq(ope); break;
    default  : gsOpeLink(ope); break;
    }
    return 0;
}
//
//operator
//
static int gsOpe(struct _gsHandle* self,cgrNode node){
    cgrNode ope;
    if(!node) return 0;
    ope=node;
    while(ope){
        gsOpeBody(self,ope);
        ope=ope->next;
    }
    return 0;
}
/*------------------------------------------------------------------
  Function Call
------------------------------------------------------------------*/
//
// base
//
static int gsFuncall(struct _gsHandle* self,cgrNode node){
    cgrNode left;
    if(!node) return 0;
    left=cgrGetNode(node,cgrKeyLeft);
    self->ope(self,left);
    self->args(self,cgrGetNode(node,cgrKeyArgs));
    return 0;
}
//
// funcall 2
//
int gsFuncall2(struct _gsHandle* self,cgrNode node){
    cgrNode task;
    if(!node) return 0;
    gsFuncall(self,node);
    task=gsCheckTask(self,node);
    if(task) self->cfg(self,cgrGetNode(task,cgrKeyVal));
    return 0;
}
//funcall 3
int gsFuncall3(struct _gsHandle* self,cgrNode node){
    cgrNode task;
    if(!node) return 0;
    gsFuncall(self,node);
    task=gsCheckTask(self,node);
    if(task){
        gsPush(self,task);
        self->cfg(self,cgrGetNode(task,cgrKeyVal));
        gsPop(self);
    }
    return 0;
}
/*------------------------------------------------------------------
  Arguments
------------------------------------------------------------------*/
static int gsArgs(struct _gsHandle* self,cgrNode node){
    if(!node) return 0;
    self->ope(self,node);
    return 0;
}
/*------------------------------------------------------------------
  to start analize
------------------------------------------------------------------*/
int gsRun(gsHandle gs){
    if(!gs) return -1;
    if(!(gs->root)) return -1;
    return gs->root(gs,gs->top);
}
/*------------------------------------------------------------------
  destroy
------------------------------------------------------------------*/
void gsDes(gsHandle gs){
    if(!gs) return;
    if(gs->stack)  cvectDes(gs->stack);
    if(gs->mstack) cmstkDes(gs->mstack);
    free(gs);
}
/*------------------------------------------------------------------
  initialization
------------------------------------------------------------------*/
gsHandle gsIni(cgrNode top){
    gsHandle gs;
    cstrMakeBuf(gs,1,struct _gsHandle,
        memError("initialization for searching parse tree"));
    gs->top=top;
    gs->stack=cvectIni(sizeof(cgrNode),40);
    gs->mstack=cmstkNew(sizeof(cgrNode));
    gs->root=(*gsRoot);
    gs->module=(*gsModule);
    gs->visible=(*gsVisible);
    gs->sig_dec=(*gsSigDec);
    gs->function=(*gsFunction);
    gs->sc_ctor=(*gsScCtor);
    gs->argdec=(*gsArgDec);
    gs->datatype=(*gsDataType);
    gs->pointer=(*gsPointer);
    gs->cfg=(*gsCfg);
    gs->vardec=(*gsVarDec);
    gs->dfg=(*gsDfg);
    gs->cfgbreak=(*gsCfgBreak);
    gs->cfgcontinue=(*gsCfgContinue);
    gs->cfgreturn=(*gsCfgReturn);
    gs->cfgif=(*gsCfgIf);
    gs->cfgelse=(*gsCfgElse);
    gs->cfgfor=(*gsCfgFor);
    gs->cfgdo=(*gsCfgDo);
    gs->cfgwhile=(*gsCfgWhile);
    gs->cfgswitch=(*gsCfgSwitch);
    gs->cfgcase=(*gsCfgCase);
    gs->cfgblock=(*gsCfgBlock);
    gs->ope=(*gsOpe);
    gs->funcall=(*gsFuncall);
    gs->args=(*gsArgs);
    gs->id=(*gsDummy);
    gs->dec_enum=(*gsDummy);
    gs->dec_class=(*gsDummy);
    gs->tdef=(*gsDummy);
    gs->tag=(*gsDummy);
    gs->work=NULL;
    return gs;
}
/*------------------------------------------------------------------
  to pop from the stack
------------------------------------------------------------------*/
cgrNode gsPop(gsHandle gs){
    cgrNode* node;
    if(!gs) return NULL;
    node=cvectPop(gs->stack);
    if(!node) return NULL;
    return cioVoid(node,cgrNode);
}
/*------------------------------------------------------------------
  to push to the stack
------------------------------------------------------------------*/
void gsPush(gsHandle gs,cgrNode node){
    if(!gs) return;
    cvectAppend(gs->stack,node);
}
/*------------------------------------------------------------------
  top of stack element
------------------------------------------------------------------*/
cgrNode gsTop(gsHandle gs){
    int posi;
    cgrNode* top;
    posi=cvectSize(gs->stack)-1;
    if(posi<0) return NULL;
    top=cvectFind(gs->stack,posi);
    if(!top) return NULL;
    else return *top;
}


/*------------------------------------------------------------------
  to peek the stack
------------------------------------------------------------------*/
//type==1
cgrNode gsPeek(gsHandle gs,int type){
    int i;
    cgrNode node;
    for(i=cvectSize(gs->stack)-1;i>=0;i--){
        node=cvectElt(gs->stack,i,cgrNode);
        if(node->type==type) return node;
    }
    return NULL;
}
//type==2
cgrNode gsPeek2(gsHandle gs,int type1,int type2){
    int i;
    cgrNode node;
    for(i=cvectSize(gs->stack)-1;i>=0;i--){
        node=cvectElt(gs->stack,i,cgrNode);
        if(node->type==type1) return node;
        else if(node->type==type2) return node;
    }
    return NULL;
}

/*------------------------------------------------------------------
  dummy function
------------------------------------------------------------------*/
int gsDummy(gsHandle self,cgrNode node){
    if(!node) return 0;
    return 0;
}


/*------------------------------------------------------------------
    checking task from funcall node
------------------------------------------------------------------*/
cgrNode gsCheckTask(gsHandle self,cgrNode node){
    char*name;
    char*tname;
    cgrNode mo,task;
    if(!node) return NULL;
    name=cgrGetIdName(node);
    if(!strcmp(name,"")) return NULL;
    mo=gsPeek(self,Sc_module);
    task=cgrGetNode(mo,cgrKeyTask);
    while(task){
        cgrNode next=task->next;
        //is function out of module
        if(task->type==Id) task=cgrGetNode(task,cgrKeyIdLinkTask);
        tname=cgrGetStr(task,cgrKeyId);
        if(!strcmp(name,tname)) return task;
        task=next;
    }
    return NULL;
}


/*------------------------------------------------------------------
    removing from stack if top is dfg node
------------------------------------------------------------------*/
void gsRemoveTopDfgFromStack(gsHandle self){
    cgrNode dfg=gsPop(self);
    cgrNode cfg=gsPop(self);
    if(dfg){
        if(dfg->type==Dfg) cgrRemoveNode(cfg,cgrKeyVal,dfg);
    }
    gsPush(self,cfg);
    gsPush(self,dfg);
}


/*------------------------------------------------------------------

    state counter

------------------------------------------------------------------*/
//
//cfg
//
static int gsCountStateCfg(gsHandle self,cgrNode node){
    int*buf=self->work;

    if(!node) return 0;
    switch(node->type){
    case Block : break;
    default : 
        (*buf)++;
        break;
    }
    gsCfg2(self,node);
    return 0;
}
//
//counter main
//
int gsCountState(cgrNode cfg){
    gsHandle gs=gsIni(cfg); //init
    int buf=0;
    
    gs->work=&buf;
    gs->cfg=(*gsCountStateCfg);
    gs->cfg(gs,cfg);
    gsDes(gs);

    return buf;
}

