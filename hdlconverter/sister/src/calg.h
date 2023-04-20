/****************************************************************************************

      FILE  : calg.h
      Copyright (C)KTDesignSystems ,all rights reserved .

    #Lisence (BSD Style)_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

     Redistribution and use in source and binary forms, with or without
     modification, are permitted provided that the following conditions
     are met:

     Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
     Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

     THIS SOFTWARE IS PROVIDED BY THE FREEBSD PROJECT ``AS IS'' AND ANY
     EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
     PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR
     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
     LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
     USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
     ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
     OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
     OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
     SUCH DAMAGE.;

    #UpDate_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    Version    Date          Comment
    ----------------------------------------------------------------------
    1.05.10    2008/02/14     cMstk was added
    1.04.09    2007/05/10     Some macros ware added
    1.04.07    2007/03/19     Some functions are added
    1.04.06    2006/10/07     Update cMap
    1.04.02    2006/08/18     Added cMessage
    1.04.01    2006/08/16     Added cStack and cQueue
    1.03.00    2006/07/28     All functions were into the include file
    1.02.03    2006/05/08     Updated cString
    1.02.02    2006/04/19     Updated cIo
                              Updated cString
    1.02.01    2006/04/18     Updated cIo (cioOpen,cioClose)
                              Updated cIo (add float format)
                              Updated cString (cstrSet,error operation)
                              Updated cVector(push element)
    1.02.00    2006/04/17     Added cIo
                              Added cString
    1.01.01    2006/04/15     Updated cVector
    1.01.00    2006/04/14     First release.
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
****************************************************************************************/
#ifndef __CALD_H
#define __CALD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*========================================================================================
    -----cIo-----
==========================================================================================

    This library is set of Input and Output format to standard IO steram.
These format are used for debug chiefly.

========================================================================================*/

//#define USECIOMEM /*use cio malloc*/

//typedef
typedef short sint;
typedef long  lint;
typedef long long llint;
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned long  int ulint;
typedef unsigned short int usint;

//cast
#define cioVoid(val,type)  (*((type*)(val)))
#define cioAddr(val)  (&(val))

//file io
//Openfile (ex. : cioOpen(fpo,"out.txt","w",return -1); etc...)
#define cioOpen(fp,fname,mode,errope) \
    {if(!(fp=fopen(fname,mode))){perror(fname);errope;}}
#define cioClose(fp) fclose(fp);

//time format
#define CIO_SEC (clock()/CLOCKS_PER_SEC)
#define cioPtime() {time_t tp;tp=time(NULL);printf("%s\n",asctime(localtime(&tp)));}

//standard io
#define CIO_EL '\n'
#define CIO_ES '\0'
#define CIO_FORMAT_INT   "%d"
#define CIO_FORMAT_INTL  "%d\n"
#define CIO_FORMAT_FLT   "%f"
#define CIO_FORMAT_FLTL  "%f\n"
#define CIO_FORMAT_STR   "%s"
#define CIO_FORMAT_STRL  "%s\n"
#define CIO_FORMAT_HEX   "%x"
#define CIO_FORMAT_HEXL  "%x\n"

#define CIO_FORMAT_MESINT   "%s : %d"
#define CIO_FORMAT_MESINTL  "%s : %d\n"
#define CIO_FORMAT_MESFLT   "%s : %f"
#define CIO_FORMAT_MESFLTL  "%s : %f\n"
#define CIO_FORMAT_MESSTR   "%s : %s"
#define CIO_FORMAT_MESSTRL  "%s : %s\n"
#define CIO_FORMAT_MESHEX   "%s : %x"
#define CIO_FORMAT_MESHEXL  "%s : %x\n"

#define CIO_OK  puts("ok") //for debug
#define CIO_OK2 puts("ok2")
#define CIO_OK3 puts("ok3")

#define cioPint(x)  printf(CIO_FORMAT_INT,x)
#define cioPintl(x) printf(CIO_FORMAT_INTL,x)
#define cioPflt(x)  printf(CIO_FORMAT_FLT,x)
#define cioPfltl(x) printf(CIO_FORMAT_FLTL,x)
#define cioPstr(x)  printf(CIO_FORMAT_STR,x)
#define cioPstrl(x) printf(CIO_FORMAT_STRL,x)
#define cioPhex(x)  printf(CIO_FORMAT_HEX,x)
#define cioPhexl(x) printf(CIO_FORMAT_HEXL,x)

#define cioPintm(mes,x)  printf(CIO_FORMAT_MESINT,mes,x)
#define cioPintlm(mes,x) printf(CIO_FORMAT_MESINTL,mes,x)
#define cioPfltm(mes,x)  printf(CIO_FORMAT_MESFLT,mes,x)
#define cioPfltlm(mes,x) printf(CIO_FORMAT_MESFLTL,mes,x)
#define cioPstrm(mes,x)  printf(CIO_FORMAT_MESSTR,mes,x)
#define cioPstrlm(mes,x) printf(CIO_FORMAT_MESSTRL,mes,x)
#define cioPhexm(mes,x)  printf(CIO_FORMAT_MESHEX,mes,x)
#define cioPhexlm(mes,x) printf(CIO_FORMAT_MESHEXL,mes,x)

#define cioSint(x)  scanf(CIO_FORMAT_INT,&x)
#define cioSflt(x)  scanf(CIO_FORMAT_FLT,&x)
#define cioSstr(x)  scanf(CIO_FORMAT_STR,x)
#define cioShex(x)  scanf(CIO_FORMAT_HEX,&x)

#define cioSintm(mes,x)  {cioPstr(mes);cioSint(x);}
#define cioSfltm(mes,x)  {cioPstr(mes);cioSflt(x);}
#define cioSstrm(mes,x)  {cioPstr(mes);cioSstr(x);}
#define cioShexm(mes,x)  {cioPstr(mes);cioShex(x);}








/*==========================================================================================
    -----cVector-----
============================================================================================
    This library is vector algorithm for C-language.
    Vector algorithm is array that can change array size freely.
    ex.) main function
    int main(int argc,char**argv)
    {
        int a=111;
        int*b;
        cVector vect;
        vect=cvectorIni(sizeof(int),4);
        cvectorInst(vect,10,&a);
        b=cvectorFind(vect,10);
        printf("%d \n",*b);
        cvectorDes(vect);
        return 0;
    }
==========================================================================================*/
typedef struct _CVECTOR{
    void* mainArray;//main vector array.
    int size; //main array size.
    int objSize; //object size of element.
    int maxElem; //maximam position.
    int len; //maxElem+1.
    int CVECTOR_BASEARRAY;
}CVECTOR;
typedef CVECTOR* cVector;
//initialise vector-----------------------------------------------------------------------
static cVector cvectorIni(int objSize,int baseArraySize);
#define cvectIni cvectorIni
//insert element--------------------------------------------------------------------------
static void* cvectorInst(cVector vect,int index,void*elem);
#define cvectInst cvectorInst
#define cvectSet(vect,index,elem) cvectInst(vect,index,&(elem))
//find element----------------------------------------------------------------------------
static void* cvectorFind(cVector vect,int index);
#define cvectFind cvectorFind
#define cvectElt(vect,index,type) cioVoid(cvectFind((vect),(index)),type)
#define cvectGet cvectElt
//destruct vector-------------------------------------------------------------------------
static void* cvectorDes(cVector vect);
#define cvectDes cvectorDes
//return array size-----------------------------------------------------------------------
#define cvectorGetSize(vect) (vect->len)
#define cvectGetSize cvectorGetSize
#define cvectSize cvectGetSize
//push element----------------------------------------------------------------------------
#define cvectorPush(vect,elem) cvectorInst(vect,cvectGetSize(vect),elem);
#define cvectPush cvectorPush
#define cvectAppend(vect,elem) cvectPush(vect,&(elem))
#define cvectAdd cvectAppend
//pop element----------------------------------------------------------------------------
static void* cvectorPop(cVector vect);
#define cvectPop cvectorPop
#define cvectSub(vect,type) cioVoid(cvectPop(vect),type)
//resize---------------------------------------------------------------------------------
#define cvectorResize(vct,size) {(vct->len=size);(vct->maxElem=i-1);}
#define cvectResize cvectorResize
//searching array------------------------------------------------------------------------
#define cvectMaps(__vect__,__type__,__function__) {      \
    int __i__;                                           \
    for(__i__=0;__i__<cvectSize(__vect__);__i__++){      \
        __type__ val;                                    \
        val=cvectGet(__vect__,__i__,__type__);           \
        __function__; }                                  }


/*========================================================================================
    -----cStack-----
==========================================================================================
    This library is stack(LIFO) data structure for C-language.
    This use cVector library.
    
    ex.) main function
    int main(int argc,char**argv){
        int i,x;
        cStack vct;
    
       vct=cstackIni(sizeof(int));
       x=10;
        cstackPush(vct,&x);
       x=5;
       cstackPush(vct,&x);
       x=*((int*)cstackPop(vct));
       cioPintl(x);
       for(i=0;i<cstackGetSize(vct);i++)
           cioPintl(*((int*)cvectFind(vct,i)));
       return 0;
    }

========================================================================================*/
typedef cVector cStack;
#define __DEF_CSTACK_SIZE 16
#define cstackIni(objSize) (cvectIni(objSize,__DEF_CSTACK_SIZE))
#define cstackPush  cvectPush
#define cstackPop   cvectPop
#define cstackGetSize cvectGetSize
#define cstackDes     cvectDes
#define cstackFind    cvectFind





/*========================================================================================
    -----cQueue-----
==========================================================================================
    This library is queue(FIFO) data structure for C-language.
    ex.) main function
    int main(int argc,char**argv)
    {
        int x;
        cQueue test;
        test=cqueueIni(sizeof(int));
        x=10;
        cqueueEnq(test,&x);
        x=11;
        cqueueEnq(test,&x);
        cioPintl(*((int*)cqueueDeq(test)));
        cioPintl(*((int*)cqueueDeq(test)));
        cqueueDes(test);
        return 0;
    }
========================================================================================*/
#define CQUEUE_BUFSIZE 100
typedef struct _CQUEUE_ELEMENT{
 void*elem; //value 
 struct _CQUEUE_ELEMENT * next; //next value
 struct _CQUEUE_ELEMENT * prev; //prev value
}CQUEUE_ELEMENT;
typedef struct{
    CQUEUE_ELEMENT* head;  //head of list
    CQUEUE_ELEMENT* tail;  //tail of list
    CQUEUE_ELEMENT* garbage; //buffer pool
    CQUEUE_ELEMENT* buflist; //buffer pool
    int bufpos;   //buffer position
    int objSize;  //object size
    int size;     //queue size
}CQUEUE;
typedef CQUEUE* cQueue;
//create new queue-----------------------------------------------------------------------
static cQueue cqueueIni(int objSize);
//destruct queue-------------------------------------------------------------------------
static cQueue cqueueDes(cQueue que);
//make buffer----------------------------------------------------------------------------
static CQUEUE_ELEMENT* cqueueMakeBuf(cQueue que);
//enqueue -------------------------------------------------------------------------------
static void* cqueueEnq(cQueue que,void*val);
//dequeue -------------------------------------------------------------------------------
static void* cqueueDeq(cQueue que);





/*========================================================================================
    -----cMap-----
==========================================================================================
    This library is hash algorithm for C-language.
    If you want to use this library,you must to preparation the 'key' function and
    'compare' function.
    ex1) key function
     int key(void*name)
     {
         return ((int*)name)[0];
     }
    ex2) compare function
     int cmp(void*A,void*B)
     {
         if(strcmp((char*)A,(char*)B)==0) return 1;
         return 0;
     }
    those function are used by this library for inquiry in hash.
    ex3) main function
    typedef struct _TEST{
        char name[100];
        int  result;
    }TEST;
    int main(int argc,char**argv)
    {
        int i;
        cMap test;
        TEST a={"usami" ,44};
        TEST b={"sugiki",70};
        
        test=cmapNew(sizeof(char[100]),sizeof(TEST),(*key),(*cmp));
        cmapInst(test,a.name,&a);
        cmapInst(test,b.name,&b);
        printf("%d\n",((TEST*)cmapFind(test,"sugiki"))->result);
        a.result=60;
        cmapUpdate(test,"sugiki",&a);
        printf("%d\n",((TEST*)cmapFind(test,"sugiki"))->result);
        cmapDes(test);
        return 0;
    }
========================================================================================*/
#define CMAP_MAINARRAY_SIZE 233
#define CMAP_BUFFER_SIZE 100
typedef struct _CMAP_ELEMENT{ //elemnt of map.
    void*key;
    void*element;//substance of element
    struct _CMAP_ELEMENT*next;//next pointer of list.
}CMAP_ELEMENT;
typedef struct _CMAP{
    int  elementNum; //cMap number of element.
    CMAP_ELEMENT** mainArray;  //cMap Main Array.
    int  bufPosition; //present place of buffer list.
    CMAP_ELEMENT* bufList; //buffer list.
    CMAP_ELEMENT* bufPool; //buffer pool for mem free.
    CMAP_ELEMENT* flagList;//list of fragments
    int nFlag; //size of flagment list
    int objSize; //object size of element
    int keySize; //object size of key
    int (*key)(void*a);//integer key(specification by user).
    int (*cmp)(void*a,void*b);//function of compare element(specification by user).
    cVector elmlist;//key list;
}CMAP;
typedef CMAP* cMap;
//create new cMap-------------------------------------------------------------------------
static cMap cmapNew(int keySize,int objSize,
    int (*cMapKey)(void*a),int (*cMapCmp)(void*a,void*b));
#define cmapIni cmapNew
//get new node----------------------------------------------------------------------------
static CMAP_ELEMENT* cmapNewNode(cMap map);
//insert element--------------------------------------------------------------------------
static void* cmapInst(cMap map,void*key,void*element);
#define cmapAdd(map,key,elm) cmapInst(map,&key,&elm)
//find element----------------------------------------------------------------------------
static void* cmapFind(cMap map,void*key);
#define cmapGet(map,key,type) cioVoid(cmapFind(map,&key),type)
//update element--------------------------------------------------------------------------
static void* cmapUpdate(cMap map,void*key,void*element);
#define cmapSet(map,key,elm) cmapUpdate(map,&key,&elm)
//delete element--------------------------------------------------------------------------
static void* cmapDel(cMap map,void*key);
#define cmapRem(map,key) cmapDel(map,&key)
//destruct cMap--------------------------------------------------------------------------
static void* cmapDes(cMap map);
//map element size
static int cmapSize(cMap map);
//searching map--------------------------------------------------------------------------
#define cmapMaps(__map__,__ktype__,__etype__,__function__){     \
    int __i__;                                                  \
    cVector __vect__=__map__->elmlist;                          \
    for(__i__=0;__i__<cvectSize(__vect__);__i__++){             \
        __ktype__ key    =cvectGet(__vect__,__i__,__ktype__);   \
        __etype__*__val__=cmapFind(__map__,&key);               \
        if(__val__){                                            \
        __etype__ val=*__val__;                                 \
        __function__;                                           \
        }                                                       \
    }                                                           \
}
//default key and cmp function definition ------------------------------------------------
static int cmapIntKey(void* key);      //integer key
static int cmapIntCmp(void* a,void*b); //integer cmp
static int cmapStrKey(void* key);      //string  key
static int cmapStrCmp(void* a,void*b); //string  cmp






/*========================================================================================
    -----cMstk-----
==========================================================================================
    This library is Map into Stack algorithm.
    
    ex.)
    int a[]={0,1};
    int b[]={6,7};

    cMstk mstk=cmstkNew(sizeof(int));
    
    cmstkSet(mstk,"0",a[0],int);
    cmstkSet(mstk,"1",a[1],int);
    print("%d\n",cmstkGet(mstk,"0",int));
    cmstkPush(mstk);
    
    cmstkSet(mstk,"0",b[0],int);
    cmstkSet(mstk,"1",b[1],int);
    print("%d\n",cmstkGet(mstk,"0",int));
    cmstkPop(mstk);

    print("%d\n",cmstkGet(mstk,"0",int));
    cmstkDes(mstk);
 

======================================================================================= */
typedef cVector cMstk;
#define cmstkSize cvectSize
//new cMstk creation ------------------------------------------------
static cMstk cmstkNew(int objSize);
//destruction -------------------------------------------------------
static void cmstkDes(cMstk obj);
//push --------------------------------------------------------------
static void cmstkPush(cMstk obj);
//pop ---------------------------------------------------------------
static void cmstkPop(cMstk obj);
//set ---------------------------------------------------------------
static void cmstkInst(cMstk obj,char*key,void*val);
#define cmstkSet(obj,key,val) cmstkInst(obj,key,&(val))
//get ---------------------------------------------------------------
static void* cmstkFind(cMstk obj,char*key);
#define cmstkGet(obj,key,type) cioVoid(cmstkFind(obj,key),type)
//rem ---------------------------------------------------------------
static void cmstkDel(cMstk obj,char*key);









/*========================================================================================
    -----cString-----
==========================================================================================
    These macro are set of operation to character string.
    These function make buffer to dynamic
    ex.) cstrSet(x,"test test",return NULL);
========================================================================================*/
typedef char* cStr;
//return string size----------------------------------------------------------------------
#define cstrGetSize(x) strlen(x)
//Check buffer----------------------------------------------------------------------------
#define cstrEbuf(x,errope) {if(!x){perror("memory"); errope;}}
//Make string buffer----------------------------------------------------------------------
#define cstrMakeBuf(x,size,type,errope) {x=(type*) \
    calloc(size,sizeof(type));cstrEbuf(x,errope);}
//reMake string buffer--------------------------------------------------------------------
#define cstrRemakeBuf(x,size,type,errope) {x=(type*) \
    realloc(x,sizeof(type)*(size));cstrEbuf(x,errope);}
//Set charactor string--------------------------------------------------------------------
#define cstrSet(x,y,errope) {if(y==NULL)x=NULL; \
    else{cstrMakeBuf(x,cstrGetSize(y)+1,char,errope);strcpy(x,y);}}
//Connect string--------------------------------------------------------------------------
#define cstrCat(x,y,errope) {if((x!=NULL)&&(y!=NULL))\
    {cstrRemakeBuf(x,cstrGetSize(x)+cstrGetSize(y)+2,char,errope);strcat(x,y);}}
//distruct string buffer------------------------------------------------------------------
#define cstrFree(x) free(x);
//split string---------------------------------------------------------------------------
static char** cstrSplit(char*str,char*ct);
//memory error---------------------------------------------------------------------------
#define cstrMemError(mem) {fprintf(stderr,"error:memory shortage:%s\n",mem);exit(-1);}










/*========================================================================================
    -----cMessage-----
==========================================================================================
    This library is collection of message for C-language.
    They have error and warning message format,and etc...

    ex.)main function
    int main(int argc,char**argv) {
        int x;
        cMsg mg;
        cMsgTbl tbl[]={ //message table
            {0,__CMSG_ERROR  ,0,"%s",NULL},
            {1,__CMSG_WARNING,1,"%s",NULL}};
        mg=cmsgIni(tbl,2);
        cmsgEcho(mg,0,"test");
        cmsgPrintEW(mg);
        cmsgDes(mg);
        return 0;
    }

======================================================================================== */

/*kind of message*/
enum __CMSG_TYPE{
    __CMSG_NOR,    //normality
    __CMSG_ERROR,  //error
    __CMSG_WARNING,//warning
};
/*cMessage table*/
typedef struct{
    int   index; //table index
    int   type;  //message type(__CMSG_TYPE)
    int   code;  //message code
    char* form;  //message format
    void (*operation)(void);//error handling operation
}CMSG_TABLE;
#define cmsgTblSize(tbl) (sizeof(tbl)/sizeof(CMSG_TABLE))

typedef CMSG_TABLE cMsgTbl;

/*cMessage handle*/
typedef struct{
    int size;//table size;
    int errflg;//error counter
    int wanflg;//warning counter
    FILE* dev; //output buffer 
    cMsgTbl* msgtbl;//message table    
}CMSG;
typedef CMSG* cMsg;

//create new cMessage handle--------------------------------------------------------------
static cMsg cmsgIni(cMsgTbl* tbl,int size);
//free cMessage handle--------------------------------------------------------------------
#define cmsgDes(mg) (free(mg))
//print message --------------------------------------------------------------------------
static void cmsgEcho(cMsg mg,int number,...);

#define cmsgGetSize(mg)    (mg->size)
#define cmsgGetErr(mg)     (mg->errflg)
#define cmsgGetWan(mg)     (mg->wanflg)
#define cmsgSetBuf(mg,buf) (mg->dev=buf)
#define cmsgPrintEW(mg) fprintf(mg->dev,\
    "%d error(s),%d warning(s).\n",mg->errflg,mg->wanflg)




/*======================================================================================//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_///
//                                                                                      //
//                       under here is core Functions for CALG                          //
//                                                                                      //
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_///
//======================================================================================*/

/***************************************************************************************
 
 -- cmap functions

***************************************************************************************/
//integer key
static int cmapIntKey(void* key){
    return cioVoid(key,int);
}
//integer cmp
static int cmapIntCmp(void* a,void*b){
    return cioVoid(a,int)==cioVoid(b,int);
}

//string key
static int cmapStrKey(void* key){
    if(cioVoid(key,char*)[0]==CIO_ES) return CIO_ES+137;
    return cioVoid(key,char*)[0]+cioVoid(key,char*)[1]+137;
}
//string cmp
static int cmapStrCmp(void* a,void*b){
    return !strcmp(cioVoid(a,char*),cioVoid(b,char*));
}
//make buffer
static CMAP_ELEMENT* cmapMakeBuffer(int keySize,int objSize){
    int i;
    CMAP_ELEMENT* ellist;
    void *usrlist;
    void *keylist;
    
    ellist=(CMAP_ELEMENT*)malloc(CMAP_BUFFER_SIZE*sizeof(CMAP_ELEMENT));
    if(ellist==NULL) return NULL;
    usrlist=calloc(CMAP_BUFFER_SIZE,objSize);
    if(usrlist==NULL){
        free(ellist);
        return NULL;
    }
    keylist=calloc(CMAP_BUFFER_SIZE,keySize);
    if(usrlist==NULL){
        free(usrlist);
        free(ellist);
        return NULL;
    }
    for(i=0;i<CMAP_BUFFER_SIZE;i++){
        ellist[i].next=NULL;
        ellist[i].element=usrlist+(objSize*i);
        ellist[i].key=keylist+(keySize*i);
    }
    return ellist;
}
//create new cMap-------------------------------------------------------------------------
static cMap cmapNew(int keySize,int objSize,
    int (*cMapKey)(void*a),int (*cMapCmp)(void*a,void*b))
{
    cMap map;
    
    map=(cMap)malloc(sizeof(CMAP));
    if(map==NULL)
    {
        perror("error memory");
        return NULL;
    }
    map->elementNum=0;
    map->mainArray=(CMAP_ELEMENT**)calloc(CMAP_MAINARRAY_SIZE,sizeof(CMAP_ELEMENT));
    if(map->mainArray==NULL)
    {
        perror("error memory");
        free(map);
        return NULL;
    }
    map->bufPosition=1;
    map->bufList=cmapMakeBuffer(keySize,objSize);
    map->bufPool=map->bufList;
    if(map->bufList==NULL)
    {
        perror("error memory");
        free(map->mainArray);
        free(map);
        return NULL;
    }
    map->flagList=NULL;
    map->nFlag=0;
    map->objSize=objSize;
    map->keySize=keySize;
    map->key=cMapKey;
    map->cmp=cMapCmp;
    map->elmlist=cvectIni(keySize,10);
    return map;
}
//get new node----------------------------------------------------------------------------
static CMAP_ELEMENT* cmapNewNode(cMap map)
{
    CMAP_ELEMENT* node;
    
    if(!map) return NULL;
    map->elementNum++;
    if(map->nFlag>0){
        node=map->flagList;
        map->flagList=map->flagList->next;
        node->next=NULL;
        map->nFlag--;
        return node;
    }
    if(map->bufPosition==CMAP_BUFFER_SIZE){
        map->bufPosition=1;
        map->bufList=cmapMakeBuffer(map->keySize,map->objSize);
        map->bufList[0].next=map->bufPool;
    map->bufPool=map->bufList;
    if(map->bufList==NULL){
            map->elementNum--;
            return NULL;
        }
    }
    node=(map->bufList)+(map->bufPosition);
    (map->bufPosition)++;
    
    return node;
}
//insert element--------------------------------------------------------------------------
static void* cmapInst(cMap map,void* key,void*element)
{
    int intkey;
    CMAP_ELEMENT* cel;
    
    if(!map) return NULL;
    intkey=map->key(key)%CMAP_MAINARRAY_SIZE;
    cel=cmapNewNode(map);
    memcpy((char*)(cel->element),(char*)element,map->objSize);
    memcpy((char*)(cel->key),(char*)key,map->keySize);
    cel->next=map->mainArray[intkey];
    map->mainArray[intkey]=cel;
    cvectPush(map->elmlist,cel->key);
    return cel->element;
}

//find element----------------------------------------------------------------------------
static void* cmapFind(cMap map,void*key)
{
    int intkey;
    CMAP_ELEMENT* cel;
    
    if(!map) return NULL;
    intkey=(map->key(key)%CMAP_MAINARRAY_SIZE);
    cel=map->mainArray[intkey];
    
    while(cel!=NULL){
        if(map->cmp(cel->key,key)) return cel->element;
        cel=cel->next;
    }
    return NULL;
}
//update element--------------------------------------------------------------------------
static void* cmapUpdate(cMap map,void*key,void*element)
{
    void*celelement;
    
    if(!map) return NULL;
    celelement=cmapFind(map,key);
    if(celelement==NULL) celelement=cmapInst(map,key,element);
    else memcpy((char*)(celelement),(char*)element,map->objSize);
    
    return celelement;
}
//delete element--------------------------------------------------------------------------
static void* cmapDel(cMap map,void*key)
{
    int intkey;
    CMAP_ELEMENT* cel;
    CMAP_ELEMENT* pre;
    
    if(!map) return NULL;
    intkey=(map->key(key)%CMAP_MAINARRAY_SIZE);
    cel=map->mainArray[intkey];
    pre=cel;
    while(cel!=NULL){
        if(map->cmp(cel->key,key)){
            if(cel==map->mainArray[intkey]) map->mainArray[intkey]=cel->next;
            else  pre->next=cel->next;
            break;
        }
        pre=cel;
        cel=cel->next;
    }
    if(cel!=NULL){
        cel->next=map->flagList;
        map->flagList=cel;
        map->nFlag++;
    }
    map->elementNum--;
    return NULL;
}
//destruct cMap--------------------------------------------------------------------------
static void* cmapDes(cMap map)
{
    CMAP_ELEMENT* bufPool;
    CMAP_ELEMENT* tmpbuf;

    if(map==NULL) return NULL;
    if(map->mainArray!=NULL) free(map->mainArray);
    if(map->bufPool!=NULL){
        bufPool=map->bufPool;
    while(bufPool){
        tmpbuf=bufPool;
        bufPool=bufPool->next;
            free(tmpbuf);
    }
    }
    cvectDes(map->elmlist);
    free(map);
    return NULL;
}
//number of element-----------------------------------------------------------------
static int cmapSize(cMap map){
    int n=0;
    int i;
    cVector vect=map->elmlist; 
    for(i=0;i<cvectSize(vect);i++){ 
        void* key=cvectFind(vect,i);
        if(cmapFind(map,key)) n++;
    }
    return n;
}

/****************************************************************************************

-- cVector Functions

****************************************************************************************/
#define CVECTOR_BASEARRAY_DEFAULT 200
//initialise vector----------------------------------------------------------------------
static cVector cvectorIni(int objSize,int baseArraySize)
{
    cVector vect;
    
    vect=(cVector)malloc(sizeof(CVECTOR));
    if(vect==NULL){
        perror("error memory");
        return NULL;
    }
    if(baseArraySize>0) vect->CVECTOR_BASEARRAY=baseArraySize;
    else vect->CVECTOR_BASEARRAY=CVECTOR_BASEARRAY_DEFAULT;
    vect->objSize=objSize;
    vect->mainArray=calloc(vect->CVECTOR_BASEARRAY,objSize);
    if(vect->mainArray==NULL){
        free(vect);
        perror("error memory");
        return NULL;
    }
    vect->size=vect->CVECTOR_BASEARRAY;
    vect->maxElem=-1;
    vect->len=vect->maxElem+1;
    return vect;
}
//insert element--------------------------------------------------------------------------
static void* cvectorInst(cVector vect,int index,void*elem)
{
    if(index<0) return NULL;
    if(vect->maxElem < index){
        vect->maxElem=index;
        vect->len=vect->maxElem+1;
    }
    if(index >= vect->size){
        while(index >= vect->size) vect->size*=2;
        vect->mainArray=realloc(vect->mainArray,vect->objSize*vect->size);
        if(vect->mainArray==NULL){
            perror("error memory");
            vect->maxElem=-1;
            vect->len=vect->maxElem+1;
            vect->size;
            return NULL;
        }
    }
    memcpy((vect->mainArray+(vect->objSize*index)),elem,vect->objSize);
    return elem;
}
//find element----------------------------------------------------------------------------
static void* cvectorFind(cVector vect,int index)
{
    if(index<0) return NULL;
    if((vect->maxElem < index) || (vect->size <= index)) return NULL;
    return vect->mainArray+(vect->objSize*index);
}
//destruct vector-------------------------------------------------------------------------
static void* cvectorDes(cVector vect)
{
    if(vect==NULL) return NULL;
    if(vect->mainArray!=NULL) free(vect->mainArray);
    free(vect);
    return NULL;
}
//pop element-------------------------------------------------------------------------
static void* cvectorPop(cVector vect){
    void*elm;
    elm=vect->mainArray+(vect->objSize*vect->maxElem);
    vect->maxElem--;
    vect->len--;
    return elm;
}


/****************************************************************************************

 -- cQueue Functions

****************************************************************************************/

//create new queue-----------------------------------------------------------------------
static cQueue cqueueIni(int objSize){
   cQueue que;
   que=(cQueue)malloc(sizeof(CQUEUE));
   if(!que){
       perror("memory");
       return NULL;
   }
   que->head=NULL;
   que->tail=NULL;
   que->buflist=NULL;
   que->garbage=NULL;
   que->bufpos=CQUEUE_BUFSIZE;
   que->objSize=objSize;
   que->size=0;
   return que;
}
//destruct  queue-----------------------------------------------------------------------
static cQueue cqueueDes(cQueue que){
    CQUEUE_ELEMENT * elem;
    CQUEUE_ELEMENT * tmpelem;
    if(!que) return NULL;
    elem=que->buflist;
    while(elem){
        tmpelem=elem;
    elem=elem->next;
    if(tmpelem[1].elem)
        free(tmpelem[1].elem);
    free(tmpelem);
    }
    free(que);
    return que;
}
//enqueue--------------------------------------------------------------------------------
static void* cqueueEnq(cQueue que,void*val){
   CQUEUE_ELEMENT*elm;
   elm=cqueueMakeBuf(que);
   if(!elm)return NULL;
   memcpy(elm->elem,val,que->objSize);
   if(!que->head){
       que->head=elm;
       que->tail=elm;
   }else{
       que->head->prev=elm;
       elm->next=que->head;
       que->head=elm;
       elm->prev=NULL;
   }
   que->size++;
   return elm->elem;
}
//dequeue -------------------------------------------------------------------------------
static void* cqueueDeq(cQueue que){
   CQUEUE_ELEMENT*elm;
   if(!que) return NULL;
   elm=que->tail;
   if(!elm) return NULL;
   que->tail=elm->prev;
   if(!que->tail) que->head=NULL;
   else que->tail->next=NULL;
   elm->prev=NULL;
   elm->next=que->garbage;
   que->garbage=elm;
   que->size--;
   return elm->elem;
}
//make buffer----------------------------------------------------------------------------
static CQUEUE_ELEMENT* cqueueMakeBuf(cQueue que){
    CQUEUE_ELEMENT*elm;
    if(!que) return NULL;
    if(que->garbage){
        elm=que->garbage;
    que->garbage=elm->next;
    }else{
        if(que->bufpos>=CQUEUE_BUFSIZE){
            int i;
        void*obj;
        elm=(CQUEUE_ELEMENT*)
            calloc(CQUEUE_BUFSIZE,sizeof(CQUEUE_ELEMENT));
        if(!elm){
            perror("memory");
            return NULL;
        }
        obj=(void*)calloc(CQUEUE_BUFSIZE-1,que->objSize);
        if(!obj){
            perror("memory");
        free(elm);
            return NULL;
        }
        elm[0].next=que->buflist;
        que->buflist=elm;
        que->bufpos=1;
        for(i=1;i<CQUEUE_BUFSIZE;i++)
            que->buflist[i].elem=obj+(i-1)*que->objSize;
    }
    elm=que->buflist+que->bufpos;
    que->bufpos++;
    }
    elm->next=NULL;
    elm->prev=NULL;
    return elm;
}

/****************************************************************************************
 
 -- cMessage Functions

****************************************************************************************/
//create new cMessage handle--------------------------------------------------------------
static cMsg cmsgIni(cMsgTbl* tbl,int size){
    cMsg mg;
    cstrMakeBuf(mg,1,CMSG,return NULL);
    mg->size=size;
    mg->errflg=0;
    mg->wanflg=0;
    mg->dev=stderr;
    mg->msgtbl=tbl;
    return mg;
}


//print message --------------------------------------------------------------------------
static void cmsgEcho(cMsg mg,int number,...){
    cMsgTbl elm;
    char*type;
    va_list vlist;
    elm=mg->msgtbl[number];
    switch(elm.type){
    case __CMSG_ERROR:
        type="error:";
    mg->errflg++;
    break;
    case __CMSG_WARNING:
        type="warning:";
    mg->wanflg++;
        break;
    default:
        type="";
        break;
    }
    fprintf(mg->dev,"%s",type);
    va_start(vlist,number);
    vfprintf(mg->dev,elm.form,vlist);
    va_end(vlist);
    fprintf(mg->dev," (CODE0x%x).\n",elm.code);
    if(elm.operation) elm.operation();
}


/****************************************************************************************

 -- cstring Functions

****************************************************************************************/
//split
static char** cstrSplit(char*strgen,char*ct){
    int i,j,k;
    int flg;
    char*tmp;
    char*str;
    char** buf;
    cVector list;
    if(!strgen) return NULL;
    if(!ct) return NULL;
    cstrSet(str,strgen,return NULL);
    list=cvectIni(sizeof(char*),20);
    for(i=0;str[i]!=CIO_ES;i++){
    for(flg=0,j=0;j<strlen(ct);j++) flg|=(str[i]==ct[j]);
    if(flg) continue;
    for(k=i;str[k]!=CIO_ES;k++){
        for(flg=0,j=0;j<strlen(ct);j++) flg|=(str[k]==ct[j]);
        if(flg) break;
    }
    tmp=str+i;
    cvectPush(list,cioAddr(tmp));
    if(str[k]==CIO_ES) break;
    i=k;
    str[k]=CIO_ES;
    }
    cstrMakeBuf(buf,cvectGetSize(list)+1,char*,return NULL);
    for(i=0;i<cvectGetSize(list);i++)
        buf[i]=cioVoid(cvectFind(list,i),char*);
    buf[i]=NULL;
    cvectDes(list);
    return buf;
}

/****************************************************************************************

 -- cmstk Functions

****************************************************************************************/
//new cMstk creation ------------------------------------------------
static cMstk cmstkNew(int objSize){
    cMstk mstk;
    cMap rootmap;
    mstk=cvectIni(sizeof(cMap),2);
    rootmap=cmapNew(sizeof(char*),objSize,(*cmapStrKey),(*cmapStrCmp));
    cvectAdd(mstk,rootmap);
    return mstk;
}
//destruction -------------------------------------------------------
static void cmstkDes(cMstk obj){
    cvectMaps(obj,cMap,cmapDes(val));
    cvectDes(obj);
}
//push --------------------------------------------------------------
static void cmstkPush(cMstk obj){
    cMap map;
    cMap rootmap;
    if(!obj) return ;
    rootmap=cvectGet(obj,0,cMap);
    map=cmapNew(sizeof(char*),rootmap->objSize,(*cmapStrKey),(*cmapStrCmp));
    cvectAdd(obj,map);
}
//pop ---------------------------------------------------------------
static void cmstkPop(cMstk obj){
    cMap map;
    map=cvectSub(obj,cMap);
    cmapDes(map);
}
//set ---------------------------------------------------------------
static void cmstkInst(cMstk obj,char*key,void*val){
    cMap map;
    map=cvectGet(obj,cvectSize(obj)-1,cMap);
    cmapInst(map,&key,val);
}
//get ---------------------------------------------------------------
static void* cmstkFind(cMstk obj,char*key){
    int i;
    cMap map;
    for(i=cvectSize(obj)-1;i>=0;i--){
        void*elm;
        map=cvectGet(obj,i,cMap);
        elm=cmapFind(map,&key);
        if(elm) return elm;
    }
    return NULL;
}
//rem ---------------------------------------------------------------
static void cmstkDel(cMstk obj,char*key){
    cMap map;
    map=cvectGet(obj,cvectSize(obj)-1,cMap);
    cmapDel(map,&key);
}



/****************************************************************************************

 -- cio Functions

****************************************************************************************/
#ifdef USECIOMEM /*use cio malloc*/
//memory map
extern cMap __cioMemMap;

//init-----------------------------------------------------------------------------------
//key
static int _cioMemKey(void*a){
    int* b=(int*)a;
    int c=*b;
    return c;
}
//cond
static int _cioMemCond(void*a,void*b){
    return cioVoid(a,void*)==cioVoid(b,void*);
}
//init
static cMap cioMemIni(void){
    cMap map;
    map=cmapIni(sizeof(void*),sizeof(int),_cioMemKey,_cioMemCond);
    return map;
}
//cio malloc-----------------------------------------------------------------------------
static void* cioMalloc(int size){
    int flg=1;
    void* pt;
    if(!__cioMemMap)__cioMemMap=cioMemIni();
    pt=malloc(size);
    if(pt) cmapUpdate(__cioMemMap,&pt,&flg);
    return pt;
}
//cio calloc-----------------------------------------------------------------------------
static void* cioCalloc(int num,int size){
    int flg=1;
    void* pt;
    if(!__cioMemMap)__cioMemMap=cioMemIni();
    pt=calloc(num,size);
    if(pt) cmapUpdate(__cioMemMap,&pt,&flg);
    return pt;
}
//cio realloc-----------------------------------------------------------------------------
static void* cioRealloc(void*pt,int size){
    int flg=1;
    void* tmppt;
    if(!__cioMemMap)__cioMemMap=cioMemIni();
    tmppt=realloc(pt,size);
    if(tmppt){
        if(pt!=tmppt){
            flg=0;
            cmapUpdate(__cioMemMap,&pt,&flg);
        }else cmapUpdate(__cioMemMap,&tmppt,&flg);
    }
    return tmppt;
}
//cio free---------------------------------------------------------------------------------
static void cioFree(void* pt){
    int flg;
    int*tmpflg;
    tmpflg=cmapFind(__cioMemMap,&pt);
    if(tmpflg){
        if(*tmpflg){
            flg=0;
            cmapUpdate(__cioMemMap,&pt,&flg);
            free(pt);
        }
    }
}
//cio memDes-------------------------------------------------------------------------------
static void cioDesMem(void){
    int  i;
    int* flg;
    cVector key;
    void*pt;
    key=__cioMemMap->elmlist;
    for(i=0;i<cvectGetSize(key);i++){
        pt=cioVoid(cvectFind(key,i),void*);
        flg=cmapFind(__cioMemMap,&pt);
        if(flg) if(*flg) free(pt);
    }
    cmapDes(__cioMemMap);
}
#define malloc  cioMalloc
#define calloc  cioCalloc
#define realloc cioRealloc
#define free    cioFree
#endif


#endif
/*end of calg*/

