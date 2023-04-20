/*
  optimization.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"

/*----------------------------------------------------------------------------

    removing dead declaration

----------------------------------------------------------------------------*/
//
//id
//
static int rdDecId(fsmdHandle self,cgrNode node){
    cgrNode sig;
    if(!node) return 0;
    sig=cgrGetNode(node,cgrKeySignal);
    if(!sig) return 0;
    cgrSetNum(sig,cgrKeyCheckVar,1);
    return 0;
}

//
//funcall
//
static int rdDecFuncall(fsmdHandle self,cgrNode node){
    cgrNode sig;
    if(!node) return 0;
    sig=cgrGetNode(node,cgrKeySignal);
    if(!sig) return 0;
    cgrSetNum(sig,cgrKeyCheckVar,1);
    return 0;
}

//
//dfg
//
static int rdDecDfg(fsmdHandle self,cgrNode node){
    cgrNode ope,next;
    if(!node) return 0;
    ope=cgrGetNode(node,cgrKeyVal);
    while(ope){
        next=ope->next;
        optProp(self,state)=node;
        self->ope(self,ope);
        ope=next;
    }
    return 0;
}
//
//control
//
static int rdDecCtrl(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    optProp(self,state)=node;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}

//
//sensitive
//
static int rdDecSen(fsmdHandle self,cgrNode node){
    cgrNode sen=node;
    while(sen){
        self->id(self,sen);
        sen=sen->next;
    }
    return 0;
}

//
//signal
//
static int rdDecSig(fsmdHandle self,cgrNode node){
    cgrNode mo=optProp(self,module);
    if(!node) return 0;
    if(!cgrGetNum(node,cgrKeyCheckVar) &&
        (cgrGetNum(node,cgrKeySignal)==Sc_signal))
        cgrRemoveNode(mo,cgrKeySignal,node);
    cgrSetNum(node,cgrKeyCheckVar,0);    
    return 0;
}
//
//instance
//
static int rdDecInst(fsmdHandle self,cgrNode node){
    cgrNode port;
    if(!node) return 0;
    port=cgrGetNode(node,cgrKeySignal);
    cgrMaps(port,
        cgrNode dec=cgrGetNode(port,cgrKeyCheckVar);
        cgrSetNum(dec,cgrKeyCheckVar,1));
    return 0;
}

//
//
//module
//
static int rdDecModule(fsmdHandle self,cgrNode node){
    cgrNode dec,proc,ctor,inst; 
    if(!node) return 0;

    //checking instance
    inst=cgrGetNode(node,cgrKeyInstance);
    cgrMaps(inst,self->inst(self,inst));
    dec=cgrGetNode(node,cgrKeySignal);

    //checking process
    proc=cgrGetNode(node,cgrKeyProcess);
    optProp(self,module)=node;
    cgrMaps(proc,
        optProp(self,proc)=proc;
        self->proc(self,proc));
    
    //checking signal declaration
    self->sigdec(self,dec);
    fsmdNameClear();
    return 0;
}

//
//removing dead declaration
//
static int rdDec(optHandle opt,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&opt;
    fsmd->id=(*rdDecId);
    fsmd->sen=(*rdDecSen);
    fsmd->pos=(*rdDecSen);
    fsmd->neg=(*rdDecSen);
    fsmd->dfg=(*rdDecDfg);
    fsmd->ctrl=(*rdDecCtrl);
    fsmd->inst=(*rdDecInst);
    fsmd->signal=(*rdDecSig);
    fsmd->module=(*rdDecModule);
    fsmd->funcall=(*rdDecFuncall);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}

/*----------------------------------------------------------------------------

    removing dead condition

----------------------------------------------------------------------------*/
//
//condition is false
//if(0)...
//
static int rdConFalse(fsmdHandle self,cgrNode node){
    cgrNode prev,next;
    if(!node) return 0;
    prev=node->prev;
    next=node->next;
    if(!next) return 0;
    if(prev){
        cgrSetContext(prev,next);
        return 0; 
    }else{
        cgrNode back=cgrGetNode(node,cgrKeyBack);
        setJumpChangeBack(next,back);
        next->prev=NULL;
    }
    if(!cgrGetNode(next,cgrKeyCond))
        fsmdRemoveState(optProp(self,proc),node);
    return 0;
}

//
//condition is true
//if(1) ...
//
static int rdConTrue(fsmdHandle self,cgrNode node){
    cgrNode prev;
    if(!node) return 0;
    node->next=NULL;
    cgrSetNode(node,cgrKeyCond,NULL);
    prev=node->prev;
    if(!prev)
        fsmdRemoveState(optProp(self,proc),node);
    return 0;
}

//
//contorol node
//
static int rdConCtrl(fsmdHandle self,cgrNode node){
    int num;
    cgrNode cnd;
    if(!node) return 0;
    if(node->type!=Ctrl) return 0;
    cnd=cgrGetNode(node,cgrKeyCond);
    if(!cnd) return 0;
    if(cnd->type!=Num) return 0;
    num=cgrGetNum(cnd,cgrKeyVal);
    if(num) rdConTrue(self,node);
    else rdConFalse(self,node);
    return 0;
}

//
//removing dead declaration
//
static int rdCon(optHandle opt,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&opt;
    fsmd->ctrl=(*rdConCtrl);
    fsmd->module=(*rdDecModule);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}

/*----------------------------------------------------------------------------

    optimization

----------------------------------------------------------------------------*/
//optimization main
int optimize(optHandle opt,cgrNode top){
    rdCon(opt,top);//removind dead condition
    rdDec(opt,top);//removing dead declaration
    if(cmsgGetErr(__sis_msg)) return -1;
    return 0;
}
//default optimization handler
optHandle optDefault(void){
    optHandle opt={0}; 

    return opt;
}

