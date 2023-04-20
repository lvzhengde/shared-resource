#!/usr/bin/env python
import os,sys

#initialization
sample_name="test-bit"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean
if clean==1:
    os.system("rm -f out.v")
    sys.exit(0)

#excuting command
def exCom(cmd) :
    if os.system(cmd) :
        print "ng:",sample_name
        sys.exit(-1)

#testing
exCom("../../sister main.cc -o out.v")
exCom("diff -c -b out.v expect.v")

print "ok:",sample_name
sys.exit(0)

