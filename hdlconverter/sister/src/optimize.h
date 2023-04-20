/*
  optimization.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __SIS_OPTIMIZE_HEAD
#define __SIS_OPTIMIZE_HEAD

//optimization handler
typedef struct _optHandle{
    cgrNode proc;
    cgrNode state;
    cgrNode module;
}optHandle;

//optimization main
extern int optimize(optHandle opt,cgrNode top);
//default optimization handler
extern optHandle optDefault(void);
//getting handler value
#define optProp(self,item) (((optHandle*)(self->work))->item)
#endif


