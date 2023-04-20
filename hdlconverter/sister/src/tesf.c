/*
  fsmd test handler .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"

//
//node addition
//
static int tesfAdd(fsmdHandle self,cgrNode node){
    cVector vect=tesfProp(self,nvect);
    if(!node) return 0;
    cvectAdd(vect,node);
    return 0;
}

//
//ope
//
static int tesfOpe(fsmdHandle self,cgrNode node){
    int ret=0;
    cgrNode ope;
    if(!node) return ret;
    ope=node;
    while(ope){
        tesfProp(self,function)(self,ope);
        switch(ope->type){
        case Id :ret=self->id(self,ope);break;
        case Num     : break;
        case Real    : break;
        case String  : break;
        case '=' : fsmdOpeLinkEq(ope,ret); break;
        default  : fsmdOpeLink(ope,ret) break;
        }
        ope=ope->next;
    }
    return ret;
}
//
//dfg
//
static int tesfDfg(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    tesfProp(self,state)=node;
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}
//
//control
//
static int tesfCtrl(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    tesfProp(self,state)=node;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    return 0;
}
//
//to start fsmd test
//
cVector tesf(cgrNode top,void*work,
    int (*function)(fsmdHandle,cgrNode)){
    fsmdHandle fsmd=fsmdIni(top);
    tesfHandle tesfh;
    tesfh.work=work;
    tesfh.function=(*function);
    tesfh.nvect=cvectIni(sizeof(cgrNode),10);
    tesfh.state=top;
    fsmd->work=&tesf;
    fsmd->ope=(*tesfOpe);
    fsmd->dfg=(*tesfDfg);
    fsmd->ctrl=(*tesfCtrl);
    fsmdRoutePush();
    fsmd->state(fsmd,top);
    fsmdRoutePop();
    return tesfh.nvect;
}

