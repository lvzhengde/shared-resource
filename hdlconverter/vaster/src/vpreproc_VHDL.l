%{
/*
  VHDL preprocessing for Vaster.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include <stdio.h>
#include "vpreproc_const.h"
%}

%%
[-][-].*[\n] return Vpreproc_type_COMMENT;
<<EOF>>  return 0;
. return Vpreproc_type_None;
%%

//
//wrap
//
int yywrap(void){
    return 1;
}

//
//make buffer
//
void* vhpreMakeBuf(FILE*fp){
    YY_BUFFER_STATE ybuf=yy_create_buffer(fp,YY_BUF_SIZE);
    yy_switch_to_buffer(ybuf);
    return (void*)ybuf;
}

//
//delete buffer
//
void vhpreDeleteBuf(void*ybuf){
    yy_delete_buffer((YY_BUFFER_STATE)ybuf);
}

//
//current buffer
//
void* vhpreCurrentBuf(void){
    return (void*)YY_CURRENT_BUFFER;
}

//
//change buffer
//
void vhpreChangeBuf(void*ybuf){
    yy_switch_to_buffer(ybuf);
}


