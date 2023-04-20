/*
  scheduling .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __SCHEDULE_HEAD
#define __SCHEDULE_HEAD

//scheduling hander
typedef struct _schHandle{
    int mul;
    int div;
    int adsb;
    int pipe;//reserved
    cgrNode dfg;
    cgrNode proc;
    cgrNode module;
}schHandle;

//default scheduling option
extern schHandle schOptDefault(void);

//scheduling
extern int schedule(schHandle sch,cgrNode top);

//setting connected node
#define schSeq(node) {                  \
    left=cgrGetNode(node,cgrKeyLeft);   \
    right=cgrGetNode(node,cgrKeyRight); \
    args=cgrGetNode(node,cgrKeyArgs);   \
    cond=cgrGetNode(node,cgrKeyCond);}
#define schSeqNext(node) {       \
    schSeq(node)                 \
    next=node->next; }

//checking output variables
#define schSwitchIDec(node,right,Function) { \
    switch(node->type){                      \
    case Inc :                               \
    case Dec:                                \
        if(right)ret&=Function(map,right,1); \
        else ret&=Function(map,left,1);      \
        return ret; }}
#define schSwitchCheck(node,flg){   \
    schSeq(node);                   \
    switch(node->type){             \
    case '=' :                      \
    case Mueq:                      \
    case Dieq:                      \
    case Moeq:                      \
    case Adeq:                      \
    case Sueq:                      \
    case Aneq:                      \
    case Xoeq:                      \
    case Oreq:                      \
    case Lseq:                      \
    case Rseq: flg=1; } } 
#define schSwitchFlg(node,flg,Function,ret) { \
    cgrNode left,right,cond,args;             \
    schSwitchCheck(node,flg);                 \
    schSwitchIDec(node,right,Function);       \
    if(left) ret&=Function(map,left,flg);     \
    if(right)ret&=Function(map,right,0);      \
    if(args) ret&=Function(map,args,0);       \
    if(cond) ret&=Function(map,cond,0); }

#endif

