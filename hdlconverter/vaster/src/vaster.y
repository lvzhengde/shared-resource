%{
/*
  parser for Vaster.
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "vaster_generic.h"

extern char*vaster_text;
static void vaster_error(char*s);
%}

%union{
    int num;
    char str[SIS_BUF_SIZE];
    double real;
    cgrNode node;
}

%expect 1

%token V_delay_num
%token V_begin V_end

%token V_posedge V_negedge V_s_or 

%token V_module V_endmodule
%token V_always V_assign V_initial
%token V_wire V_reg V_input V_output V_integer

%token V_id V_num V_string
%token V_rshif V_lshif 
%token V_neq V_eq 
%token V_nor V_nand V_ninv
%token V_oror V_anan V_gre V_les

%token V_if V_else
%token V_case V_endcase V_default
%token V_for V_while

%%

//parser root
ROOT : DECLARE
     ;
     
DECLARE
    : //null
    | DECLARE MODULE //module
    ;

MODULE 
    : V_module V_id MODULE_ARGS ';' 
      MODULE_BLOCK
      V_endmodule
    ;

MODULE_ARGS 
    : //null
    | '(' ')'
    | '(' MODULE_ARGS2 ')'
    ;

MODULE_ARGS2
    : V_id
    | MODULE_ARGS2 ',' V_id
    ;
MODULE_BLOCK 
    : MODULE_BLOCK2
    ;

MODULE_BLOCK2 
    : //null
    | MODULE_BLOCK2 DEC_ASSIGN
    | MODULE_BLOCK2 DEC_PROCESS
    | MODULE_BLOCK2 DEC_INITIAL
    | MODULE_BLOCK2 DEC_VARIABLE
    ;

DELAY 
    : V_delay_num
    ;

DEC_ASSIGN
    : V_assign ASSIGNMENTS ';'
    | V_assign DELAY ASSIGNMENTS ';'
    ;

ASSIGNMENTS 
    : DFG 
    | ',' DFG
    ;

DEC_PROCESS
    : V_always '@' SENSITIVE STATE
    | V_always DELAY STATE
    ;

SENSITIVE 
    : '(' SENSITIVE_LIST ')'
    ;

SENSITIVE_LIST 
    : SENSITIVE_LIST2
    | SENSITIVE_LIST V_s_or SENSITIVE_LIST2
    ;

SENSITIVE_LIST2
    : V_id
    | V_posedge V_id
    | V_negedge V_id
    ;

BLOCK 
    : V_begin STATE_LIST V_end
    ;

STATE_LIST 
    : //null
    | STATE_LIST STATE
    ;

STATE 
    : ';'
    | DFG ';'
    | DELAY
    | STATE_IF
    | STATE_CASE
    | STATE_FOR 
    | STATE_WHILE 
    | BLOCK
    ;

STATE_FOR 
    : V_for '(' DFG ';' OPE_IF ';' DFG ')' STATE
    ;

STATE_WHILE 
    : V_while '('OPE_IF')' STATE
    ;

STATE_CASE 
    : V_case '(' OPE_IF ')' STATE_CASE_ITEM V_endcase
    ;

STATE_CASE_ITEM 
    : //null
    | STATE_CASE_ITEM V_num     ':' STATE
    | STATE_CASE_ITEM V_default ':' STATE
    ;

STATE_IF 
    : STATE_IF2
    | STATE_IF2 V_else STATE
    ;

STATE_IF2 
    : V_if '(' OPE_IF ')' STATE
    ;

DEC_INITIAL 
    : V_initial STATE
    ;

DEC_VARIABLE 
    : VAR_TYPE VAR_ARRAY VARIABLE ';'
    ;

VARIABLE 
    : V_id VAR_ARRAY
    | VARIABLE ',' V_id VAR_ARRAY
    ;

VAR_TYPE 
    : V_reg
    | V_wire
    | V_input
    | V_output
    | V_integer
    ;

VAR_ARRAY 
    : //null
    | '[' DFG ':' DFG ']'
    ;

DFG : OPE1 
    | OPE12 '='   OPE1 
    | OPE12 V_les OPE1 
    | OPE12 '='   DELAY OPE1 
    | OPE12 V_les DELAY OPE1 
    ;

OPE1 : OPE2 
     | OPE1 '?' OPE2 ':' OPE2 
     ;

OPE2 : OPE3 
     | OPE2 V_oror OPE3 
     ;

OPE3 : OPE4 
     | OPE3 V_anan OPE4 
     ;

OPE4 : OPE5 
     | OPE4 '|'   OPE5 
     | OPE4 V_nor OPE5
    ;

OPE5 : OPE6 
     | OPE5 '^'    OPE6 
     | OPE5 V_ninv OPE6 
     ;

OPE6 : OPE7 
     | OPE6 '&'    OPE7 
     | OPE6 V_nand OPE7 
     ;

OPE7 : OPE8
     | OPE7 V_lshif OPE8 
     | OPE7 V_rshif OPE8
     ;

OPE8 : OPE9
     | OPE8 '+' OPE9 
     | OPE8 '-' OPE9 
     ;

OPE9 : OPE10 
     | OPE9 '*' OPE10
     | OPE9 '/' OPE10
     | OPE9 '%' OPE10
     ;

OPE10 : OPE11 
      | '~' OPE11
      | '!' OPE11 
      ;

OPE11 : OPE12 
      | '-' OPE12 
      | '+' OPE12
      ;

OPE12 : VAL 
      | OPE12 '.' VAL3 
      ;

VAL : VAL2
    | V_num
    | V_string
    ;

VAL2 : VAL3 
     | '(' DFG ')' 
     | '{' CONNECT '}' 
     ;

VAL3 : ID 
     | VAL3 '[' DFG ']' 
     ;

ID  : V_id
    ; 
     
CONNECT : DFG 
        | CONNECT ',' DFG 
        ;

OPE_IF 
    : OPE_IF1
    ;

OPE_IF1 
    : OPE_IF2 
    | OPE_IF1 '?' OPE_IF2 ':' OPE_IF2 
    ;

OPE_IF2
    : OPE_IF3 
    | OPE_IF2 V_oror OPE_IF3 
    ;

OPE_IF3 
    : OPE_IF4 
    | OPE_IF3 V_anan OPE_IF4 
    ;

OPE_IF4
    : OPE_IF5 
    | OPE_IF4 '|'   OPE_IF5 
    | OPE_IF4 V_nor OPE_IF5
    ;

OPE_IF5
    : OPE_IF6 
    | OPE_IF5 '^'    OPE_IF6 
    | OPE_IF5 V_ninv OPE_IF6 
    ;

OPE_IF6
    : OPE_IF7 
    | OPE_IF6 '&'    OPE_IF7 
    | OPE_IF6 V_nand OPE_IF7 
    ;

OPE_IF7
    : OPE_IF8 
    | OPE_IF7 V_eq  OPE_IF8 
    | OPE_IF7 V_neq OPE_IF8 
    ;

OPE_IF8
    : OPE7 
    | OPE_IF8 V_gre OPE7
    | OPE_IF8 V_les OPE7 
    | OPE_IF8 '>' OPE7
    | OPE_IF8 '<' OPE7 
    ;

%%
//
//wrap
//
int vaster_wrap(void){
    return 1;
}
//
// error
//
static void vaster_error(char* s){
   cmsgEcho(__sis_msg,2,__sis_file,__sis_line,vaster_text,s);
}

