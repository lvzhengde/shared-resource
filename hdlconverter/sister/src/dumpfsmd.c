/*
  to dump FSMD.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"
/*------------------------------------------------------------------
  state numbering
------------------------------------------------------------------*/
//
//setting number
//
int stateNumSet(cgrNode node,int *val){
    int num;
    if(!node) return *val;
    num=cgrGetNum(node,cgrKeyNumber);
    if(num) return *val;
    (*val)++;
    cgrSetNum(node,cgrKeyNumber,*val);
    stateNumSet(node->jump,val);
    if(node->type!=Dfg) stateNumSet(node->next,val);
    return *val;
}

//
//cleaning number
//
int stateNumClean(cgrNode node){
    int num;
    if(!node) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    if(!num) return 0;
    cgrSetNum(node,cgrKeyNumber,0);
    stateNumClean(node->jump);
    if(node->type!=Dfg) stateNumClean(node->next);
    return 0;
}

/*------------------------------------------------------------------
  to dump FSMD
------------------------------------------------------------------*/
//
//handler for dump
//
typedef struct _dumpHandle{
    FILE*fp;
    cMap map;
    cgrNode top;

}dumpHandle;

//
//to dump state
//
static int dumpFsmdDfg(dumpHandle dmp,cgrNode node);
static int dumpFsmdDef(dumpHandle dmp,cgrNode node); 
static int dumpFsmdCtr(dumpHandle dmp,cgrNode node);
static int dumpFsmdState(dumpHandle dmp,cgrNode node){ 
    int num;
    uchar flg=1;
    if(!node) return 0;
    num=cgrGetNum(node,cgrKeyNumber);
    if(cmapFind(dmp.map,&num)) return 0;
    cmapUpdate(dmp.map,&num,&flg);
    switch(node->type){
        case Dfg  :dumpFsmdDfg(dmp,node);break;
        case Def  :dumpFsmdDef(dmp,node);break;
        case Ctrl :dumpFsmdCtr(dmp,node);break;
    }
    return 0;
}

//
//dump def control
//
static int dumpFsmdCtr(dumpHandle dmp,cgrNode node){ 
    cgrNode jump=node->jump;
    cgrNode next=node->next;
    cgrNode prev=node->prev;
    cgrNode ope=cgrGetNode(node,cgrKeyCond);
    int jto=cgrGetNum(jump,cgrKeyNumber);
    int nto=cgrGetNum(next,cgrKeyNumber);
    int pto=cgrGetNum(prev,cgrKeyNumber);
    int num=cgrGetNum(node,cgrKeyNumber);
    cgrNode back=cgrGetNode(node,cgrKeyBack);
    fprintf(dmp.fp,"    $control %d\n",num);
    fprintf(dmp.fp,"      next : %d\n",nto);
    fprintf(dmp.fp,"      prev : %d\n",pto);
    fprintf(dmp.fp,"      to   : %d\n",jto);
    fprintf(dmp.fp,"      from :");
    while(back){
        jump=back->jump;
        jto=cgrGetNum(jump,cgrKeyNumber);
        fprintf(dmp.fp," %d",jto);
        back=back->next;
    }
    fprintf(dmp.fp,"\n");
    while(ope){
        fprintf(dmp.fp,"      @");
        osyscOutOpe(dmp.fp,ope);
        fprintf(dmp.fp,"\n");
        ope=ope->next;
    }
    ope=cgrGetNode(node,cgrKeyVal);
    while(ope){
        fprintf(dmp.fp,"      #");
        osyscOutOpe(dmp.fp,ope);
        fprintf(dmp.fp,"\n");
        ope=ope->next;
    }
    fprintf(dmp.fp,"\n");
    dumpFsmdState(dmp,node->jump);
    dumpFsmdState(dmp,node->next);
    return 0;
}


//
//dump def state
//
static int dumpFsmdDef(dumpHandle dmp,cgrNode node){ 
    cgrNode jump=node->jump;
    cgrNode next=node->next;
    int jto=cgrGetNum(jump,cgrKeyNumber);
    int nto=cgrGetNum(next,cgrKeyNumber);
    int num=cgrGetNum(node,cgrKeyNumber);
    cgrNode back=cgrGetNode(node,cgrKeyBack);
    fprintf(dmp.fp,"    $def %d\n",num);
    fprintf(dmp.fp,"      next : %d\n",nto);
    fprintf(dmp.fp,"      to   : %d\n",jto);
    fprintf(dmp.fp,"      from :");
    while(back){
        jump=back->jump;
        jto=cgrGetNum(jump,cgrKeyNumber);
        fprintf(dmp.fp," %d",jto);
        back=back->next;
    }
    fprintf(dmp.fp,"\n");
    dumpFsmdState(dmp,node->jump);
    dumpFsmdState(dmp,node->next);
    return 0;
}

//
//dump dfg state
//
static int dumpFsmdDfg(dumpHandle dmp,cgrNode node){ 
    cgrNode jump=node->jump;
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    int jto=cgrGetNum(jump,cgrKeyNumber);
    int num=cgrGetNum(node,cgrKeyNumber);
    cgrNode back=cgrGetNode(node,cgrKeyBack);
    fprintf(dmp.fp,"    $dfg %d\n",num);
    fprintf(dmp.fp,"      to   : %d\n",jto);
    fprintf(dmp.fp,"      from :");
    while(back){
        jump=back->jump;
        jto=cgrGetNum(jump,cgrKeyNumber);
        fprintf(dmp.fp," %d",jto);
        back=back->next;
    }
    fprintf(dmp.fp,"\n");
    while(ope){
        fprintf(dmp.fp,"      #");
        osyscOutOpe(dmp.fp,ope);
        fprintf(dmp.fp,"\n");
        ope=ope->next;
    }
    fprintf(dmp.fp,"\n");
    dumpFsmdState(dmp,node->jump);
    return 0;
}

//
//to dump process
//
static int dumpFsmdProcess(dumpHandle dmp,cgrNode node){ 
    cgrNode proc;
    cgrNode state;
    proc=node;
    while(proc){
        int num=0;
        char*name=cgrGetStr(proc,cgrKeyId);
        cgrNode state=cgrGetNode(proc,cgrKeyVal);
        fprintf(dmp.fp,"  $process %s:\n",name);
        dmp.map=cmapIni(
            sizeof(int),sizeof(uchar),(*cmapIntKey),(*cmapIntCmp));
        stateNumClean(state);
        stateNumSet(state,&num);
        dumpFsmdState(dmp,state);
        cmapDes(dmp.map);
        proc=proc->next;
    }
    fprintf(dmp.fp,"\n");
    return 0;
}


//
//to dump signals
//
static int dumpFsmdSignal(dumpHandle dmp,cgrNode node){ 
    cgrNode sig;
    sig=node; 
    fprintf(dmp.fp,"  $signals:\n");
    while(sig){
        char*opt=cgrGetStr(sig,cgrKeyOpt);
        char*name=cgrGetStr(sig,cgrKeyId);
        int size=cgrGetNum(sig,cgrKeySize);
        int type=cgrGetNum(sig,cgrKeyType);
        int signal=cgrGetNum(sig,cgrKeySignal);
        cgrNode array=cgrGetNode(sig,cgrKeyArray);

        fprintf(dmp.fp,"    ");
        switch(signal){
        case Sc_in:    fprintf(dmp.fp,"$sc_in");break;
        case Sc_out:   fprintf(dmp.fp,"$sc_out");break;
        case Sc_signal:fprintf(dmp.fp,"$sc_signal");break;
        }
        fprintf(dmp.fp," ");
        switch(type){
        case Sc_int:
            fprintf(dmp.fp,"sc_int<%d>",size);break;
        case Sc_uint:
            fprintf(dmp.fp,"sc_uint<%d>",size);break;
        case Sc_bigint:
            fprintf(dmp.fp,"sc_bigint<%d>",size);break;
        case Sc_biguint:
            fprintf(dmp.fp,"sc_biguint<%d>",size);break;
        case Void:
            fprintf(dmp.fp,"void");break;
        case Bool:
            fprintf(dmp.fp,"bool");break;
        case Schar:
            fprintf(dmp.fp,"char");break;
        case Uchar:
            fprintf(dmp.fp,"unsigned char");break;
        case Sshort:
            fprintf(dmp.fp,"short");break;
        case Ushort:
            fprintf(dmp.fp,"unsigned short");break;
        case Sint:
            fprintf(dmp.fp,"int");break;
        case Uint:
            fprintf(dmp.fp,"unsigned int");break;
        case Slong:
            fprintf(dmp.fp,"long");break;
        case Ulong:
            fprintf(dmp.fp,"unsigned long");break;
        }
        fprintf(dmp.fp," %s",name);
        while(array){
            cgrNode index=cgrGetNode(array,cgrKeyRight);
            if(index) switch(index->type){
            case Num :
                fprintf(dmp.fp,"[%d]",cgrGetNum(index,cgrKeyVal));
                break;
            case Id  :
                fprintf(dmp.fp,"[%s]",cgrGetStr(index,cgrKeyVal));
                break;
            default :  
                fprintf(dmp.fp,"[?]");
                break;
            }
            array=array->next;
        }
        if(strcmp("",opt))fprintf(dmp.fp," (%s)",opt);
        fprintf(dmp.fp,"\n");
        sig=sig->next;
    }
    return 0;
}


//
//to dump modules
//
static int dumpFsmdModule(dumpHandle dmp,cgrNode node){
    cgrNode mo;
    mo=node;
    while(mo){
        if(mo->type==Sc_module){
            char*name=cgrGetStr(mo,cgrKeyId);
            cgrNode sig=cgrGetNode(mo,cgrKeySignal);
            cgrNode proc=cgrGetNode(mo,cgrKeyProcess);
            fprintf(dmp.fp,"$module %s(%s:%d)\n",
                name,mo->file,mo->line);
            dumpFsmdSignal(dmp,sig); 
            dumpFsmdProcess(dmp,proc); 
            fprintf(dmp.fp,"$end-module\n\n");
        }
        mo=mo->next;
    }
    return 0;
}


//
//main process to dump
//
void dumpFsmd(cgrNode top,char*filename){
    dumpHandle dmp;
    //initialization
    if(!(dmp.fp=fopen(filename,"w"))){
        cmsgEcho(__sis_msg,0,filename);
        return ;
    }
    dmp.top=top;
    dmp.map=NULL;
    //to dump
    dumpFsmdModule(dmp,dmp.top);
    //destruction
    fclose(dmp.fp);
    return ;
}

