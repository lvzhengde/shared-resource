/*
  Vaster preprocess constant values
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __VPREPROC_CONST_HEAD
#define __VPREPROC_CONST_HEAD

enum __VPREPROC_TYPE{
    //None
    Vpreproc_type_None=1,
    
    //ID
    Vpreproc_type_ID,
    Vpreproc_type_MID,

    //parameter
    Vpreproc_type_PARAM,  //###

    //macro
    Vpreproc_type_RESET_ALL,    //###
    Vpreproc_type_TIMESCALE,    //###
    Vpreproc_type_DEFINE,       //###
    Vpreproc_type_INCLUDE,      //###
    Vpreproc_type_IFDEF,        //###
    Vpreproc_type_ELSE,         //###
    Vpreproc_type_ENDIF,        //###
    Vpreproc_type_DEFAULT_NETTYPE,    //###
    Vpreproc_type_UNCONNECTED_DRIVE,  //###
    Vpreproc_type_NOUNCONNECTED_DRIVE,//###
    Vpreproc_type_SIGNED,   //###
    Vpreproc_type_UNSIGNED, //###

    //comment
    Vpreproc_type_COMMENT,//###

    //space
    Vpreproc_type_SPACE,

    //end line
    Vpreproc_type_EL,  //escaped
    Vpreproc_type_EEL, //escaped
};

#endif

