/*
    verilog HDL preprocesser
    (c)KTDesignSystemc Corp.
*/

#ifndef __OVHDL_HEAD
#define __OVHDL_HEAD

//output vhdl file structure
typedef struct _oVhdlWork{
    int indent;
    FILE*fp;
    char*fname;
    VasOptHandle opt;
}* oVhdlWork;


//output vhdl file
extern int ovhdl(char*fname,cgrNode top,VasOptHandle opt);
extern int ovhdlOpe(oVhdlWork work,cgrNode node);

#endif

