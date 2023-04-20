%{
/*
  parser for Sister.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "generic.h"
//#define DEBUG
extern char* sister_text;
static void sister_error(char*s);
%}

%expect 1

%union{
    int num;
    char str[SIS_BUF_SIZE];
    double real;
    cgrNode node;
}

%token Class Struct
%token Module Process
%token Public Private
%token Id Num String Real
%token Rshif Lshif Inc Dec
%token Dcolon Eq Neq Arr
%token Mueq Dieq Moeq Adeq Sueq
%token Lseq Rseq Aneq Xoeq Oreq
%token Oror Anan Gre Les
%token Return Continue Break
%token If Else For Do While
%token Switch Case Default New Del
%token Sc_signal Sc_in Sc_out
%token Sc_fifo_in Sc_fifo_out Sc_fifo
%token Sc_int Sc_uint Sc_bigint Sc_biguint
%token Void Bool Schar Uchar Sshort Ushort
%token Sint Uint Slong Ulong Float Double
%token Sc_module Sc_ctor Data_opt 
%token Enum Typedef

//dummy
%token Function Dfg Arry Cast Connect
%token Sc_signal_key Array Esp Declare
%token Data_type Block Funcall Instance
%token Back Ctrl Def Tag
%token OpString

%type<node> OPE4 OPE3 OPE2 OPE1 OPE15
%type<node> OPE9 OPE8 OPE7 OPE6 OPE5
%type<node> OPE10  OPE14 OPE13 OPE12 OPE11
%type<node> DFG FUNCALL ARGS DATA_TYPE 
%type<node> ALL_DATA_TYPE C_DATA_TYPE SC_DATA_TYPE DATA_OPT
%type<node> IF ELSE SWITCH CASE DO WHILE
%type<node> FOR FOR_DFG FOR_DFG_VAR CFG CFG_STATE INSTANCE
%type<node> VAR_DEC VAR_DEC_VAR VAR_DEC_VAR2 
%type<node> VAL VAL2 VAL3 VAL4 VAL5 ID CONNECT
%type<node> ARG_DEC ARG_DEC_VAR POINTER POINTER2 RETURN
%type<node> SC_SIGNAL_KEY SIG_DEC
%type<node> FUNCTION FUNCTION2 FUNCTION3
%type<node> SC_CONSTR VISIBLE MODULE_BLOCK
%type<node> CLASS_KEY METHOD MODULE DECLARE
%type<node> ENUM ENUM_BODY ENUM_BODY2 ENUM_VAL 
%type<node> TAG TAG_VARS CLASS CLASS_BODY CLASS_BODY2 
%type<node> CLASS_BODY_VAR 
%type<node> CLASS_DEF CLASS_DEF2 CLASS_DEF3 
%type<node> ENUM_DEF FUNCTION_TYPE 
%type<node> CLASS_KEY_TAG TYPEDEF

%type<str>  String Id Data_opt
%type<real> Real
%type<num>  Num
%%

//parser root
ROOT : DECLARE {cgrSetRoot(cgrGetHead($1));}
     ;
     
DECLARE
    : {$$=NULL;}
    | DECLARE MODULE {
         cgrSetContext($1,$2);
         $$=$2;}
    | DECLARE METHOD {
         cgrSetContext($1,$2);
         $$=$2;}
    | DECLARE ENUM ';' {
         cgrSetContext($1,$2);
         $$=$2;}
    | DECLARE CLASS_DEF2 ';' {
         cgrSetContext($1,$2);
         $$=$2;}
    | DECLARE TYPEDEF ';' {
         cgrSetContext($1,$2);
         $$=$2;}
    | DECLARE FUNCTION {
         cgrSetContext($1,$2);
         cgrSetNum($2,cgrKeyIsOutOfModule,1);
         $$=$2;}
    ;

MODULE :  Sc_module '(' Id ')' '{' MODULE_BLOCK '}' ';' {
             $$=cgrNew(Sc_module,__sis_file,__sis_line);
             cgrSetStr($$,cgrKeyId,$3); 
             cgrSetNode($$,cgrKeyVal,cgrGetHead($6)); }
       ;

METHOD : FUNCTION_TYPE Id Dcolon Id '(' ARG_DEC ')' '{' CFG '}' {
           $$=cgrNew(Function,__sis_file,__sis_line);
           cgrSetStr($$,cgrKeyId,$4); 
           cgrSetStr($$,cgrKeyPar,$2); 
           cgrSetNode($$,cgrKeyType,$1); 
           cgrSetNode($$,cgrKeyVal,cgrGetHead($9)); 
           cgrSetNode($$,cgrKeyArgs,cgrGetHead($6));
           cgrSetNum($$,cgrKeyIsFuncBody,1); }
       | FUNCTION_TYPE Id Dcolon Id '(' ARG_DEC ')' ';' {
           $$=cgrNew(Function,__sis_file,__sis_line);
           cgrSetStr($$,cgrKeyId,$4); 
           cgrSetStr($$,cgrKeyPar,$2); 
           cgrSetNode($$,cgrKeyType,$1); 
           cgrSetNode($$,cgrKeyArgs,cgrGetHead($6));}
       ;

MODULE_BLOCK : {$$=NULL;}
             | MODULE_BLOCK VISIBLE ':' {
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK SIG_DEC ';' { 
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK INSTANCE ';' { 
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK FUNCTION  {
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK SC_CONSTR {
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK ENUM_DEF ';'{
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK CLASS_DEF ';'{
                 cgrSetContext($1,$2);
                 $$=$2;}
             | MODULE_BLOCK TYPEDEF ';'{
                 cgrSetContext($1,$2);
                 $$=$2;}
             ;

INSTANCE: Id POINTER2 Id {
            $$=cgrNew(Instance,__sis_file,__sis_line);
            cgrSetStr($$,cgrKeyId,$3);
            cgrSetStr($$,cgrKeyType,$1);
            cgrSetNode($$,cgrKeyPointer,cgrGetHead($2)); }
          ;

SC_CONSTR : Sc_ctor '(' Id ')' '{' CFG '}' {
               $$=cgrNew(Sc_ctor,__sis_file,__sis_line);
               cgrSetStr($$,cgrKeyId,$3);
               cgrSetNode($$,cgrKeyVal,cgrGetHead($6)); }
          ;

FUNCTION : FUNCTION_TYPE FUNCTION2 {
               $$=$2;
               cgrSetNode($$,cgrKeyType,$1); }
         ;
FUNCTION2 : Id FUNCTION3 {
                $$=$2;
               cgrSetStr($$,cgrKeyId,$1); }
          ;
FUNCTION3 :'(' ARG_DEC ')' ';' {
               $$=cgrNew(Function,__sis_file,__sis_line);
               cgrSetNode($$,cgrKeyArgs,cgrGetHead($2)); }
          |'(' ARG_DEC ')' '{' CFG '}' {
               $$=cgrNew(Function,__sis_file,__sis_line);
               if(!$5){
                   cgrNode dummy;
                   dummy=cgrNew(Dfg,$$->file,$$->line);
                   cgrSetNode($$,cgrKeyVal,dummy);
               }else cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
               cgrSetNode($$,cgrKeyArgs,cgrGetHead($2)); }
          ;

FUNCTION_TYPE 
    : DATA_TYPE {$$=$1;}
    | CLASS_KEY_TAG {$$=$1;}
    | Id {
        $$=cgrNew(Tag,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$1); }
    | Data_opt Id {
        cgrNode dataopt=cgrNew(Data_opt,__sis_file,__sis_line);
        cgrSetStr(dataopt,cgrKeyVal,$1); 
        $$=cgrNew(Tag,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyOpt,$$);
        cgrSetStr($$,cgrKeyId,$2); }
    ;

SIG_DEC : DATA_OPT SC_SIGNAL_KEY '<' FUNCTION_TYPE '>' VAR_DEC_VAR {
            $$=cgrNew(Sc_signal_key,__sis_file,__sis_line);
            cgrSetNode($$,cgrKeyOpt,$1);
            cgrSetNode($$,cgrKeyType,$2);
            cgrSetNode($$,cgrKeySize,$4);
            cgrSetNode($$,cgrKeyVal,cgrGetHead($6));}
        ;

SC_SIGNAL_KEY
    : Sc_in       {$$=cgrNew(Sc_in      ,__sis_file,__sis_line);}
    | Sc_out      {$$=cgrNew(Sc_out     ,__sis_file,__sis_line);}
    | Sc_signal   {$$=cgrNew(Sc_signal  ,__sis_file,__sis_line);}
    | Sc_fifo_in  {$$=cgrNew(Sc_fifo_in ,__sis_file,__sis_line);}
    | Sc_fifo_out {$$=cgrNew(Sc_fifo_out,__sis_file,__sis_line);}
    | Sc_fifo     {$$=cgrNew(Sc_fifo    ,__sis_file,__sis_line);}
    ;

VAR_DEC : DATA_TYPE VAR_DEC_VAR {
            cgrSetNode($1,cgrKeyVal,cgrGetHead($2));
            $$=$1;}
        | TAG    {$$=$1;}    
        | ENUM   {$$=$1;} 
        | CLASS  {$$=$1;}
        ;

TYPEDEF : Typedef DATA_TYPE  Id {
            $$=cgrNew(Typedef,__sis_file,__sis_line);
            cgrSetStr($$,cgrKeyTag,$3);
            cgrSetNode($$,cgrKeyVal,$2); } 
        | Typedef CLASS_DEF3 Id {
            $$=cgrNew(Typedef,__sis_file,__sis_line);
            cgrSetStr($$,cgrKeyTag,$3);
            cgrSetNode($$,cgrKeyVal,$2); } 
        ;

CLASS_DEF : CLASS_KEY Id '{' CLASS_BODY '}' {
            $$=$1;
            cgrSetStr($$,cgrKeyTag,$2);
            cgrSetNode($$,cgrKeyClassBody,cgrGetHead($4)); }
          ;

CLASS_DEF2
       : CLASS_KEY    '{' CLASS_BODY '}' TAG_VARS {
            $$=$1;
            cgrSetNode($$,cgrKeyClassBody,cgrGetHead($3));
            cgrSetNode($$,cgrKeyVal,$5); } 
       | CLASS_KEY Id '{' CLASS_BODY '}' TAG_VARS {
            $$=$1;
            cgrSetStr($$,cgrKeyTag,$2);
            cgrSetNode($$,cgrKeyClassBody,cgrGetHead($4));
            cgrSetNode($$,cgrKeyVal,$6); }
       ;

CLASS_DEF3 
    : CLASS_DEF {$$=$1;}
    | CLASS_KEY '{' CLASS_BODY '}' {
        $$=$1;
        cgrSetNode($$,cgrKeyClassBody,cgrGetHead($3)); }
    | CLASS_KEY Id {
        $$=$1;
        cgrSetNum($$,cgrKeyIsClassTag,1);
        cgrSetStr($$,cgrKeyTag,$2);
        }
    ;

CLASS  : CLASS_DEF2 {$$=$1;}
       | CLASS_KEY Id TAG_VARS {
            $$=$1;
            cgrSetNum($$,cgrKeyIsClassTag,1);
            cgrSetStr($$,cgrKeyTag,$2);
            cgrSetNode($$,cgrKeyVal,$3); }     
       ;

CLASS_KEY : Class  {$$=cgrNew(Class,__sis_file,__sis_line);}
          | Struct {$$=cgrNew(Struct,__sis_file,__sis_line);}
          ;

CLASS_BODY  : {$$=NULL;}
            | CLASS_BODY VISIBLE ':' {
                cgrSetContext($1,$2);
                $$=$2;}
           | CLASS_BODY CLASS_BODY2 {
                cgrSetContext($1,$2);
                $$=$2;}
            ;

CLASS_BODY2 : ALL_DATA_TYPE CLASS_BODY_VAR ';' {
                cgrSetNode($1,cgrKeyVal,cgrGetHead($2));
                $$=$1; }
            | ALL_DATA_TYPE Id FUNCTION3 {
                $$=$3;
                cgrSetNode($$,cgrKeyType,$1);
                cgrSetStr($$,cgrKeyId,$2); }
            ;

ALL_DATA_TYPE
    : DATA_TYPE  {$$=$1;}
    | DATA_OPT CLASS_DEF3 {$$=$2;
        cgrSetNode($$,cgrKeyOpt,$1); }
    | DATA_OPT ENUM_DEF   {$$=$2;
        cgrSetNode($$,cgrKeyOpt,$1); }
    | DATA_OPT Id { //tag
        $$=cgrNew(Tag,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$2); 
        cgrSetNode($$,cgrKeyOpt,$1); }
    ;

CLASS_BODY_VAR
    : VAL5 {$$=$1;}
    | CLASS_BODY_VAR ',' VAL5 {
         cgrSetContext($1,$3);
         $$=$3;}
    ;

VISIBLE : Public   {$$=cgrNew(Public,__sis_file,__sis_line);}
        | Private  {$$=cgrNew(Private,__sis_file,__sis_line);}
        ;

ENUM_DEF : Enum    '{' ENUM_BODY '}' {
           $$=cgrNew(Enum,__sis_file,__sis_line);
           cgrSetNode($$,cgrKeyEnumBody,cgrGetHead($3)); }
         | Enum Id '{' ENUM_BODY '}' {
           $$=cgrNew(Enum,__sis_file,__sis_line);
           cgrSetStr($$,cgrKeyTag,$2); 
           cgrSetNode($$,cgrKeyEnumBody,cgrGetHead($4)); }
         ;



ENUM : Enum    '{' ENUM_BODY '}' TAG_VARS {
            $$=cgrNew(Enum,__sis_file,__sis_line);
            cgrSetNode($$,cgrKeyEnumBody,cgrGetHead($3));
            cgrSetNode($$,cgrKeyVal,cgrGetHead($5));}
     | Enum Id '{' ENUM_BODY '}' TAG_VARS {
            $$=cgrNew(Enum,__sis_file,__sis_line);
            cgrSetNode($$,cgrKeyEnumBody,cgrGetHead($4)); 
            cgrSetStr($$,cgrKeyTag,$2); 
            cgrSetNode($$,cgrKeyVal,cgrGetHead($6));}
     ;


ENUM_BODY : {$$=NULL;}
          | ENUM_BODY2 {$$=$1;}
          | ENUM_BODY2 ',' {$$=$1;}
          ;

ENUM_BODY2 : ENUM_VAL {$$=$1;}
           | ENUM_BODY2 ',' ENUM_VAL {
                cgrSetContext($1,$3);
                $$=$3; }
           ;

ENUM_VAL : Id  {
             $$=cgrNew(Id,__sis_file,__sis_line);
             cgrSetStr($$,cgrKeyId,$1); }
         | Id '=' Num {
             $$=cgrNew(Id,__sis_file,__sis_line);
             cgrSetStr($$,cgrKeyId,$1); 
             cgrSetNum($$,cgrKeyIni,$3); 
             cgrSetNum($$,cgrKeyIsEnumSet,1); }
         ;

TAG_VARS : {$$=NULL;}
         | VAR_DEC_VAR {$$=cgrGetHead($1);}
         ;

TAG : Id  VAR_DEC_VAR {
        $$=cgrNew(Tag,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$1);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($2)); }
    ;


VAR_DEC_VAR
    : VAR_DEC_VAR2 {$$=$1;}
    | VAR_DEC_VAR ',' VAR_DEC_VAR2 {
         cgrSetContext($1,$3);
         $$=$3;}
    ;

VAR_DEC_VAR2
    : VAL4 {$$=$1;}
    | VAL4 '=' DFG {
        $$=cgrNew('=',__sis_file,__sis_line);
        cgrSetNode($1,cgrKeyPar,$$);
        cgrSetNode($3,cgrKeyPar,$$);
        cgrSetNode($$,cgrKeyLeft,$1);
        cgrSetNode($$,cgrKeyRight,$3); }
    ;

POINTER : {$$=NULL;} 
        | POINTER '*'{
            $$=cgrNew('*',__sis_file,__sis_line);
            cgrSetContext($1,$$); }
        | POINTER '&'{
            $$=cgrNew('&',__sis_file,__sis_line);
            cgrSetContext($1,$$); }
        ;

POINTER2 : '*' { $$=cgrNew('*',__sis_file,__sis_line); }
         | '&' { $$=cgrNew('&',__sis_file,__sis_line); }
         ;

ARG_DEC
    : {$$=NULL;}
    | ARG_DEC_VAR {$$=$1;}
    | ARG_DEC ',' ARG_DEC_VAR{
        cgrSetContext($1,$3);
        $$=$3; }
    ;

ARG_DEC_VAR
    : FUNCTION_TYPE POINTER {
        cgrSetNode($1,cgrKeyPointer,cgrGetHead($2));
        $$=$1;}
    | FUNCTION_TYPE POINTER OPE14 {
        cgrSetNode($1,cgrKeyVal,$3);
        cgrSetNode($1,cgrKeyPointer,cgrGetHead($2));
        $$=$1;}
    ;

CLASS_KEY_TAG :
    CLASS_KEY Id {
        $$=$1;
        cgrSetNum($$,cgrKeyIsClassTag,1);
        cgrSetStr($$,cgrKeyTag,$2); }
    ;

DATA_TYPE 
    : DATA_OPT C_DATA_TYPE {
          $$=cgrNew(Data_type,__sis_file,__sis_line);
          cgrSetNode($$,cgrKeyOpt,$1);
          cgrSetNode($$,cgrKeyType,$2); }
    | DATA_OPT SC_DATA_TYPE '<' Num '>' {
          $$=cgrNew(Data_type,__sis_file,__sis_line);
          cgrSetNode($$,cgrKeyOpt,$1);
          cgrSetNode($$,cgrKeyType,$2);
          cgrSetNum($$,cgrKeySize,$4); }
    ;

SC_DATA_TYPE
    : Sc_int     {$$=cgrNew(Sc_int,__sis_file,__sis_line);}
    | Sc_uint    {$$=cgrNew(Sc_uint,__sis_file,__sis_line);}
    | Sc_bigint  {$$=cgrNew(Sc_bigint,__sis_file,__sis_line);}
    | Sc_biguint {$$=cgrNew(Sc_biguint,__sis_file,__sis_line);}
    ;

C_DATA_TYPE 
    : Void      {$$=cgrNew(Void,__sis_file,__sis_line);}
    | Bool      {$$=cgrNew(Bool,__sis_file,__sis_line);}
    | Schar     {$$=cgrNew(Schar,__sis_file,__sis_line);}
    | Uchar     {$$=cgrNew(Uchar,__sis_file,__sis_line);}
    | Sshort    {$$=cgrNew(Sshort,__sis_file,__sis_line);}
    | Ushort    {$$=cgrNew(Ushort,__sis_file,__sis_line);}
    | Sint      {$$=cgrNew(Sint,__sis_file,__sis_line);}
    | Uint      {$$=cgrNew(Uint,__sis_file,__sis_line);}
    | Slong     {$$=cgrNew(Slong,__sis_file,__sis_line);}
    | Ulong     {$$=cgrNew(Ulong,__sis_file,__sis_line);}
    | Float     {$$=cgrNew(Float,__sis_file,__sis_line);}
    | Double    {$$=cgrNew(Double,__sis_file,__sis_line);}
    ; 


DATA_OPT : {$$=NULL;}
         | Data_opt {
             $$=cgrNew(Data_opt,__sis_file,__sis_line);
             cgrSetStr($$,cgrKeyVal,$1); }
         ;

CFG : {$$=NULL;}
    | CFG CFG_STATE{
        cgrSetContext($1,cgrGetTail($2));
        $$=cgrGetTail($2); }
    | CFG ';' { $$=$1;}    
    ;

CFG_STATE
    : DFG ';' {
        $$=cgrNew(Dfg,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$1);}
    | VAR_DEC  ';' {$$=$1;}
    | Break    ';' {$$=cgrNew(Break,__sis_file,__sis_line);}
    | Continue ';' {$$=cgrNew(Continue,__sis_file,__sis_line);}
    | RETURN   {$$=$1;} 
    | IF       {$$=$1;}
    | ELSE     {$$=$1;}
    | FOR      {$$=$1;}
    | DO       {$$=$1;}
    | WHILE    {$$=$1;}
    | SWITCH   {$$=$1;}
    | TYPEDEF  {$$=$1;}
    |'{' CFG '}' {
        $$=cgrNew(Block,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($2));}
    ;


RETURN : Return DFG ';' {
          $$=cgrNew(Return,__sis_file,__sis_line);
          cgrSetNode($$,cgrKeyVal,$2);}
       | Return ';' { $$=cgrNew(Return,__sis_file,__sis_line); }
       ;

SWITCH 
    : Switch '(' DFG ')' '{' CASE '}' {
        $$=cgrNew(Switch,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($6));}
    ;

CASE : {$$=NULL;}
     | CASE Case VAL ':' CFG {
        $$=cgrNew(Case,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
        cgrSetContext($1,$$);}
     | CASE Default  ':' CFG {
        $$=cgrNew(Default,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
        cgrSetContext($1,$$);}
     ;

DO : Do '{' CFG '}' While '(' DFG ')' ';' {
        $$=cgrNew(Do,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$7);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($3));}
   | Do '{' CFG '}' While '(' VAR_DEC ')' ';' {
        $$=cgrNew(Do,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$7);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($3));}
   ;

WHILE : While '(' DFG ')' CFG_STATE {
           $$=cgrNew(While,__sis_file,__sis_line);
           cgrSetNode($$,cgrKeyCond,$3);
           cgrSetNode($$,cgrKeyVal,$5);}
      | While '(' VAR_DEC ')' CFG_STATE {
           $$=cgrNew(While,__sis_file,__sis_line);
           cgrSetNode($$,cgrKeyCond,$3);
           cgrSetNode($$,cgrKeyVal,$5);}
      ;

FOR : For '(' FOR_DFG ';' DFG ';' FOR_DFG ')' CFG_STATE {
        $$=cgrNew(For,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyIni,cgrGetHead($3));
        cgrSetNode($$,cgrKeyCond,cgrGetHead($5));
        cgrSetNode($$,cgrKeyStep,cgrGetHead($7));
        cgrSetNode($$,cgrKeyVal,$9); }
    | For '(' FOR_DFG ';' ';' FOR_DFG ')' CFG_STATE {
        $$=cgrNew(For,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyIni,cgrGetHead($3));
        cgrSetNode($$,cgrKeyStep,cgrGetHead($6));
        cgrSetNode($$,cgrKeyVal,$8); }
    | For '(' FOR_DFG ';' DFG ';' FOR_DFG ')' ';' {
        $$=cgrNew(For,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyIni,cgrGetHead($3));
        cgrSetNode($$,cgrKeyCond,cgrGetHead($5));
        cgrSetNode($$,cgrKeyStep,cgrGetHead($7)); }
    | For '(' FOR_DFG ';' ';' FOR_DFG ')' ';' {
        $$=cgrNew(For,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyIni,cgrGetHead($3));
        cgrSetNode($$,cgrKeyStep,cgrGetHead($6)); }
    ;

FOR_DFG 
    : {$$=NULL;}
    | VAR_DEC { $$=$1;}
    | FOR_DFG_VAR {$$=$1;}
    ;

FOR_DFG_VAR 
    : DFG { 
        $$=cgrNew(Dfg,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$1); }
    | FOR_DFG_VAR ',' DFG {
        $$=cgrNew(Dfg,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$3);
        cgrSetContext($1,$$); }
    ;

IF : If '(' DFG ')' CFG_STATE {
        $$=cgrNew(If,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3);
        cgrSetNode($$,cgrKeyVal,$5);}
   | If '(' DFG ')' ';' {
        $$=cgrNew(If,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3); }
   ;
   
ELSE : Else CFG_STATE {
        $$=cgrNew(Else,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$2);}
     | Else ';' { $$=cgrNew(Else,__sis_file,__sis_line); }
     ;

DFG : OPE1 {$$=$1;}
    | OPE13 '='  OPE1 {
            $$=cgrNew('=',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Mueq OPE1 {
            $$=cgrNew(Mueq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Dieq OPE1 {
            $$=cgrNew(Dieq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Moeq OPE1 {
            $$=cgrNew(Moeq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Adeq OPE1 {
            $$=cgrNew(Adeq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Sueq OPE1 {
            $$=cgrNew(Sueq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Lseq OPE1 {
            $$=cgrNew(Lseq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Rseq OPE1 {
            $$=cgrNew(Rseq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Aneq OPE1 {
            $$=cgrNew(Aneq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Xoeq OPE1 {
            $$=cgrNew(Xoeq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    | OPE13 Oreq OPE1 {
            $$=cgrNew(Oreq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
    ;

OPE1 : OPE2 {$$=$1;}
     | OPE1 '?' OPE2 ':' OPE2 {
            $$=cgrNew('?',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($5,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyCond,$1);
            cgrSetNode($$,cgrKeyLeft,$3);
            cgrSetNode($$,cgrKeyRight,$5); }
     ;

OPE2 : OPE3 {$$=$1;}
     | OPE2 Oror OPE3 {
            $$=cgrNew(Oror,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     ;

OPE3 : OPE4 {$$=$1;}
     | OPE3 Anan OPE4 {
            $$=cgrNew(Anan,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     ;

OPE4 : OPE5 {$$=$1;}
     | OPE4 '|' OPE5 {
            $$=cgrNew('|',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE5 : OPE6 {$$=$1;}
     | OPE5 '^' OPE6 {
            $$=cgrNew('^',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE6 : OPE7 {$$=$1;}
     | OPE6 '&' OPE7 {
            $$=cgrNew('&',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE7 : OPE8 {$$=$1;}
     | OPE7 Eq OPE8 {
            $$=cgrNew(Eq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     | OPE7 Neq OPE8 {
            $$=cgrNew(Neq,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE8 : OPE9 {$$=$1;}
     | OPE8 Gre OPE9 {
            $$=cgrNew(Gre,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     | OPE8 Les OPE9 {
            $$=cgrNew(Les,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     | OPE8 '>' OPE9 {
            $$=cgrNew('>',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     | OPE8 '<' OPE9 {
            $$=cgrNew('<',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     ;

OPE9 : OPE10 {$$=$1;}
     | OPE9 Lshif OPE10 {
            $$=cgrNew(Lshif,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     | OPE9 Rshif OPE10 {
            $$=cgrNew(Rshif,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
     ;

OPE10 : OPE11 {$$=$1;}
      | OPE10 '+' OPE11 {
            $$=cgrNew('+',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
      | OPE10 '-' OPE11 {
            $$=cgrNew('-',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE11 : OPE12 {$$=$1;}
      | OPE11 '*' OPE12{
            $$=cgrNew('*',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
      | OPE11 '/' OPE12{
            $$=cgrNew('/',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }
      | OPE11 '%' OPE12{
            $$=cgrNew('%',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($$,cgrKeyRight,$3); }

OPE12 : OPE13 {$$=$1;}
      | Inc OPE13 {
            $$=cgrNew(Inc,__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | Dec OPE13 {
            $$=cgrNew(Dec,__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | OPE13 Inc {
            $$=cgrNew(Inc,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); }
      | OPE13 Dec {
            $$=cgrNew(Dec,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); }
      | '~' OPE13 {
            $$=cgrNew('~',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | '!' OPE13 {
            $$=cgrNew('!',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      ;

OPE13 : OPE14 {$$=$1;}
      | '*' OPE13 {
            $$=cgrNew('*',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | '&' OPE13 {
            $$=cgrNew('&',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | New OPE13 {
            $$=cgrNew(New,__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | Del OPE13 {
            $$=cgrNew(Del,__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | '(' DATA_TYPE POINTER ')' OPE14 {
            $$=cgrNew(Cast,__sis_file,__sis_line);
            cgrSetNode($5,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyType,$2);
            cgrSetNode($$,cgrKeyPointer,$3);
            cgrSetNode($$,cgrKeyCond,$5); }
      ;

OPE14 : OPE15 {$$=$1;}
      | '-' OPE15 {
            $$=cgrNew('-',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | '+' OPE15 {
            $$=cgrNew('+',__sis_file,__sis_line);
            cgrSetNode($2,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyRight,$2); }
      | OPE15 '(' ARGS ')' { 
            $$=cgrNew(Funcall,__sis_file,__sis_line);
            if($1->type==Id) {
                cgrSetStr($$,cgrKeyId,cgrGetStr($1,cgrKeyVal));
            }else{
                cgrSetNode($$,cgrKeyLeft,$1);
                cgrSetNode($1,cgrKeyPar,$$);
            }
            cgrSetNode($$,cgrKeyArgs,cgrGetHead($3));
            cgrSetNode(cgrGetHead($3),cgrKeyPar,$$); }
      ;

OPE15 : VAL {$$=$1;}
      | OPE15 Arr VAL3 {
            $$=cgrNew(Arr,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); 
            cgrSetNode($$,cgrKeyRight,$3); }
      | OPE15 '.' VAL3 {
            $$=cgrNew('.',__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); 
            cgrSetNode($$,cgrKeyRight,$3); }
    ;

VAL : VAL2 {$$=$1;}
    | Num{
        $$=cgrNew(Num,__sis_file,__sis_line);
        cgrSetNum($$,cgrKeyVal,$1); }
    | Real{
        $$=cgrNew(Real,__sis_file,__sis_line);
        cgrSetReal($$,cgrKeyVal,$1); }
    | String{
        $$=cgrNew(String,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$1); }
    ;

VAL2 : VAL3 {$$=$1;}
     | '(' CONNECT ')' {
        if(!($2->prev)) $$=$2;
        else {
            $$=cgrNew(Connect,__sis_file,__sis_line);
            cgrSetNode($$,cgrKeyCond,cgrGetHead($2));
            cgrSetNode(cgrGetHead($2),cgrKeyPar,$$); }}
     ;

VAL3 : VAL4    {$$=$1;}
     | FUNCALL {$$=$1;}
     ;

VAL4 : ID {$$=$1;}
     | VAL3 '[' DFG ']' {
            $$=cgrNew(Array,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); 
            cgrSetNode($$,cgrKeyRight,$3); }
    ;

VAL5 : ID  { $$=$1;}
     | VAL5 '[' DFG ']' {
            $$=cgrNew(Array,__sis_file,__sis_line);
            cgrSetNode($1,cgrKeyPar,$$);
            cgrSetNode($3,cgrKeyPar,$$);
            cgrSetNode($$,cgrKeyLeft,$1); 
            cgrSetNode($$,cgrKeyRight,$3); }
    ;

ID : Id {
       $$=cgrNew(Id,__sis_file,__sis_line);
       cgrSetStr($$,cgrKeyVal,$1); }
    ; 
     
CONNECT : DFG {$$=$1;}
        | CONNECT ',' DFG {
            cgrSetContext($1,$3);
            $$=$3; }
        ;

FUNCALL 
    : VAL4 '(' ARGS ')' {
        $$=cgrNew(Funcall,__sis_file,__sis_line);
        if($1->type==Id) {
            cgrSetStr($$,cgrKeyId,cgrGetStr($1,cgrKeyVal));
        }else{
            cgrSetNode($$,cgrKeyLeft,$1);
            cgrSetNode($1,cgrKeyPar,$$);
        }
        cgrSetNode($$,cgrKeyArgs,cgrGetHead($3));
        cgrSetNode(cgrGetHead($3),cgrKeyPar,$$); }
    ;

ARGS : {$$=NULL;}
     | DFG {$$=$1;}
     | ARGS ',' DFG{
         cgrSetContext($1,$3);
         $$=$3; }
     ;


%%
//
//wrap
//
int sister_wrap(void){
    return 1;
}
//
// error
//
static void sister_error(char* s){
    cmsgEcho(__sis_msg,2,__sis_file,__sis_line,sister_text,s);
}
//
// debug main
//
#ifdef DEBUG
int main(int argc,char**argv){
    return sister_parse();
}
#endif

