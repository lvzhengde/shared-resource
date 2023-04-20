/*
    output verilog HDL
    (c)KTDesignSystemc Corp.
*/

#ifndef __VHSER_HEAD
#define __VHSER_HEAD
#include "vaster_generic.h"

typedef struct _vhSerHandle{
    cgrNode top;
    VasOptHandle opt;
    void*work;

    int (*root)(struct _vhSerHandle* self,cgrNode node);
    int (*file)(struct _vhSerHandle* self,cgrNode node);
    int (*arch)(struct _vhSerHandle* self,cgrNode node);
    int (*decl)(struct _vhSerHandle* self,cgrNode node);
    int (*func)(struct _vhSerHandle* self,cgrNode node);
    int (*funcdec)(struct _vhSerHandle* self,cgrNode node);
    int (*comp)(struct _vhSerHandle* self,cgrNode node);
    int (*cnst)(struct _vhSerHandle* self,cgrNode node);
    int (*subtype)(struct _vhSerHandle* self,cgrNode node);
    int (*type)(struct _vhSerHandle* self,cgrNode node);
    int (*sig)(struct _vhSerHandle* self,cgrNode node);
    int (*var)(struct _vhSerHandle* self,cgrNode node);
    int (*alias)(struct _vhSerHandle* self,cgrNode node);
    int (*enti)(struct _vhSerHandle* self,cgrNode node);
    int (*entibody)(struct _vhSerHandle* self,cgrNode node);
    int (*generic)(struct _vhSerHandle* self,cgrNode node);
    int (*port)(struct _vhSerHandle* self,cgrNode node);
    int (*declst)(struct _vhSerHandle* self,cgrNode node);
    int (*vardec)(struct _vhSerHandle* self,cgrNode node);
    int (*init)(struct _vhSerHandle* self,cgrNode node);
    int (*datatype)(struct _vhSerHandle* self,cgrNode node);
    int (*range)(struct _vhSerHandle* self,cgrNode node);
    int (*rangeto)(struct _vhSerHandle* self,cgrNode node);
    int (*porttype)(struct _vhSerHandle* self,cgrNode node);
    int (*idlst)(struct _vhSerHandle* self,cgrNode node);
    int (*use)(struct _vhSerHandle* self,cgrNode node);
    int (*useid)(struct _vhSerHandle* self,cgrNode node);
    int (*lib)(struct _vhSerHandle* self,cgrNode node);
    int (*cfg)(struct _vhSerHandle* self,cgrNode node);
    int (*cfgbody)(struct _vhSerHandle* self,cgrNode node);
    int (*wait)(struct _vhSerHandle* self,cgrNode node);
    int (*waitfor)(struct _vhSerHandle* self,cgrNode node);
    int (*waittil)(struct _vhSerHandle* self,cgrNode node);
    int (*waiton)(struct _vhSerHandle* self,cgrNode node);
    int (*next)(struct _vhSerHandle* self,cgrNode node);
    int (*exit)(struct _vhSerHandle* self,cgrNode node);
    int (*nul)(struct _vhSerHandle* self,cgrNode node);
    int (*statecase)(struct _vhSerHandle* self,cgrNode node);
    int (*casebody)(struct _vhSerHandle* self,cgrNode node);
    int (*generate)(struct _vhSerHandle* self,cgrNode node);
    int (*stateloop)(struct _vhSerHandle* self,cgrNode node);
    int (*statewhile)(struct _vhSerHandle* self,cgrNode node);
    int (*statefor)(struct _vhSerHandle* self,cgrNode node);
    int (*stateif)(struct _vhSerHandle* self,cgrNode node);
    int (*stateelse)(struct _vhSerHandle* self,cgrNode node);
    int (*instance)(struct _vhSerHandle* self,cgrNode node);
    int (*genemap)(struct _vhSerHandle* self,cgrNode node);
    int (*portmap)(struct _vhSerHandle* self,cgrNode node);
    int (*mapbody)(struct _vhSerHandle* self,cgrNode node);
    int (*proc)(struct _vhSerHandle* self,cgrNode node);
    int (*block)(struct _vhSerHandle* self,cgrNode node);
    int (*dfg)(struct _vhSerHandle* self,cgrNode node);
    int (*dfgwhen)(struct _vhSerHandle* self,cgrNode node);
    int (*dfgelse)(struct _vhSerHandle* self,cgrNode node);
    int (*ope)(struct _vhSerHandle* self,cgrNode node);
    int (*funcall)(struct _vhSerHandle* self,cgrNode node);
    int (*set)(struct _vhSerHandle* self,cgrNode node);
    int (*num)(struct _vhSerHandle* self,cgrNode node);
    int (*id)(struct _vhSerHandle* self,cgrNode node);
    int (*attr)(struct _vhSerHandle* self,cgrNode node);
    int (*oper)(struct _vhSerHandle* self,cgrNode node);
    int (*package)(struct _vhSerHandle* self,cgrNode node);
    int (*retu)(struct _vhSerHandle* self,cgrNode node);

}* vhSerHandle;

extern vhSerHandle vhSerIni(
    cgrNode top,VasOptHandle opt,void*work);
extern void vhSerDes(vhSerHandle hhdl);

#endif

