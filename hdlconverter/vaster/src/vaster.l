%{
/*
  lexer for Vaster.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "vaster_generic.h"
#include "vaster.tab.h"

static void vFileLine(char* s);
%}

%%
[/][/].+[\n] vFileLine(yytext); //file and line number
[ \t]+             ;//white space
[\n]         __sis_line++;//new line
[\\][\n]     __sis_line++;//escaped new line

"module"    return V_module;
"endmodule" return V_endmodule;

"assign"  return V_assign;
"always"  return V_always;
"initial" return V_initial;

"reg"     return V_reg;
"wire"    return V_wire;
"input"   return V_input;
"output"  return V_output;
"integer" return V_integer;

"begin"   return V_begin;
"end"     return V_end;

"if"     return V_if;
"else"   return V_else;

"case"    return V_case;
"endcase" return V_endcase;
"default" return V_default;

"for"   return V_for;
"while" return V_while;

"posedge" return V_posedge; 
"negedge" return V_negedge;
"or"      return V_s_or;

"~^" return V_ninv;
">>" return V_rshif;
"<<" return V_lshif; 
"!=" return V_neq;
"==" return V_eq;
"~|" return V_nor;
"~&" return V_nand;
"||" return V_oror;
"&&" return V_anan;
">=" return V_gre;
"<=" return V_les;

[#][0-9]+ return V_delay_num;
["](([^"\n])|([\\]["]))*["]  return V_string;
[_$a-zA-Z][_$a-zA-Z0-9]*     return V_id;
([_0-9]+['][a-zA-Z])?[_0-9]+ return V_num;
.  return yytext[0];
%%

//
// file and line number
//
static void vFileLine(char* s){
    char**fl=cstrSplit(s+2," \t");
    strncpy(__sis_file,fl[0],SIS_FILE_NAME_SIZE);
    __sis_line=atoi(fl[1]);
    free(fl);
}

