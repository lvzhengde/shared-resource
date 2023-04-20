#!/usr/bin/env python
import sys,os

tests=[
    "preprocess"            ,
    "pars"                  ,
    "vhdl_preprocess"       ,
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


