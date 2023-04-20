/*
  checking syntax.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#ifndef CHECK_HEAD
#define CHECK_HEAD


//check2 option handler
typedef struct _chkOptHandle2{
    int noreset;
    char*outfile;
    char*rstname;
    char chain;
}chk2OptHandle;

//checking syntax and creating graph for synthesizing
extern int check(cgrNode top,chk2OptHandle opt);
//checking after optimization
extern int check2(cgrNode top,chk2OptHandle opt);
//default option handler
extern chk2OptHandle chk2OptDefault(void);


//searching stack
static cgrNode __checkVectElt(cVector stack,int i){
    cgrNode* node;
    if(i<0) return NULL;
    node=(cgrNode*)cvectFind(stack,i);
    if(!node) return NULL;
    return *node;
}
#define setJumpForStack(self,node,i)            \
    for(i=cvectSize(self->stack)-1,             \
    node=__checkVectElt(self->stack,i);         \
    i>=0;--i,node=__checkVectElt(self->stack,i))

//setting jump node
extern int exIfAddJump(cgrNode node,cgrNode back);
#define setJumpChangeBack exIfAddJump
//setting par node
extern int remDeadSetPar(
    gsHandle self,cgrNode node,cgrNode ope);
//checking process
#define chkRun(function){ \
    int ret;              \
    gsHandle gs;          \
    gs=function(top);     \
    ret=gsRun(gs);        \
    gsDes(gs);            \
   top=cgrGetRoot();      }
//checking process 2
#define chkRun2(function){ \
    int ret;               \
    gsHandle gs;           \
    gs=function(opt,top);  \
    ret=gsRun(gs);         \
    gsDes(gs);             \
    top=cgrGetRoot();      }
#define chkRun3(function,function2){ \
    int ret;               \
    gsHandle gs;           \
    gs=function(opt,top);  \
    ret=gsRun(gs);         \
    function2(gs);         \
    gsDes(gs);             \
    top=cgrGetRoot();      }

//transeration
extern int traRWFuncall(gsHandle self,cgrNode node);

//checking reset process
extern gsHandle rstProc(chk2OptHandle opt,cgrNode top);

//checking variables declaration
extern gsHandle chkVar(chk2OptHandle opt,cgrNode top);
extern void chkVarDes(gsHandle self);

//transration to SSA form
extern gsHandle toSsa(chk2OptHandle opt,cgrNode top);

extern gsHandle remDead(cgrNode top);  //removing dead code 
extern gsHandle commu(cgrNode top);    //commutation   
extern gsHandle chkFunc(cgrNode top);  //checking function out of module
extern gsHandle chkProc(cgrNode top);  //checking processes and tasks
extern gsHandle ignThread(cgrNode top);//to ignore CTHREAD
extern gsHandle chkRec(cgrNode top);   //checking recursive call
extern gsHandle divVar(cgrNode top);   //variable declaration division 
extern gsHandle chkTdef(cgrNode top);  //checking typedef
extern gsHandle chkRange(cgrNode top); //checking range function
extern gsHandle chkEnum(cgrNode top);  //checking enum declaration
extern gsHandle chkClass(cgrNode top); //checking class declaration
extern gsHandle traRW(cgrNode top);    //translation read/write
extern gsHandle distri(cgrNode top);   //distribution (a*c+b*c)=c*(a+b)
extern gsHandle remDec(cgrNode top);   //removing variable declaration
extern gsHandle creVar(cgrNode top);   //variable name creation
extern gsHandle fnInte(cgrNode top);   //jointing function call interface
extern gsHandle exArr(cgrNode top);    //multi dimension array expansion
extern gsHandle tmpBit(cgrNode top);   //deciding bitwidth for temporaly variable
extern gsHandle traPtr(cgrNode top);   //pointer translation
extern gsHandle getSen(cgrNode top);   //getting sensitive
extern gsHandle creIns(cgrNode top);   //getting instance
extern gsHandle chkWait(cgrNode top);  //checking wait
extern gsHandle chkSub(cgrNode top);   //checking synthesizable
extern gsHandle setJump(cgrNode top);  //translation from control graph to FSMD
extern gsHandle n2d(cgrNode top);      //translation NFA to DFA
extern gsHandle preChkVar(cgrNode top);//prechecking variable declaration
extern gsHandle chkIfElse(cgrNode top);//checking if/else
extern gsHandle preChkSig(cgrNode top);//prechecking signal declaration
extern gsHandle setFunc(cgrNode top);  //setting function out of module
extern gsHandle remTask(cgrNode top);  //removing unused task
extern gsHandle remCast(cgrNode top);  //removing cast 
//making header and footer string
extern int mkHedFot(cgrNode top,char*toolname,char*outname,char*tm);

#define checkExSet1 {                                       \
    chkRun(distri);     /*distribution (a*c+b*c)=c*(a+b)*/  \
    chkRun(remDead);    /*removing dead code*/              \
    chkRun(commu);      /*commutation*/                     \
    chkRun(distri);     /*distribution (a*c+b*c)=c*(a+b)*/  \
    chkRun(remDead);    /*removing dead code*/              \
    chkRun(commu);      /*commutation*/                     \
    chkRun(distri);     /*distribution (a*c+b*c)=c*(a+b)*/  \
    chkRun(remDead);    /*removing dead code*/              \
    chkRun2(toSsa);     /*transration to SSA form*/         }

#endif

