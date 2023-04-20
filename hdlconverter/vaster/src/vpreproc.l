%{
/*
  preprocessing for Vaster.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include <stdio.h>
#include "vpreproc_const.h"
%}

%%
[ \t] return Vpreproc_type_SPACE;
[/][*](.|[\n])*[*][/] return Vpreproc_type_COMMENT;
[/][/].*[\n]   return Vpreproc_type_COMMENT;
"parameter"  return Vpreproc_type_PARAM;
"`reset_all" return Vpreproc_type_RESET_ALL;
"`timescale" return Vpreproc_type_TIMESCALE;
"`define"    return Vpreproc_type_DEFINE;
"`include"   return Vpreproc_type_INCLUDE;
"`ifdef"     return Vpreproc_type_IFDEF;
"`else"      return Vpreproc_type_ELSE;
"`endif"     return Vpreproc_type_ENDIF;
"`default_nettype"     return Vpreproc_type_DEFAULT_NETTYPE;
"`unconnected_drive"   return Vpreproc_type_UNCONNECTED_DRIVE;
"`nounconnected_drive" return Vpreproc_type_NOUNCONNECTED_DRIVE;
"`signed"   return Vpreproc_type_SIGNED;
"`unsigned" return Vpreproc_type_UNSIGNED;

[`][_$a-zA-Z][_$a-zA-Z]* return Vpreproc_type_MID;
[_$a-zA-Z][_$a-zA-Z]* return Vpreproc_type_ID;
[\\][\n] return Vpreproc_type_EEL;
[\n]     return Vpreproc_type_EL;
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
void* vpreMakeBuf(FILE*fp){
    YY_BUFFER_STATE ybuf=yy_create_buffer(fp,YY_BUF_SIZE);
    yy_switch_to_buffer(ybuf);
    return (void*)ybuf;
}

//
//delete buffer
//
void vpreDeleteBuf(void*ybuf){
    yy_delete_buffer((YY_BUFFER_STATE)ybuf);
}

//
//current buffer
//
void* vpreCurrentBuf(void){
    return (void*)YY_CURRENT_BUFFER;
}

//
//change buffer
//
void vpreChangeBuf(void*ybuf){
    yy_switch_to_buffer(ybuf);
}


