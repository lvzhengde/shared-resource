/*
  control graph .
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef CGR_HEAD
#define CGR_HEAD

//
//conts
//
enum _cgrKeyType{
    //for parse
    cgrKeyId=1,
    cgrKeyVal,
    cgrKeyOpt,
    cgrKeyIni,
    cgrKeyPar,
    cgrKeySize,
    cgrKeyCond,
    cgrKeyLeft,
    cgrKeyRight,
    cgrKeyArgs,
    cgrKeyType,
    cgrKeyStep,
    cgrKeyPointer,
    cgrKeyIsInstancePort,    //instance port
    cgrKeyCtor,              //SC_CTOR
    cgrKeyIsPartSelec,       //part selected variable
    cgrKeyClassDec,          //class declar node
    cgrKeyClassBody,         //class body
    cgrKeyIsClassTag,        //variable declaration with class tag
    cgrKeyClassElmIndex,     //class variable's bit position
    cgrKeyClassElmOffset,    //class variable's offset
    cgrKeyClassElmOffsetDef, //class variable's offset
    cgrKeyTag,               //enum , struct etc ... tag
    cgrKeyEnumBody,          //enum body
    cgrKeyEnumMax,           //max enum number 
    cgrKeyIsFuncBody,        //None:prototype declare, 1:function body
    cgrKeyIsEnumSet,         //None:Id, 1:Id=Num 
    cgrKeyIsOutOfModule,     //the function declared out of module
    cgrKeyIdLinkTask,        //id node links task node
    cgrKeyIsNonBlocking,     //no blocking assignment
    cgrKeyIsFifo,            //the node was sc_fifo
    cgrKeyDeclared,          //declared variable/function
    cgrKeyDelayDeclared,     //delay variable declare node
    cgrKeyProcType,          //process type
                             //0:SC_THREAD,1:SC_METHOD
    cgrKeyFifoSigType,       //0:non,1:ready,2:strobe

    //for synthesize
    cgrKeyTask,
    cgrKeyProcess,
    cgrKeyMethFlg,
    cgrKeySignal,
    cgrKeyAddress,
    cgrKeyInstance,
    cgrKeySensitive,
    cgrKeySensitivePos,
    cgrKeySensitiveNeg,
    cgrKeyArray,
    cgrKeyArraySize,
    cgrKeyJump,
    cgrKeyBack,
    cgrKeyVarType, //0:normaly,1:temporaly
    cgrKeyFuncInter,
    cgrKeyPipeDummy,
    cgrKeyCheckVar,
    cgrKeySchVarListNext, //next node of variable list (for scheduling)
    cgrKeyWait,
    cgrKeyInput,       //process input variables
    cgrKeyOutput,      //process output variables
    cgrKeyNextProcess, //next process
    cgrKeyPrevProcess, //prev process
    cgrKeyClkDomain,   //clock domain
    cgrKeyLogicType,   //0:combinantial,1:sequential
    cgrKeyHandshakeReady,   //ready signal for handshake
    cgrKeyHandshakeStrobe,  //strobe signal for handshake
    cgrKeyDecChanged,       //changed declared node
    cgrKeyDecIsChanged,     //is changed declared node
    cgrKeyTlResetOut  ,     //for TL model reset process
    
    //for output
    cgrKeyReset,
    cgrKeyResetProc,
    cgrKeyNumber,
    cgrKeyStateVar,
    cgrKeyLoopFront,
    cgrKeyLoopBack,
    cgrKeyWire,
    cgrKeyMemMap,
    cgrKeyFormSw,  //format switch 0:both,1:systemc,2:verilogHDL

    //end of conts
    cgrKeyTypeEnd,
};

//
//node definition
//
typedef struct _cgrNode{
    uint type;
    cMap map;
    uint line;
    char* file;
    struct _cgrNode* next;
    struct _cgrNode* prev;
    struct _cgrNode* jump;
}* cgrNode;

//
//functions
//
//initialization
extern int cgrIni(void); 
//destruction
extern void cgrDes(void);
//getting root node
extern cgrNode cgrGetRoot(void); 
//setting root node
extern void cgrSetRoot(cgrNode node); 
//new node creation
extern cgrNode cgrNew(int type,char*file,int line); 
//setting attribute as number
extern void cgrSetNum(cgrNode node,int key,int val); 
//setting attribute as string
extern void cgrSetStr(cgrNode node,int key,char* val);
//setting attribute as real number
extern void cgrSetReal(cgrNode node,int key,double val);
//setting attribute as a node
extern void cgrSetNode(cgrNode node,int key,cgrNode val);
//getting attribute as number
extern int cgrGetNum(cgrNode node,int key);
//getting attribute as string
extern char* cgrGetStr(cgrNode node,int key);
//getting attribute as real number
extern double cgrGetReal(cgrNode node,int key);
//getting attribute as a node
extern cgrNode cgrGetNode(cgrNode node,int key);
//getting the head node
extern cgrNode cgrGetHead(cgrNode node);
//getting the tail node
extern cgrNode cgrGetTail(cgrNode node);
//adding the node
extern void cgrAddNode(cgrNode node0,int key,cgrNode node1);
//removing the node
extern void cgrRemoveNode(cgrNode par,int key,cgrNode node);
//inserting the node
extern void cgrInstNode(cgrNode prev,cgrNode next);
//replacing the node
extern void cgrReplaceNode(
    cgrNode be,cgrNode af,cgrNode par,int key);
//setting context between two nodes
extern void cgrSetContext(cgrNode node0,cgrNode node1);
//getting bit width from operator
extern int cgrBitWidth(cgrNode node);
//operator cleation with linked node
extern cgrNode cgrCreateOpe(int type,
    cgrNode left,cgrNode right,cgrNode cond);
//getting bit width from number
extern int cgrBitWidthNum(int num);
//copying cfg node
extern cgrNode cgrCopyCfg(cgrNode node);
//copying function node
extern cgrNode cgrCopyFunction(cgrNode node);
//copy data type node
extern int cgrCopyDataType5(cgrNode dec,cgrNode out);
extern cgrNode cgrCopyDataType4(cgrNode dec);
extern cgrNode cgrCopyDataType3(cgrNode dec);
extern int cgrCopyDataType2(cgrNode dec,cgrNode out);
extern int cgrCopyDataType(cgrNode dec,cgrNode out);
// copy signal declaration
extern cgrNode cgrCopySignalKey(cgrNode node);
//get max left node
extern cgrNode cgrGetMaxLeft(cgrNode node);
//operator identification 0:no,1:yes
extern int cgrIdentOpe(cgrNode a,cgrNode b);
// setting suffix string to variables
extern int cgrSetSuffix(cgrNode ope,char*suf);
extern int cgrSetSuffixList(cgrNode ope,char*suf);
//make suffix string
extern char* cgrSetSuffixString(char*s,char*suffix);
//getting name;
extern char* cgrGetIdName(cgrNode node);
//making variable declare node from signal_key
extern cgrNode cgrMakeDecFromSignalKey(cgrNode sig);
//appending node
extern int cgrAppendNode(cgrNode prev,cgrNode next);

//checking name competition from list
//   -1 : error 
//    0 : normaly
//    1 : competition
extern int cgrCheckCompet(cgrNode par,int key,char*name);

//checcking and to declare left and right node
#define cgrDecLR(node,ret)                      \
    cgrNode left,right;                         \
    if(!node) return ret;                       \
    left=cgrGetNode(node,cgrKeyLeft);           \
    right=cgrGetNode(node,cgrKeyRight); 
//loop for list    
#define cgrMaps(__loop_key__,__FUNCTION__)   \
    while(__loop_key__){                     \
    cgrNode __loop_next__=__loop_key__->next;\
    __FUNCTION__;                            \
    __loop_key__=__loop_next__;              }
    

#endif

