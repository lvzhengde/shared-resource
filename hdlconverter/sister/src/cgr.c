/*
  control graph .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "generic.h"

/*------------------------------------------------------------------
  structure definition
------------------------------------------------------------------*/
//
//graph handler
//
typedef struct _cgrHandle{
    cMap lst;//node pool
    cVector strbuf;//buffer for string
}* cgrHandle;

//
//a value of map node
//
typedef union{
    int num;
    char*str;
    double real;
    cgrNode node;
}mapNodeVal;

//
//a map node
//
typedef struct{
    int type;//0:num,1:str,2:real,3::node
    mapNodeVal val;
}mapNode;

/*------------------------------------------------------------------
  static global declaration
------------------------------------------------------------------*/
//
// the root node
//
static cgrNode __sis_root=NULL;

//
// the graph handler
//
static cgrHandle __sis_cgr=NULL;

//
// buffer handling
//
//set with copy
#define cgrSetStrBuf(s,ts){                    \
    cstrSet(s,ts,memError("syntax checking")); \
    cvectAppend(__sis_cgr->strbuf,s);}
//set without copy
#define cgrSetStrBuf2(s){                     \
    cvectAppend(__sis_cgr->strbuf,s);}


/*------------------------------------------------------------------
  initialization
------------------------------------------------------------------*/
int cgrIni(void){
    cstrMakeBuf(__sis_cgr,1,struct _cgrHandle,
        memError("initialization for syntax checking"));
    __sis_cgr->lst=cmapIni(sizeof(int),
        sizeof(struct _cgrNode),(*cmapIntKey),(*cmapIntCmp));
    if(!(__sis_cgr->lst)){
        memError("initialization for syntax checking");
        free(__sis_cgr);
        __sis_cgr=NULL;
        return -1;
    }
    __sis_cgr->strbuf=cvectIni(sizeof(char*),256);
    if(!(__sis_cgr->strbuf)){
        memError("initialization for syntax checking");
        cmapDes(__sis_cgr->lst);
        free(__sis_cgr);
        __sis_cgr=NULL;
        return -1;
    }
    return 0;
}

/*------------------------------------------------------------------
  destruction
------------------------------------------------------------------*/
void cgrDes(void){
    int i;
    cVector fvect;
    if(!__sis_cgr) return;
    fvect=__sis_cgr->lst->elmlist;
    for(i=0;i<cvectSize(fvect);i++){
        struct _cgrNode node;
        node=cioVoid(cmapFind(__sis_cgr->lst,&i),struct _cgrNode);
        cmapDes(node.map);
    }
    cmapDes(__sis_cgr->lst);
    for(i=0;i<cvectSize(__sis_cgr->strbuf);i++){
        char* str;
        str=cvectElt(__sis_cgr->strbuf,i,char*);
        free(str);
    }
    cvectDes(__sis_cgr->strbuf);
    free(__sis_cgr);
    __sis_cgr=NULL;
}

/*------------------------------------------------------------------
  getting root node
------------------------------------------------------------------*/
cgrNode cgrGetRoot(void){
    return __sis_root;
}
/*------------------------------------------------------------------
  setting root node
------------------------------------------------------------------*/
void cgrSetRoot(cgrNode node){
    __sis_root=node;
}
/*------------------------------------------------------------------
  new node creation
------------------------------------------------------------------*/
//
// key function
//
static int mapKey(void* val){
    return cioVoid(val,int);
}

//
// comparation function
//
static int mapCmp(void*a,void*b){
    int key0=cioVoid(a,int);
    int key1=cioVoid(b,int);
    return key0==key1;
}

//
// creation
//
cgrNode cgrNew(int type,char*file,int line){
    int t;
    cgrNode node;
    struct _cgrNode tnode={0};
    static int maxid=0;
    t=maxid;
    cmapUpdate(__sis_cgr->lst,&t,&tnode);
    node=(cgrNode)cmapFind(__sis_cgr->lst,&t);
    maxid++;
    node->type=type;
    node->line=line;
    cgrSetStrBuf(node->file,file);
    node->map=cmapNew(
        sizeof(int),sizeof(mapNode),(*mapKey),(*mapCmp));
    node->next=NULL;
    node->prev=NULL;
    node->jump=NULL;
    return node;
}
/*------------------------------------------------------------------
  setting attribute as number
------------------------------------------------------------------*/
void cgrSetNum(cgrNode node,int key,int val){
    mapNode mval;
    if(!node) return;
    mval.type=0;
    mval.val.num=val;
    cmapUpdate(node->map,&key,&mval);
}

/*------------------------------------------------------------------
  setting attribute as string
------------------------------------------------------------------*/
void cgrSetStr(cgrNode node,int key,char* val){
    mapNode mval;
    if(!node) return;
    if(val[0]=='\0') return ;
    mval.type=1;
    cgrSetStrBuf(mval.val.str,val);
    cmapUpdate(node->map,&key,&mval);
}

/*------------------------------------------------------------------
  setting attribute as real number
------------------------------------------------------------------*/
void cgrSetReal(cgrNode node,int key,double val){
    mapNode mval;
    if(!node) return;
    mval.type=2;
    mval.val.real=val;
    cmapUpdate(node->map,&key,&mval);
}
/*------------------------------------------------------------------
  setting attribute as a node
------------------------------------------------------------------*/
void cgrSetNode(cgrNode node,int key,cgrNode val){
    mapNode mval;
    if(!node) return;
    mval.type=3;
    mval.val.node=val;
    cmapUpdate(node->map,&key,&mval);
}
/*------------------------------------------------------------------
  getting attribute as number
------------------------------------------------------------------*/
int cgrGetNum(cgrNode node,int key){
    mapNode* mval;
    if(!node) return 0;
    mval=(mapNode*)cmapFind(node->map,&key);
    if(!mval) return 0;
    if(mval->type!=0) return 0;
    return mval->val.num;
}
/*------------------------------------------------------------------
  getting attribute as string
------------------------------------------------------------------*/
char* cgrGetStr(cgrNode node,int key){
    mapNode* mval;
    if(!node) return "";
    mval=(mapNode*)cmapFind(node->map,&key);
    if(!mval) return "";
    if(mval->type!=1) return "";
    return mval->val.str;
}
/*------------------------------------------------------------------
  getting attribute as real number
------------------------------------------------------------------*/
double cgrGetReal(cgrNode node,int key){
    mapNode* mval;
    if(!node) return 0.0;
    mval=(mapNode*)cmapFind(node->map,&key);
    if(!mval) return 0.0;
    if(mval->type!=2) return 0.0;
    return mval->val.real;
}
/*------------------------------------------------------------------
  getting attribute as a node
------------------------------------------------------------------*/
cgrNode cgrGetNode(cgrNode node,int key){
    mapNode* mval;
    if(!node) return NULL;
    mval=(mapNode*)cmapFind(node->map,&key);
    if(!mval) return NULL;
    if(mval->type!=3) return NULL;
    return mval->val.node;
}

/*------------------------------------------------------------------
  getting the head node
------------------------------------------------------------------*/
cgrNode cgrGetHead(cgrNode node){
    if(!node) return NULL;
    while(node->prev) node=node->prev;
    return node;
}
/*------------------------------------------------------------------
  getting the tail node
------------------------------------------------------------------*/
cgrNode cgrGetTail(cgrNode node){
    if(!node) return NULL;
    while(node->next) node=node->next;
    return node;
}

/*------------------------------------------------------------------
  adding the node
------------------------------------------------------------------*/
void cgrAddNode(cgrNode node0,int key,cgrNode node1){
    cgrNode head;
    if(!node0) return ;
    head=cgrGetNode(node0,key);
    cgrSetContext(node1,head);
    cgrSetNode(node0,key,node1);
}

/*------------------------------------------------------------------
  removing the node
------------------------------------------------------------------*/
void cgrRemoveNode(cgrNode par,int key,cgrNode node){
    cgrNode next,prev;
    cgrNode top;
    if(!node) return ;
    top=cgrGetNode(par,key);
    next=node->next;
    prev=node->prev;
    if((top==node) || (!prev)){
        cgrSetNode(par,key,next);
        if(next) next->prev=NULL;
    }else cgrSetContext(prev,next);
}
/*------------------------------------------------------------------
  replacing the node
------------------------------------------------------------------*/
void cgrReplaceNode(cgrNode be,cgrNode af,cgrNode par,int key){
    cgrNode prev;
    cgrNode next;
    if(!be) return ;
    prev=be->prev;
    next=be->next;
    if(!prev) cgrSetNode(par,key,af);
    else{
        prev->next=af;
        af->prev=prev;
    }
    af->next=next;
    if(next) next->prev=af;
}

/*------------------------------------------------------------------
  inserting the node
------------------------------------------------------------------*/
void cgrInstNode(cgrNode prev,cgrNode next){
    cgrNode pnext;
    if(!prev) return ;
    pnext=prev->next;
    cgrSetContext(prev,next);
    cgrSetContext(next,pnext);
}

/*------------------------------------------------------------------
  setting context between two nodes
------------------------------------------------------------------*/
void cgrSetContext(cgrNode node0,cgrNode node1){
    if(node0) node0->next=node1;
    if(node1) node1->prev=node0;
}

/*------------------------------------------------------------------
  copying cfg node
------------------------------------------------------------------*/
//copying attr
#define cgrCopyCfgAttr(p,src,key,function) {\
    cgrNode ope=cgrGetNode(src,key);        \
    ope=function(ope);                      \
    cgrSetNode(p,key,ope);                  }
//copying cfg node
cgrNode cgrCopyCfg(cgrNode node){
    cgrNode p,next;
    if(!node) return NULL;
    p=cgrNew(node->type,node->file,node->line);
    switch(p->type){
    case For :
        cgrCopyCfgAttr(p,node,cgrKeyIni,cgrCopyCfg);
        cgrCopyCfgAttr(p,node,cgrKeyStep,cgrCopyCfg);
    case If :
    case Do :
    case Case :
    case While :
    case Switch :
        cgrCopyCfgAttr(p,node,cgrKeyCond,fsmdCopyOpe);
    case Else :
    case Block :
    case Default :
        cgrCopyCfgAttr(p,node,cgrKeyVal,cgrCopyCfg);
        break;
    case Data_type :
        cgrCopyCfgAttr(p,node,cgrKeyType,cgrCopyCfg);
        cgrCopyCfgAttr(p,node,cgrKeyOpt,cgrCopyCfg);
        fsmdCopyAttr(Num,p,node,cgrKeySize);
    case Dfg :
    case Return :
        cgrCopyCfgAttr(p,node,cgrKeyVal,fsmdCopyOpe);
        break;
    }
    next=cgrCopyCfg(node->next);
    cgrSetContext(p,next);
    return p;
}
/*------------------------------------------------------------------
  copying function node
------------------------------------------------------------------*/
cgrNode cgrCopyFunction(cgrNode node){
    char*name;           //function name
    cgrNode func;        //copied function node
    cgrNode cfg,cfg2;    //cfg node
    cgrNode type,type2;  //copied function node
    cgrNode args,args2;  //copied function node

    //checking node
    if(!node) return NULL;
    //get name
    name=cgrGetStr(node,cgrKeyId);

    //get cfg
    cfg=cgrGetNode(node,cgrKeyVal);
    cfg2=cgrCopyCfg(cfg);

    //get type
    type=cgrGetNode(node,cgrKeyType);
    type2=cgrCopyDataType3(type);

    //get args
    args=cgrGetNode(node,cgrKeyArgs);
    args2=cgrCopyDataType4(args);

    //get function
    func=cgrNew(Function,node->file,node->line);
    cgrSetStr(func,cgrKeyId,name);
    cgrSetNode(func,cgrKeyVal,cfg2);
    cgrSetNode(func,cgrKeyType,type2);
    cgrSetNode(func,cgrKeyArgs,args2);

    return func;
}

/*------------------------------------------------------------------
  deciding bit width from operator
------------------------------------------------------------------*/
//getting bit width from number
int cgrBitWidthNum(int num){
    int i,n,f;
    if(num<0){
        num=-num;
        f=1;
    }else f=0;
    n=num>>1;
    for(i=1;n>0;i++) n>>=1;
    return i+f;
}
//gettind bit width from declaration
static int cgrBitWidthDec(cgrNode node){
    cgrNode dec;
    if(!node) return 1;
    dec=cgrGetNode(node,cgrKeySignal);
    if(!dec) return 1;
    return cgrGetNum(dec,cgrKeySize);
}
//funcall
static int cgrBitWidthFuncall(cgrNode node,cgrNode dec){
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(args){
        cgrNode args2=args->next;
        if(args2){
            if((args->type==Num)&&(args2->type==Num)){
                int s=cgrGetNum(args ,cgrKeyVal);
                int e=cgrGetNum(args2,cgrKeyVal);
                int b=e-s;
                if(b<0) return (-b)+1;
                else return b+1;
            }
        }
    }
    return cgrGetNum(dec,cgrKeySize);
}
//getting bit width from operator
#define cgrBitWidthBra(node,Key,out){    \
    cgrNode tmp=cgrGetNode(node,Key);    \
    out=cgrBitWidth(tmp);                }
#define cgrBitWidthOpe(node,left,right)  {  \
    cgrBitWidthBra(node,cgrKeyLeft,left);   \
    cgrBitWidthBra(node,cgrKeyRight,right); }
#define cgrBitWidthMax(node){        \
    int left,right;                  \
    cgrBitWidthOpe(node,left,right); \
    if(left>right) return left;      \
    else return right;               }
int cgrBitWidth(cgrNode node){
    int left,right;
    if(!node) return 1;
    switch(node->type){
    case Id : return cgrBitWidthDec(node);
    case Num: return cgrBitWidthNum(cgrGetNum(node,cgrKeyVal));
    case Arr :
    case '.' :
    case '=' :return cgrBitWidth(cgrGetNode(node,cgrKeyRight));
    case Rshif:
    case Array:
    case '/':
    case '%':
    case Mueq:
    case Dieq:
    case Moeq:
    case Adeq:
    case Sueq:
    case Lseq:
    case Rseq:
    case Aneq:
    case Xoeq:
    case Oreq:return cgrBitWidth(cgrGetNode(node,cgrKeyLeft));
    case '&' :
        if(!cgrGetNode(node,cgrKeyLeft))
            return cgrBitWidth(cgrGetNode(node,cgrKeyRight));
    case '|':
    case '~':
    case '^':
    case Inc:
    case Dec:
    case Oror:
    case Anan:
    case '?' : cgrBitWidthMax(node);
    case '+':
    case '-':
        cgrBitWidthOpe(node,left,right);
        if(left>right) return left+1;
        else return right+1;
    case '*':
        if(!cgrGetNode(node,cgrKeyLeft))
            return cgrBitWidth(cgrGetNode(node,cgrKeyRight));
    case Lshif:
        cgrBitWidthOpe(node,left,right);
        return left+right;
    case Funcall :{
        cgrNode dec;
        dec=cgrGetNode(node,cgrKeySignal);
        if(!dec) return 1;
        switch(dec->type){
        case Function:return cgrBitWidth(cgrGetNode(dec,cgrKeyType));
        case Declare :
            return cgrBitWidthFuncall(node,dec);
        default :return 1;
        }}
    case Data_type :{
        int size=cgrGetNum(node,cgrKeySize);
        cgrNode type=cgrGetNode(node,cgrKeyType);
        switch(type->type){
        case Bool :size=1;break;    
        case Uchar :size=8;break;    
        case Schar :size=8;break;    
        case Ushort :size=16;break;    
        case Sshort :size=16;break;    
        case Uint :size=64;break;    
        case Sint :size=64;break;    
        case Ulong :size=64;break;    
        case Slong :size=64;break;    
        }return size;
        }
    case Connect :{
        int sum=0;
        cgrNode val=cgrGetNode(node,cgrKeyCond);
        while(val){
            sum+=cgrBitWidth(val);
            val=val->next;
        }
        return sum;
    }}
    return 1;
}
/*------------------------------------------------------------------
  operator cleation with linked node
------------------------------------------------------------------*/
#define getLineFile(node,file,line) {\
    file=node->file;line=node->line; }
cgrNode cgrCreateOpe(int type,
    cgrNode left,cgrNode right,cgrNode cond){
    cgrNode n;
    int line=0;
    char*file="";
    if(left){ getLineFile(left,file,line); }
    else if(right){ getLineFile(right,file,line);}
    else if(cond) { getLineFile(cond,file,line); }
    n=cgrNew(type,file,line);
    cgrSetNode(n,cgrKeyLeft,left);
    cgrSetNode(left,cgrKeyPar,n);
    cgrSetNode(n,cgrKeyRight,right);
    cgrSetNode(right,cgrKeyPar,n);
    cgrSetNode(n,cgrKeyCond,cond);
    cgrSetNode(cond,cgrKeyPar,n);
    return n;
}

/*------------------------------------------------------------------
  copy data type node
------------------------------------------------------------------*/

//
// copy datatype node  
//
int cgrCopyDataType(cgrNode dec,cgrNode out){
    cgrNode ptr_dec=NULL;  //pointer declaration
    cgrNode ptr_out=NULL;  //pointer declaration

    cgrCopyDataType5(dec,out);

    //get declaration
    ptr_dec=cgrGetNode(dec,cgrKeyPointer);
    if(ptr_dec) ptr_out=fsmdCopyOpe(ptr_dec);

    //set data type
    cgrSetNode(out,cgrKeyPointer,ptr_out);

    return 0;
}

//
// copy datatype node (class/struct)
//
int cgrCopyDataType2(cgrNode dec,cgrNode out){
    char*tag; //tag string

    //check node
    if(!dec) return 0;
    if(!out) return 0;

    //checking type
    switch(dec->type){
    case Class  :
    case Struct : break;
    default : cgrCopyDataType(dec,out); break;
    }

    //set tag string
    out->type=dec->type;
    tag=cgrGetStr(dec,cgrKeyTag);
    cgrSetStr(out,cgrKeyTag,tag);


    //typedef struct aaa bbb;
    if(cgrGetNum(dec,cgrKeyIsClassTag)) cgrSetNum(out,cgrKeyIsClassTag,1);
    //typdef struct {...} bbb;
    else{
        cgrNode body;  //struct body element
        cgrNode dtpre; //preview data type node
        dtpre=NULL;
        body=cgrGetNode(dec,cgrKeyClassBody);
        while(body){
            cgrNode dt;  //data type node
            cgrNode var; //variable node
            cgrNode varpre; //preview variable node
            //create data type
            dt=cgrNew(Data_type,body->file,body->line);
            switch(body->type){
            case Class  :
            case Struct :
                cgrCopyDataType2(body,dt);
                break;
            default :
                cgrCopyDataType(body,dt);
                break;
            }
            cgrSetContext(dtpre,dt);
            dtpre=dt;

            //create variable
            varpre=NULL;
            var=cgrGetNode(body,cgrKeyVal);
            while(var){
                cgrNode newvar;//new variable node
                newvar=fsmdCopyOpe(var);
                cgrSetContext(varpre,newvar);
                varpre=newvar;
                var=var->next;
            }
            cgrSetNode(dt,cgrKeyVal,cgrGetHead(varpre));
            body=body->next;
        }
        cgrSetNode(out,cgrKeyClassBody,cgrGetHead(dtpre));
    }
    return 0;
}
//
// copy data type
//
cgrNode cgrCopyDataType3(cgrNode dec){
    cgrNode out;
    if(!dec) return 0;
    out=cgrNew(Data_type,dec->file,dec->line);
    switch(dec->type){
    case Class  :
    case Struct :
        cgrCopyDataType2(dec,out);
        break;
    default :
        cgrCopyDataType(dec,out);
        break;
    }
    return out;
}

//
// copy data type list
//
cgrNode cgrCopyDataType4(cgrNode dec){
    cgrNode dec2;
    cgrNode o1,o2;
    dec2=dec;
    o1=NULL;
    o2=NULL;
    while(dec2){
        cgrNode var=cgrGetNode(dec2,cgrKeyVal);
        cgrNode var2=fsmdCopyOpe(var);
        o2=cgrCopyDataType3(dec2);
        cgrSetNode(o2,cgrKeyVal,var2);
        cgrSetContext(o1,o2);
        o1=o2;
        dec2=dec2->next;
    }
    return cgrGetHead(o1);
}
//
// copy datatype node  
//
int cgrCopyDataType5(cgrNode dec,cgrNode out){
    cgrNode type_dec=NULL; //type declaration
    cgrNode type_out=NULL; //output type
    cgrNode opt_dec=NULL;  //option declaration
    cgrNode opt_out=NULL;  //option declaration
    int size_dec;     //size declaration
    char*opt_dec_str; //option string
    int index;        //index for structure
    int offset;       //offset for structure
    int offsetdef;    //default offset for structure

    //get declaration
    opt_dec =cgrGetNode(dec,cgrKeyOpt);
    opt_dec_str=cgrGetStr(opt_dec,cgrKeyVal);
    type_dec=cgrGetNode(dec,cgrKeyType);
    size_dec=cgrGetNum(dec,cgrKeySize);
    index=cgrGetNum(dec,cgrKeyClassElmIndex);
    offset=cgrGetNum(dec,cgrKeyClassElmOffset);
    offsetdef=cgrGetNum(dec,cgrKeyClassElmOffsetDef);

    //create out declaration
    if(opt_dec) opt_out=cgrNew(opt_dec->type,opt_dec->file,opt_dec->line);
    cgrSetStr(opt_out,cgrKeyVal,opt_dec_str);
    type_out=cgrNew(type_dec->type,type_dec->file,type_dec->line);

    //set data type
    out->type=Data_type;
    cgrSetNode(out,cgrKeyOpt,opt_out);
    cgrSetNode(out,cgrKeyType,type_out);
    cgrSetNum(out,cgrKeySize,size_dec);
    cgrSetNum(out,cgrKeyClassElmIndex,index);
    cgrSetNum(out,cgrKeyClassElmOffset,offset);
    cgrSetNum(out,cgrKeyClassElmOffsetDef,offsetdef);

    return 0;
}


/*------------------------------------------------------------------
  get max left node
------------------------------------------------------------------*/
cgrNode cgrGetMaxLeft(cgrNode node){
    if(!node) return NULL;
    while(1){
        cgrNode left;
        left=cgrGetNode(node,cgrKeyLeft);
        if(!left) return node;
        node=left;
    }
    return NULL;
}

/*------------------------------------------------------------------
    operator identification 0:no,1:yes
------------------------------------------------------------------*/

//ident id
static int cgrIdentOpeId(cgrNode a,cgrNode b){
    char*ai=cgrGetStr(a,cgrKeyVal);
    char*bi=cgrGetStr(b,cgrKeyVal);
    if(strcmp(ai,bi)) return 0;
    return 1;
}
//ident number
static int cgrIdentOpeNum(cgrNode a,cgrNode b){
    int ai=cgrGetNum(a,cgrKeyVal);
    int bi=cgrGetNum(b,cgrKeyVal);
    if(ai!=bi) return 0;
    return 1;
}
//ident funcall
static int cgrIdentOpeFuncall(cgrNode a,cgrNode b){
    cgrNode aarg=cgrGetNode(a,cgrKeyArgs);
    cgrNode barg=cgrGetNode(b,cgrKeyArgs);
    cgrNode alef=cgrGetNode(a,cgrKeyLeft);
    cgrNode blef=cgrGetNode(b,cgrKeyLeft);
    char*aname=cgrGetStr(a,cgrKeyId);
    char*bname=cgrGetStr(b,cgrKeyId);
    if(!cgrIdentOpe(aarg,barg)) return 0;
    if(!cgrIdentOpe(alef,blef)) return 0;
    if(alef&&blef) return 1;
    if(strcmp(aname,bname)) return 0;
    return 1;
}

//ident root
int cgrIdentOpe(cgrNode a,cgrNode b){
    int ret=0;
    cgrNode left,right,cond;
    while(a&&b){
        if(a->type!=b->type) return 0;
        switch(a->type){
        case Id      : 
            ret=cgrIdentOpeId(a,b);
            break;
        case Num       :
            ret=cgrIdentOpeNum(a,b);
            break;
        case Funcall   : 
            ret=cgrIdentOpeFuncall(a,b);
            break;
        case '=' : 
            cgrIdentOpe(cgrGetNode(a,cgrKeyLeft),
                        cgrGetNode(b,cgrKeyLeft));
            cgrIdentOpe(cgrGetNode(a,cgrKeyRight),
                        cgrGetNode(b,cgrKeyRight));
            break;
        default  : 
            cgrIdentOpe(cgrGetNode(a,cgrKeyLeft),
                        cgrGetNode(b,cgrKeyLeft));
            cgrIdentOpe(cgrGetNode(a,cgrKeyRight),
                        cgrGetNode(b,cgrKeyRight));
            cgrIdentOpe(cgrGetNode(a,cgrKeyCond),
                        cgrGetNode(b,cgrKeyCond));
            break;
        }
        if(!ret) return 0;
        a=a->next;
        b=b->next;
    }
    if(a==b) return 1;
    return 0;
}


/*------------------------------------------------------------------
    setting suffix string to variables
------------------------------------------------------------------*/
//
//make string
//
char* cgrSetSuffixString(char*s,char*suffix){
    char*buf;
    cstrSet(buf,s,memError("syntax checking"));
    cstrCat(buf,suffix,memError("syntax checking"));
    cgrSetStrBuf2(buf);

    return buf;
}

//
//id
//
static int cgrSetSuffixId(cgrNode ope,char*suf){
    char*name;
    char*name2;
    name=cgrGetStr(ope,cgrKeyVal);
    name2=cgrSetSuffixString(name,suf);
    cgrSetStr(ope,cgrKeyVal,name2);
    cstrFree(name2);
    return 0;
}
//
//funcall
//
static int cgrSetSuffixFuncall(cgrNode ope,char*suf){
    char*name;
    char*name2;
    cgrNode left;
    left=cgrGetNode(ope,cgrKeyLeft);
    if(left){
        left=cgrGetMaxLeft(left);
        cgrSetSuffix(left,suf);
    }else{
        name=cgrGetStr(ope,cgrKeyId);
        name2=cgrSetSuffixString(name,suf);
        cgrSetStr(ope,cgrKeyId,name2);
        cstrFree(name2);
    }
    return 0;
}

//
//int a;->a_suffix;
//
int cgrSetSuffix(cgrNode ope,char*suf){
    if(!ope) return 0;
    switch(ope->type){
    case Id      : return cgrSetSuffixId(ope,suf);
    case Funcall : return cgrSetSuffixFuncall(ope,suf);
    default      :
        cgrSetSuffix(cgrGetNode(ope,cgrKeyCond),suf);
        cgrSetSuffix(cgrGetNode(ope,cgrKeyLeft),suf);
        cgrSetSuffix(cgrGetNode(ope,cgrKeyRight),suf);
    }
    return 0;
}

//
//int a,b,c;->a_suffix,b->suffix,c->suffix
//
int cgrSetSuffixList(cgrNode ope,char*suf){
    while(ope){
        cgrSetSuffix(ope,suf);
        ope=ope->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    copy signal declaration
------------------------------------------------------------------*/
cgrNode cgrCopySignalKey(cgrNode node){
    cgrNode sig =NULL;               //new signal
    cgrNode opt =NULL,opt2 =NULL;    //signal option
    cgrNode var =NULL,var2 =NULL;    //signal variable
    cgrNode size=NULL,size2=NULL;    //signal size
    cgrNode type=NULL,type2=NULL;    //signal type

    //getting variable declaration
    var=cgrGetNode(node,cgrKeyVal);
    opt=cgrGetNode(node,cgrKeyOpt);
    size=cgrGetNode(node,cgrKeySize);
    type=cgrGetNode(node,cgrKeyType);

    //make ready signal
    sig=cgrNew(Sc_signal_key,node->file,node->line);
    
    //set type
    if(type) type2 =cgrNew(type->type,node->file,node->line);
    cgrSetNode(sig,cgrKeyType,type2);
    
    //set option
    if(opt) opt2 =cgrNew(opt->type,node->file,node->line);
    cgrSetNode(sig,cgrKeyOpt,opt2);

    //set size
    if(size) size2=cgrCopyDataType3(size);
    cgrSetNode(sig,cgrKeySize,size2);
    
    //set variable
    if(var) var2=fsmdCopyOpe(var);
    cgrSetNode(sig,cgrKeyVal,var2);

    return sig;
}
/*------------------------------------------------------------------
    get id/funcall name
------------------------------------------------------------------*/
char* cgrGetIdName(cgrNode node){
    if(!node) return "";
    if(node->type==Id) return cgrGetStr(node,cgrKeyVal);
    else{ 
       cgrNode left=cgrGetMaxLeft(node);
       if(!left) return "";
       if(left->type==Id) return cgrGetStr(left,cgrKeyVal);
       if(left->type==Funcall) return cgrGetStr(left,cgrKeyId);
    }
    return "";
}
/*------------------------------------------------------------------
    
    append node

------------------------------------------------------------------*/
int cgrAppendNode(cgrNode prev,cgrNode next){
    cgrNode tail=cgrGetTail(prev);
    if(!tail) return 0;
    cgrSetContext(tail,next);
    return 0;
}
/*------------------------------------------------------------------
    
    making variable declare node from signal_key

------------------------------------------------------------------*/
cgrNode cgrMakeDecFromSignalKey(cgrNode sig){ 
    cgrNode var=cgrGetNode(sig,cgrKeyVal);
    cgrNode var2=fsmdCopyOpe(var);
    cgrNode size=cgrGetNode(sig,cgrKeySize);
    cgrNode dec=cgrCopyDataType3(size);
    cgrSetNode(dec,cgrKeyVal,var2);
    return dec;
}

/*------------------------------------------------------------------
    check name competition
   -1 : error 
    0 : normaly
    1 : competition
------------------------------------------------------------------*/
int cgrCheckCompet(cgrNode par,int key,char*name){
    cgrNode node;
    if(!par) return -1;
    node=cgrGetNode(par,key);
    while(node){
        char*name2=cgrGetIdName(node);
        if(!strcmp(name,name2)) return 1;
        node=node->next;
    }
    return 0;
}



