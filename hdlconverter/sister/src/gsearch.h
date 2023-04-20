/*
  control graph searcher.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#ifndef GSEARCH_HEAD
#define GSEARCH_HEA

//struct
typedef struct _gsHandle{
    cgrNode top;
    cVector stack;
    cMstk   mstack;
    int (*root)(struct _gsHandle* self,cgrNode node);
    int (*module)(struct _gsHandle* self,cgrNode node);
    int (*visible)(struct _gsHandle* self,cgrNode node);
    int (*sig_dec)(struct _gsHandle* self,cgrNode node);
    int (*function)(struct _gsHandle* self,cgrNode node);
    int (*sc_ctor)(struct _gsHandle* self,cgrNode node);
    int (*argdec)(struct _gsHandle* self,cgrNode node);
    int (*datatype)(struct _gsHandle* self,cgrNode node);
    int (*pointer)(struct _gsHandle* self,cgrNode node);
    int (*cfg)(struct _gsHandle* self,cgrNode node);
    int (*vardec)(struct _gsHandle* self,cgrNode node);
    int (*dfg)(struct _gsHandle* self,cgrNode node);
    int (*cfgbreak)(struct _gsHandle* self,cgrNode node);
    int (*cfgcontinue)(struct _gsHandle* self,cgrNode node);
    int (*cfgreturn)(struct _gsHandle* self,cgrNode node);
    int (*cfgif)(struct _gsHandle* self,cgrNode node);
    int (*cfgelse)(struct _gsHandle* self,cgrNode node);
    int (*cfgfor)(struct _gsHandle* self,cgrNode node);
    int (*cfgdo)(struct _gsHandle* self,cgrNode node);
    int (*cfgwhile)(struct _gsHandle* self,cgrNode node);
    int (*cfgswitch)(struct _gsHandle* self,cgrNode node);
    int (*cfgcase)(struct _gsHandle* self,cgrNode node);
    int (*cfgblock)(struct _gsHandle* self,cgrNode node);
    int (*ope)(struct _gsHandle* self,cgrNode node);
    int (*funcall)(struct _gsHandle* self,cgrNode node);
    int (*args)(struct _gsHandle* self,cgrNode node);
    int (*id)(struct _gsHandle* self,cgrNode node);
    int (*dec_enum)(struct _gsHandle* self,cgrNode node);
    int (*dec_class)(struct _gsHandle* self,cgrNode node);
    int (*tdef)(struct _gsHandle* self,cgrNode node);
    int (*tag)(struct _gsHandle* self,cgrNode node);
    void* work;
}* gsHandle;

//util method
extern int gsCfg2(struct _gsHandle* self,cgrNode node);
extern int gsModule2(struct _gsHandle* self,cgrNode node);
extern int gsModule3(struct _gsHandle* self,cgrNode node);
extern int gsModule4(struct _gsHandle* self,cgrNode node);
extern int gsModule5(struct _gsHandle* self,cgrNode node);
extern int gsCfgBody(struct _gsHandle*self,cgrNode cfg);
extern int gsOpeBody(struct _gsHandle* self,cgrNode ope);
extern int gsFuncall2(struct _gsHandle* self,cgrNode node);
extern int gsFuncall3(struct _gsHandle* self,cgrNode node);
//checking task from funcall node
cgrNode gsCheckTask(gsHandle self,cgrNode node);

//else
extern int gsCfgElse(struct _gsHandle* self,cgrNode node);

//state counter
extern int gsCountState(cgrNode cfg);

//removing from stack if top is dfg node
void gsRemoveTopDfgFromStack(gsHandle self);

//to start analize
extern int gsRun(gsHandle gs);
//destroy
extern void gsDes(gsHandle gs);
//initialization
extern gsHandle gsIni(cgrNode top);
//to pop from the stack
extern cgrNode gsPop(gsHandle gs);
//to push to the stack
extern void gsPush(gsHandle gs,cgrNode node);
//to peek the stack
extern cgrNode gsPeek(gsHandle gs,int type);
extern cgrNode gsPeek2(gsHandle gs,int type1,int type2);
// top of stack element
cgrNode gsTop(gsHandle gs);
//dummy function
extern int gsDummy(gsHandle self,cgrNode node);
//searching operator node
#define gsOpeLinkEq(node) {                       \
    self->ope(self,cgrGetNode(node,cgrKeyRight)); \
    self->ope(self,cgrGetNode(node,cgrKeyLeft));  }
#define gsOpeLink(node) {                         \
    self->ope(self,cgrGetNode(node,cgrKeyCond));  \
    self->ope(self,cgrGetNode(node,cgrKeyLeft));  \
    self->ope(self,cgrGetNode(node,cgrKeyRight)); }

#endif


