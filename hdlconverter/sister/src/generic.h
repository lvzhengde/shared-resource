/*
  generic definition.
  This file is a part of Sister.
  (c)KTDesignSystems, All Rights Reserved.
*/

#ifndef GENERIC_HEAD
#define GENERIC_HEAD

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define SIS_BUF_SIZE 2048
#define SIS_FILE_NAME_SIZE 256
#define USECIOMEM //use sinmple gc
#include "calg.h"
#include "cgr.h"
#include "gsearch.h"
#include "sister.tab.h"
#include "check.h"
#include "fsmd.h"
#include "dumpfsmd.h"
#include "outsysc.h"
#include "outverilog.h"
#include "schedule.h"
#include "optimize.h"
#include "tesf.h"
#include "ptnutil.h"
#include "nosche.h"

extern int yyperse(void);//parser
extern int prelex(void); //preprocesser
extern void conMsg(void);//message initialization
extern void desMsg(void);//message destraction
extern void memError(char*s); //memory error hendling

extern cMsg __sis_msg; // messages handler
extern int __sis_line; //number of line
extern char __sis_file[SIS_FILE_NAME_SIZE]; //file name
#define VERSION PACKAGE_VERSION
#define SISTER_DUMP  "dump.fsmd"
#define SISTER_DUMP2 "dump2.fsmd"
#endif

