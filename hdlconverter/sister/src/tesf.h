/*
  fsmd test handler .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __TESF_HEAD
#define __TESF_HEAD

//test handler
typedef struct{
    void*work;
    cgrNode state;
    int (*function)(fsmdHandle,cgrNode);
    cVector nvect;
}tesfHandle;
//test handler accessing
#define tesfProp(self,item) \
    (((tesfHandle*)(self->work))->item)
#define tesfItem(self,type,item) \
    (((type*)tesfProp(self,work))->item)
#endif
