#!/usr/bin/env python
#ppm image to hex data
import sys

INFO_NAME="image.info"

i=0
buffer=[]
for line in sys.stdin :
    if line[0]=="#" : continue
    if   i==0 : m=line.strip()
    elif i==1 : 
        item=line.strip().split(" ");
        w=item[0]
        h=item[1]
    else : buffer.append(line)
    i+=1

info=open(INFO_NAME,"w")
if info==None : sys.exit(-1)
info.write(m+"\n")
info.write(w+"\n")
info.write(h+"\n")
info.close()

for line in buffer:
    print "%x"%int(line)
sys.exit(0)

