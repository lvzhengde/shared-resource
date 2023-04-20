#!/usr/bin/env python
#ppm image to hex data
import sys

INFO_NAME="image.info"

info=open(INFO_NAME,"r")
if info==None : sys.exit(0) 
m=info.readline().strip()
w=info.readline().strip()
h=info.readline().strip()
info.close()
print m
print w,h

for line in sys.stdin : print int(line,16)

sys.exit(0)

