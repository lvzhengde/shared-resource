/*
 Sister the Behavior Synthesizer
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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sister.h"

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>

/*------------------------------------------------------------------
  global
-------------------------------------------------------------------*/
cMap __cioMemMap;

/*------------------------------------------------------------------
   version
-------------------------------------------------------------------*/
void dispVersion(void){
    printf("\n\
*+. Sister(%s) the High-level Synthesizer.           \n\
*+. (c)KTDesignSystems Corp, All Rights Reserved.    \n\
    BSD Style Lisence, tsuchiya@ktds.co.jp           \n\
    \n",VERSION);
}

/*------------------------------------------------------------------
   help
-------------------------------------------------------------------*/
void dispHelp(void){
    printf("\
Usage:sister [options] SystemC-file ...                             \n\
   options:                                                         \n\
      -o <filename>  : to specify the output file name.             \n\
                                                                    \n\
      -add <num>  : the max number of adders(or subers).            \n\
      -mul <num>  : the max number of multipliers.                  \n\
      -div <num>  : the max number of dividers.                     \n\
                                                                    \n\
      -pipe <delay> : to specify the pipeline multipliers           \n\
                      delay cycles (default:1).                     \n\
                                                                    \n\
      -chain        : temporary register is not created             \n\
                                                                    \n\
      --reset-name <regex> : to specify reset signal name pattern.  \n\
                             by regular expression.                 \n\
                   default : ([rR][sS][tT])|([rR][eE][sS][eE][tT])  \n\
                                                                    \n\
      --no-reset   : to do not create reset process automatically.  \n\
      --no-schedule: to do not schedule.(CA->RTL transration)       \n\
                     only verilog HDL output format.                \n\
                                                                    \n\
      -E  : only preprocessing.                                     \n\
      -E1 : only preprocessing without GCC preprocesser.            \n\
                                                                    \n\
      -C  : to stop after checking syntax.                          \n\
      -C2 : to stop after checking syntax without preprocessing.    \n\
                                                                    \n\
      --systemc : to output as SystemC format.                      \n\
      --dump  : to dump the FSMD for debugging.                     \n\
      --dump2 : to dump the FSMD after scheduling for debugging.    \n\
                                                                    \n\
      --help   : displaying help.                                   \n\
      --version: displaying version.                                \n\
                                                                    \n\
  If there is a problem,please send mail to me.                     \n\
  tsuchiya@ktds.co.jp ,                                             \n\
  (c)KTDesignSystems Corp, All Rights Reserved.                     \n\
    \n");
}

/*------------------------------------------------------------------
  analyzing arguments
-------------------------------------------------------------------*/
typedef struct{
    char* self;    //self executable file
    char* outfile; //output file name
    uchar help;    //displaying help
    uchar version; //displaying version
    uchar prepro;  //preprocessing
    uchar prepro1; //preprocessing without gcc
    uchar check;   //checking syntax
    uchar check2;  //checking syntax without preprocessing
    uchar debug;   //to dump the FSMD
    uchar debug2;  //to dump the FSMD after scheduling
    uchar sysc;    //to output as SystemC format
    uchar noreset; //to do not create reset process
    uchar nosched; //to do not sucheduling
    uchar add;     //adders(and subbers)
    uchar mul;     //multilpiers
    uchar div;     //dividers
    uchar pipe;    //pipeline
    uchar chain;   //chaining
    char*rstname;  //reset name pattern
    cVector files; //systemc files
} argsHandle;

//constractor
static argsHandle conArgsHandle(void){
    argsHandle args;
    args.help=0;
    args.version=0;
    args.prepro=0;
    args.prepro1=0;
    args.check=0;
    args.check2=0;
    args.debug=0;
    args.debug2=0;
    args.sysc=0;
    args.noreset=0;
    args.nosched=0;
    args.outfile="out";
    args.add=12;
    args.mul=1;
    args.div=1;
    args.pipe=1;
    args.chain=0;
    args.rstname="([rR][sS][tT])|([rR][eE][sS][eE][tT])";
    args.files=cvectIni(sizeof(char*),25);
    return args;
}

//destractor
static void desArgsHandle(argsHandle args){
    cvectDes(args.files);
}

//analyze main
argsHandle analyArgs(int argc,char**argv){
    int i;
    argsHandle args;
    args=conArgsHandle();
    args.self=argv[0];
    for(i=1;i<argc;i++){
        if(!strcmp(argv[i],"--help")) args.help=1;
        else if(!strcmp(argv[i],"--version")) args.version=1;
        else if(!strcmp(argv[i],"--systemc")) args.sysc=1;
        else if(!strcmp(argv[i],"--dump")) args.debug=1;
        else if(!strcmp(argv[i],"--dump2")) args.debug2=1;
        else if(!strcmp(argv[i],"-E"))  args.prepro=1;
        else if(!strcmp(argv[i],"-E1")) args.prepro1=1;
        else if(!strcmp(argv[i],"-C"))  args.check=1;
        else if(!strcmp(argv[i],"-C2")) args.check2=1;
        else if(!strcmp(argv[i],"--no-reset"))    args.noreset=1;
        else if(!strcmp(argv[i],"--no-schedule")) args.nosched=1;
        else if(!strcmp(argv[i],"-chain")) args.chain=1;
        else if(!strcmp(argv[i],"-o")){
            if((++i)>=argc) break;
            args.outfile=argv[i];
        }else if(!strcmp(argv[i],"-add")){
            if((++i)>=argc) break;
            args.add=atoi(argv[i]);
        }else if(!strcmp(argv[i],"-mul")){
            if((++i)>=argc) break;
            args.mul=atoi(argv[i]);
        }else if(!strcmp(argv[i],"-div")){
            if((++i)>=argc) break;
            args.div=atoi(argv[i]);
        }else if(!strcmp(argv[i],"-pipe")){
            if((++i)>=argc) break;
            args.pipe=atoi(argv[i]);
        }else if(!strcmp(argv[i],"--reset-name")){
            if((++i)>=argc) break;
            args.rstname=argv[i];
        }else if('-'==argv[i][0]) cmsgEcho(__sis_msg,1,argv[i]);
        else cvectAppend(args.files,argv[i]);
    }
    return args;
}


/*------------------------------------------------------------------
   to start process
-------------------------------------------------------------------*/
//checking if file exists
static int checkFileExists(argsHandle args){
    int i,ret=0;
    for(i=0;i<cvectSize(args.files);i++){
        char* fname=cvectElt(args.files,i,char*);
        FILE* fp=fopen(fname,"r");
        if(!fp){ 
            cmsgEcho(__sis_msg,0,fname);
            ret=-1;
        }else fclose(fp);
    }
    return ret;
}
//step executing
//0:nothing,1:executing,-1:error
static int stepexec(argsHandle args){
    int ret=0;
    
    //-E1 : only preprocessing without GCC preprocesser.
    if(args.prepro1) ret=prelex();
    //-C2 : to stop after checking syntax without preprocessing.
    else if(args.check2) ret=sister_parse();
    //normally
    else return 0;

    if(ret) return -1;
    return 1;
}

//preprocessing wrapper
//0:normally,1:executing and stop,-1:error
static int preprocess(argsHandle args){
    return preprocess2(args.self,args.files,args.prepro);
}

//parsing
static int pars(argsHandle args){
    int ret;
    chk2OptHandle chk2=chk2OptDefault();

    if(cgrIni()<0) return -1;
    //parse
    ret=sister_parse();
    wait(NULL);
    if(ret<0) return -1;
    chk2.noreset=args.noreset;
    chk2.outfile=args.outfile;
    chk2.rstname=args.rstname;
    chk2.chain=args.chain;
    ret=check(cgrGetRoot(),chk2);
    if(ret<0) return -1;
    if(args.check) return 1;

    //to dump the FSMD
    if(args.debug) dumpFsmd(cgrGetRoot(),SISTER_DUMP);
    
    return 0;
}

//synthesizing
static int synthesize(argsHandle args){
    int ret;
    time_t tm=time(NULL);
    schHandle sch=schOptDefault();
    optHandle opt=optDefault();
    chk2OptHandle chk2=chk2OptDefault();
    
    //optimization
    ret=optimize(opt,cgrGetRoot());
    if(ret) return -1;
    
    //making header and footer
     mkHedFot(cgrGetRoot(),"Sister",args.outfile,ctime(&tm));

    //output without scheduling
    if(args.nosched) return 0;

    //checking after optimization
    chk2.noreset=args.noreset;
    chk2.outfile=args.outfile;
    chk2.rstname=args.rstname;
    chk2.chain=args.chain;
    ret=check2(cgrGetRoot(),chk2);
    if(ret<0) return -1;

    //scheduling
    sch.adsb=args.add;
    sch.mul=args.mul;
    sch.div=args.div;
    sch.pipe=args.pipe;
    ret=schedule(sch,cgrGetRoot());
    if(ret) return -1;

    return 0;
}

//output
static int output(argsHandle args){
    //numbering
    numbFsmd(cgrGetRoot());

    //SystemC format
    if(args.sysc) 
        return outsysc(args.outfile,cgrGetRoot());

    //Verilog HDL format
    else 
        return outverilog(args.outfile,cgrGetRoot());

    return 0;
}

//process main
#define ctrProc(ret)      \
    if(ret<0) return -1;  \
    else if(ret) return 0;
static int run(argsHandle args){
    int ret;
    //checking if file exists
    if(checkFileExists(args)) return -1;
    
    //preprocessing
    ret=preprocess(args);
    ctrProc(ret);
    
    //checking syntax
    ret=pars(args);
    ctrProc(ret);
    
    //synthesizing
    ret=synthesize(args);
    ctrProc(ret);
    
    //to dump the FSMD
    if(args.debug2) dumpFsmd(cgrGetRoot(),SISTER_DUMP2);

    //output without scheduling
    if(args.nosched){
        ret=nosche(args.outfile,cgrGetRoot());
        if(ret<0) return -1;
        return 0;
    }
    //output with scheduling
    ret=output(args);
    ctrProc(ret);

    return 0;
}

/*------------------------------------------------------------------
   main
-------------------------------------------------------------------*/
#define destruction(args){\
    desMsg();             \
    cgrDes();             \
    desArgsHandle(args);  \
    cioDesMem();}

int main(int argc,char**argv){
    int ret;
    argsHandle args;
    
    //Memory initialization
    __cioMemMap=cioMemIni();
    
    //message initialization
    conMsg();

    //analyzing arguments
    args=analyArgs(argc,argv);
    
    //displaying version
    if(args.version) dispVersion();

    //displaying help 
    if(args.help) dispHelp();

    //step execute
    ret=stepexec(args);
    if(ret<0){
        cmsgPrintEW(__sis_msg);
        destruction(args);
        return -1;
    }else if(ret){
        destruction(args);
        return 0;
    }

    //nothing args
    if(!(args.help||args.version||
        cvectSize(args.files))){
        printf("sister: no input file.\n");
        destruction(args);
        return -1;
    }else if((args.help||args.version) &&
      !cvectSize(args.files)) return 0;

    //to start process
    if(run(args)){
        cmsgPrintEW(__sis_msg);
        destruction(args);
        return -1;
    }

    //Memory free
    destruction(args);
    return 0;
}

