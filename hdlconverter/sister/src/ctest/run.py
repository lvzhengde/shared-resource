#!/usr/bin/env python
import sys,os

tests=[
    "test-bit"          ,
    "test-cast"         ,
    "test-class"        ,
    "test-distribution" ,
    "test-enum"         ,
    "test-function"     ,
    "test-instance"     ,
    "test-no-schedule"  ,
    "test-part-select"  ,
    "test-pointer"      ,
    "test-posneg"       ,
    "test-struct"       ,
    "test-typedef"      ,
    ]

args=["./run.py"]
for i in range(1,len(sys.argv)) :
    args.append(" ")
    args.append(sys.argv[i])

com=''.join(args)
for d in tests :
    os.chdir(d)
    os.system(com)
    os.chdir("../")


