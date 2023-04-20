%{
/*
  parser for Vaster(VHDL).
  (c)KTDesignSystems, All Rights Reserved.
*/
#include "vaster_generic.h"

extern char*vaster_VHDL_text;
static void vaster_VHDL_error(char*s);
%}

%union{
    int num;
    char str[SIS_BUF_SIZE];
    double real;
    cgrNode node;
}

%token VH_time

%token VH_and VH_or VH_xor VH_nand VH_nor
%token VH_not VH_abs VH_mod VH_rem 
%token VH_mul_mul VH_not_eq VH_les VH_gre

%token VH_to VH_downto

%token VH_alias
%token VH_type VH_subtype VH_variable
%token VH_null VH_exit VH_next
%token VH_loop VH_for VH_while
%token VH_on VH_until VH_wait

%token VH_package VH_body
%token VH_library VH_use
%token VH_id VH_num
%token VH_is VH_of
%token VH_case VH_when VH_others
%token VH_process VH_block
%token VH_begin VH_end
%token VH_entity VH_archit

%token VH_return
%token VH_signal
%token VH_attribute
%token VH_procedure

%token VH_if VH_then VH_elsif VH_else
%token VH_generic
%token VH_generate
%token VH_component

%token VH_port VH_in VH_out VH_inout
%token VH_map

%token VH_const VH_right_vect VH_colon_eq

%token VH_range VH_right VH_left VH_high
%token VH_low VH_length VH_event VH_stable

/*dummy*/
%token VH_dfg VH_set
%token VH_instance VH_func VH_funcall
%token VH_funcargs VH_datatype
%token VH_comment

/*type*/
%type<node> DECLARE DECLARE2
%type<node> STATE_DFG
%type<node> ASSIGN ASSIGN2 ASSIGN_WHEN ASSIGN_ELSE
%type<node> VAL ATTR OPE6 OPE5 OPE4 OPE3 OPE2 OPE1 OPE
%type<node> FUNCALL FUNCALL_ARGS FUNCALL_ARGS2 FUNCALL_ARGS3
%type<node> SET SET2 SET3

%type<node> PACKAGE PACKAGE_BODY PACKAGE_BODY_BODY
%type<node> LIBRARY LIBRARY_ID 
%type<node> USE USE_ID 
%type<node> ENTITY ENTITY_END ENTITY_BODY ENTTITY_BODY2 
%type<node> STATE_NULL STATE_EXIT

%type<node> STATE_RETURN
%type<node> STATE_NEXT STATE_NEXT_WHEN 
%type<node> ENTITY_PORT  
%type<node> ARCHIT ARCHIT_END 
%type<node> UTIL_ID_LIST UTIL_ID_LIST2 UTIL_INIT 
%type<node> UTIL_VAR_DECL_LIST 
%type<node> UTIL_VAR_DECL UTIL_VAR_DECL_RANGE 
%type<node> UTIL_RANGE UTIL_RANGE_TO 
%type<node> UTIL_END UTIL_DECL UTIL_DECL2 
%type<node> UTIL_SIGTYPE UTIL_SIGTYPE2

%type<node> STATE_ALIAS STATE_ALIAS_RANGE 
%type<node> STATE_WAIT STATE_WAIT_ON 
%type<node> STATE_WAIT_UNTIL STATE_WAIT_FOR 
%type<node> STATE_VARIABLE STATE_TYPE 
%type<node> STATE_SUBTYPE 
%type<node> STATE_INSTANCE STATE_INSTANCE_TYPE 
%type<node> STATE_INSTANCE_BODY STATE_INSTANCE_PORT_MAP
%type<node> STATE_INSTANCE_PORT_MAP_BODY 
%type<node> STATE_INSTANCE_PORT_MAP_BODY2

%type<node> STATE_INSTANCE_GENERIC_MAP
%type<node> STATE_IF STATE_ELSE
%type<node> FUNCTION_DECL FUNCTION_DECL_RETURN 
%type<node> FUNCTION_DECL_TYPE FUNCTION_DECL_ARGS
%type<node> FUNCTION FUNCTION_END 
%type<node> SIGNAL STATE_PROCESS
%type<node> ATTRIBUTE ATTR_NAME ATTR_TYPE
%type<node> STATE_PROCESS_DECL STATE_PROCESS_SENS 
%type<node> STATE_CFG STATE_CFG2 STATE_BLOCK 
%type<node> STATE_BLOCK_GUARD 

%type<node> DATATYPE DATATYPE_ARGS 
%type<node> STATE_CONST 
%type<node> STATE_CASE STATE_CASE_BODY STATE_CASE_BODY2
%type<node> STATE_COMPONENT 
%type<node> STATE_FOR STATE_WHILE STATE_LOOP 
%type<node> STATE_LOOP_TYPE STATE_LOOP_ID
%type<node> STATE_GENERATE STATE_GENERIC 
%type<str> VH_id VH_num VH_time

%%

//=========================================================
// parser root
//=========================================================
ROOT : DECLARE {
       cgrSetRoot(cgrGetHead($1));
       }
     ;
     
DECLARE 
    : {$$=NULL;} /*empty*/
    | DECLARE DECLARE2  {
        $$=$2;
        cgrSetContext($1,$$);
      }
    ;

DECLARE2
    : PACKAGE {       //package
      $$=$1;
      }
    
    | PACKAGE_BODY{   //package body
      $$=$1;
      }

    | ENTITY {        //module
      $$=$1; 
      }

    | ARCHIT {        //architechrue
      $$=$1; 
      }

    | LIBRARY {       //library
      $$=$1; 
      }

    | USE     {       //use
      $$=$1; 
      }

    | FUNCTION    {   //function
      $$=$1; 
      }

    | STATE_TYPE  {   //type declaration
      $$=$1; 
      }

    | STATE_SUBTYPE { //type declaration
      $$=$1; 
      }

    | STATE_VARIABLE{ //variable declaration
      $$=$1; 
      }

    | STATE_COMPONENT{ //component
      $$=$1; 
      }

    | STATE_ALIAS{     //aliase
      $$=$1; 
      }
    
    | STATE_CONST{     //const
      $$=$1; 
      }


    ;

//=========================================================
// package
//=========================================================
PACKAGE 
    : VH_package VH_id VH_is
      UTIL_DECL
      VH_end UTIL_END ';' {
      $$=cgrNew(VH_package,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyDeclared,cgrGetHead($4));
      }
    ;

PACKAGE_BODY 
    : VH_package VH_body VH_id VH_is
      PACKAGE_BODY_BODY VH_end UTIL_END ';' {
      $$=cgrNew(VH_package,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$3);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      }
    ;

PACKAGE_BODY_BODY 
    : DECLARE {$$=cgrGetHead($1);}
    ;

//=========================================================
// library
//=========================================================
LIBRARY 
    : VH_library LIBRARY_ID ';' {
        $$=cgrNew(VH_library,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($2));
        }
    ;

LIBRARY_ID 
    : VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$1);
      }
    | LIBRARY_ID ',' VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$3);
        cgrSetContext($1,$$);
      }
    ;

//=========================================================
// use
//=========================================================
USE : VH_use USE_ID ';' {
        $$=cgrNew(VH_use,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($2));
        }
    ;

USE_ID 
    : VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$1);
      }
    | USE_ID '.' VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$3);
        cgrSetContext($1,$$);
      }
    ;

//=========================================================
// entity
//=========================================================
ENTITY 
    : VH_entity VH_id VH_is 
      ENTITY_BODY VH_end ENTITY_END ';' {
        $$=cgrNew(VH_entity,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$2);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;

ENTITY_END 
    : UTIL_END           {$$=NULL;}
    | VH_entity UTIL_END {$$=NULL;}
    ;

ENTITY_BODY 
    : {$$=NULL;}/*empty*/
    | ENTITY_BODY ENTTITY_BODY2 {
        $$=$2;
        cgrSetContext($1,$2);
      }
    ;

ENTTITY_BODY2 
    : ENTITY_PORT   {$$=$1;}
    | STATE_GENERIC {$$=$1;}
    ;
//=========================================================
// port
//=========================================================
ENTITY_PORT
    : VH_port '(' UTIL_VAR_DECL_LIST ')' ';' {
        $$=cgrNew(VH_port,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($3));
      }
    ;

//=========================================================
// generic
//=========================================================
STATE_GENERIC 
    : VH_generic '(' UTIL_VAR_DECL_LIST ')' ';' {
      $$=cgrNew(VH_generic,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($3));
      }
    ;
//=========================================================
// data_type
//=========================================================
DATATYPE 
    : VH_id {
      $$=cgrNew(VH_datatype,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$1);
      }
    | VH_id '(' DATATYPE_ARGS ')' {
      $$=cgrNew(VH_datatype,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$1);
      cgrSetNode($$,cgrKeyArgs,cgrGetHead($3));
      }
    | VH_signal {
      $$=cgrNew(VH_datatype,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,"signal");
      }
    ;

DATATYPE_ARGS 
    : {$$=NULL;}/*empty*/
    | UTIL_RANGE_TO {
      $$=$1;
      }
    ;
//=========================================================
// architecture
//=========================================================
ARCHIT 
    : VH_archit VH_id VH_of USE_ID VH_is
      UTIL_DECL
      VH_begin 
      STATE_CFG
      VH_end ARCHIT_END ';' {
        $$=cgrNew(VH_archit,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$2);
        cgrSetNode($$,cgrKeyPar,$4);
        cgrSetNode($$,cgrKeyDeclared,cgrGetHead($6));
        cgrSetNode($$,cgrKeyVal,cgrGetHead($8));
      }
    ;

ARCHIT_END 
    : UTIL_END {$$=NULL;}
    | VH_archit UTIL_END {$$=NULL;}
    ;


//=========================================================
// util state
//=========================================================
UTIL_ID_LIST 
    : {$$=NULL;}/*empty*/
    | UTIL_ID_LIST2 {
        $$= cgrGetHead($1);
      }
    ;

UTIL_ID_LIST2
    : VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$1);
     }
    | UTIL_ID_LIST2 ',' VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$3);
        cgrSetContext($1,$$);
      }
    ;

UTIL_INIT 
    : {$$=NULL;}/*empty*/
    | VH_colon_eq OPE {
      $$=$2;
      }
    ;

UTIL_VAR_DECL_LIST 
    : {$$=NULL;}/*empty*/
    | UTIL_VAR_DECL {$$=$1;}
    | UTIL_VAR_DECL_LIST ';' UTIL_VAR_DECL {
      $$=$3;
      cgrSetContext($1,$3);
      }
    ;
    
UTIL_VAR_DECL
    : UTIL_ID_LIST ':' UTIL_SIGTYPE DATATYPE 
        UTIL_VAR_DECL_RANGE UTIL_INIT {
        $$=cgrNew(VH_variable,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyId,cgrGetHead($1));
        cgrSetNode($$,cgrKeySignal,$3);
        cgrSetNode($$,cgrKeyType,$4);
        cgrSetNode($$,cgrKeyOpt,$5);
        cgrSetNode($$,cgrKeyIni,$6);
      }
    ;

UTIL_VAR_DECL_RANGE 
    : {$$=NULL;} /*empty*/
    | UTIL_RANGE {$$=$1;}
    ;

UTIL_SIGTYPE 
    : {$$=NULL;} /*empty*/
    | UTIL_SIGTYPE2 {$$=$1;}
    ;

UTIL_SIGTYPE2
    : VH_in   {$$=cgrNew(VH_in,__sis_file,__sis_line);}
    | VH_out  {$$=cgrNew(VH_out,__sis_file,__sis_line);}
    | VH_inout{$$=cgrNew(VH_inout,__sis_file,__sis_line);}
    ;

UTIL_RANGE 
    : VH_range UTIL_RANGE_TO {
        $$=cgrNew(VH_range,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$2);
      }
    ;

UTIL_RANGE_TO 
    : OPE VH_to OPE {
      $$=cgrCreateOpe(VH_to,$1,$3,NULL); 
      }
    | OPE VH_downto OPE{
      $$=cgrCreateOpe(VH_downto,$1,$3,NULL); 
      }
    ;

UTIL_END 
    : {$$=NULL;}/*empty*/
    | VH_id {
        $$=cgrNew(VH_id,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,$1);
      }
    ;

UTIL_DECL 
    : {$$=NULL;}/*empty*/
    | UTIL_DECL UTIL_DECL2 {
        $$=$2;
        cgrSetContext($1,$$);
      }
    ;

UTIL_DECL2 
    : STATE_VARIABLE   {$$=$1;}
    | STATE_SUBTYPE    {$$=$1;}
    | STATE_TYPE       {$$=$1;}
    | STATE_ALIAS      {$$=$1;}
    | STATE_COMPONENT  {$$=$1;}
    | STATE_CONST      {$$=$1;}
    | FUNCTION         {$$=$1;}
    | SIGNAL           {$$=$1;}
    | ATTRIBUTE        {$$=$1;}
    ;

//=========================================================
// alias
//=========================================================
STATE_ALIAS 
    : VH_alias VH_id VH_is OPE6 STATE_ALIAS_RANGE ';' {
      $$=cgrNew(VH_alias,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyVal,$4);
      cgrSetNode($$,cgrKeyOpt,$5);
      }
    ;

STATE_ALIAS_RANGE 
    : {$$=NULL;}/*empty*/
    |'[' UTIL_ID_LIST ']' {
      $$=cgrGetHead($2);
      }
//=========================================================
// variable
//=========================================================
STATE_VARIABLE 
    : VH_variable UTIL_VAR_DECL ';' {
      $$=cgrNew(VH_variable,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,$2);
      }
    ;
//=========================================================
// signal
//=========================================================
SIGNAL 
    : VH_signal UTIL_VAR_DECL ';' {
        $$=cgrNew(VH_signal,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$2);
      }
    ;
//=========================================================
// attribute
//=========================================================
ATTRIBUTE
    : VH_attribute ATTR_NAME ':' ATTR_TYPE ';' {
        $$=cgrNew(VH_attribute,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyId,$2);
        cgrSetNode($$,cgrKeyType,$4);
        }
    ;

ATTR_NAME 
    : VH_id {
      $$=cgrNew(VH_id,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$1); 
      }
    | VH_id VH_of OPE {
      $$=cgrNew(VH_id,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$1); 
      cgrSetNode($$,cgrKeySignal,$3); 
      }
    ;

ATTR_TYPE 
    : DATATYPE {$$=$1;}
    | DATATYPE VH_is VAL {
        $$=$1;
        cgrSetNode($$,cgrKeySignal,$3);
      }
    ;

//=========================================================
// type
//=========================================================
STATE_TYPE 
    : VH_type VH_id VH_is '(' UTIL_ID_LIST ')' ';' {
      $$=cgrNew(VH_type,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      }
    ;

//=========================================================
// subtype
//=========================================================
STATE_SUBTYPE 
    : VH_subtype VH_id VH_is DATATYPE UTIL_VAR_DECL_RANGE ';'{
      $$=cgrNew(VH_subtype,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyType,$4);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      }
    ;

//=========================================================
// constant
//=========================================================
STATE_CONST
    : VH_const UTIL_VAR_DECL ';' {
      $$=cgrNew(VH_const,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,$2);
      }
    ;
//=========================================================
// component
//=========================================================
STATE_COMPONENT
    : VH_component VH_id
      ENTITY_BODY 
      VH_end VH_component ';' {
      $$=cgrNew(VH_component,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($3));
      }
    ;
//=========================================================
// function declare
//=========================================================
FUNCTION_DECL 
    : FUNCTION_DECL_TYPE VH_id FUNCTION_DECL_ARGS
      FUNCTION_DECL_RETURN {
        $$=cgrNew(VH_func,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyType,$1);
        cgrSetStr($$,cgrKeyId,$2);
        cgrSetNode($$,cgrKeyArgs,cgrGetHead($3));
        cgrSetNode($$,cgrKeyProcType,cgrGetHead($4));
      }
    ;

FUNCTION_DECL_RETURN 
    : {$$=NULL;}/*empty*/
    | VH_return DATATYPE {
        $$=$2;
      }
    ;

FUNCTION_DECL_TYPE 
    : VH_func {
        $$=cgrNew(VH_func,__sis_file,__sis_line);
      }
    | VH_procedure {
        $$=cgrNew(VH_procedure,__sis_file,__sis_line);
      }
    ;

FUNCTION_DECL_ARGS
    : {$$=NULL;}/*empty*/
    | '(' UTIL_VAR_DECL_LIST ')' {
        $$=$2;
      }
    ;

//=========================================================
// function definition
//=========================================================
FUNCTION 
    : FUNCTION_DECL ';' {
      $$=$1;
     }
    | FUNCTION_DECL VH_is UTIL_DECL
      VH_begin STATE_CFG VH_end FUNCTION_END ';' {
      $$=$1;
      cgrSetNode($$,cgrKeyDeclared,cgrGetHead($3));
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      }
    ;

FUNCTION_END 
    : UTIL_END {$$=$1;}
    | FUNCTION_DECL_TYPE UTIL_END {
      $$=$2;
      }
    ;


//=========================================================
// STATE_CFG
//=========================================================
STATE_CFG 
    : {$$=NULL;}/*empty*/
    | STATE_CFG STATE_CFG2 {
      $$=$2;
      cgrSetContext($1,$$);
      }
    ;
    
STATE_CFG2 
    : STATE_DFG     {$$=$1;}
    | STATE_BLOCK   {$$=$1;}
    | STATE_PROCESS {$$=$1;}
    | STATE_INSTANCE{$$=$1;}
    | STATE_IF      {$$=$1;}
    | STATE_LOOP    {$$=$1;}
    | STATE_GENERATE{$$=$1;}
    | STATE_NULL    {$$=$1;}
    | STATE_NEXT    {$$=$1;}
    | STATE_RETURN  {$$=$1;}
    | STATE_EXIT    {$$=$1;}
    | STATE_CASE    {$$=$1;}
    | STATE_WAIT    {$$=$1;}
    ;
//=========================================================
// block
//=========================================================
STATE_BLOCK 
    : VH_id ':' VH_block STATE_BLOCK_GUARD
      UTIL_DECL
      VH_begin
      STATE_CFG
      VH_end VH_block UTIL_END ';' {
        $$=cgrNew(VH_block,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyId,$1);
        cgrSetNode($$,cgrKeyCond,$4);
        cgrSetNode($$,cgrKeyDeclared,$5);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($7));
      }
    ;

STATE_BLOCK_GUARD 
    : {$$=NULL;} /*empty*/
    | '(' OPE ')' {
      $$=$2;
      }
    ;

//=========================================================
// process
//=========================================================
STATE_PROCESS
    : STATE_LOOP_ID VH_process STATE_PROCESS_SENS  
      STATE_PROCESS_DECL
      VH_begin
      STATE_CFG
      VH_end VH_process UTIL_END ';' {
        $$=cgrNew(VH_process,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyId,$1);
        cgrSetNode($$,cgrKeySensitive,$3);
        cgrSetNode($$,cgrKeyDeclared,$4);
        cgrSetNode($$,cgrKeyVal,cgrGetHead($6));
      }
    ;

STATE_PROCESS_DECL 
    : UTIL_DECL {
        $$=cgrGetHead($1);
      }
    | UTIL_END VH_is UTIL_DECL {
      $$=cgrGetHead($3);
      }
    ;

STATE_PROCESS_SENS 
    : {$$=NULL;}/*empty*/
    | '(' UTIL_ID_LIST ')' {
      $$=cgrGetHead($2);
      }
    ;


//=========================================================
// instance
//=========================================================
STATE_INSTANCE
    : VH_id ':' STATE_INSTANCE_TYPE OPE6 
        STATE_INSTANCE_BODY ';' {
      $$=cgrNew(VH_instance,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$1);
      cgrSetNode($$,cgrKeyType,$3);
      cgrSetNode($$,cgrKeyDeclared,cgrGetHead($4));
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      }
    ;

STATE_INSTANCE_TYPE 
    : {$$=NULL;} /*empty*/
    | VH_entity {
      $$=cgrNew(VH_entity,__sis_file,__sis_line);}
    ;

STATE_INSTANCE_BODY
    : {$$=NULL;}/*empty*/
    | STATE_INSTANCE_BODY STATE_INSTANCE_PORT_MAP    {
        $$=$2;
        cgrSetContext($1,$2);
        }
    | STATE_INSTANCE_BODY STATE_INSTANCE_GENERIC_MAP {
        $$=$2;
        cgrSetContext($1,$2);
        }
    ;

//=========================================================
// port map
//=========================================================
STATE_INSTANCE_PORT_MAP
    : VH_port VH_map '(' STATE_INSTANCE_PORT_MAP_BODY ')' {
      $$=cgrNew(VH_port,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;
 
STATE_INSTANCE_PORT_MAP_BODY 
    : {$$=NULL;} /*empty*/
    | STATE_INSTANCE_PORT_MAP_BODY2 {$$=$1;}
    | STATE_INSTANCE_PORT_MAP_BODY ',' STATE_INSTANCE_PORT_MAP_BODY2{
      $$=$3;
      cgrSetContext($1,$$);
      }
    ;

STATE_INSTANCE_PORT_MAP_BODY2
    :  VAL {
       $$=cgrNew(VH_id,__sis_file,__sis_line);
       cgrSetNode($$,cgrKeyVal,$1);
       }
    |  VAL VH_right_vect VAL {
       $$=cgrNew(VH_id,__sis_file,__sis_line);
       cgrSetNode($$,cgrKeyId,$1);
       cgrSetNode($$,cgrKeyVal,$3);
       }
    ;

//=========================================================
// generic map
//=========================================================
STATE_INSTANCE_GENERIC_MAP
    : VH_generic VH_map '(' STATE_INSTANCE_PORT_MAP_BODY ')' {
      $$=cgrNew(VH_generic,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;
 
//=========================================================
// if
//=========================================================
STATE_IF 
    : VH_if OPE VH_then 
      STATE_CFG
      STATE_ELSE
      VH_end VH_if ';' {
      $$=cgrNew(VH_if,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      cgrSetNode($$,cgrKeyStep,cgrGetHead($5));
      }
    ;

STATE_ELSE
    : {$$=NULL;}/*empty*/
    | STATE_ELSE VH_elsif OPE VH_then STATE_CFG {
      $$=cgrNew(VH_else,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$3);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($5));
      cgrSetContext($1,$$);
      }
    | STATE_ELSE VH_else STATE_CFG {
      $$=cgrNew(VH_else,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($3));
      cgrSetContext($1,$$);
      }
    ;

//=========================================================
// for
//=========================================================
STATE_FOR 
    : VH_for VH_id VH_in UTIL_RANGE_TO {
      $$=cgrNew(VH_for,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyCond,$4);
    }
    | VH_for VH_id VH_in OPE { 
      $$=cgrNew(VH_for,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$2);
      cgrSetNode($$,cgrKeyCond,$4);
    }

   ; 

//=========================================================
// while
//=========================================================
STATE_WHILE
    : VH_while OPE {
      $$=cgrNew(VH_while,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$2);
      }
    ; 
//=========================================================
// loop
//=========================================================
STATE_LOOP 
    : STATE_LOOP_ID STATE_LOOP_TYPE VH_loop
      STATE_CFG
      VH_end VH_loop UTIL_END ';' {
      $$=cgrNew(VH_loop,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyId,$1);
      cgrSetNode($$,cgrKeyType,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;

STATE_LOOP_TYPE 
    : {$$=NULL;}/*empty*/
    | STATE_FOR {
      $$=$1;
      }
    | STATE_WHILE{
      $$=$1;
      }
    ;

STATE_LOOP_ID
    : {$$=NULL;}/*empty*/
    | VH_id ':' {
      $$=cgrNew(VH_id,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$1);
      }
    ;

//=========================================================
// generate
//=========================================================
STATE_GENERATE
    : STATE_LOOP_ID STATE_LOOP_TYPE VH_generate
      STATE_CFG2
      VH_end VH_generate UTIL_END ';' {
      $$=cgrNew(VH_generate,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyId,$1);
      cgrSetNode($$,cgrKeyType,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;


//=========================================================
// case
//=========================================================
STATE_CASE 
    : VH_case OPE VH_is 
      STATE_CASE_BODY 
      VH_end VH_case ';' {
      $$=cgrNew(VH_case,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
      }
    ;

STATE_CASE_BODY 
    : {$$=NULL;} /*empty*/
    | STATE_CASE_BODY STATE_CASE_BODY2 {
      $$=$2;
      cgrSetContext($1,$$);
      }
    ;

STATE_CASE_BODY2
    : VH_when OPE VH_right_vect STATE_CFG{
      $$=cgrNew(VH_when,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
    }
    | VH_when UTIL_RANGE_TO VH_right_vect STATE_CFG {
      $$=cgrNew(VH_when,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$2);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
    }

    | VH_when VH_others VH_right_vect STATE_CFG{
      $$=cgrNew(VH_when,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($4));
    }
    ;

//=========================================================
// null
//=========================================================
STATE_NULL 
    : VH_null ';' {
        $$=cgrNew(VH_null,__sis_file,__sis_line);
      }
    ;

//=========================================================
// exit
//=========================================================
STATE_EXIT
    : VH_exit UTIL_END STATE_NEXT_WHEN ';' {
        $$=cgrNew(VH_exit,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3);
      }
    ;


//=========================================================
// next
//=========================================================
STATE_NEXT 
    : VH_next UTIL_END STATE_NEXT_WHEN ';' {
        $$=cgrNew(VH_next,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyCond,$3);
      }
    ;

STATE_NEXT_WHEN 
    : {$$=NULL;}/*empty*/
    | VH_when OPE {
        $$=$2;
      }
    ;

//=========================================================
// return
//=========================================================
STATE_RETURN 
    : VH_return OPE ';' {
      $$=cgrNew(VH_return,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,$2);
      }
    ;


//=========================================================
// wait
//=========================================================
STATE_WAIT 
    : VH_wait STATE_WAIT_ON 
      STATE_WAIT_UNTIL STATE_WAIT_FOR ';' {
      $$=cgrNew(VH_wait,__sis_file,__sis_line);
      if($4) cgrAddNode($$,cgrKeyCond,$4);
      if($3) cgrAddNode($$,cgrKeyCond,$3);
      if($2) cgrAddNode($$,cgrKeyCond,$2);
      }
    ;

STATE_WAIT_ON 
    : {$$=NULL;}/*empty*/
    | VH_on UTIL_ID_LIST {
      $$=cgrNew(VH_on,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,cgrGetHead($2));
      }
    ;

STATE_WAIT_UNTIL
    : {$$=NULL;}/*empty*/
    | VH_until OPE {
      $$=cgrNew(VH_until,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal,$2);
      }
    ;

STATE_WAIT_FOR 
    : {$$=NULL;}/*empty*/
    | VH_for VH_num VH_time {
      $$=cgrNew(VH_for,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$2);
      cgrSetStr($$,cgrKeyType,$3);
      }
    ;



//=========================================================
// DFG/operation
//=========================================================
STATE_DFG
    : ASSIGN ASSIGN_WHEN ';' {
      $$=cgrNew(VH_dfg,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyVal ,$1);
      cgrSetNode($$,cgrKeyCond,$2);
      }
    ;

ASSIGN_WHEN 
    : /*empty*/ {$$=NULL;}
    | VH_when OPE ASSIGN_ELSE {
      $$=cgrNew(VH_when,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$3);
      cgrSetNode($$,cgrKeyVal,$2); }
    ;

ASSIGN_ELSE 
    : /*empty*/ {$$=NULL;}
    | VH_else OPE ASSIGN_WHEN {
      $$=cgrNew(VH_else,__sis_file,__sis_line);
      cgrSetNode($$,cgrKeyCond,$3);
      cgrSetNode($$,cgrKeyVal,$2); }
    ;

ASSIGN 
    : ASSIGN2 {$$=$1;}
    | VAL VH_les ASSIGN2 {
      $$=cgrCreateOpe(VH_les,$1,$3,NULL); }
    | VAL VH_colon_eq ASSIGN2 {
      $$=cgrCreateOpe(VH_colon_eq,$1,$3,NULL); }
    ;

ASSIGN2 
    : OPE2 {$$=$1;}
    | ASSIGN2 VH_and  OPE2 {
      $$=cgrCreateOpe(VH_and,$1,$3,NULL); }
    | ASSIGN2 VH_or   OPE2 {
      $$=cgrCreateOpe(VH_or,$1,$3,NULL); }
    | ASSIGN2 VH_xor  OPE2 {
      $$=cgrCreateOpe(VH_xor,$1,$3,NULL); }
    | ASSIGN2 VH_nand OPE2 {
      $$=cgrCreateOpe(VH_nand,$1,$3,NULL); }
    | ASSIGN2 VH_nor  OPE2 {
      $$=cgrCreateOpe(VH_nor,$1,$3,NULL); }
    ;


//=========================================================
// operation
//=========================================================
OPE : OPE1 {$$=$1;}
    | OPE VH_and  OPE1 {
      $$=cgrCreateOpe(VH_and,$1,$3,NULL); }
    | OPE VH_or   OPE1 {
      $$=cgrCreateOpe(VH_or,$1,$3,NULL); }
    | OPE VH_xor  OPE1 {
      $$=cgrCreateOpe(VH_xor,$1,$3,NULL); }
    | OPE VH_nand OPE1 {
      $$=cgrCreateOpe(VH_nand,$1,$3,NULL); }
    | OPE VH_nor  OPE1 {
      $$=cgrCreateOpe(VH_nor,$1,$3,NULL); }
    ;

OPE1: OPE2 {$$=$1;}
    | OPE1 '='       OPE2 {
      $$=cgrCreateOpe('=',$1,$3,NULL); }
    | OPE1 VH_not_eq OPE2 {
      $$=cgrCreateOpe(VH_not_eq,$1,$3,NULL); }
    | OPE1 '<'       OPE2 {
      $$=cgrCreateOpe('<',$1,$3,NULL); }
    | OPE1 '>'       OPE2 {
      $$=cgrCreateOpe('>',$1,$3,NULL); }
    | OPE1 VH_les    OPE2 {
      $$=cgrCreateOpe(VH_les,$1,$3,NULL); }
    | OPE1 VH_gre    OPE2 {
      $$=cgrCreateOpe(VH_gre,$1,$3,NULL); }
    ;

OPE2: OPE3 {$$=$1;}
    | OPE2 '&' OPE3 {
      $$=cgrCreateOpe('&',$1,$3,NULL); }
    | OPE2 '+' OPE3 {
      $$=cgrCreateOpe('+',$1,$3,NULL); }
    | OPE2 '-' OPE3 {
      $$=cgrCreateOpe('-',$1,$3,NULL); }
    ;

OPE3: OPE4 {$$=$1;}
    | '+' OPE4 {
      $$=cgrCreateOpe('+',NULL,$2,NULL); }
    | '-' OPE4 {
      $$=cgrCreateOpe('+',NULL,$2,NULL); }
    ;

OPE4: OPE5 {$$=$1;}
    | OPE4 '*'    OPE5{
      $$=cgrCreateOpe('*',$1,$3,NULL); }
      
    | OPE4 '/'    OPE5{
      $$=cgrCreateOpe('/',$1,$3,NULL); }

    | OPE4 VH_mod OPE5{
      $$=cgrCreateOpe(VH_mod,$1,$3,NULL); }

    | OPE4 VH_rem OPE5{
      $$=cgrCreateOpe(VH_rem,$1,$3,NULL); }
    ;

OPE5: OPE6 {$$=$1;}
    | VH_not OPE6 {
      $$=cgrCreateOpe(VH_not,NULL,$2,NULL); }

    | VH_abs OPE6{
      $$=cgrCreateOpe(VH_abs,NULL,$2,NULL); }

    | OPE5 VH_mul_mul OPE6{
      $$=cgrCreateOpe(VH_mul_mul,$1,$3,NULL); }
    ;

OPE6: VAL {$$=$1;}
    | OPE6 '.' VAL {
      $$=cgrCreateOpe('.',$1,$3,NULL); }
    ;

VAL : VH_id ATTR {
      $$=cgrNew(VH_id,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$1); 
      cgrSetNode($$,cgrKeyOpt,$2); }

    | VH_num {
      $$=cgrNew(VH_num,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyVal,$1); }

    | '(' OPE ')' { $$=$2;} 

    | SET     { $$=$1;}
    | FUNCALL { $$=$1;}
    ;

SET : '(' SET2 ')' {
       $$=cgrNew(VH_set,__sis_file,__sis_line);
       cgrSetNode($$,cgrKeyVal,cgrGetHead($2));
      }
    ;

SET2 : SET3 {$$=$1;}
     | SET2 ',' SET3{
       $$=$3;
       cgrSetContext($1,$3);
       }
     ;

SET3 : OPE VH_right_vect OPE {
       $$=cgrNew(VH_id,__sis_file,__sis_line);
       cgrSetNode($$,cgrKeyCond,$1);
       cgrSetNode($$,cgrKeyVal,$3);
       }

     | VH_others VH_right_vect OPE{
       $$=cgrNew(VH_id,__sis_file,__sis_line);
       cgrSetNode($$,cgrKeyVal,$3);
       }
     ;

ATTR: {$$=NULL;}/*empty*/

    | '\'' VH_range {
        $$=cgrNew(VH_range,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"range");
        }

    | '\'' VH_right{
        $$=cgrNew(VH_right,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"right");
        }

    | '\'' VH_left{
        $$=cgrNew(VH_left,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"left");
        }

    | '\'' VH_high{
        $$=cgrNew(VH_high,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"high");
        }
    
    | '\'' VH_low{
        $$=cgrNew(VH_low,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"low");
        }
    
    | '\'' VH_length{
        $$=cgrNew(VH_length,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"length");
        }
    
    | '\'' VH_event{
        $$=cgrNew(VH_event,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"event");
        }
    
    | '\'' VH_stable{
        $$=cgrNew(VH_stable,__sis_file,__sis_line);
        cgrSetStr($$,cgrKeyVal,"stable");
        }
    
    | '\'' '(' FUNCALL_ARGS ')' {
        $$=cgrNew(VH_funcargs,__sis_file,__sis_line);
        cgrSetNode($$,cgrKeyVal,$3);
        }
    ;

FUNCALL 
    : VH_id '(' FUNCALL_ARGS ')' {
      $$=cgrNew(VH_funcall,__sis_file,__sis_line);
      cgrSetStr($$,cgrKeyId,$1);
      cgrSetNode($$,cgrKeyArgs,cgrGetHead($3));
      cgrSetNode(cgrGetHead($3),cgrKeyPar,$$); }
    ;

FUNCALL_ARGS 
    : /*empty*/ {$$=NULL;} 
    | UTIL_RANGE_TO {$$=$1;}
    | FUNCALL_ARGS2 {$$=$1;}
    ;

FUNCALL_ARGS2 
    : FUNCALL_ARGS3 {$$=$1;}
    | FUNCALL_ARGS2 ',' FUNCALL_ARGS3{
      cgrSetContext($1,$3);
      $$=$3; }
    ;

FUNCALL_ARGS3 
    : OPE {$$=$1;} 
    | VH_id VH_right_vect OPE { 
      $$=cgrCreateOpe(VH_right_vect,NULL,$3,NULL);
      cgrSetStr($$,cgrKeyVal,$1);
      } 
    ;

%%

//
//wrap
//
int vaster_VHDL_wrap(void){
    return 1;
}
//
// error
//
static void vaster_VHDL_error(char* s){
   cmsgEcho(__sis_msg,2,__sis_file,__sis_line,vaster_VHDL_text,s);
}

