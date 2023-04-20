/*
 Vaster the RTL->CA converter
 (c)KTDesignSystems, All Rights Reserved.

 #Lisence (BSD Style)

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above
 copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided
 with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE FREEBSD PROJECT ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.;
*/

/*------------------------------------------------------------------
    includes
-------------------------------------------------------------------*/
#include <stdio.h>
#include "vaster.h"

/*------------------------------------------------------------------
    global variables
-------------------------------------------------------------------*/
cMap __cioMemMap; //memory map

/*------------------------------------------------------------------
   version
-------------------------------------------------------------------*/
void dispVersion(void){
    printf("\n\
*+. Vaster (%s)                                      \n\
*+. http://ktds.co.jp, support@ktds.co.jp            \n\
*+. (c)KTDesignSystems Corp, All Rights Reserved.    \n\
    \n",VERSION);
}

/*------------------------------------------------------------------
   help
-------------------------------------------------------------------*/
void dispHelp(void){
    printf("\
Usage: vaster [options] VerilogHDL-file ...                           \n\
      options:                                                        \n\
                                                                      \n\
      -o <filename>  : to specify the output file name.               \n\
                       default file name is 'out'                     \n\
                                                                      \n\
      -vhdl : setting input file format to VHDL                       \n\
              default : verilogHDL.                                   \n\
                                                                      \n\
      -E  : only preprocessing.                                       \n\
                                                                      \n\
      --help   : displaying help.                                     \n\
      --version: displaying version.                                  \n\
                                                                      \n\
  http://ktds.co.jp, support@ktds.co.jp                               \n\
  (c)KTDesignSystems Corp, All Rights Reserved.                       \n\
      \n");
}


/*------------------------------------------------------------------
    displaying information    
-------------------------------------------------------------------*/
static void printInfo(VasOptHandle opt){
    if(opt->version) dispVersion();
    if(opt->help)    dispHelp();
}
/*------------------------------------------------------------------
    checking args
-------------------------------------------------------------------*/
static VasOptHandle chkArgs(int argc,char**argv){
    int i; 
    VasOptHandle args=vasOptIni();
    args->self=argv[0];
    for(i=1;i<argc;i++){
        if(!strcmp(argv[i],"--help")) args->help=1;
        else if(!strcmp(argv[i],"--version")) args->version=1;
        else if(!strcmp(argv[i],"-E"))    args->prepro=1;
        else if(!strcmp(argv[i],"-vhdl")) args->inform=1;
        else if(!strcmp(argv[i],"-o")){
            if((++i)>=argc) break;
            args->outfile=argv[i];
        }else if('-'==argv[i][0]) cmsgEcho(__sis_msg,1,argv[i]);
        else cvectAppend(args->files,argv[i]);
    }   

    return args;
}

/*------------------------------------------------------------------
    init
-------------------------------------------------------------------*/
static VasOptHandle globalIni(int argc,char**argv){
    __cioMemMap=cioMemIni(); //memory map ini
    conMsg();                //message initialization
    if(cgrIni()<0) return NULL; //cgr initialization
    return chkArgs(argc,argv);
}
/*------------------------------------------------------------------
    destruction
-------------------------------------------------------------------*/
static void globalDes(VasOptHandle opt){
    desMsg();
    cgrDes();
    vasOptHandleDes(opt);
    cioDesMem();
}

/*------------------------------------------------------------------
    convert run
-------------------------------------------------------------------*/
static int runConv(VasOptHandle opt){
    int ret=0;
    
    //parsing
    ret=vPars(opt);
    if(ret) return -1;

    //convert
    //###

    return ret;
}
/*------------------------------------------------------------------
    start convert
-------------------------------------------------------------------*/
static int startConv(VasOptHandle opt){
    int ret=0;

    if(opt->prepro){ //only preprocess
        if(opt->inform==0) ret=vprepro(opt)     ; //verilog
        else               ret=vprepro_VHDL(opt); //VHDL
    
    }else ret=runConv(opt); //convert
    return ret;
}

/*------------------------------------------------------------------
    main
-------------------------------------------------------------------*/
int main(int argc,char**argv){
    int ret=0;

    //init(read option)
    VasOptHandle opt=globalIni(argc,argv);
    if(!opt){
        memError("unknown");
        return -1;
    }

    //displaying information
    printInfo(opt);

    //start convert
    if(!(opt->help||opt->version||
        cvectSize(opt->files))){
        printf("vaster: no input file.\n");
        ret=-1;
    }else{
        ret=startConv(opt);
        if(ret) cmsgPrintEW(__sis_msg);
    }
    //destruction
    globalDes(opt);

    return ret;
}

