/*
  pattern matching utilities .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef __PTNUTIL
#define __PTNUTIL

//
// pattern
//
enum _ptnutilPattern{
    ptnutilType_None,//error

    //Id pattern
    ptnutilType_A,   //aaa
    ptnutilType_B,   //aaa[3][2][1]
    ptnutilType_C,   //aaa(4)
    ptnutilType_D,   //aaa(4)[10][20]
    ptnutilType_E,   //aaa[10][20](3)
    
    //signal connection pattern
    ptnutilType_F,   //u0.sig(sig);
    ptnutilType_G,   //u0->sig(sig);

    //read/write function pattern
    ptnutilType_H,   //x.read();
    ptnutilType_I,   //x.write(A);
    ptnutilType_J,   //x[].read();
    ptnutilType_K,   //x[].write(A);

    //cfg structure
    ptnutilType_WhileDfg,      //while-dfg     --while
    ptnutilType_WhileCnd,      //while-cond
    ptnutilType_DoDfg,         //do-dfg        --do
    ptnutilType_DoCnd,         //do-cond
    ptnutilType_ForDfg,        //for-dfg       --for
    ptnutilType_ForIni,        //for-init
    ptnutilType_ForIniDataType,//for-init-datatype
    ptnutilType_ForCnd,        //for-cond
    ptnutilType_ForStp,        //for-step
    ptnutilType_IfDfg,         //if-dfg        --if
    ptnutilType_IfCnd,         //if-cond
    ptnutilType_SwCnd,         //switch-cond   --switch
    ptnutilType_CaseDfg,       //case-dfg      --case
    ptnutilType_Dfg,           //dfg           --dfg

    //checking state machine
    ptnutilType_If_If,         //loop: If-If
    ptnutilType_If_Sw,         //loop: If-Switch
    ptnutilType_Sw_If,         //loop: Switch-If
    ptnutilType_Sw_Sw,         //loop: Switch-Switch

    //checking condition(If/Switchi) pattern
    ptnutilType_CondIf,           //loop: if
    ptnutilType_CondIfElse,       //loop: if-else
    ptnutilType_CondIfElseElse,   //loop: if-else if ... -else
    ptnutilType_CondDef,          //loop: default
    ptnutilType_CondCase,         //loop: case
    ptnutilType_CondCaseDef,      //loop: case-default
    ptnutilType_CondCaseCase,     //loop: case-case 
    ptnutilType_CondCaseCaseDef,  //loop: case-case ... -default

};

//pattern matching (Id)
extern int ptnutilMatch(cgrNode node);

//pattern matching (signal connection)
extern int ptnutilMatchSigCon(cgrNode node);

//pattern matching (read/write function)
extern int ptnutilMatchReadWrite(cgrNode node);

/* pattern matching (cfg struct)
    SC_THREAD modele : func(...){ while(1){...}} */
extern int ptnutilMatchCfgStruct(cVector stack);

//pattern matching (state machine)
extern int ptnutilMatchStateMachine(cVector stack);

//pattern matching (condition (If/Switch) pattern)
extern int ptnutilMatchCondIfSw(cVector stack);
#endif
