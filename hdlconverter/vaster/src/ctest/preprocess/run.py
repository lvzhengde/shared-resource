#!/usr/bin/env python
import os,sys

#test command
tlist=[
    "../../vaster -E  sample.v &> out.v",
    "diff -c -b out.v expect.v &> out.log" ,
    "../../vaster -E  sample2.v &> out.v",
    "diff -c -b out.v expect2.v &> out.log" ,
    "../../vaster -E  sample3.v &> out.v",
    "diff -c -b out.v expect3.v &> out.log" ,
    "../../vaster -E  sample4.v &> out.v",
    "diff -c -b out.v expect4.v &> out.log" ,
    "../../vaster -E  sample5.v &> out.v",
    "diff -c -b out.v expect5.v &> out.log" ,
    "../../vaster -E  sample6.v &> out.v",
    "diff -c -b out.v expect6.v &> out.log" ,
    "../../vaster -E  sample7.v &> out.v",
    "diff -c -b out.v expect7.v &> out.log" ,
    "../../vaster -E  sample8.v &> out.v",
    "diff -c -b out.v expect8.v &> out.log" ,
    "../../vaster -E  sample9.v &> out.v",
    "diff -c -b out.v expect9.v &> out.log" ,
    "../../vaster -E  sample10.v &> out.v",
    "diff -c -b out.v expect10.v &> out.log" ,
    "../../vaster -E  sample11.v &> out.v",
    "diff -c -b out.v expect11.v &> out.log" ,
    "../../vaster -E  sample12.v &> out.v",
    "diff -c -b out.v expect12.v &> out.log" ,
    "../../vaster -E  sample13.v &> out.v",
    "diff -c -b out.v expect13.v &> out.log" ,
    ]

#initialization
sample_name="preprocess"
clean=0
for item in sys.argv :
    if item=="clean" : clean=1

#clean
if clean==1:
    os.system("rm -f out.v")
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

