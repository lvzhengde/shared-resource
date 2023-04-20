#!/usr/bin/env python
import os,sys

#test command
tlist=[
    "../../vaster -vhdl sample.vhdl            &> out.vhdl",
    "../../vaster -vhdl sample_block.vhdl      &> out.vhdl",
    "../../vaster -vhdl sample_case.vhdl       &> out.vhdl",
    "../../vaster -vhdl sample_component.vhdl  &> out.vhdl",
    "../../vaster -vhdl sample_instance.vhdl   &> out.vhdl",
    "../../vaster -vhdl sample_const.vhdl      &> out.vhdl",
    ]

#initialization
sample_name="vhdl_pars"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean
if clean==1:
    os.system("rm -f out.vhdl")
    os.system("rm -f out.log")
    os.system("rm -f *.cc")
    os.system("rm -f *.o")
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

