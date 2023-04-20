#!/usr/bin/env python
import os,sys

#test command
tlist=[
    "../../vaster -E -vhdl sample.vhdl &> out.vhdl",
    "diff -c -b out.vhdl expect.vhdl &> out.log" ,
    ]

#initialization
sample_name="vhdl_preprocess"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean
if clean==1:
    os.system("rm -f out.vhdl")
    os.system("rm -f out.log")
    sys.exit(0)

#excuting command
def exCom(cmd) :
    if os.system(cmd) :
        print "ng '"+sample_name+"':'"+cmd+"'"
        sys.exit(-1)

#testing
for t in tlist : exCom(t)


print "ok:",sample_name
sys.exit(0)

