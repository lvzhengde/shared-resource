#!/usr/bin/env python
import os,sys

#initialization
sample_name="sample0003"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean directory
if clean :
    os.system("rm -fr simulate");
    os.system("rm -fr diff.log");
    os.system("rm -fr debug.cc");
    os.system("rm -fr out.data");
    os.system("rm -fr sample.o");
    os.system("rm -fr ppu.v");
    sys.exit(0)

#excuting command
def exCom(cmd) :
    if os.system(cmd) :
        print "error:",sample_name
        sys.exit(-1)

#testing
exCom("../../src/sister ppu.cc -o ppu.v")
exCom("../../src/sister ppu.cc -o debug.cc --systemc")
exCom("./a2c.py sample.s > sample.o")
exCom("iverilog -o simulate top.v ppu.v")
exCom("./simulate > out.data")
exCom("diff -b -c expect.data out.data > diff.log")

sys.exit(0)

