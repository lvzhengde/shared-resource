/*
  to dump FSMD.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef DUMPFSMD_HEAD
#define DUMPFSMD_HEAD


//cleaning number
extern int stateNumClean(cgrNode node);
//state numbering
extern int stateNumSet(cgrNode node,int*num);
//to dump FSMD
extern void dumpFsmd(cgrNode top,char*filename);

#endif


