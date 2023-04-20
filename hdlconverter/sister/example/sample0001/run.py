#!/usr/bin/env python
import os,sys

#initialization
sample_name="sample0001"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean directory
if clean :
    os.system("rm -fr debug.cc")
    os.system("rm -fr simulate")
    os.system("rm -fr diff.log")
    os.system("rm -fr out.data")
    os.system("rm -fr out.v")
    sys.exit(0)

#excuting command
def exCom(cmd) :
    if os.system(cmd) :
        print "error:",sample_name
        sys.exit(-1)

#testing
exCom("../../src/sister --systemc -o debug.cc sample.cc")
exCom("../../src/sister -o out.v sample.cc")
exCom("iverilog -o simulate top.v out.v")
exCom("./simulate > out.data")
exCom("diff -c -b expect.data out.data > diff.log")

sys.exit(0)

