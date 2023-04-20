%{
/*
  preprocessing for Sister.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include <stdio.h>
#include <string.h>
static unsigned int  scmain=0; //ignore sc_main  
static unsigned int  bstk=0; 
static unsigned char bsw=0;  
#define pblock(bsw) {                       \
    if(bsw&&(!scmain)) printf("%s",yytext); \
    else{                                   \
        int i,size;                         \
        size=strlen(yytext);                \
        for(i=0;i<size;i++){                \
            if(yytext[i]=='\n') puts("");   \
        } } }
#define bswoff(bstk,bsw){      \
    if(!bstk){                 \
        if(bsw) bsw--;         \
        if(scmain) scmain--;   \
    }}
//#define DEBUG
%}

SPACE [ \n\t]*
ARG_BEGIN "("
ARG_END   ")"
BLOCK_BEGIN "{"
BLOCK_END   "}"
ID [_a-zA-Z][_a-zA-Z0-9]*{SPACE}
TEMP [<]{SPACE}({ID}|[0-9]+){SPACE}[>]{SPACE}
ARGS {ARG_BEGIN}[^)]*{ARG_END}{SPACE}
CLASS_KEY ("class"|"strucot"){SPACE}
VARDEF  {ID}("["[^\]]*"]")?{SPACE}
VARLIST ({VARDEF}","{SPACE})*{VARDEF}
MODULE_DEC "SC_MODULE"{SPACE}{ARG_BEGIN}{SPACE}{ID}{SPACE}{ARG_END}{SPACE}{BLOCK_BEGIN}{SPACE}
MODULE_END {BLOCK_END}{SPACE}({VARLIST})?";"{SPACE}
FUNC_DEC {ID}+({TEMP}{ID}+)?"::"{SPACE}{ID}{ARGS}{BLOCK_BEGIN}{SPACE}
FUNC_DEC2 ({ID}{SPACE})+{ARGS}{BLOCK_BEGIN}{SPACE}
FUNC_DEC3 ({ID}{SPACE})+{ARGS}{SPACE}[;]
SC_MAIN ({ID}{SPACE})*sc_main{SPACE}{ARGS}{BLOCK_BEGIN}{SPACE}
ENUM_DEC "enum"{SPACE}({ID}{SPACE})?{BLOCK_BEGIN}{SPACE}
TYPEDEF_DEC1 "typedef"{SPACE}[^;{]+";"
TYPEDEF_DEC2 "typedef"{SPACE}[^;{]+"{"
CLASS_DEC  "class"{SPACE}({ID}{SPACE})?{BLOCK_BEGIN}{SPACE}
STRUCT_DEC "struct"{SPACE}({ID}{SPACE})?{BLOCK_BEGIN}{SPACE}
FILE_STATE  "#"[ \t]*[0-9]+([ \t]*(\"[^\"]*\")*[ \t]*[0-9]+{SPACE})?

%%
{FILE_STATE}   {printf(yytext);}
{MODULE_END}   {pblock(bsw);bstk--;bswoff(bstk,bsw);}
{MODULE_DEC}   {bsw++;bstk++;pblock(bsw);}
{SC_MAIN}      {scmain++;bstk++;}
{FUNC_DEC3}    {if(!scmain)printf(yytext);}
{FUNC_DEC2}    {if(!bsw)bsw++;pblock(bsw);bstk++;}
{FUNC_DEC}     {if(!bsw)bsw++;pblock(bsw);bstk++;}
{ENUM_DEC}     {if(!bsw)bsw++;pblock(bsw);bstk++;}
{TYPEDEF_DEC1} {if(!scmain) printf("%s",yytext);}
{TYPEDEF_DEC2} {if(!bsw)bsw++;pblock(bsw);bstk++;}
{CLASS_DEC}    {if(!bsw)bsw++;pblock(bsw);bstk++;}
{STRUCT_DEC}   {if(!bsw)bsw++;pblock(bsw);bstk++;}
{BLOCK_END}    {pblock(bsw);bstk--;bswoff(bstk,bsw);}
{BLOCK_BEGIN}  {pblock(bsw);bstk++;}
. {pblock(bsw);}
%%

//
//wrap
//
int yywrap(void){
    return 1;
}

#ifdef DEBUG
//
// main
//
int main(int argc,char**argv){
    return yylex();
}
#endif
