/*
    vaster API object
    (c)KTDesignSystemc Corp.
*/

#include "vaster.h"

/*------------------------------------------------------------------
    option handler init
-------------------------------------------------------------------*/
VasOptHandle vasOptIni(void){
    VasOptHandle opt;
    cstrMakeBuf(opt,1,struct _VasOptHandle,memError("initialization"));
    
    //information
    opt->self="vaster";
    opt->help=0;
    opt->version=0;

    // I/O
    opt->inform=0;
    opt->outfile="out";
    opt->files=cvectIni(sizeof(char*),5);

    //process
    opt->prepro=0;

    return opt;
}


/*------------------------------------------------------------------
    handler destruction
-------------------------------------------------------------------*/
void vasOptHandleDes(VasOptHandle opt){
    cvectDes(opt->files);
    free(opt);
}

/*------------------------------------------------------------------
   parsing
-------------------------------------------------------------------*/
int vPars(VasOptHandle opt){
    int ret=0;
    pid_t chp;
    int pfd[2];

    //making pipeline
    if(pipe(pfd)<0) memError("preprocessing");
    chp=fork();
    //making subprocess
    if(chp<0) memError("preprocessing");

    //parent
    if(chp){
        //setting pipeline to preprocess
        dup2(0,pfd[1]);
        dup2(pfd[0],0);
        close(pfd[0]);
        //parsing
        if(opt->inform==0) ret=vaster_parse(); //verilog
        else ret=vaster_VHDL_parse(); //VHDL
        wait(NULL);
        dup2(pfd[1],0);
        close(pfd[1]);
    //subprocess
    }else{
        int i=2,j=2;
        char*buf[SIS_BUF_SIZE+4];
        buf[0]=opt->self;
        buf[1]="-E";
        if(opt->inform==1){
            buf[2]="-vhdl";//VHDL
            i++;
            j++;
        }
        for(i;(i-j)<cvectSize(opt->files);i++){
            if(i>=SIS_BUF_SIZE) memError("preprocessing");
            buf[i]=cvectElt(opt->files,i-j,char*);
        }
        buf[i]=(char*)NULL;
        dup2(pfd[1],1);
        close(pfd[1]);
        close(pfd[0]);
        execvp(opt->self,buf);
        memError("preprocessing");
    }
    return ret;
}

