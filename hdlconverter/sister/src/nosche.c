/*
  to output without scheduling.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"
//
//state
//

//
//data flow
//
static int noscheDfg(fsmdHandle self,cgrNode node){
    FILE*fp=overiProp(self,fp);
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    if(!ope) return 0;
    osyscIndent(fp,overiProp(self,indent));
    overiOutOpe2(fp,ope,1);
    fprintf(fp,";\n");
    return 0;
}
//
//output block
//
static cgrNode noscheDfgBlock(fsmdHandle self,cgrNode node);
static cgrNode noscheOutBlock(fsmdHandle self,cgrNode node){
    cgrNode tjump;
    FILE*fp=overiProp(self,fp);
    fprintf(fp,"begin\n");
    overiProp(self,indent)+=2;
    tjump=noscheDfgBlock(self,node->jump);
    overiProp(self,indent)-=2;
    osyscIndent(fp,overiProp(self,indent));
    fprintf(fp,"end\n");
    return tjump;
}
//
//ctrl block
//
static cgrNode noscheCtrlBlock(fsmdHandle self,cgrNode node){
    int p=0;
    FILE*fp=overiProp(self,fp);
    cgrNode next,jump;
    if(!node) return NULL;
    if(node->type!=Ctrl) return NULL;
    next=node;
    jump=NULL;
    while(next){
        cgrNode tjump;
        cgrNode cond=cgrGetNode(next,cgrKeyCond);
        if(fsmdRouteCheck(next)) return NULL;
        fsmdRouteSet(next);
        //if(aaa) begin
        if(cond){ 
            osyscIndent(fp,overiProp(self,indent));
            if(p) fprintf(fp,"else ");
            fprintf(fp,"if(");
            overiOutOpe2(fp,cond,1);
            fprintf(fp,") ");
            tjump=noscheOutBlock(self,next);
            p=1;
        //else ...    
        }else if(next->jump!=tjump){
            osyscIndent(fp,overiProp(self,indent));
            fprintf(fp,"else ");
            tjump=noscheOutBlock(self,next);
        //end block    
        }else  return jump;
        jump=tjump;
        next=next->next;
    }
    return jump;
}

//
//dfg block
//
static cgrNode noscheDfgBlock(fsmdHandle self,cgrNode node){
    cgrNode jump;
    if(!node) return NULL;
    jump=node;
    while(jump){
        cgrNode back;
        if(fsmdRouteCheck(jump)) return NULL;
        //ctrl
        if(jump->type!=Dfg) jump=noscheCtrlBlock(self,jump);
        //dfg
        else{
            fsmdRouteSet(jump);
            self->dfg(self,jump);
            jump=jump->jump;
        }
        back=cgrGetNode(jump,cgrKeyBack);
        if(back){
            if(back->next) return jump;
        }
    }
    return NULL;
}
//
//state
//
static int noscheState(fsmdHandle self,cgrNode node){
    cgrNode jump;
    if(!node) return 0;
    switch(node->type){
    case Dfg :
        jump=noscheDfgBlock(self,node);
        self->state(self,jump);
        break;
    case Ctrl:
        jump=noscheCtrlBlock(self,node);
        self->state(self,jump);
        break;
    }
    return 0;
}

//
//process
//
static int noscheProc(fsmdHandle self,cgrNode node){
    cgrNode proc=node;
    cgrNode var,state;
    FILE*fp=overiProp(self,fp);
    if(!node) return 0;
    while(proc){
        int f=0;
        char* varname;
        cgrNode rproc=cgrGetNode(node,cgrKeyResetProc);
        overiProp(self,proc)=proc;
        osyscIndent(fp,4);
        fprintf(fp,"always@(");
        overiSens(fp,proc,cgrKeySensitive,"",f);
        overiSens(fp,proc,cgrKeySensitivePos,"posedge",f);
        overiSens(fp,proc,cgrKeySensitiveNeg,"negedge",f);
        fprintf(fp,") begin\n");
        state=cgrGetNode(proc,cgrKeyVal);
        fsmdRouteReset();
        overiProp(self,indent)=6;
        self->state(self,state);
        osyscIndent(fp,4);
        fprintf(fp,"end\n\n");
        proc=proc->next;
    }
    return 0;
}


//
//to output without 
//
int nosche(char*outfile,cgrNode top){
    fsmdHandle fsmd;
    fsmd=overiIni(outfile,top);
    fsmd->proc=(*noscheProc);
    fsmd->state=(*noscheState);
    fsmd->dfg=(*noscheDfg);
    fsmd->run(fsmd);
    overiDes(fsmd);
    return 0;
}

