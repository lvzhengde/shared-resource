/*
  sister header.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef SISTER_HEAD
#define SISTER_HEAD

#include "generic.h"

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>

//preprocessing
//0:normally,1:executing and stop,-1:error
extern int preprocess2(char*self,cVector files,char prepro);
extern int preprocess3(char*self,cVector files,char prepro,int* fd);
extern void closePreprocess3(int fd);

//find file/directory path
extern char* findPath(char* path);
#endif

