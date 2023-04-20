/*
    output verilog HDL
    (c)KTDesignSystemc Corp.
*/

#include "vaster.h"

/*------------------------------------------------------------------
    operator
-------------------------------------------------------------------*/
static int vhSerOperator(vhSerHandle self,cgrNode node){
    cgrNode left=cgrGetNode(node,cgrKeyLeft);
    cgrNode right=cgrGetNode(node,cgrKeyRight);
    self->ope(self,left);
    self->ope(self,right);
    return 0;
}
/*------------------------------------------------------------------
    attr
-------------------------------------------------------------------*/
static int vhSerAttr(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_funcargs :
        self->ope(self,cgrGetNode(node,cgrKeyVal));
        break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    id
-------------------------------------------------------------------*/
static int vhSerId(vhSerHandle self,cgrNode node){
    cgrNode attr=cgrGetNode(node,cgrKeyOpt);
    if(!node) return 0;
    if(attr) self->attr(self,attr);
    return 0;
}
/*------------------------------------------------------------------
    num
-------------------------------------------------------------------*/
static int vhSerNum(vhSerHandle self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
    set
-------------------------------------------------------------------*/
static int vhSerSet(vhSerHandle self,cgrNode node){
    cgrNode val=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    while(val){
        cgrNode ope=cgrGetNode(val,cgrKeyVal);
        cgrNode cond=cgrGetNode(val,cgrKeyCond);
        self->ope(self,ope);
        self->ope(self,cond);
        val=val->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    funcall
-------------------------------------------------------------------*/
static int vhSerFuncall(vhSerHandle self,cgrNode node){
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(!node) return 0;
    self->ope(self,args);
    return 0;
}

/*------------------------------------------------------------------
    ope
-------------------------------------------------------------------*/
static int vhSerOpe(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    while(node){
        switch(node->type){
        case VH_id      :self->id(self,node);       break;
        case VH_num     :self->num(self,node);      break;
        case VH_set     :self->set(self,node);      break;
        case VH_funcall :self->funcall(self,node);  break;
        default         :self->oper(self,node);     break;
        }
        node=node->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    dfg else
-------------------------------------------------------------------*/
static int vhSerDfgElse(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    self->ope(self,body);
    if(cond) self->dfgwhen(self,cond);
    return 0;
}
/*------------------------------------------------------------------
    dfg when
-------------------------------------------------------------------*/
static int vhSerDfgWhen(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    self->ope(self,body);
    if(cond) self->dfgelse(self,cond);
    return 0;
}

/*------------------------------------------------------------------
    dfg
-------------------------------------------------------------------*/
static int vhSerDfg(vhSerHandle self,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);    
    cgrNode cnd=cgrGetNode(node,cgrKeyCond);    
    if(!node) return 0;
    self->ope(self,ope);
    if(cnd) self->dfgwhen(self,cnd);
    return 0;
}
/*------------------------------------------------------------------
    block
-------------------------------------------------------------------*/
static int vhSerBlock(vhSerHandle self,cgrNode node){
    cgrNode gurd=cgrGetNode(node,cgrKeyCond);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    if(gurd) self->ope(self,gurd);
    self->decl(self,decl);
    self->cfg(self,body);
    return 0;
}
/*------------------------------------------------------------------
    process
-------------------------------------------------------------------*/
static int vhSerProc(vhSerHandle self,cgrNode node){
    cgrNode sen=cgrGetNode(node,cgrKeySensitive);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    if(sen) self->idlst(self,sen);
    self->decl(self,decl);
    self->cfg(self,body);
    return 0;
}

/*------------------------------------------------------------------
    port map body
-------------------------------------------------------------------*/
static int vhSerPortMapBody(vhSerHandle self,cgrNode node){
    cgrNode val=cgrGetNode(node,cgrKeyVal);
    self->ope(self,val);
    return 0;
}
/*------------------------------------------------------------------
    port map
-------------------------------------------------------------------*/
static int vhSerPortMap(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    while(body){
        self->mapbody(self,body);
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    generic map
-------------------------------------------------------------------*/
static int vhSerGenericMap(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    while(body){
        self->ope(self,body);
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    instance
-------------------------------------------------------------------*/
static int vhSerInstance(vhSerHandle self,cgrNode node){
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->ope(self,decl);
    if(body){
        switch(body->type){
        case VH_port    : self->portmap(self,body);break;
        case VH_generic : self->genemap(self,body);break;
        default :  break;
        }
    }
    return 0;
}
/*------------------------------------------------------------------
    else
-------------------------------------------------------------------*/
static int vhSerElse(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    if(cond) self->ope(self,cond);
    self->cfg(self,body);
    return 0;
}
/*------------------------------------------------------------------
    if
-------------------------------------------------------------------*/
static int vhSerIf(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    cgrNode step=cgrGetNode(node,cgrKeyStep);
    if(!node) return 0;
    self->ope(self,cond);
    self->cfg(self,body);
    while(step){
        self->stateelse(self,step);
        step=step->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    for
-------------------------------------------------------------------*/
static int vhSerFor(vhSerHandle self,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    self->rangeto(self,cond);
    return 0;
}
/*------------------------------------------------------------------
    while
-------------------------------------------------------------------*/
static int vhSerWhile(vhSerHandle self,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    self->ope(self,cond);
    return 0;
}
/*------------------------------------------------------------------
    loop
-------------------------------------------------------------------*/
static int vhSerLoop(vhSerHandle self,cgrNode node){
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    if(type){
        switch(type->type){
        case VH_for   : self->statefor(self,type);  break;
        case VH_while : self->statewhile(self,type);break;
        default : break;
        }
    }
    self->cfg(self,body);
    return 0;
}
/*------------------------------------------------------------------
    generate
-------------------------------------------------------------------*/
static int vhSerGenerate(vhSerHandle self,cgrNode node){
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    if(type){
        switch(type->type){
        case VH_for   : self->statefor(self,type);  break;
        case VH_while : self->statewhile(self,type);break;
        default : break;
        }
    }
    self->cfg(self,body);
    return 0;
}
/*------------------------------------------------------------------
    case body
-------------------------------------------------------------------*/
static int vhSerCaseBody(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    if(cond) self->rangeto(self,cond);
    self->cfg(self,body);
    return 0;
}
/*------------------------------------------------------------------
    case
-------------------------------------------------------------------*/
static int vhSerCase(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    self->ope(self,cond);
    while(body){
        self->casebody(self,body);
        body=body->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    null
-------------------------------------------------------------------*/
static int vhSerNull(vhSerHandle self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
    exit
-------------------------------------------------------------------*/
static int vhSerExit(vhSerHandle self,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(cond) self->ope(self,cond);
    return 0;
}
/*------------------------------------------------------------------
    return
-------------------------------------------------------------------*/
static int vhSerReturn(vhSerHandle self,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    if(ope) self->ope(self,ope);
    return 0;
}


/*------------------------------------------------------------------
    next
-------------------------------------------------------------------*/
static int vhSerNext(vhSerHandle self,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(cond) self->ope(self,cond);
    return 0;
}

/*------------------------------------------------------------------
    wait on
-------------------------------------------------------------------*/
static int vhSerWaitOn(vhSerHandle self,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyVal);
    self->idlst(self,id);
    return 0;
}
/*------------------------------------------------------------------
    wait until
-------------------------------------------------------------------*/
static int vhSerWaitUntil(vhSerHandle self,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    self->ope(self,ope);
    return 0;
}
/*------------------------------------------------------------------
    wait for
-------------------------------------------------------------------*/
static int vhSerWaitFor(vhSerHandle self,cgrNode node){
    return 0;
}

/*------------------------------------------------------------------
    wait
-------------------------------------------------------------------*/
static int vhSerWait(vhSerHandle self,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    while(cond){
        switch(cond->type){
        case VH_on    :self->waiton(self,cond); break;
        case VH_for   :self->waitfor(self,cond);break;
        case VH_until :self->waittil(self,cond);break;
        default : break;
        }
        cond=cond->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    cfg body
-------------------------------------------------------------------*/
static int vhSerCfgBody(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_dfg      : self->dfg(self,node);      break;
    case VH_block    : self->block(self,node);    break; 
    case VH_process  : self->proc(self,node);     break;
    case VH_instance : self->instance(self,node); break;
    case VH_if       : self->stateif(self,node);  break;
    case VH_loop     : self->stateloop(self,node);break;
    case VH_generate : self->generate(self,node); break;
    case VH_null     : self->nul(self,node);      break;
    case VH_exit     : self->exit(self,node);     break;
    case VH_return   : self->retu(self,node);     break;
    case VH_next     : self->next(self,node);     break;
    case VH_case     : self->statecase(self,node);break;
    case VH_wait     : self->wait(self,node);     break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    cfg
-------------------------------------------------------------------*/
static int vhSerCfg(vhSerHandle self,cgrNode node){
    cgrNode cfg=node;
    while(cfg){
        cgrNode next=cfg->next;
        self->cfgbody(self,cfg);
        cfg=next;
    }
    return 0;
}

/*------------------------------------------------------------------
    library
-------------------------------------------------------------------*/
static int vhSerLib(vhSerHandle self,cgrNode node){
    cgrNode nlib=cgrGetNode(node,cgrKeyVal);
    self->idlst(self,nlib);
    return 0;
}

/*------------------------------------------------------------------
    use id
-------------------------------------------------------------------*/
static int vhSerUseId(vhSerHandle self,cgrNode node){
    cgrNode id=node;
    //while(id) id=id->next;
    return 0;
}
/*------------------------------------------------------------------
    use
-------------------------------------------------------------------*/
static int vhSerUse(vhSerHandle self,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyVal);
    self->useid(self,id);
    return 0;
}
/*------------------------------------------------------------------
    id list
-------------------------------------------------------------------*/
static int vhSerIdList(vhSerHandle self,cgrNode node){
    //cgrNode id=node;
    //while(id) id=id->next;
    return 0;
}

/*------------------------------------------------------------------
    port port type
-------------------------------------------------------------------*/
static int vhSerPortType(vhSerHandle self,cgrNode node){
    return 0;
}
/*------------------------------------------------------------------
    range to
-------------------------------------------------------------------*/
static int vhSerRangeTo(vhSerHandle self,cgrNode node){
    cgrNode left=cgrGetNode(node,cgrKeyLeft);
    cgrNode right=cgrGetNode(node,cgrKeyRight);
    if(!node) return 0;
    switch(node->type){
    case VH_to     :
    case VH_downto :
        self->ope(self,left);
        self->ope(self,right);
        break;
    default :
        self->ope(self,node);
        break;
    }
    return 0;
}
/*------------------------------------------------------------------
    range
-------------------------------------------------------------------*/
static int vhSerRange(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    self->rangeto(self,body);
    return 0;
}

/*------------------------------------------------------------------
    datatype
-------------------------------------------------------------------*/
static int vhSerDataType(vhSerHandle self,cgrNode node){
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(args) self->rangeto(self,args);
    return 0;
}
/*------------------------------------------------------------------
    ini
-------------------------------------------------------------------*/
static int vhSerConsIni(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    self->ope(self,node);
    return 0;
}

/*------------------------------------------------------------------
    variable declaration
-------------------------------------------------------------------*/
static int vhSerVarDecl(vhSerHandle self,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyId);
    cgrNode ptype=cgrGetNode(node,cgrKeySignal);
    cgrNode dtype=cgrGetNode(node,cgrKeyType);
    cgrNode ini=cgrGetNode(node,cgrKeyIni);
    self->idlst(self,id);
    self->porttype(self,ptype);
    self->datatype(self,dtype);
    self->init(self,ini);
    return 0;
}
/*------------------------------------------------------------------
    variable declaration list
-------------------------------------------------------------------*/
static int vhSerVarDeclList(vhSerHandle self,cgrNode node){
    cgrNode body=node;
    while(body){
        self->vardec(self,body);
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    port
-------------------------------------------------------------------*/
static int vhSerPort(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    self->declst(self,body);
    return 0;
}
/*------------------------------------------------------------------
    generic
-------------------------------------------------------------------*/
static int vhSerGeneric(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    self->declst(self,body);
    return 0;
}

/*------------------------------------------------------------------
    entity_body
-------------------------------------------------------------------*/
static int vhSerEntityBody(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_port    :self->port(self,node);   break;
    case VH_generic :self->generic(self,node);break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    entity
-------------------------------------------------------------------*/
static int vhSerEntity(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    while(body){
        self->entibody(self,body);
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    alias
-------------------------------------------------------------------*/
static int vhSerAlias(vhSerHandle self,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    cgrNode opt=cgrGetNode(node,cgrKeyOpt);
    if(!node) return 0;
    self->ope(self,ope);
    if(opt) self->idlst(self,ope);
    return 0;
}
/*------------------------------------------------------------------
    variable
-------------------------------------------------------------------*/
static int vhSerVariable(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->vardec(self,body);
    return 0;
}
/*------------------------------------------------------------------
    signal
-------------------------------------------------------------------*/
static int vhSerSignal(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->vardec(self,body);
    return 0;
}

/*------------------------------------------------------------------
    type
-------------------------------------------------------------------*/
static int vhSerType(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->idlst(self,body);
    return 0;
}

/*------------------------------------------------------------------
    subtype
-------------------------------------------------------------------*/
static int vhSerSubtype(vhSerHandle self,cgrNode node){
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->datatype(self,type);
    if(body) self->range(self,body);
    return 0;
}

/*------------------------------------------------------------------
    const
-------------------------------------------------------------------*/
static int vhSerConst(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->vardec(self,body);
    return 0;
}
/*------------------------------------------------------------------
    component
-------------------------------------------------------------------*/
static int vhSerCompo(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    while(body){
        self->entibody(self,body);
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    function declaration
-------------------------------------------------------------------*/
static int vhSerFuncDecl(vhSerHandle self,cgrNode node){
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(!node) return 0;
    if(args) self->declst(self,args);
    return 0;
}
/*------------------------------------------------------------------
    function
-------------------------------------------------------------------*/
static int vhSerFunc(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    if(!node) return 0;
    self->funcdec(self,node);
    self->decl(self,decl);
    self->cfg(self,body);
    return 0;
}

/*------------------------------------------------------------------
    util declare
-------------------------------------------------------------------*/
static int vhSerDecl(vhSerHandle self,cgrNode node){
    cgrNode decl=node;
    while(decl){
        switch(decl->type){
        case VH_variable  : self->var(self,node);    break;
        case VH_subtype   : self->subtype(self,node);break;
        case VH_type      : self->type(self,node);   break;
        case VH_component : self->comp(self,node);   break;
        case VH_const     : self->cnst(self,node);   break;
        case VH_func      : self->func(self,node);   break;
        case VH_alias     : self->alias(self,decl);  break;
        case VH_signal    : self->sig(self,decl);    break;
        default : break;
        }
        decl=decl->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    architecture
-------------------------------------------------------------------*/
static int vhSerArch(vhSerHandle self,cgrNode node){
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    self->decl(self,decl);
    self->cfg(self,body);
    return 0;
}

/*------------------------------------------------------------------
    package
-------------------------------------------------------------------*/
static int vhSerPackage(vhSerHandle self,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    
    if(decl) self->decl(self,node);
    if(body) self->file(self,node);

    return 0;
}
/*------------------------------------------------------------------
    top declare
-------------------------------------------------------------------*/
static int vhSerFile(vhSerHandle self,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_entity    : self->enti(self,node);   break;
    case VH_archit    : self->arch(self,node);   break;
    case VH_library   : self->lib(self,node);    break;
    case VH_use       : self->use(self,node);    break;
    case VH_alias     : self->alias(self,node);  break;
    case VH_variable  : self->var(self,node);    break;
    case VH_subtype   : self->subtype(self,node);break;
    case VH_type      : self->type(self,node);   break;
    case VH_component : self->comp(self,node);   break;
    case VH_const     : self->cnst(self,node);   break;
    case VH_func      : self->func(self,node);   break;
    case VH_package   : self->package(self,node);break;
    default : break;
    }
    return 0;
}

/*------------------------------------------------------------------
    root
-------------------------------------------------------------------*/
static int vhSerRoot(vhSerHandle self,cgrNode node){
    cgrNode dec=node;
    while(dec){
        self->file(self,dec);
        dec=dec->next;
    }
    return 0;
}

/*------------------------------------------------------------------
   vhdl parse tree searcher destruction
-------------------------------------------------------------------*/
void vhSerDes(vhSerHandle hhdl){
    free(hhdl);
}
/*------------------------------------------------------------------
   vhdl parse tree searcher init
-------------------------------------------------------------------*/
vhSerHandle vhSerIni(cgrNode top,VasOptHandle opt,void*work){
    vhSerHandle hhdl;
    cstrMakeBuf(hhdl,1,struct _vhSerHandle,
        memError("making buffer"));
    hhdl->top=top;
    hhdl->opt=opt;
    hhdl->work=work;

    hhdl->root=(*vhSerRoot);
    hhdl->file=(*vhSerFile);
    hhdl->arch=(*vhSerArch);
    hhdl->decl=(*vhSerDecl);
    hhdl->func=(*vhSerFunc);
    hhdl->funcdec=(*vhSerFuncDecl);
    hhdl->comp=(*vhSerCompo);
    hhdl->cnst=(*vhSerConst);
    hhdl->subtype=(*vhSerSubtype);
    hhdl->type=(*vhSerType);
    hhdl->sig=(*vhSerSignal);
    hhdl->var=(*vhSerVariable);
    hhdl->alias=(*vhSerAlias);
    hhdl->enti=(*vhSerEntity);
    hhdl->entibody=(*vhSerEntityBody);
    hhdl->generic=(*vhSerGeneric);
    hhdl->port=(*vhSerPort);
    hhdl->declst=(*vhSerVarDeclList);
    hhdl->vardec=(*vhSerVarDecl);
    hhdl->init=(*vhSerConsIni);
    hhdl->datatype=(*vhSerDataType);
    hhdl->range=(*vhSerRange);
    hhdl->rangeto=(*vhSerRangeTo);
    hhdl->porttype=(*vhSerPortType);
    hhdl->idlst=(*vhSerIdList);
    hhdl->use=(*vhSerUse);
    hhdl->useid=(*vhSerUseId);
    hhdl->lib=(*vhSerLib);
    hhdl->cfg=(*vhSerCfg);
    hhdl->cfgbody=(*vhSerCfgBody);
    hhdl->wait=(*vhSerWait);
    hhdl->waitfor=(*vhSerWaitFor);
    hhdl->waittil=(*vhSerWaitUntil);
    hhdl->waiton=(*vhSerWaitOn);
    hhdl->next=(*vhSerNext);
    hhdl->exit=(*vhSerExit);
    hhdl->retu=(*vhSerReturn);
    hhdl->nul=(*vhSerNull);
    hhdl->statecase=(*vhSerCase);
    hhdl->casebody=(*vhSerCaseBody);
    hhdl->generate=(*vhSerGenerate);
    hhdl->stateloop=(*vhSerLoop);
    hhdl->statewhile=(*vhSerWhile);
    hhdl->statefor=(*vhSerFor);
    hhdl->stateif=(*vhSerIf);
    hhdl->stateelse=(*vhSerElse);
    hhdl->instance=(*vhSerInstance);
    hhdl->genemap=(*vhSerPortMap);
    hhdl->portmap=(*vhSerPortMap);
    hhdl->mapbody=(*vhSerPortMapBody);
    hhdl->proc=(*vhSerProc);
    hhdl->block=(*vhSerBlock);
    hhdl->dfg=(*vhSerDfg);
    hhdl->dfgwhen=(*vhSerDfgWhen);
    hhdl->dfgelse=(*vhSerDfgElse);
    hhdl->ope=(*vhSerOpe);
    hhdl->funcall=(*vhSerFuncall);
    hhdl->set=(*vhSerSet);
    hhdl->num=(*vhSerNum);
    hhdl->id=(*vhSerId);
    hhdl->attr=(*vhSerAttr);
    hhdl->oper=(*vhSerOperator);
    hhdl->package=(*vhSerPackage);
    return hhdl;
}


