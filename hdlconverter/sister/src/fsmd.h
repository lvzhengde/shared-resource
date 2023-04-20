/*
  FSMD access utiliteis.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/


#ifndef __FSMD_HEAD
#define __FSMD_HEAD

//
//FSMD accessing class
//
typedef struct _fsmdHandle{
    cgrNode top;
    void* work;
    int (*run)(struct _fsmdHandle*self);
    int (*module)(struct _fsmdHandle*self,cgrNode node);
    int (*inst)(struct _fsmdHandle*self,cgrNode node);
    int (*sigdec)(struct _fsmdHandle*self,cgrNode node);
    int (*signal)(struct _fsmdHandle*self,cgrNode node);
    int (*proc)(struct _fsmdHandle*self,cgrNode node);
    int (*state)(struct _fsmdHandle*self,cgrNode node);
    int (*ctrl)(struct _fsmdHandle*self,cgrNode node);
    int (*dfg)(struct _fsmdHandle*self,cgrNode node);
    int (*ope)(struct _fsmdHandle*self,cgrNode node);
    int (*id)(struct _fsmdHandle*self,cgrNode node);
    int (*sen)(struct _fsmdHandle*self,cgrNode node);
    int (*pos)(struct _fsmdHandle*self,cgrNode node);
    int (*neg)(struct _fsmdHandle*self,cgrNode node);
    int (*funcall)(struct _fsmdHandle*self,cgrNode node);
}* fsmdHandle;

//distruction
extern void fsmdDes(fsmdHandle fsmd);
//initialization
extern fsmdHandle fsmdIni(cgrNode top);
//dummy
extern int fsmdDummy(fsmdHandle self,cgrNode node);
 
//
//checking FSMD search route
//
//reset
extern cMap fsmdRouteReset(void);
//clear
extern void fsmdRouteClear(void);
//setting node
extern void fsmdRouteSet(cgrNode node);
//checking node
extern int fsmdRouteCheck(cgrNode node);
//to push to stack and new route creation
extern cMap fsmdRoutePush(void);
//to pop from stack 
extern cMap fsmdRoutePop(void);

//
//unique name cleation
//
//clear
extern void fsmdNameClear(void);
//reset
extern cMap fsmdNameReset(void);
//set
extern void fsmdNameSet(char*name);
//new name cleation
extern cgrNode fsmdNameCreate(char*prefix,char*file,
    int line,int size,int type,int signal,char*opt);

//
//others
//
//removing node
extern int fsmdRemoveNode(cgrNode node);
//copying operator  node
extern cgrNode fsmdCopyOpe(cgrNode node);
//setting jump node
extern void fsmdSetJump(cgrNode node0,cgrNode node1);
//removing jump node
extern int fsmdRemoveJump(cgrNode node0,cgrNode node1);
//removing state node
extern int fsmdRemoveState(cgrNode proc,cgrNode node);

//copying addribute
#define fsmdCopyAttr(type,node0,node1,key)         \
    cgrSet##type(node0,key,cgrGet##type(node1,key))\
//searching operator node
#define fsmdOpeLinkEq(node,ret) {                      \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyRight)); \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyLeft));  }
#define fsmdOpeLink(node,ret) {                        \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyArgs));  \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyCond));  \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyLeft));  \
    ret|=self->ope(self,cgrGetNode(node,cgrKeyRight)); }

#endif

