%{
/*
  lexer for Vaster.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "vaster_generic.h"
#include "vaster_VHDL.tab.h"

static void vFileLine(char* s);

#define retId(ret) { \
    strncpy(vaster_VHDL_lval.str,yytext,SIS_BUF_SIZE); \
    return ret; }

%}
%%
[ \t] ;
[\n]  __sis_line++;

"--"[^\n]+[\n] vFileLine(yytext);

[rR][aA][nN][gG][eE]     return VH_range;
[rR][iI][gG][hH][tT]     return VH_right;
[lL][eE][fF][tT]         return VH_left;
[hH][iI][gG][hH]         return VH_high;
[lL][oO][wW]             return VH_low;
[lL][eE][nN][gH][tY][hH] return VH_length;
[eE][vV][eE][nN][tT]     return VH_event;
[sS][tT][aA][bB][lL][eE] return VH_stable;

[nN][uU][lL][lL]  return VH_null;
[eE][xX][iI][tT]  return VH_exit;
[nN][eE][xX][tT]  return VH_next;

[tT][yY][pP][eE]                  return VH_type;
[sS][uU][bB][tY][yY][pP][eE]      return VH_subtype;
[vV][aA][rR][iI][aA][bB][lL][eE]  return VH_variable;
[aA][lL][iI][aA][sS]              return VH_alias;

[lL][oO][oO][pP]     return VH_loop;
[wW][hH][iI][lL][eE] return VH_while;
[fF][oO][rR]         return VH_for;
[oO][nN]             return VH_on;
[uU][nN][tT][iI][lL] return VH_until;
[wW][aA][iI][tT]     return VH_wait;

[uU][sS][eE] return VH_use;
[lL][iI][bB][rR][aA][rR][yY] return VH_library;

[iI][fF]             return VH_if;
[tT][hH][eE][nN]     return VH_then;
[eE][lL][sS][iI][fF] return VH_elsif;
[eE][lL][sS][eE]     return VH_else;


[gG][eE][nN][eE][rR][iI][cC]     return VH_generic;
[gG][eE][nN][eE][rR][aA][tT][eE] return VH_generate;
[cC][oO][nN][sS][tT][aA][nN][tT] return VH_const;

[rR][eE][tT][uU][rR][nN]     return VH_return;
[pP][rR][oO][cC][eE][sS][sS] return VH_process;
[bB][lL][oO][cC][kK]         return VH_block;
[cC][aA][sS][eE]             return VH_case;
[wW][hH][eE][nN]             return VH_when;
[oO][tT][hH][eE][rR][sS]     return VH_others;

[bB][eE][gG][iI][nN]  return VH_begin;
[eE][nN][dD]          return VH_end;
[bB][oO][dD][yY]      return VH_body;

[iI][sS]  return VH_is;
[oO][fF]  return VH_of;

[pP][aA][cC][kK][aA][gG][eE]                     return VH_package;
[eE][nN][tT][iI][tT][yY]                         return VH_entity;
[aA][rR][cC][hH][iI][tT][eE][cC][tT][uU][rR][eE] return VH_archit;
[cC][oO][mM][pP][oO][nN][eE][nN][tT]             return VH_component;

[sS][iI][gG][nN][aA][lL]             return VH_signal;
[aA][tT][tT][rR][iI][bB][uU][tT][eE] return VH_attribute;
[fF][uU][nN][cC][tT][iI][oO][nN]     return VH_func;
[pP][rR][oO][cC][eE][dD][uU][rR][eE] return VH_procedure;

[pP][oO][rR][tT]     return VH_port;
[mM][aA][pP]         return VH_map;
[iI][nN]             return VH_in;
[oO][uU][tT]         return VH_out;
[iI][nN][oO][uU][tT] return VH_inout;

[tT][oO]                 return VH_to;
[dD][oO][wW][nN][tT][oO] return VH_downto;

[aA][nN][dD]     return VH_and;
[oO][rR]         return VH_or;
[xX][oO][rR]     return VH_xor;
[nN][aA][nN][dD] return VH_nand;
[nN][oO][rR]     return VH_nor;
[nN][oO][tT]     return VH_not;
[aA][bB][sS]     return VH_abs;
[mM][oO][dD]     return VH_mod;
[rR][eE][mM]     return VH_rem;

"**"   return VH_mul_mul;
"/="   return VH_not_eq;
"<="   return VH_les;
">="   return VH_gre;

"=>" return VH_right_vect;
":=" return VH_colon_eq;

([nN][sS])|([mM][sS])   retId(VH_time)
[_a-zA-Z]([_a-zA-Z0-9]*[a-zA-Z0-9][_]?)? retId(VH_id)
[0-9][_0-9]* retId(VH_num)

(("2")|("8")|("16"))[#][0-9a-fA-F][_0-9a-fA-F]*[#] retId(VH_num)
['][\\]?.['] retId(VH_num)
[bBoOxX]?["]([^"]|([\\]["]))*["] retId(VH_num)

['"]?[0-9]+['"]? return VH_num; retId(VH_id)

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

