/*
  scheduling .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "generic.h"
//
//scheduling header accessing
//
#define schProp(self) ((schHandle*)(self->work))
#define schPropItem(self,item) (((schHandle*)(self->work))->item)

/*------------------------------------------------------------------

    asap

------------------------------------------------------------------*/
//
//adder for operator
//
static schHandle schAddOpe(schHandle a,schHandle b){
    schHandle sch;
    sch.mul=a.mul+b.mul;
    sch.div=a.div+b.div;
    sch.adsb=a.adsb+b.adsb;
    return sch;
}

//
//to count number of operators
//
static schHandle schCountOpe(cgrNode node){
    schHandle fsch={0};
    cgrNode left,right,cond,next,args;
    if(!node) return fsch;
    schSeqNext(node);
    if(left && right) switch(node->type){
        case '+' :fsch.adsb++;break;
        case '-' :fsch.adsb++;break;
        case '*' :fsch.mul++;break;
        case '/' :fsch.div++;break;
        case '%' :fsch.div++;break;
        case Adeq :fsch.adsb++;break;
        case Sueq :fsch.adsb++;break;
        case Mueq :fsch.mul++;break;
        case Dieq :fsch.div++;break;
        case Moeq :fsch.div++;break;
    }
    if(left) fsch=schAddOpe(fsch,schCountOpe(left));
    if(right)fsch=schAddOpe(fsch,schCountOpe(right));
    if(cond) fsch=schAddOpe(fsch,schCountOpe(cond));
    if(next) fsch=schAddOpe(fsch,schCountOpe(next));
    return fsch;
}

//
//comparing operators
//
static int schCmpOpe (schHandle lim,schHandle sval){
    if(lim.div < sval.div) return 0;
    if(lim.mul < sval.mul) return 0;
    if(lim.adsb < sval.adsb) return 0;
    return 1;
}

//
//set node as list
//
static int schMapOpeSetList(cMap map,char*name,cgrNode node){
    cgrNode*pre;
    pre=cmapFind(map,&name);
    if(pre){
        if(*pre==node) return 0; 
        cgrSetNode(node,cgrKeySchVarListNext,*pre);
    }
    cmapUpdate(map,&name,&node);
    return 0;
}

//
//getting output variables
//flg : output flag
//
static int schMapOpe(cMap map,cgrNode node,char flg){
    int ret=1;
    cgrNode next;
    if(!node) return 0;

    //id  x, x[]..
    if(flg && (node->type==Id)){
        char*name=cgrGetStr(node,cgrKeyVal);
        schMapOpeSetList(map,name,node);
    
    //funcall x(),x[]()()
    }else if(flg && (node->type==Funcall)){
        cgrNode left;
        left=cgrGetNode(node,cgrKeyLeft);
        //x(...)
        if(!left){
            char*name=cgrGetStr(node,cgrKeyId); 
            schMapOpeSetList(map,name,node);

        //x[...](...)(...)
        }else{
            cgrNode lleft=cgrGetMaxLeft(node);
            char*name=cgrGetStr(lleft,cgrKeyVal);
            schMapOpeSetList(map,name,lleft);
        }
    }
    next=node->next;
    schSwitchFlg(node,flg,schMapOpe,ret);
    if(next) ret&=schMapOpe(map,next,0);
    return 0;
}
//
//checking dependent with patterns B and B
// x(...)=  ...
// ... = x(...) ...
//
static int schCheckDepCC(cgrNode onode,cgrNode inode){
    cgrNode om,on;  //output max, input min
    cgrNode im,in;  //input  max, input min
    int omi,oni;    //output max, input min
    int imi,ini;    //input  max, input min

    //get output max
    om=cgrGetNode(onode,cgrKeyArgs);
    if(!om) return -1;
    if(om->type!=Num) return 1;
    omi=cgrGetNum(om,cgrKeyVal);
    
    //get output min
    on=om->next;
    if(!on) return -1;
    if(on->type!=Num) return 1;
    oni=cgrGetNum(on,cgrKeyVal);
    
    //get input max
    im=cgrGetNode(inode,cgrKeyArgs);
    if(!im) return -1;
    if(im->type!=Num) return 1;
    imi=cgrGetNum(im,cgrKeyVal);
    
    //get input min
    in=im->next;
    if(!in) return -1;
    if(in->type!=Num) return 1;
    ini=cgrGetNum(in,cgrKeyVal);
    
    //check
    if(ini>omi) return 0;
    if(imi<oni) return 0;

    return 1;
}
//
//checking dependent with patterns C and C
// x[...]=  ...
// ... = x[...] ...
//
static int schCheckDepBB(cgrNode onode,cgrNode inode){
    cgrNode oa;//output array node
    cgrNode ia;//output array node

    oa=cgrGetNode(onode,cgrKeyPar);
    ia=cgrGetNode(inode,cgrKeyPar);

    while(oa && ia){
        cgrNode or; //output right node
        cgrNode ir; //input right node
        int oi; //output array size
        int ii; //input array size

        if((ia->type!=Array) && (oa->type!=Array)) return 1;
        if((oa->type!=Array) && (ia->type==Array)) return -1;
        if((ia->type!=Array) && (oa->type==Array)) return -1;
        
        //get output array size
        or=cgrGetNode(oa,cgrKeyRight);
        if(!or) return -1;
        if(or->type!=Num) return 1;
        oi=cgrGetNum(or,cgrKeyVal);

        //get output array size
        ir=cgrGetNode(ia,cgrKeyRight);
        if(!ir) return -1;
        if(ir->type!=Num) return 1;
        ii=cgrGetNum(ir,cgrKeyVal);
        
        //check
        if(ii!=oi) return 0;
        oa=cgrGetNode(oa,cgrKeyPar);
        ia=cgrGetNode(ia,cgrKeyPar);
    }
    return 1;
}
//
//checking dependent with patterns C and D
// x[...]=  ...
// ... = x[...](...) ...
//
static int schCheckDepBE(cgrNode onode,cgrNode inode){
     cgrNode left=cgrGetMaxLeft(inode);
     return schCheckDepBB(onode,inode);
}
//
//checking dependent with patterns D and D
// x[...](...)=  ...
// ... = x[...](...) ...
//
static int schCheckDepEE(cgrNode onode,cgrNode inode){
    cgrNode il=cgrGetMaxLeft(inode);
    cgrNode ol=cgrGetMaxLeft(onode);
    int flg=schCheckDepBB(ol,il);
    if(!flg) return 0;

    ol=cgrGetNode(inode,cgrKeyPar);
    il=cgrGetNode(onode,cgrKeyPar);
    while(ol&&il){
        if((ol->type==Funcall)&&(il->type==Funcall)) break;
        if(ol->type!=Array) return -1;
        if(il->type!=Array) return -1;
        ol=cgrGetNode(ol,cgrKeyPar);
        il=cgrGetNode(il,cgrKeyPar);
    }
    flg=schCheckDepCC(ol,il);
    return flg;
}

//
//checking dependent with patterns
//0:not competi, 1:compete
//
static int schCheckDep(cgrNode out,cgrNode in){
    int inp;  //input var pattern
    int outp; //output var pattern

    inp=ptnutilMatch(in);
    outp=ptnutilMatch(out);
    switch(outp){
    // x=
    case ptnutilType_A :{
        switch(inp){
        case ptnutilType_A: return 1;
        case ptnutilType_C: return 1;
        case ptnutilType_B: return 1;
        case ptnutilType_E: return 1;
        default : return -1;
        }
    }    
    // x(...)=
    case ptnutilType_C : {
        switch(inp){
        case ptnutilType_A: return 1;
        case ptnutilType_C: return schCheckDepCC(out,in); 
        default : return -1;
        }
    }
    // x[...]=
    case ptnutilType_B : {
        switch(inp){
        case ptnutilType_A: return 1;
        case ptnutilType_B: return schCheckDepBB(out,in);
        case ptnutilType_E: return schCheckDepBE(out,in); 
        default : return -1;
        }
    }
    // x[...](...)=
    case ptnutilType_E : {
        switch(inp){
        case ptnutilType_A: return 1;
        case ptnutilType_B: return schCheckDepBE(in,out);
        case ptnutilType_E: return schCheckDepEE(out,in);
        default : return -1;
        }
    }
    //error
    default : return -1;
    }
}

//
//start checking dependent 
//0:not competi, 1:competi
//
static int schCheckDepStart(cMap map,cgrNode node,int key){
    char*name=cgrGetStr(node,key);
    cgrNode*out=cmapFind(map,&name);
    if(out){
        cgrNode ovar=*out;
        while(ovar){
            int flg=schCheckDep(ovar,node);
            if(flg<0){
                cmsgEcho(__sis_msg,52,node->file,node->line,name);
                return -1;
            }
            else if(flg==1)  return 1;
            ovar=cgrGetNode(ovar,cgrKeySchVarListNext);
        }
    }
    return 0;
}

//
//checking dependenct variables from operator
//
static int schCheckVarOpe(cMap map,cgrNode node,char flg){
    int ret=1;
    cgrNode next;

    if(!node) return 0;
    
    //id  x, x[]..
    if(node->type==Id){
        if(schCheckDepStart(map,node,cgrKeyVal)) return 0;
    //funcall x(),x[]()()
    }else if(node->type==Funcall){
        cgrNode left;
        left=cgrGetNode(node,cgrKeyLeft);
        //x(...)
        if(!left){
            if(schCheckDepStart(map,node,cgrKeyId)) return 0;
        //x[...](...)(...)
        }else{
            left=cgrGetMaxLeft(node);
            if(schCheckDepStart(map,left,cgrKeyVal)) return 0;
        }
    }
    next=node->next;
    schSwitchFlg(node,flg,schCheckVarOpe,ret);
    if(next) ret&=schCheckVarOpe(map,next,0);
    return ret;
}

//
//checking dependent variables
//
static int schCheckVar(cMap map,cgrNode node){
    int flg=0,ret=1;
    if(!node) return 0;
    schSwitchFlg(node,flg,schCheckVarOpe,ret);
    return ret;
}

//
//checking operator depending
//
#define schMoveOpeFlg(node,function) {              \
    cMap map=cmapIni(sizeof(char*),                 \
        sizeof(cgrNode),(*cmapStrKey),(*cmapStrCmp)); \
    schMapOpe(map,node,0);                          \
    function;                                       \
    cmapDes(map);                                   }
static int schMoveOpeDep(
    fsmdHandle self,cgrNode node,cgrNode front){
    int flg;
    cgrNode ope;
    if(!node) return 1;
    if(!front) return 1;
    schMoveOpeFlg(node,
        ope=cgrGetNode(front,cgrKeyVal);
        for(flg=1;ope&&flg;ope=ope->next)
        flg&=schCheckVar(map,ope));
    return flg;
}

//
//checking depending
//
#define schMoveOpeTmp(node,function) {  \
    cgrNode _prev,_next;                \
    _prev=node->prev;                   \
    _next=node->next;                   \
    node->prev=NULL;                    \
    node->next=NULL;                    \
    function;                           \
    node->prev=_prev;                   \
    node->next=_next;                   }
static int schDepend(
    fsmdHandle self,cgrNode node,cgrNode front){
    int flg;
    cgrNode cope,fope;
    schHandle ssch,csch,msch,fsch;
    if(!node) return 0;
    if(!front) return 0;
    if(cgrGetNum(front,cgrKeyWait)) return 0;
    schMoveOpeTmp(node,msch=schCountOpe(node));
    fope=cgrGetNode(front,cgrKeyVal);
    cope=cgrGetNode(front,cgrKeyCond);
    fsch=schCountOpe(fope);
    csch=schCountOpe(cope);
    ssch=schAddOpe(csch,fsch);
    flg=schCmpOpe(*schProp(self),schAddOpe(msch,ssch));
    if(!flg) return 0;
    schMoveOpeFlg(
        cgrGetNode(front,cgrKeyVal),
        flg=schCheckVar(map,node));
    if(!flg) return 0;
    schMoveOpeTmp(node,flg=schMoveOpeDep(self,node,front));
    if(flg) return 2;
    return 1;
}

//
//moving operator node
//
#define schCheckFront(self,node,front,back){      \
    cgrNode proc=schPropItem(self,proc);          \
    if(cgrGetNode(proc,cgrKeyVal)==node) return 0;\
    back=cgrGetNode(node,cgrKeyBack);             \
    if(!back) return 0;                           \
    if(back->next) return 0;                      \
    front=back->jump;                             \
    if(front==node) return 0;                     }

static int schMoveOpe(fsmdHandle self,cgrNode node){
    cMap map;
    char flg;
    cgrNode dfg,back,front;
    if(!node) return 0;
    dfg=schPropItem(self,dfg);
    if(cgrGetNum(dfg,cgrKeyWait)) return 0;
    if(dfg->type!=Dfg) return 0;
    schCheckFront(self,dfg,front,back);
    flg=schDepend(self,node,front);
    if(!flg) return 0;
    cgrRemoveNode(dfg,cgrKeyVal,node);
    if(!cgrGetNode(dfg,cgrKeyVal) &&
       !cgrGetNum(dfg,cgrKeyWait))
       fsmdRemoveNode(dfg);
    cgrAddNode(front,cgrKeyVal,node);
    schPropItem(self,dfg)=front;
    if(flg==2) schMoveOpe(self,node);
    return 0;
}

//
//operation
//
static int schOpe(fsmdHandle self,cgrNode node){
    cgrNode back;
    if(!node) return 0;
    schMoveOpe(self,node);
    return 0;
}

//
//DFG
//
static int schDfg(fsmdHandle self,cgrNode node){
    cgrNode ope,next,jump;
    if(!node) return 0;
    jump=node->jump;
    ope=cgrGetNode(node,cgrKeyVal);
    while(ope){
        next=ope->next;
        schPropItem(self,dfg)=node;
        self->ope(self,ope);
        ope=next;
    }
    return 0;
}

//
//schCtrl Pattern A
//if...else{ if()... } -> if...else if...
//
static int schCtrlA(fsmdHandle self,cgrNode node,cgrNode front){
    cgrNode prev;
    if(!node) return 0;
    if(!front) return 0;
    prev=front->prev;
    if(!prev) return 0;
    if(front->next) return 0;
    cgrSetContext(prev,node);
    cgrSetNode(node,cgrKeyBack,NULL);
    front->jump=NULL;
    front->prev=NULL;
    return 0;
}

//
//schCtrl Pattern B
//if...else if{ if()... }-> if...else if...
//
static int schCtrlB(fsmdHandle self,cgrNode node,cgrNode front){
    cgrNode head,inode,prev,next,fcond,ncond;
    if(!node) return 0;
    if(!front) return 0;
    next=front->next;
    if(!next) return 0;
    fcond=cgrGetNode(front,cgrKeyCond);
    if(!fcond) return 0;
    inode=node;
    while(inode){
        cgrNode cond;
        cgrNode lcond=fsmdCopyOpe(fcond);
        ncond=cgrGetNode(inode,cgrKeyCond);
        if(ncond){
            cond=cgrCreateOpe(Anan,lcond,ncond,NULL);
        }else cond=lcond;
        cgrSetNode(inode,cgrKeyCond,cond);
        inode=inode->next;
    }
    cgrSetContext(cgrGetTail(node),next);
    prev=front->prev;
    head=cgrGetHead(node);
    cgrSetNode(head,cgrKeyBack,NULL);
    if(prev) cgrSetContext(prev,head);
    else{
        cgrNode back=cgrGetNode(front,cgrKeyBack);
        setJumpChangeBack(head,back);
        cgrSetNode(front,cgrKeyBack,NULL);
    }
    front->jump=NULL;
    return 0;
}

//
//control
//
static int schCtrl(fsmdHandle self,cgrNode node){
    int flg=2;
    cgrNode ftop,ntop,nobj,back,front,fcond;
    if(!node) return 0;
    schCheckFront(self,node,front,back);
    if(front->type!=Ctrl) return 0;
    if(cgrGetNode(front,cgrKeyVal)) return 0;
    ntop=cgrGetHead(node);
    ftop=cgrGetHead(front);
    nobj=ntop;
    //checking dependent
    while(nobj){
        cgrNode ope=cgrGetNode(nobj,cgrKeyCond);
        if(cgrGetNode(nobj,cgrKeyVal)) return 0;
        while(ftop){
            flg&=schDepend(self,ope,ftop);
            ftop=ftop->next;
        }
        if(!flg) return 0;
        nobj=nobj->next;
    }
    fcond=cgrGetNode(front,cgrKeyCond);
    //if...else{ if()... } -> if...else if...
    if(!fcond) schCtrlA(self,ntop,front);
    //if...else if{ if()... }-> if...else if...
    else schCtrlB(self,ntop,front);
    return 0;
}

//
//process
//
static int schProc(fsmdHandle self,cgrNode node){
    cgrNode state,rst;
    if(!node) return 0;
    schPropItem(self,proc)=node;
    state=cgrGetNode(node,cgrKeyVal);
    fsmdRouteReset();
    self->state(self,state);
    rst=cgrGetNode(node,cgrKeyResetProc);
    if(rst) self->state(self,rst);
    return 0;
}

//
//ASAP
//
static int asap(schHandle sch,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&sch;
    fsmd->dfg=(*schDfg);
    fsmd->ope=(*schOpe);
    fsmd->ctrl=(*schCtrl);
    fsmd->proc=(*schProc);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}

/*------------------------------------------------------------------

    pipeline initialization

------------------------------------------------------------------*/
//
//dummy dfg node creation
//
static cgrNode pipIniDummyNewDfg(fsmdHandle self,cgrNode node){
    cgrNode eq,dfg,left,right;
    if(!node) return NULL;
    left=fsmdCopyOpe(node);
    right=cgrNew(Id,node->file,node->line);
    cgrSetStr(right,cgrKeyVal,"__SIS_DUMMY__");
    eq=cgrCreateOpe('=',left,right,NULL);
    cgrSetNum(eq,cgrKeyPipeDummy,1);
    dfg=cgrNew(Dfg,node->file,node->line);
    cgrSetNode(dfg,cgrKeyVal,eq);
    return dfg;
}

//
//dummy dfg
//
#define pipIniDummyAddDfg(dfg,prev) {         \
    cgrNode back=cgrGetNode(prev,cgrKeyBack); \
    setJumpChangeBack(dfg,back);              \
    cgrSetNode(prev,cgrKeyBack,NULL);         \
    fsmdSetJump(dfg,prev);                    }
static int pipIniDummyDfg(
    fsmdHandle self,cgrNode dfg,cgrNode node){
    cgrNode prev=dfg;
    int i,pipnum=schPropItem(self,pipe);
    if(!dfg) return 0;
    if(!node) return 0;
    for(i=0;i<pipnum;i++){
        cgrNode dum=pipIniDummyNewDfg(self,node);
        cgrNode ope1=cgrGetNode(dum,cgrKeyVal);
        cgrNode ope2=cgrGetNode(prev,cgrKeyVal);
        pipIniDummyAddDfg(dum,prev);
        cgrSetNode(dum,cgrKeyVal,ope2);
        cgrSetNode(prev,cgrKeyVal,ope1);
        prev=dum;
    }
    return 0;    
}
//
//temporary variable creation
//
static cgrNode pipIniTempVar(fsmdHandle self,cgrNode node){
    int size=cgrBitWidth(node);
    cgrNode mo=schPropItem(self,module);
    cgrNode dec=fsmdNameCreate("__tmp_",
        node->file,node->line,size,Sc_int,Sc_signal,"");
    cgrNode var=cgrNew(Id,node->file,node->line);
    cgrSetStr(var,cgrKeyVal,cgrGetStr(dec,cgrKeyId));
    cgrAddNode(mo,cgrKeySignal,dec);
    return var;
} 
//
//dummy control
//
static int pipIniDummyCtrl(
    fsmdHandle self,cgrNode ctrl,cgrNode node){
    cgrNode dfg,var,prev;
    cgrNode par=cgrGetNode(node,cgrKeyPar);
    if(!par){
        var=pipIniTempVar(self,node);
        par=cgrCreateOpe('=',var,node,NULL);
    }else var=cgrGetNode(par,cgrKeyLeft);
    var=fsmdCopyOpe(var);
    cgrSetNode(ctrl,cgrKeyCond,var);
    dfg=cgrNew(Dfg,node->file,node->line);
    cgrSetNode(dfg,cgrKeyVal,par);
    prev=cgrGetHead(ctrl);
    pipIniDummyAddDfg(dfg,prev);
    self->dfg(self,dfg);
    return 0;
}

//
//dummy operation
//
static int pipIniDummy(fsmdHandle self,cgrNode node){
    cgrNode dfg=schPropItem(self,dfg);
    cgrNode par,left,prev;
    if(!node) return 0;
    if(fsmdRouteCheck(node)) return 0;
    fsmdRouteSet(node);
    if(dfg->type==Ctrl)
        return pipIniDummyCtrl(self,dfg,node);
    par=cgrGetNode(node,cgrKeyPar);
    if(!par) return 0;
    if(par->type!='=') return 0;
    left=cgrGetNode(par,cgrKeyLeft);
    if(dfg->type==Dfg) pipIniDummyDfg(self,dfg,left);
    return 0;
}

//
//  operation
//
static int pipIniOpe(fsmdHandle self,cgrNode node){
    int ret=0;
    cgrNode ope;
    if(!node) return ret;
    ope=node;
    while(ope){
        switch(ope->type){
        case Id :ret=self->id(self,ope);break;
        case '*' :
            pipIniDummy(self,ope);
        default :
            ret=self->ope(self,cgrGetNode(ope,cgrKeyCond));
            ret=self->ope(self,cgrGetNode(ope,cgrKeyLeft));
            ret=self->ope(self,cgrGetNode(ope,cgrKeyRight));
            break;
        }
        ope=ope->next;
    }
    return ret;
}

//
//signal declaration
//
static int pipIniSigDec(fsmdHandle self,cgrNode node){
    cgrNode dec=node;
    fsmdNameReset();
    while(dec){
        char*name=cgrGetStr(dec,cgrKeyId);
        fsmdNameSet(name);
        dec=dec->next;
    }
    return 0;
}

//
//control node
//
static int pipIniCtrl(fsmdHandle self,cgrNode node){
    if(!node) return 0;
    schPropItem(self,dfg)=node;
    self->ope(self,cgrGetNode(node,cgrKeyCond));
    self->ope(self,cgrGetNode(node,cgrKeyVal));
    return 0;
}

//
//module
//
static int pipIniModule(fsmdHandle self,cgrNode node){
    cgrNode dec,proc; 
    if(!node) return 0;
    dec=cgrGetNode(node,cgrKeySignal);
    self->sigdec(self,dec);
    proc=cgrGetNode(node,cgrKeyProcess);
    schPropItem(self,module)=node;
    while(proc){
        self->proc(self,proc);
        proc=proc->next;
    }
    fsmdNameClear();
    return 0;
}

//
//pipeline scheduling initialazation
//
static int pipIni(schHandle sch,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&sch;
    fsmd->dfg=(*schDfg);
    fsmd->ope=(*pipIniOpe);
    fsmd->ctrl=(*pipIniCtrl);
    fsmd->module=(*pipIniModule);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}

/*------------------------------------------------------------------

    pipeline dummy node clearning

------------------------------------------------------------------*/
//
//  operation
//
static int pipFinOpe(fsmdHandle self,cgrNode node){
    cgrNode ope,next;
    cgrNode dfg=schPropItem(self,dfg);
    if(!node) return 0;
    for(ope=node;ope;ope=next){
        next=ope->next;
        if(ope->type!='=') continue;
        if(!cgrGetNum(ope,cgrKeyPipeDummy)) continue;
        cgrRemoveNode(dfg,cgrKeyVal,ope);
    }
    return 0;
}
//
//pipeline dummy node clearning
//
static int pipFin(schHandle sch,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&sch;
    fsmd->dfg=(*schDfg);
    fsmd->ope=(*pipFinOpe);
    fsmd->ctrl=(*pipIniCtrl);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}
/*------------------------------------------------------------------

    cleaning wait node

------------------------------------------------------------------*/
//
//dfg node
//
static int cleWaitDfg(fsmdHandle self,cgrNode node){
    int num;
    cgrNode jump;
    if(!node) return 0;
    num=cgrGetNum(node,cgrKeyWait);
    if((!num)||(num==2)) return 0;
    jump=node->jump;
    if(cgrGetNum(jump,cgrKeyWait))
        cgrSetNum(node,cgrKeyWait,2);
    else{
        cgrNode proc=schPropItem(self,proc);
        cgrNode back=cgrGetNode(node,cgrKeyBack);
        cgrMaps(back,
            if(cgrGetNum(back->jump,cgrKeyWait))
            cgrSetNum(back->jump,cgrKeyWait,2));
        fsmdRemoveState(proc,node);    
    }
    return 0;
}

//
//cleaning wait node
//
static int cleWait(schHandle sch,cgrNode top){
    fsmdHandle fsmd;
    fsmd=fsmdIni(top);
    fsmd->work=&sch;
    fsmd->dfg=(*cleWaitDfg);
    fsmd->proc=(*schProc);
    fsmd->run(fsmd);
    fsmdDes(fsmd);
    return 0;
}

/*------------------------------------------------------------------

    scheduling main

------------------------------------------------------------------*/
//
//scheduling
//
int schedule(schHandle sch,cgrNode top){
    pipIni(sch,top);
    asap(sch,top);
    pipFin(sch,top);
    cleWait(sch,top);
    if(cmsgGetErr(__sis_msg)) return -1;
    return 0;
}

//
//default scheduling option
//
schHandle schOptDefault(void){
    schHandle sch;
    sch.mul=1;
    sch.div=1;
    sch.adsb=12;
    sch.pipe=1;
    return sch;
}

