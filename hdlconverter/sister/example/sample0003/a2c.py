#!/usr/bin/env python
# asm to code

import os,sys,re

#
# asm to code
#
class a2c :
    #attributes
    base=0
    regs={}
    table={}
    TABLE_FILE="./ppu.h"
   
    #string elaboration
    def strElb(self,str) :
        str=re.sub(r"^\s*","",str)
        str=re.sub(r"\s*$","",str)
        str=str.lower()
        return str

    #getting table data
    def getData(setf,text,key):
        istart=r"[$]"+key+r">>[^\n]*\n"
        itext=r"([^$]+)"
        iend=r"\n[^$]+[$]"+key+"<<"
        m=re.search(istart+itext+iend,text).group(1)
        if m==None : 
            print file,":parse error."
            sys.exit(-1)
        m=m.replace("#define","")
        m=m.replace("//"," ")
        return m.split("\n")

    #loadint instruction table
    def loadInstTable(self,text):
        ilst=self.getData(text,"INSTRUCTION")
        for item in ilst :
            item=self.strElb(item)
            iobj=re.split(r"\s*",item)
            self.table[iobj[0]]=[iobj[1],iobj[len(iobj)-1]]
        return 0;
    
    #loading register table
    def loadRegTable(self,text):
        ilst=self.getData(text,"REGISTER")
        for item in ilst :
            item=self.strElb(item)
            iobj=re.split(r"\s*",item)
            self.regs[iobj[0]]=iobj[1]
        return 0;

    #table file loading
    def loadTable(self,file):
        fp=open(file,"r")
        if fp==None :
            print file," was not fount."
            sys.exit(-1)
        text=fp.read()
        fp.close()
        item=re.search("__BASE_ADDRESS__[^\n]+",text)
        if item != None :
            item=self.strElb(item.group(0))
            self.base=int(re.split("\s*",item)[1])
        self.loadInstTable(text)
        self.loadRegTable(text)
        return 0

    #code creation
    def createCode(self,file):
        ret=0
        fp=open(file,"r")
        if fp==None :
            print file," was not fount."
            sys.exit(-1)
        text=fp.read()
        text=self.strElb(text)
        ilst=re.split("\s+",text)
        #getting label
        adr=0
        ltbl={}
        for item in ilst:
            item=self.strElb(item)
            if re.search(r"[:]$",item) :
                item=re.sub(":","",item)
                ltbl[item]=adr+self.base
            else : adr+=1    
        #assembling
        j=0
        for item in ilst:
            item=self.strElb(item)
            #instruction
            if self.table.get(item)!=None:print self.table[item][0]
            #label 
            elif re.search(r"[:]$",item):
                adr=adr
                continue
            #address
            elif ltbl.get(item):print "0x%x"%ltbl[item]
            #registers
            elif self.regs.get(item):print self.regs[item]
            #constant value
            elif re.match(r"([0][xX])?[0-9]+",item):print item
            #error
            else :
                print item,": ?"
                ret=-1
            j+=1
        for i in range(256-j) :
            print "0x0%x"%0
        return ret
    
    #to start code creation
    def run(self,args):
        if len(args)!=2 :
            print "usage: a2c <infile.s>"
            return -1
        if self.loadTable(self.TABLE_FILE) :return -1
        if self.createCode(args[1]) : return -1
        return 0

#
# main
#
if __name__=="__main__" : 
    obj=a2c()
    sys.exit(obj.run(sys.argv))

