/*
  pattern matching utilities .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "generic.h"

/*=====================================================================

  pattern matching (Id)

======================================================================*/
int ptnutilMatch(cgrNode node){
    cgrNode left;

    //error
    if(!node) return ptnutilType_None;
    left=cgrGetMaxLeft(node);

    if(left->type==Id){
        cgrNode par=cgrGetNode(left,cgrKeyPar);
        if(!par) return ptnutilType_A; //aaa
        if(par->type==Array){
            while(par){
                //aaa[1][2][3](10)
                if(par->type==Funcall) return ptnutilType_E;
                //aaa[1][2][3] ...
                else if(par->type!=Array) return ptnutilType_B;
                par=cgrGetNode(par,cgrKeyPar);
            }
            //aaa[1][2][3]
            return ptnutilType_B;
        //aaa
        }else return ptnutilType_A; 
    }else if(left->type==Funcall){
        cgrNode par=cgrGetNode(left,cgrKeyPar);
        //a(3)[10][20]
        if(par){
            if(par->type==Array) return ptnutilType_D;
        }
        //a(3)
        return ptnutilType_C;
    }
    //error
    return ptnutilType_None;
}

/*=====================================================================

  pattern matching (signal connection)

======================================================================*/
int ptnutilMatchSigCon(cgrNode node){
    cgrNode par;  //parent node
    cgrNode right;//right node
    cgrNode left; //max left node

    //checking node
    if(!node) return ptnutilType_None;
    left=cgrGetMaxLeft(node);
    if(!left) return ptnutilType_None;
    if(left->type!=Id) return ptnutilType_None;
    
    //checking par
    par=cgrGetNode(left,cgrKeyPar);
    if(!par) return ptnutilType_None;
    if((par->type!='.') && (par->type!=Arr)) return ptnutilType_None;

    //checking right
    right=cgrGetNode(par,cgrKeyRight);
    if(!right) return ptnutilType_None;
    if(right->type!=Funcall) return ptnutilType_None;
    
    //matching pattern
    if(par->type=='.') return ptnutilType_F;   //u0.sig(sig);
    if(par->type==Arr) return ptnutilType_G;   //u0->sig(sig);
    return ptnutilType_None; //error
}

/*=====================================================================

  pattern matching (read/write function)
    ptnutilType_H,   //x.read();
    ptnutilType_I,   //x.write(A);
    ptnutilType_J,   //x[].read();
    ptnutilType_K,   //x[].write(A);

======================================================================*/
int ptnutilMatchReadWrite(cgrNode node){
    char*fname;
    cgrNode par,lpar,left,right;
    
    par=NULL;
    if(!node) return ptnutilType_None; //error
    switch(node->type){
    case Funcall :
        right=node;
        par =cgrGetNode(right,cgrKeyPar);
        left=cgrGetMaxLeft(par);
        break;
    case Arr :
    case '.' :
        par=node;
        left=cgrGetMaxLeft(par);
        right=cgrGetNode(par,cgrKeyRight);
        break;
    default : 
        left=cgrGetMaxLeft(node);
        par=left;
        do{
            cgrNode tmp=par;
            par=cgrGetNode(tmp,cgrKeyPar);
            if(!par) return ptnutilType_None; //error
            if(cgrGetNode(par,cgrKeyLeft)!=tmp)
                return ptnutilType_None; //error
        }while(par->type==Array);
        if((par->type!='.')&&(par->type!=Arr))    
            return ptnutilType_None; //error
        right=cgrGetNode(par,cgrKeyRight);
        break;
    }
    //checking parent node
    if(!par) return ptnutilType_None; //error
    if((par->type!='.')&&(par->type!=Arr))
        return ptnutilType_None; //error
    
    //checking right node
    if(right->type!=Funcall) return ptnutilType_None; //error
    fname=cgrGetStr(right,cgrKeyId);
    
    //checking left parent node
    lpar=cgrGetNode(left,cgrKeyPar);
    if(!lpar)  return ptnutilType_None; //error

    //write
    if(!strcmp(fname,"write")){
        if(lpar->type==Array) return ptnutilType_K;//x[].write(A);
        else                  return ptnutilType_I;//x.write(A);
    //read
    }else if(!strcmp(fname,"read")){
        if(lpar->type==Array) return ptnutilType_J;//x[].read();
        else                  return ptnutilType_H;//x.read();
    }
    return ptnutilType_None; //error
}



/*=====================================================================

  pattern matching (cfg structure)
  
======================================================================*/
//
// checking main while/do-while loop
//
int ptnutilMatchCfgStructLoop(cVector stack,cgrNode seed,int i){
    cgrNode proc=NULL;
    int rettype;

    //checking seed node
    if(!seed) return ptnutilType_None;

    //setting return type
    switch(seed->type){
    case Do    : rettype = ptnutilType_DoDfg;
    case While : rettype = ptnutilType_WhileDfg;
    case For   : rettype = ptnutilType_ForDfg;
    default    : return    ptnutilType_None;
    }

    //find process node fron stack
    for(i;i>=0;i--){
        cgrNode* n=cvectFind(stack,i);
        if(!n) break;
        if((*n)->type==Function){
            proc=*n;
            break;
        }
    }

    //if not found process node
    if(!proc) return rettype;
    else{ //checking main loop
        cgrNode cfg=cgrGetNode(proc,cgrKeyVal);
        while(cfg){
            if(cfg==seed)  return ptnutilType_Dfg;
            cfg=cfg->next;
        }
        return rettype;
    }

    //error
    return ptnutilType_None;
}
//
//checking for loop
//
int ptnutilMatchCfgStructFor(
    cVector stack,cgrNode fornode,cgrNode dfg,int i){
    cgrNode ini=cgrGetNode(fornode,cgrKeyIni);
    cgrNode stp=cgrGetNode(fornode,cgrKeyStep);

    //checking init
    while(ini){
        if(ini==dfg){
            if(dfg->type==Data_type) 
                return ptnutilType_ForIniDataType;
            else return ptnutilType_ForIni;
        }
        ini=ini->next;
    }
    //checking step
    while(stp){
        if(stp==dfg) return ptnutilType_ForStp;
        stp=stp->next;
    }
    return ptnutilMatchCfgStructLoop(stack,fornode,i);
}

//
// main
//
int ptnutilMatchCfgStruct(cVector stack){
    cgrNode prev=NULL;
    int stksize=cvectSize(stack);
    int i,state=0;

    for(i=stksize-1;i>=0;i--){
        cgrNode* seed=cvectFind(stack,i);
        if(!seed) return ptnutilType_None; //error

        switch(state){
        case 0 : //seed is cond node
            //checking seed node 
            switch((*seed)->type){
            case Do        : return ptnutilType_DoCnd;
            case While     : return ptnutilType_WhileCnd;
            case For       : return ptnutilType_ForCnd;
            case If        : return ptnutilType_IfCnd;
            case Switch    : return ptnutilType_SwCnd;
            case Data_type :
            case Dfg       : prev=*seed;state=1;break;
            default        : return ptnutilType_None; //error
            }
            break;
        case 1 : //seed is dfg
            switch((*seed)->type){
            case Do   : return ptnutilMatchCfgStructLoop(stack,*seed,i);
            case While: return ptnutilMatchCfgStructLoop(stack,*seed,i);
            case For  : return ptnutilMatchCfgStructFor(stack,*seed,prev,i);
            case If   : return ptnutilType_IfDfg;
            case Else : return ptnutilType_IfDfg;
            case Case : return ptnutilType_CaseDfg;
            case Block: break;
            default   : return ptnutilType_Dfg;
            }
            break;
        }
    }
    return ptnutilType_None; //error
}

/*=====================================================================

  pattern matching (state machine)
  
======================================================================*/
int ptnutilMatchStateMachine(cVector stack){
    cgrNode* n=NULL; //seed node
    cgrNode* m=NULL; //master node
    cgrNode* l=NULL; //loop node
    int i,j,stksize=cvectSize(stack);
    
    //getting seed node
    j=stksize-1;
    for(i=j;i>=0;i--){
        n=cvectFind(stack,i);
        if(!n) return ptnutilType_None; //error
        if((*n)->type==If)     break;
        if((*n)->type==Else)   break;
        if((*n)->type==Switch) break;
    }

    //getting master node
    j=i-1;
    for(i=j;i>=0;i--){
        m=cvectFind(stack,i);
        if(!m) return ptnutilType_None; //error
        if((*m)->type==If)     break;
        if((*m)->type==Else)   break;
        if((*m)->type==Switch) break;
    }

    //getting loop node
    j=i;
    for(i=j-1;i>=0;i--){
        l=cvectFind(stack,i);
        if(!l) return ptnutilType_None; //error
        if((*l)->type==Do)    break;
        if((*l)->type==While) break;
        if((*l)->type==For)   break;
    }

    //checking node
    if((!n) || (!m) || (!l)) return ptnutilType_None; //error
    
    //return value
    if((*n)->type==Switch){
        if((*m)->type==Switch)  return ptnutilType_Sw_Sw;
        else  return ptnutilType_Sw_If;
    }else{
        if((*m)->type==Switch)  return ptnutilType_If_Sw;
        else  return ptnutilType_If_If;
    }

    return ptnutilType_None; //error
}


/*=====================================================================

  pattern matching (condition(If/Switch) pattern)
  
======================================================================*/
int ptnutilMatchCondIfSw(cVector stack){
    cgrNode* n=NULL; //seed node
    int i,stksize=cvectSize(stack);
    
    //getting seed node
    for(i=stksize-1;i>=0;i--){
        n=cvectFind(stack,i);
        if(!n) return ptnutilType_None; //error
        if((*n)->type==If)      break;
        if((*n)->type==Else)    break;
        if((*n)->type==Case)    break;
        if((*n)->type==Default) break;
        if((*n)->type==Switch)  break;
    }
    if(!n)    return ptnutilType_None; //error

    //SWITCH
    if((*n)->type==Switch){
        cgrNode cfgcase=cgrGetNode(*n,cgrKeyVal);
        cgrNode next=NULL;
        if(!cfgcase) return ptnutilType_None;
        //default
        if(cfgcase->type==Default) return ptnutilType_CondDef;
        next=cfgcase->next;
        //case
        if(!next) return ptnutilType_CondCase;
        //case-default
        if(next->type==Default)return ptnutilType_CondCaseDef;
        //case-case
        if(!next->next) return ptnutilType_CondCaseCase;
        //case-case-...
        return ptnutilType_CondCaseCaseDef;
    //CASE/DEFAULT
    }else if(((*n)->type==Case)||((*n)->type==Default)){
        cgrNode ctop=(*n);
        cgrNode cnext=NULL;
        cgrNode cnext2=NULL;
        //getting top case node
        while(ctop->prev){
            if((ctop->type!=Case)||(ctop->type!=Default)) break;
            ctop=ctop->prev;
        }
        //checking top case node
        if((ctop->type!=Case)||(ctop->type==Default))
             return ptnutilType_None; //error
        //default
        if(ctop->type==Default) return ptnutilType_CondDef;
        cnext=ctop->next;
        //case
        if(!cnext) return ptnutilType_CondCase;
        //case-default
        if(cnext->type==Default) return ptnutilType_CondCaseDef;
        cnext2=cnext->next;
        if(!cnext2)  return ptnutilType_CondCaseCase;
        return ptnutilType_CondCaseCaseDef;

    //ELSE
    }else if((*n)->type==Else){ //else
        cgrNode m=(*n)->prev;
        cgrNode l=NULL;
        if(!m) return ptnutilType_None;
        //if-else
        if(m->type==If)  return ptnutilType_CondIfElse;
        //...else if-else
        else if(m->type==Else) return ptnutilType_CondIfElseElse;
        else  return ptnutilType_None;
    
    //IF
    }else if((*n)->type==If){
        cgrNode itop=NULL;
        cgrNode inext=NULL;
        cgrNode inext2=NULL;

        //... if
        itop=(*n);
        inext=itop->next;
        //if
        if(!inext) return ptnutilType_CondIf;
        
        //if ...
        if(inext->type!=Else) return ptnutilType_CondIf;
        inext2=inext->next;

        //if- else ;
        if(!inext2) return ptnutilType_CondIfElse;
       
        //if- else -
        if(!inext2) return ptnutilType_CondIfElse;

        //if- else if{...} ...
        if(inext2->type!=Else) return ptnutilType_CondIfElse;

        //if- else if{...} -else
        return ptnutilType_CondIfElseElse;
    }
    return ptnutilType_None; //error
}
