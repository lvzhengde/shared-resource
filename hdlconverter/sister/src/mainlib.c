/*
  sister main library.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#include "sister.h"

//
//find path
//
char* findPath(char* path){
    int i; //loop variable
    int pid; //process id
    int pathlen; //path length
    static char buf[MAXPATHLEN];//header path
    char procpath[MAXPATHLEN];//process symbolic link path
    buf[0]='\0';
    pid=getpid(); //get the process id
    sprintf(procpath,"/proc/%d/exe",pid);
    if(!realpath(procpath,buf)) return NULL; //get real path
    pathlen=strlen(buf); //get directory name
    for(i=pathlen-1;i>=0;i--){
        if(buf[i]=='/') break;
    }
    if(i<0) return NULL;
    buf[i+1]='\0';
    strncat(buf,path,MAXPATHLEN);
    return buf;
}


//
//find header file
//
static char* findheader(void){
    return findPath("../include");
}

//
//close preprocess3
//
void closePreprocess3(int fd){
    wait(NULL);
    dup2(fd,0);
    close(fd);
}


//
//preprocessing
//0:normally,1:executing and stop,-1:error
//
int preprocess3(char*self,cVector files,char prepro,int* fd){
    char* headpath; //header file directory path
    headpath=findheader();
    if(!headpath){
        cmsgEcho(__sis_msg,46);
        return -1;
    }
    if(prepro){ //"-E" as "g++ -E source.cpp | sister -E1"
        int i,ret=0;
        char* buf;
        cstrSet(buf,"g++ -E -I",memError("preprocessing."));
        cstrCat(buf,headpath, memError("preprocessing."));
        cstrCat(buf," -I", memError("preprocessing."));
        cstrCat(buf,headpath, memError("preprocessing."));
        cstrCat(buf,"../ ", memError("preprocessing."));
        for(i=0;i<cvectSize(files);i++){
            cstrCat(buf,cvectElt(files,i,char*),
             memError("preprocessing."));
            cstrCat(buf," ",memError("preprocessing."));
        }
        cstrCat(buf," | ",memError("preprocessing."));
        cstrCat(buf,self,memError("preprocessing."));
        cstrCat(buf," -E1",memError("preprocessing."));

        ret=system(buf);
        cstrFree(buf);
        if(ret)return -1;
        else return 1;
    }else{ // to fork and to connect standard I/O by pileline
        pid_t chp;
        int pfd[2];
        if(pipe(pfd)<0)
            memError("creating a pipe for preprocessing.");
        chp=fork();
        if(chp<0){
            memError("forking a process for preprocessing.");
            return -1;
        }
        //parent
        else if(chp){
            //close(pfd[1]);
            dup2(0,pfd[1]);
            dup2(pfd[0],0);
            close(pfd[0]);
            *fd=pfd[1];
        }
        //child
        else {
            int i;
            char* buf[SIS_BUF_SIZE+4];
            buf[0]=self; 
            buf[1]="-E";
            for(i=2;(i-2)<cvectSize(files);i++){
                if(i>=SIS_BUF_SIZE)
                    memError("loading files.");
                buf[i]=cvectElt(files,i-2,char*);
            }
            buf[i]=(char*)NULL;
            dup2(pfd[1],1);
            close(pfd[1]);
            close(pfd[0]);
            execvp(self,buf);
            memError("forking a process for preprocessing.");
        }
    }
    return 0;
}


//
//preprocessing
//0:normally,1:executing and stop,-1:error
//
int preprocess2(char*self,cVector files,char prepro){
    int fd;  //file descriptor
    int ret; //return flg
    ret=preprocess3(self,files,prepro,&fd);
    close(fd);
    return  ret;
}


