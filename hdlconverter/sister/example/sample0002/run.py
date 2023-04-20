#!/usr/bin/env python
import os,sys

#initialization
sample_name="sample0002"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean directory
if clean :
    os.system("rm -fr in.hex")
    os.system("rm -fr image.info")
    os.system("rm -fr debug.cc")
    os.system("rm -fr simulate")
    os.system("rm -fr diff.log")
    os.system("rm -fr out.hex")
    os.system("rm -fr out.ppm")
    os.system("rm -fr out.v")
    sys.exit(0)

#excuting command
def exCom(cmd) :
    if os.system(cmd) :
        print "error:",sample_name
        sys.exit(-1)

#testing
exCom("../../src/sister -mul 2 -pipe 1 --systemc -o debug.cc sample.cc")
exCom("../../src/sister -mul 2 -pipe 1 -o out.v sample.cc")
exCom("python p2h.py <in.ppm > in.hex")
exCom("iverilog -o simulate top.v out.v")
exCom("./simulate > out.hex")
exCom("python h2p.py <out.hex > out.ppm")
exCom("diff -b -c expect.ppm out.ppm > diff.log")

sys.exit(0)

