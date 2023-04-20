/*
    VHDL preprocesser
    (c)KTDesignSystemc Corp.
*/

#include "vprepro.h"
#include "vpreproc_const.h"

#include <string.h>

/*------------------------------------------------------------------
    global definition
------------------------------------------------------------------*/
extern int vpreVHDLlex(void); //lexter
extern FILE* vpreVHDLin;      //lex file stream
extern char* vpreVHDLtext;    //lex text buffer


/*------------------------------------------------------------------
    run preprocess state
------------------------------------------------------------------*/
static int vhpreRunFileState(
    VasOptHandle opt,VpreHandle vphdl,int token){
    switch(token){
    //comment
    case Vpreproc_type_COMMENT : printf("\n");break;
    //default
    default : printf(vpreVHDLtext);break;
    }
    return 0;
}
/*------------------------------------------------------------------
    run preprocess loop
------------------------------------------------------------------*/
static int vhpreRunFileLoop(VasOptHandle opt,VpreHandle vphdl){
    int ret=0;
    int token=0;
    vpreVHDLin=vphdl->fp;

    //parse start
    while(token=vpreVHDLlex())
        vhpreRunFileState(opt,vphdl,token);
    printf("\n");
    return ret;
}
/*------------------------------------------------------------------
    run preprocess one file
------------------------------------------------------------------*/
static int vhpreRunFile(
    VasOptHandle opt,VpreHandle vphdl,char*fname){
    int ret=0;
    FILE*fp=fopen(fname,"r");
    if(!fp){
        cmsgEcho(__sis_msg,0,fname);
        ret=-1;
    }else{
        void* ybuf=vhpreMakeBuf(fp);
        vphdl->fp=fp;
        vphdl->line=1;
        vphdl->file=fname;
        printf("--%s %d\n",vphdl->file,vphdl->line);
        ret=vhpreRunFileLoop(opt,vphdl); 
        vhpreDeleteBuf(ybuf); 
        fclose(fp);
    }

    return ret;
}


/*------------------------------------------------------------------
    run preprocess
------------------------------------------------------------------*/
static int vhpreRun(VasOptHandle opt,VpreHandle vphdl){
    int ret=0;
    cvectMaps(opt->files,char*,
        vhpreRunFile(opt,vphdl,val));
    return ret;
}
/*------------------------------------------------------------------
    VHDL preprocesser
------------------------------------------------------------------*/
int vprepro_VHDL(VasOptHandle opt){
    int ret=0;
    
    //making buffer
    VpreHandle vphdl=vpreIni();

    //run process
    ret=vhpreRun(opt,vphdl);

    //destruction
    vpreDes(vphdl);

    return ret;
}

