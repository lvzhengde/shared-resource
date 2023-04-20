#!/usr/bin/env python
import os,sys

#initialization
sample_name="sample0004"
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

## dummy

sys.exit(0)

