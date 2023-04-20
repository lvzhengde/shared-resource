/*
    verilog HDL preprocesser
    (c)KTDesignSystemc Corp.
*/

#ifndef __VPREPRO_HEAD
#define __VPREPRO_HEAD

#include "vaster_generic.h"

//preprocess handler
typedef struct _VpreHandle{
    FILE* fp;   //file pointer
    char* file; //file name
    int   line; //line number
    cMap  map;  //directive string buffer
    cVector  sbuf;  //string buffer
}*VpreHandle;

//define table
typedef struct _VpreDefNode{
    char  type;   //0:const,1:function
    char* id;     //defined id
    char* str;    //defined string
    cVector args; //function args list
}*VpreDefNode;

extern int vprepro(VasOptHandle opt);

//constructor/destructor
extern VpreHandle vpreIni(void);
extern void vpreDes(VpreHandle vphdl);


//control buffer
extern void* vpreMakeBuf(FILE*fp);
extern void  vpreDeleteBuf(void*ybuf);
extern void* vpreCurrentBuf(void);
extern void  vpreChangeBuf(void*ybuf);
//control buffer(VHDL)
extern void* vhpreMakeBuf(FILE*fp);
extern void  vhpreDeleteBuf(void*ybuf);
extern void* vhpreCurrentBuf(void);
extern void  vhpreChangeBuf(void*ybuf);
#endif

