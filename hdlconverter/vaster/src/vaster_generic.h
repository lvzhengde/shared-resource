/*
    vaster header
    (c)KTDesignSystemc Corp.
*/

#ifndef __VASTER_GENERIC_HEAD
#define __VASTER_GENERIC_HEAD

#include "sister.h"

//
// generic option handler
//
typedef struct _VasOptHandle{
    
    //program information ----
    char*self;    //command name
    char help;    //help
    char version; //version
    
    //I/O files ---
    int   inform;  //input format(0:verilog,1:VHDL)
    char* outfile; //output file
    cVector files; //input file list

    //process
    char prepro;   //preprocess

}*VasOptHandle;


//preprocesser
extern int vprepro(VasOptHandle opt);
extern int vprepro_VHDL(VasOptHandle opt);

//parser
#include "vprepro.h"
extern int vaster_parse(void);
extern int vaster_VHDL_parse(void);

//init/destruct
extern VasOptHandle vasOptIni(void);
extern void vasOptHandleDes(VasOptHandle opt);

//priprocessing(-E) and parsing
extern int vPars(VasOptHandle opt);

#include "ovhdl.h"
#include "vhser.h"
#endif

