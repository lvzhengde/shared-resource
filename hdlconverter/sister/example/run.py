#!/usr/bin/env python
import sys,os

PREFIX="./sample"
START=1
END=3

option=""
for item in sys.argv :
    if item=="clean" : option=" "+"clean"

for i in range(START,END+1) :
    dirname=PREFIX
    dirname+="%.4d"%i
    os.chdir(dirname)
    print "into:",dirname,"",option,"..."
    os.system("python run.py "+option)
    os.chdir("../")

sys.exit(0)


