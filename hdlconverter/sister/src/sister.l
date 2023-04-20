%{
/*
  lexer for Sister.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "generic.h"

int __sis_line=1; //number of line
char __sis_file[SIS_FILE_NAME_SIZE]=""; //parsed file name
static void getFileInfo(char*text); //getting file information
static uint htoi(char*s); //hex to integer
static char* checkNewLine(char*text);

//setting number
#define setNum(tkn) {     \
  sister_lval.num=atoi(yytext); \
  return tkn; }

//setting Id
#define setId(tkn) {                       \
  strncpy(sister_lval.str,yytext,SIS_BUF_SIZE); \
  return tkn; }

//setting string
#define setStr(tkn) {                        \
  yytext[strlen(yytext)-1]='\0';             \
  strncpy(sister_lval.str,yytext+1,SIS_BUF_SIZE); \
  return tkn; }

//setting charactor
#define setChar(tkn) {                         \
  if(!strcmp(yytext,"'\\''")) sister_lval.num='\''; \
  else sister_lval.num=yytext[1];                   \
  return tkn; }

//setting real number
#define setReal(tkn) {      \
  sister_lval.real=atof(yytext); \
  return tkn; }

%}

SPACE [ \t\n]+
SCHAR ("signed"{SPACE})?"char"
UCHAR ("unsigned"{SPACE})"char"
SSHORT ("signed"{SPACE})?"short"({SPACE}"int")?
USHORT ("unsigned"{SPACE})"short"({SPACE}"int")?
SINT ("signed"{SPACE})?"int"
UINT ("unsigned"{SPACE})"int"
SLONG ("signed"{SPACE})?"long"({SPACE}"int")?
ULONG ("unsigned"{SPACE})"long"({SPACE}"int")?
FILE_STATE "#"[ \t]*[0-9]+([ \t]*(\"[^\"]*\")*([ \t]*[0-9]+)?{SPACE}*)?

%%
[ \t]+ ;
[\n]         __sis_line++;
{FILE_STATE} getFileInfo(yytext);
"class"     return Class;
"struct"    return Struct;
"public"    return Public;
"private"   return Private;
"SC_MODULE" return Sc_module;
"SC_CTOR"   return Sc_ctor;
"if"        return If;
"else"      return Else;
"do"        return Do;
"for"       return For;
"while"     return While;
"switch"    return Switch;
"case"      return Case;
"default"   return Default;
"return"    return Return;
"break"     return Break;
"continue"  return Continue;
"deltete"   return Del;
"new"       return New;
"enum"      return Enum;
"typedef"   return Typedef;

"sc_in"       return Sc_in;
"sc_out"      return Sc_out;
"sc_signal"   return Sc_signal;
"sc_fifo_in"  return Sc_fifo_in;
"sc_fifo_out" return Sc_fifo_out;
"sc_fifo"     return Sc_fifo;

"sc_int"     return Sc_int;
"sc_uint"    return Sc_uint;
"sc_bigint"  return Sc_bigint;
"sc_biguint" return Sc_biguint;

"void" return Void;
"bool" return Bool;
{SCHAR}  {checkNewLine(yytext);return Schar;}
{UCHAR}  {checkNewLine(yytext);return Uchar;}
{SSHORT} {checkNewLine(yytext);return Sshort;}
{USHORT} {checkNewLine(yytext);return Ushort;}
{SINT}   {checkNewLine(yytext);return Sint;}
{UINT}   {checkNewLine(yytext);return Uint;}
{SLONG}  {checkNewLine(yytext);return Slong;}
{ULONG}  {checkNewLine(yytext);return Ulong;}
"float"     return Float;
"double"    return Double;

"signed"   return Sint;
"unsigned" return Uint;

"auto"   |
"inline" |
"static" |
"extern" |
"register" setId(Data_opt);

"->"  return Arr;
"=="  return Eq;
"!="  return Neq;
">="  return Gre;
"<="  return Les;
"||"  return Oror;
"&&"  return Anan;
"<<"  return Lshif;
">>"  return Rshif;
"++"  return Inc;
"--"  return Dec;
"*="  return Mueq;
"/="  return Dieq;
"%="  return Moeq;
"+="  return Adeq;
"-="  return Sueq;
"&="  return Aneq;
"^="  return Xoeq;
"|="  return Oreq;
"<<=" return Lseq;
">>=" return Rseq;
"::"  return Dcolon;

"0"[xX][0-9]+ return htoi(yytext);
([0-9]+[.][0-9]*) setReal(Real);
([0-9]*[.][0-9]+) setReal(Real);

[0-9]+ setNum(Num);
[']([^']|"\\'")['] setChar(Num);
[_a-zA-Z][_a-zA-Z0-9]* setId(Id);
["](([^"\n])|([\\]["]))*["] setStr(String);
.  return yytext[0];
%%

/*
 hex to integer
*/
#define atox__digit(i) ((int)pow(16.0,(double)(i)))
static uint htoi(char*s){
    int size,i,num;
    size=strlen(s);
    for(i=0,num=0;i<size-2;i++){
        switch(s[size-i-1]){
        case '0':num+=0x0 * atox__digit(i);break;
        case '1':num+=0x1 * atox__digit(i);break;
        case '2':num+=0x2 * atox__digit(i);break;
        case '3':num+=0x3 * atox__digit(i);break;
        case '4':num+=0x4 * atox__digit(i);break;
        case '5':num+=0x5 * atox__digit(i);break;
        case '6':num+=0x6 * atox__digit(i);break;
        case '7':num+=0x7 * atox__digit(i);break;
        case '8':num+=0x8 * atox__digit(i);break;
        case '9':num+=0x9 * atox__digit(i);break;
        case 'a':num+=0xa * atox__digit(i);break;
        case 'A':num+=0xa * atox__digit(i);break;
        case 'b':num+=0xb * atox__digit(i);break;
        case 'B':num+=0xb * atox__digit(i);break;
        case 'c':num+=0xc * atox__digit(i);break;
        case 'C':num+=0xc * atox__digit(i);break;
        case 'd':num+=0xd * atox__digit(i);break;
        case 'D':num+=0xd * atox__digit(i);break;
        case 'e':num+=0xe * atox__digit(i);break;
        case 'E':num+=0xe * atox__digit(i);break;
        case 'f':num+=0xf * atox__digit(i);break;
        case 'F':num+=0xf * atox__digit(i);break;
        }
    }
    sister_lval.num=num;
    return Num;
}

/*
 getting file information
*/
static void getFileInfo(char*text){
    uchar i;
    char** info=cstrSplit(text," \t\n");
    for(i=0;info[i];i++){
        switch(i){
        case 2 :
            strncpy(__sis_file,info[i],SIS_FILE_NAME_SIZE);
            break;
        case 1 :
            __sis_line=atoi(info[i])-1;
            break;
        }
    }
    checkNewLine(text);
    free(info);
}
/*
 checking new line
*/
static char* checkNewLine(char*text){
    int i,size;
    size=strlen(text);
    for(i=0;i<size;i++){
        if(text[i]=='\n'){ __sis_line++; }
    }
    return text;
}

