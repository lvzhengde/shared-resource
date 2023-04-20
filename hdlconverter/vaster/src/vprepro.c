/*
    verilog HDL preprocesser
    (c)KTDesignSystemc Corp.
*/

#include "vprepro.h"
#include "vpreproc_const.h"

#include <string.h>

/*------------------------------------------------------------------
    global definition
------------------------------------------------------------------*/
extern int vprelex(void); //lexter
extern FILE* vprein;      //lex file stream
extern char* vpretext;    //lex text buffer

/*------------------------------------------------------------------
    handler constructor
------------------------------------------------------------------*/
VpreHandle vpreIni(void){
    VpreHandle vphdl;
    cstrMakeBuf(vphdl,1,struct _VpreHandle,
        memError("preprocessing"));
    vphdl->file="None";
    vphdl->line=1;
    vphdl->map=cmapNew(sizeof(char*),
        sizeof(struct _VpreDefNode),(*cmapStrKey),(*cmapStrCmp));
    vphdl->sbuf=cvectIni(sizeof(char*),253);
    return vphdl;
}

/*------------------------------------------------------------------
    handler destructor
------------------------------------------------------------------*/
void vpreDes(VpreHandle vphdl){
    int i,ssize;
    if(!vphdl) return ;
    ssize=cvectSize(vphdl->sbuf);
    for(i=0;i<ssize;i++){
        char**s=cvectFind(vphdl->sbuf,i);
        if(s) free(*s);
    }
    cmapMaps(vphdl->map,char*,
        struct _VpreDefNode,if(val.args) cvectDes(val.args));
    cvectDes(vphdl->sbuf);
    cmapDes(vphdl->map);
    free(vphdl);
}
/*------------------------------------------------------------------
    reset all
------------------------------------------------------------------*/
static void vpreRunResetAll(VpreHandle vphdl){
    cmapDes(vphdl->map);
    vphdl->map=cmapNew(sizeof(char*),
        sizeof(struct _VpreDefNode),(*cmapStrKey),(*cmapStrCmp));
}

/*------------------------------------------------------------------
    ignore space
------------------------------------------------------------------*/
static int vpreRunIgnSpace(VpreHandle vphdl){
    int token;
    while(token=vprelex()) //ignore space
        if(token==Vpreproc_type_EEL){
            printf("\n");
            vphdl->line++;
        }else if(token!=Vpreproc_type_SPACE) break;
    return token;
}
/*------------------------------------------------------------------
    define
------------------------------------------------------------------*/
static void vpreRunDefine(VpreHandle vphdl){
    int token=0;
    VpreDefNode tnode;
    struct _VpreDefNode node;
    
    char*sbuf;
    //id
    token=vpreRunIgnSpace(vphdl);
    if(!token) return ;
    cstrSet(node.id,"`"     ,memError("preprocessing"));
    cstrCat(node.id,vpretext,memError("preprocessing"));
    cvectPush(vphdl->sbuf,&(node.id));

    //function of const
    token=vpreRunIgnSpace(vphdl);
    if(!token) return ;

    //function 
    if(vpretext[0]=='('){
        node.type=1;
        node.args=cvectIni(sizeof(char*),4);
        while(token=vpreRunIgnSpace(vphdl)){
            if(vpretext[0]==')') break;
            if(vpretext[0]==',') continue;
            else{
                char*str;
                cstrSet(str,vpretext,
                    memError("preprocessing"));
                cvectPush(node.args  ,&str);
                cvectPush(vphdl->sbuf,&str);
            }
        }
        if(!token) return ;
        token=vpreRunIgnSpace(vphdl);
        if(!token) return ;
    //const
    }else{
        node.type=0;
        node.args=NULL;
    }
    //string
    cstrSet(node.str,"",memError("preprocessing"));
    do{
        if(token==Vpreproc_type_EEL){
            printf("\n");
            vphdl->line++;
            cstrCat(node.str," ",memError("preprocessing"));
        }else if(token==Vpreproc_type_EL){
            printf("\n");
            vphdl->line++;
            break;
        }else{
            cstrCat(node.str,vpretext,
                memError("preprocessing"));
        }
    }while(token=vprelex());
    cvectPush(vphdl->sbuf,&(node.str));

    tnode=cmapFind(vphdl->map,&(node.id));
    if(tnode){
        if(tnode->args) cvectDes(tnode->args);
    }else cmapUpdate(vphdl->map,&(node.id),&node);
}

/*------------------------------------------------------------------
    string replace
------------------------------------------------------------------*/
static char* vpreRunReplace(char* tstr,char* f,char* t){
    int size=0;
    char* p=NULL;
    if(!t) return "";
    if(!f) return "";
    if(!tstr) return "";

    size=strlen(f);
    while(p=strstr(tstr,f)){ 
        char*tstr2;
        cstrSet(tstr2,tstr,memError("preprocessing"));
        tstr2[p-tstr]='\0';
        cstrCat(tstr2,t,memError("preprocessing"));
        cstrCat(tstr2,p+size,memError("preprocessing"));
        cstrFree(tstr);
        tstr=tstr2;
    }
    return tstr;
}
/*------------------------------------------------------------------
    macro args
------------------------------------------------------------------*/
static cVector vpreRunMacroArgs(VpreHandle vphdl){
    int stk=-1;
    int token=0;
    char*buf=NULL;
    cVector vect=cvectIni(sizeof(char*),5);

    cstrSet(buf,"",memError("preprocessing"));
    while(token=vpreRunIgnSpace(vphdl)){
        if(vpretext[0]=='('){
            stk++;
            if(stk>0)cstrCat(buf,vpretext,memError("preprocessing"));
        }else if(vpretext[0]==')'){
            stk--;
            if(stk<0){
                cvectPush(vect,&buf);
                cvectPush(vphdl->sbuf,&buf);
                break;
            }else cstrCat(buf,vpretext,memError("preprocessing"));
        }else if(vpretext[0]==','){
            if(!stk){
                cvectPush(vect,&buf);
                cvectPush(vphdl->sbuf,&buf);
                cstrSet(buf,"",memError("preprocessing"));
            }else cstrCat(buf,vpretext,memError("preprocessing"));
        }else cstrCat(buf,vpretext,memError("preprocessing"));
    }
    return vect;
}
/*------------------------------------------------------------------
    macro ID
------------------------------------------------------------------*/
static int vpreRunMacroId(VpreHandle vphdl){
    VpreDefNode node=cmapFind(vphdl->map,&vpretext);
    //find
    if(!node){
        char* tstr;
        cstrSet(tstr,"`",memError("preprocessing"));
        cstrCat(tstr,vpretext,memError("preprocessing"));
        node=cmapFind(vphdl->map,&vpretext);
        cstrFree(tstr);
    }
    if(!node){
        printf(vpretext);
        return 0;
    }
    //const
    if(node->type==0) printf(node->str);
    
    //function
    else if(node->type==1){
        int i=0;
        char*tstr=NULL; //string buffer
        cVector args=vpreRunMacroArgs(vphdl); //called   argument list
        int vsize=cvectSize(node->args); //declared argument size
        int argsize=cvectSize(args);     //called   argument size
        cstrSet(tstr,node->str,memError("preprocessing"));
        //checking args
        for(i=0;(i<argsize)&&(i<vsize);i++){
            char**t=cvectFind(args,i);
            char**f=cvectFind(node->args,i);
            if((!t && !f)) break;
            tstr=vpreRunReplace(tstr,*f,*t);
        }
        printf(tstr);
        cstrFree(tstr);
        cvectDes(args);
    }
    return 0;
}

/*------------------------------------------------------------------
    include
------------------------------------------------------------------*/
static int vpreRunFile(
    VasOptHandle opt,VpreHandle vphdl,char*fname);
static int vpreRunInclude(VasOptHandle opt,VpreHandle vphdl){
    int state=0;
    int token=0;
    char*buf=NULL;

    cstrSet(buf,"",memError("preprocessing"));
    while(token=vpreRunIgnSpace(vphdl)){
        switch(state){
        case 0 :
            if(vpretext[0]!='"') return 0;
            else state=1;
            break;
        case 1 :
            if(vpretext[0]!='"'){
                cstrCat(buf,vpretext,memError("preprocessing"));
            }else state=2;
            break;
        default : return 0;
        }
        if(state==2) break;
    }
    if(token){
        void* ybuf=vpreCurrentBuf();
        VpreHandle vphdl2=vpreIni();
        vpreRunFile(opt,vphdl2,buf);
        vpreDes(vphdl2);
        vprein=vphdl->fp;
        vpreChangeBuf(ybuf);
        printf("//%s %d\n",vphdl->file,vphdl->line);
    }
    cstrFree(buf);
    return 0;
}
/*------------------------------------------------------------------
    comment    
------------------------------------------------------------------*/
static int vpreRunComment(VpreHandle vphdl){
    int i,size=strlen(vpretext);
    for(i=0;i<size;i++){
        if(vpretext[i]=='\n'){
            printf("\n");
            vphdl->line++;
        }
    }
    return 0;
}

/*------------------------------------------------------------------
    ifdef / else / endif   
------------------------------------------------------------------*/
static int vpreRunFileState(
    VasOptHandle opt,VpreHandle vphdl,int token);
static int vpreRunIfdef(VasOptHandle opt,VpreHandle vphdl){
    int stk=0;
    int token=0;
    char sw=0; //0:else,1:defined

    //checking token
    while(token=vpreRunIgnSpace(vphdl)){
        if(token==Vpreproc_type_EL) return 0;
        else if(token==Vpreproc_type_EEL) continue;
        else break;
    }
    if(!token) return 0;

    //checking switch
    {
        char*buf=NULL;
        cstrSet(buf,"`",memError("preprocessing"));
        cstrCat(buf,vpretext,memError("preprocessing"));
        if(cmapFind(vphdl->map,&buf)) sw=1;
        cstrFree(buf);
    }
    
    //parsing
    while(token=vprelex()){
        switch(token){
        case Vpreproc_type_ELSE : if(!stk) sw=!sw;break;
        case Vpreproc_type_ENDIF:
            if(!sw){
                if(stk<=0) return 0;
                else stk--;
            }else return 0;
        default : 
            if(sw) vpreRunFileState(opt,vphdl,token);
            else switch(token){
            case Vpreproc_type_IFDEF:
                stk++;
                break;;
            case Vpreproc_type_COMMENT :
                vpreRunComment(vphdl);
                break;
            case Vpreproc_type_EL  :
            case Vpreproc_type_EEL :
                printf("\n");
                vphdl->line++;
                break;
            }
            break;
        }
    }
    return 0;
}
/*------------------------------------------------------------------
    parameter
------------------------------------------------------------------*/
static int vpreRunParam(VasOptHandle opt,VpreHandle vphdl){
    int state=0;
    int token=0;
    char*buf1=NULL;
    char*buf2=NULL;
    VpreDefNode tnode;
    struct _VpreDefNode node;
    
    cstrSet(buf1,"",memError("preprocessing"));
    //getting string
    while(token=vpreRunIgnSpace(vphdl)){
        switch(state){
        case 0 :
            if(vpretext[0]=='='){
                cstrSet(buf2,"",memError("preprocessing"));
                state=1;
            }else cstrCat(buf1,vpretext,memError("preprocessing"));
            break;
        case 1 :
            if(vpretext[0]==';') state=2;
            else cstrCat(buf2,vpretext,memError("preprocessing"));
            break;
        }
        if(state==2) break;
    }
    tnode=cmapFind(vphdl->map,&buf1);
    if(tnode) if(tnode->type==1) cvectDes(tnode->args);
    node.type=0;
    node.id=buf1;
    node.str=buf2;
    node.args=NULL;
    cmapUpdate(vphdl->map,&buf1,&node);
    cvectPush(vphdl->sbuf,&buf1);
    cvectPush(vphdl->sbuf,&buf2);
    return 0;
}

/*------------------------------------------------------------------
    ID
------------------------------------------------------------------*/
static int vpreRunId(VpreHandle vphdl){
    VpreDefNode node=cmapFind(vphdl->map,&vpretext);
    if(node) printf(node->str);
    else printf(vpretext);
    return 0;
}
/*------------------------------------------------------------------
    run preprocess state
------------------------------------------------------------------*/
static int vpreRunFileState(
    VasOptHandle opt,VpreHandle vphdl,int token){
    switch(token){
    //ID
    case Vpreproc_type_ID : vpreRunId(vphdl);      break; 
    case Vpreproc_type_MID: vpreRunMacroId(vphdl); break;

    //parameter
    case Vpreproc_type_PARAM:vpreRunParam(opt,vphdl);break;

    //macro
    case Vpreproc_type_RESET_ALL:vpreRunResetAll(vphdl);   break;
    case Vpreproc_type_TIMESCALE:printf(vpretext);         break;
    case Vpreproc_type_DEFINE   :vpreRunDefine(vphdl);     break;
    case Vpreproc_type_INCLUDE  :vpreRunInclude(opt,vphdl);break;
    case Vpreproc_type_IFDEF    :vpreRunIfdef(opt,vphdl);  break;
    case Vpreproc_type_ELSE     :printf(vpretext);break;
    case Vpreproc_type_ENDIF    :printf(vpretext);break;
    case Vpreproc_type_DEFAULT_NETTYPE    :printf(vpretext);break;
    case Vpreproc_type_UNCONNECTED_DRIVE  :printf(vpretext);break;
    case Vpreproc_type_NOUNCONNECTED_DRIVE:printf(vpretext);break;
    case Vpreproc_type_SIGNED   :printf(vpretext);break;
    case Vpreproc_type_UNSIGNED :printf(vpretext);break; 
    
    //comment
    case Vpreproc_type_COMMENT:vpreRunComment(vphdl);break;

    //endline
    case Vpreproc_type_EL :
    case Vpreproc_type_EEL:
        vphdl->line++;
        printf(vpretext);
        break;

    //default
    default : printf(vpretext);break;
    }
    return 0;
}

/*------------------------------------------------------------------
    run preprocess loop
------------------------------------------------------------------*/
static int vpreRunFileLoop(VasOptHandle opt,VpreHandle vphdl){
    int ret=0;
    int token=0;
    vprein=vphdl->fp;

    //parse start
    while(token=vprelex()) vpreRunFileState(opt,vphdl,token);
    printf("\n");
    return ret;
}
/*------------------------------------------------------------------
    run preprocess one file
------------------------------------------------------------------*/
static int vpreRunFile(
    VasOptHandle opt,VpreHandle vphdl,char*fname){
    int ret=0;
    FILE*fp=fopen(fname,"r");
    if(!fp){
        cmsgEcho(__sis_msg,0,fname);
        ret=-1;
    }else{
        void* ybuf=vpreMakeBuf(fp);
        vphdl->fp=fp;
        vphdl->line=1;
        vphdl->file=fname;
        printf("//%s %d\n",vphdl->file,vphdl->line);
        ret=vpreRunFileLoop(opt,vphdl);
        vpreDeleteBuf(ybuf);
        fclose(fp);
    }

    return ret;
}

/*------------------------------------------------------------------
    run preprocess
------------------------------------------------------------------*/
static int vpreRun(VasOptHandle opt,VpreHandle vphdl){
    int ret=0;
    cvectMaps(opt->files,char*,
        ret|=vpreRunFile(opt,vphdl,val));
    return ret;
}
/*------------------------------------------------------------------
    verilogHDL preprocesser
------------------------------------------------------------------*/
int vprepro(VasOptHandle opt){
    int ret=0;

    //making buffer
    VpreHandle vphdl=vpreIni();

    //run preprocess
    ret=vpreRun(opt,vphdl);

    //destruction
    vpreDes(vphdl);

    return ret;
}

