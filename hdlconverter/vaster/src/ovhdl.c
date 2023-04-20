/*
    output verilog HDL
    (c)KTDesignSystemc Corp.
*/

#include "vaster.h"

/*------------------------------------------------------------------
    macros
-------------------------------------------------------------------*/
#define upIndent(function,work,node){ \
        work->indent+=4;              \
        function(work,node);          \
        work->indent-=4;              } 
/*------------------------------------------------------------------
    function prototype declaration
-------------------------------------------------------------------*/
static int ovhdlIdList(oVhdlWork work,cgrNode node);
static int ovhdlDeclare(oVhdlWork work,cgrNode node);
static int ovhdlDfgWhen(oVhdlWork work,cgrNode node);
static int ovhdlCfg(oVhdlWork work,cgrNode node);
static int ovhdlRangeTo(oVhdlWork work,cgrNode node);
static int ovhdlOpe2(oVhdlWork work,cgrNode node);

/*------------------------------------------------------------------
    ope Left Right
-------------------------------------------------------------------*/
static int ovhdlOpeLeftRight(
    oVhdlWork work,cgrNode node,char*ope){
    cgrNode left=cgrGetNode(node,cgrKeyLeft);
    cgrNode right=cgrGetNode(node,cgrKeyRight);
    if(left) ovhdlOpe2(work,left);
    fprintf(work->fp, "%s",ope);
    if(right) ovhdlOpe2(work,right);
    return 0;
}
/*------------------------------------------------------------------
    operator
-------------------------------------------------------------------*/
static int ovhdlOperator(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_and    : ovhdlOpeLeftRight(work,node," and "); break;
    case VH_or     : ovhdlOpeLeftRight(work,node," or ");  break;
    case VH_xor    : ovhdlOpeLeftRight(work,node," xor "); break;
    case VH_nand   : ovhdlOpeLeftRight(work,node," nand ");break;
    case VH_nor    : ovhdlOpeLeftRight(work,node," nor "); break;
    case '='       : ovhdlOpeLeftRight(work,node,"=");     break;
    case VH_not_eq : ovhdlOpeLeftRight(work,node,"/=");    break;
    case '<'       : ovhdlOpeLeftRight(work,node,"<");     break;
    case '>'       : ovhdlOpeLeftRight(work,node,">");     break;
    case VH_les    : ovhdlOpeLeftRight(work,node,"<=");    break;
    case VH_gre    : ovhdlOpeLeftRight(work,node,">=");    break;
    case '&'       : ovhdlOpeLeftRight(work,node,"&");     break;
    case '+'       : ovhdlOpeLeftRight(work,node,"+");     break;
    case '-'       : ovhdlOpeLeftRight(work,node,"-");     break;
    case '*'       : ovhdlOpeLeftRight(work,node,"*");     break;
    case '/'       : ovhdlOpeLeftRight(work,node,"/");     break;
    case VH_mod    : ovhdlOpeLeftRight(work,node," mod "); break;
    case VH_rem    : ovhdlOpeLeftRight(work,node," rem "); break;
    case VH_not    : ovhdlOpeLeftRight(work,node," not "); break;
    case VH_abs    : ovhdlOpeLeftRight(work,node," abs "); break;
    case VH_mul_mul: ovhdlOpeLeftRight(work,node,"**");    break;
    case '.'       : ovhdlOpeLeftRight(work,node,".");     break;
    case VH_to     : ovhdlRangeTo(work,node);              break;
    case VH_downto : ovhdlRangeTo(work,node);              break;

    }
    return 0;
}
/*------------------------------------------------------------------
    attr
-------------------------------------------------------------------*/
static int ovhdlAttr(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_funcargs :
        fprintf(work->fp,"'(");
        ovhdlOpe(work,cgrGetNode(node,cgrKeyVal));
        fprintf(work->fp,")");
        break;
    default : 
        fprintf(work->fp,"'%s",cgrGetStr(node,cgrKeyVal));
        break;
    }
    return 0;
}
/*------------------------------------------------------------------
    id
-------------------------------------------------------------------*/
static int ovhdlId(oVhdlWork work,cgrNode node){
    char*str=cgrGetStr(node,cgrKeyVal);
    cgrNode attr=cgrGetNode(node,cgrKeyOpt);
    if(!node) return 0;
    fprintf(work->fp,"%s",str);
    if(attr) ovhdlAttr(work,attr);
    return 0;
}
/*------------------------------------------------------------------
    num
-------------------------------------------------------------------*/
static int ovhdlNum(oVhdlWork work,cgrNode node){
     char*str=cgrGetStr(node,cgrKeyVal);
    if(!node) return 0;
    fprintf(work->fp,"%s",str);
    return 0;
}
/*------------------------------------------------------------------
    set
-------------------------------------------------------------------*/
static int ovhdlSet(oVhdlWork work,cgrNode node){
    cgrNode val=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    fprintf(work->fp,"(");
    while(val){
        cgrNode ope=cgrGetNode(val,cgrKeyVal);
        cgrNode cond=cgrGetNode(val,cgrKeyCond);
        if(cond){
            ovhdlOpe(work,cond);
            fprintf(work->fp,"=>");
        }else fprintf(work->fp,"others=>");
        ovhdlOpe(work,ope);
        if(val->next) fprintf(work->fp,",");
        val=val->next;
    }
    fprintf(work->fp,")");
    return 0;
}
/*------------------------------------------------------------------
    funcall
-------------------------------------------------------------------*/
static int ovhdlFuncall(oVhdlWork work,cgrNode node){
    char*id=cgrGetStr(node,cgrKeyId);
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(!node) return 0;
    fprintf(work->fp,"%s(",id);
    ovhdlOpe(work,args);
    fprintf(work->fp,")");
    return 0;
}
/*------------------------------------------------------------------
    ope2
-------------------------------------------------------------------*/
static int ovhdlOpe2(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    while(node){
        switch(node->type){
        case VH_id      :ovhdlId(work,node);       break;
        case VH_num     :ovhdlNum(work,node);      break;
        case VH_set     :ovhdlSet(work,node);      break;
        case VH_funcall :ovhdlFuncall(work,node);  break;
        default         :
            fprintf(work->fp,"(");
            ovhdlOperator(work,node); 
            fprintf(work->fp,")");
            break;
        }
        if(node->next) fprintf(work->fp,",");
        node=node->next;
    }
    return 0;
}


/*------------------------------------------------------------------
    ope
-------------------------------------------------------------------*/
int ovhdlOpe(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    while(node){
        switch(node->type){
        case VH_id      :ovhdlId(work,node);       break;
        case VH_num     :ovhdlNum(work,node);      break;
        case VH_set     :ovhdlSet(work,node);      break;
        case VH_funcall :ovhdlFuncall(work,node);  break;
        default         :ovhdlOperator(work,node); break;
        }
        if(node->next) fprintf(work->fp,",");
        node=node->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    dfg else
-------------------------------------------------------------------*/
static int ovhdlDfgElse(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    fprintf(work->fp,"else ");
    ovhdlOpe(work,body);
    if(cond){
        fprintf(work->fp,"\n");
        ovhdlDfgWhen(work,cond);
    }
    return 0;
}
/*------------------------------------------------------------------
    dfg when
-------------------------------------------------------------------*/
static int ovhdlDfgWhen(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    fprintf(work->fp," when ");
    ovhdlOpe(work,body);
    if(cond){
        fprintf(work->fp,"\n");
        osyscIndent(work->fp,work->indent);
        ovhdlDfgElse(work,cond);
    }
    return 0;
}


/*------------------------------------------------------------------
    dfg
-------------------------------------------------------------------*/
static int ovhdlDfg(oVhdlWork work,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);    
    cgrNode cnd=cgrGetNode(node,cgrKeyCond);    
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    ovhdlOpe(work,ope);
    if(cnd) ovhdlDfgWhen(work,cnd);
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    block
-------------------------------------------------------------------*/
static int ovhdlBlock(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode gurd=cgrGetNode(node,cgrKeyCond);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"%s : block",name);
    if(gurd){
        fprintf(work->fp,"(");
        ovhdlOpe(work,gurd);
        fprintf(work->fp,")");
    }
    fprintf(work->fp,"\n");
    upIndent(ovhdlDeclare,work,decl);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"begin\n");
    upIndent(ovhdlCfg,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end block %s;\n\n",name);
    return 0;
}
/*------------------------------------------------------------------
    process
-------------------------------------------------------------------*/
static int ovhdlProc(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyId);
    cgrNode sen=cgrGetNode(node,cgrKeySensitive);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    if(id) fprintf(work->fp,"%s:",cgrGetStr(id,cgrKeyVal));
    fprintf(work->fp,"process ");
    if(sen){
        fprintf(work->fp,"(");
        ovhdlIdList(work,sen);
        fprintf(work->fp,")");
    }
    fprintf(work->fp,"\n");
    upIndent(ovhdlDeclare,work,decl);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"begin\n");
    upIndent(ovhdlCfg,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end process");
    if(id) fprintf(work->fp," %s",cgrGetStr(id,cgrKeyVal));
    fprintf(work->fp,";\n\n");
    return 0;
}

/*------------------------------------------------------------------
    port map body
-------------------------------------------------------------------*/
static int ovhdlPortMapBody(oVhdlWork work,cgrNode node){
    cgrNode val=cgrGetNode(node,cgrKeyVal);
    char* str=cgrGetStr(node,cgrKeyId);
    if(str){
        if(strcmp(str,"")) fprintf(work->fp,"%s=>",str);
    }
    ovhdlOpe(work,val);
    return 0;
}

/*------------------------------------------------------------------
    port map
-------------------------------------------------------------------*/
static int ovhdlPortMap(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    fprintf(work->fp,"port map (");
    while(body){
        ovhdlPortMapBody(work,body);
        if(body->next) fprintf(work->fp,",");
        body=body->next;
    }
    fprintf(work->fp,")");
    return 0;
}
/*------------------------------------------------------------------
    generic map
-------------------------------------------------------------------*/
static int ovhdlGenericMap(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    fprintf(work->fp,"generic map (");
    while(body){
        ovhdlOpe(work,body);
        if(body->next) fprintf(work->fp,",");
        body=body->next;
    }
    fprintf(work->fp,")");
    return 0;

    return 0;
}
/*------------------------------------------------------------------
    instance
-------------------------------------------------------------------*/
static int ovhdlInstance(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"%s:",name);
    if(type){
        switch(type->type){
        case VH_entity:fprintf(work->fp,"entity ");break;
        default : break;
        }
    }
    ovhdlOpe(work,decl);
    fprintf(work->fp," ");
    if(body){
        switch(body->type){
        case VH_port    : ovhdlPortMap(work,body);break;
        case VH_generic : ovhdlGenericMap(work,body);break;
        default :  break;
        }
    }
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    else
-------------------------------------------------------------------*/
static int ovhdlElse(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    if(cond){
        fprintf(work->fp,"elsif(");
        ovhdlOpe(work,cond);
        fprintf(work->fp,") then\n");
    }else fprintf(work->fp,"else\n");
    upIndent(ovhdlCfg,work,body);
    return 0;
}
/*------------------------------------------------------------------
    if
-------------------------------------------------------------------*/
static int ovhdlIf(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    cgrNode step=cgrGetNode(node,cgrKeyStep);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"if(");
    ovhdlOpe(work,cond);
    fprintf(work->fp,") then\n");
    upIndent(ovhdlCfg,work,body);
    while(step){
        ovhdlElse(work,step);
        step=step->next;
    }
    fprintf(work->fp,"end if;\n");
    return 0;
}
/*------------------------------------------------------------------
    for
-------------------------------------------------------------------*/
static int ovhdlFor(oVhdlWork work,cgrNode node){
    char*id=cgrGetStr(node,cgrKeyId);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    fprintf(work->fp,"for %s in ",id);
    ovhdlRangeTo(work,cond);
    fprintf(work->fp," ");
    return 0;
}
/*------------------------------------------------------------------
    while
-------------------------------------------------------------------*/
static int ovhdlWhile(oVhdlWork work,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    fprintf(work->fp,"while ");
    ovhdlOpe(work,cond);
    fprintf(work->fp," ");
    return 0;
}

/*------------------------------------------------------------------
    loop
-------------------------------------------------------------------*/
static int ovhdlLoop(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyId);
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    if(id) fprintf(work->fp,"%s:",cgrGetStr(id,cgrKeyVal));
    if(type){
        switch(type->type){
        case VH_for   : ovhdlFor(work,type);  break;
        case VH_while : ovhdlWhile(work,type);break;
        default : break;
        }
    }
    fprintf(work->fp,"loop\n");
    upIndent(ovhdlCfg,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end loop");
    if(id) fprintf(work->fp," %s",cgrGetStr(id,cgrKeyVal));
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    generate
-------------------------------------------------------------------*/
static int ovhdlGenerate(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyId);
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    if(id) fprintf(work->fp,"%s:",cgrGetStr(id,cgrKeyVal));
    if(type){
        switch(type->type){
        case VH_for   : ovhdlFor(work,type);  break;
        case VH_while : ovhdlWhile(work,type);break;
        default : break;
        }
    }
    fprintf(work->fp,"generate\n");
    upIndent(ovhdlCfg,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end generate");
    if(id) fprintf(work->fp," %s",cgrGetStr(id,cgrKeyVal));
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    case body
-------------------------------------------------------------------*/
static int ovhdlCaseBody(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"when ");
    if(cond){
        ovhdlOpe(work,cond);
        fprintf(work->fp,"=>\n");
    }else fprintf(work->fp,"others\n");
    upIndent(ovhdlCfg,work,body);
    return 0;
}
/*------------------------------------------------------------------
    case
-------------------------------------------------------------------*/
static int ovhdlCase(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"case(");
    ovhdlOpe(work,cond);
    fprintf(work->fp,") is\n");
    while(body){
        ovhdlCaseBody(work,body);
        body=body->next;
    }
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end case\n;");
    return 0;
}

/*------------------------------------------------------------------
    null
-------------------------------------------------------------------*/
static int ovhdlNull(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"null;\n");
    return 0;
}
/*------------------------------------------------------------------
    exit
-------------------------------------------------------------------*/
static int ovhdlExit(oVhdlWork work,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"exit");
    if(cond){
        fprintf(work->fp," when ");
        ovhdlOpe(work,cond);
    }
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    next
-------------------------------------------------------------------*/
static int ovhdlNext(oVhdlWork work,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"next");
    if(cond){
        fprintf(work->fp," when ");
        ovhdlOpe(work,cond);
    }
    fprintf(work->fp,";\n");
    return 0;
}

/*------------------------------------------------------------------
    wait on
-------------------------------------------------------------------*/
static int ovhdlWaitOn(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyVal);
    fprintf(work->fp,"on ");
    ovhdlIdList(work,id);
    return 0;
}
/*------------------------------------------------------------------
    wait until
-------------------------------------------------------------------*/
static int ovhdlWaitUntil(oVhdlWork work,cgrNode node){
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    fprintf(work->fp,"until ");
    ovhdlOpe(work,ope);
    return 0;
}
/*------------------------------------------------------------------
    wait for
-------------------------------------------------------------------*/
static int ovhdlWaitFor(oVhdlWork work,cgrNode node){
    char* num=cgrGetStr(node,cgrKeyVal);
    char* type=cgrGetStr(node,cgrKeyType);
    fprintf(work->fp,"for %s%s",num,type);
    return 0;
}

/*------------------------------------------------------------------
    wait
-------------------------------------------------------------------*/
static int ovhdlWait(oVhdlWork work,cgrNode node){
    cgrNode cond=cgrGetNode(node,cgrKeyCond);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"wait");
    while(cond){
        fprintf(work->fp," ");
        switch(cond->type){
        case VH_on    :ovhdlWaitOn(work,cond);break;
        case VH_for   :ovhdlWaitFor(work,cond);break;
        case VH_until :ovhdlWaitUntil(work,cond);break;
        default : break;
        }
        cond=cond->next;
    }

    fprintf(work->fp,";\n");
    return 0;
}


/*------------------------------------------------------------------
    cfg body
-------------------------------------------------------------------*/
static int ovhdlCfgBody(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_dfg      : ovhdlDfg(work,node);     break;
    case VH_block    : ovhdlBlock(work,node);   break; 
    case VH_process  : ovhdlProc(work,node);    break;
    case VH_instance : ovhdlInstance(work,node);break;
    case VH_if       : ovhdlIf(work,node);      break;
    case VH_loop     : ovhdlLoop(work,node);    break;
    case VH_generate : ovhdlGenerate(work,node);break;
    case VH_null     : ovhdlNull(work,node);    break;
    case VH_exit     : ovhdlExit(work,node);    break;
    case VH_next     : ovhdlExit(work,node);    break;
    case VH_case     : ovhdlCase(work,node);    break;
    case VH_wait     : ovhdlWait(work,node);    break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    cfg
-------------------------------------------------------------------*/
static int ovhdlCfg(oVhdlWork work,cgrNode node){
    cgrNode cfg=node;
    if(!node) return 0;
    while(cfg){
        ovhdlCfgBody(work,cfg);
        cfg=cfg->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    library
-------------------------------------------------------------------*/
static int ovhdlLib(oVhdlWork work,cgrNode node){
    cgrNode nlib=cgrGetNode(node,cgrKeyVal);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"library ");
    ovhdlIdList(work,nlib);
    fprintf(work->fp,";\n");
    return 0;
}

/*------------------------------------------------------------------
    use id
-------------------------------------------------------------------*/
static int ovhdlUseId(oVhdlWork work,cgrNode node){
    cgrNode id=node;
    while(id){
        char*str=cgrGetStr(id,cgrKeyVal);
        fprintf(work->fp,str);
        if(id->next) fprintf(work->fp,".");
        id=id->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    use
-------------------------------------------------------------------*/
static int ovhdlUse(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyVal);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"use ");
    ovhdlUseId(work,id);
    fprintf(work->fp,";\n");
    return 0;
}

/*------------------------------------------------------------------
    id list
-------------------------------------------------------------------*/
static int ovhdlIdList(oVhdlWork work,cgrNode node){
    cgrNode id=node;
    while(id){
        char*str=cgrGetStr(id,cgrKeyVal);
        fprintf(work->fp,str);
        if(id->next) fprintf(work->fp,",");
        id=id->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    port port type
-------------------------------------------------------------------*/
static int ovhdlPortType(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_in   : fprintf(work->fp,"in ");    break;
    case VH_out  : fprintf(work->fp,"out ");   break;
    case VH_inout: fprintf(work->fp,"inout "); break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    range to
-------------------------------------------------------------------*/
static int ovhdlRangeTo(oVhdlWork work,cgrNode node){
    cgrNode left=cgrGetNode(node,cgrKeyLeft);
    cgrNode right=cgrGetNode(node,cgrKeyRight);
    if(!node) return 0;
    ovhdlOpe(work,left);
    switch(node->type){
    case VH_to     :fprintf(work->fp," to ");
    case VH_downto :fprintf(work->fp," downto ");
    default : break;
    }
    ovhdlOpe(work,right);
    return 0;
}
/*------------------------------------------------------------------
    range
-------------------------------------------------------------------*/
static int ovhdlRange(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    fprintf(work->fp,"range ");
    ovhdlRangeTo(work,body);
    return 0;
}

/*------------------------------------------------------------------
    datatype
-------------------------------------------------------------------*/
static int ovhdlDataType(oVhdlWork work,cgrNode node){
    char*id=cgrGetStr(node,cgrKeyId);
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    if(!node) return 0;
    fprintf(work->fp,id);
    if(args){
        fprintf(work->fp,"(");
        ovhdlRangeTo(work,args);
        fprintf(work->fp,")");
    }
    return 0;
}
/*------------------------------------------------------------------
    ini
-------------------------------------------------------------------*/
static int ovhdlIni(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    fprintf(work->fp,":=");
    ovhdlOpe(work,node);
    return 0;
}

/*------------------------------------------------------------------
    variable declaration
-------------------------------------------------------------------*/
static int ovhdlVarDecl(oVhdlWork work,cgrNode node){
    cgrNode id=cgrGetNode(node,cgrKeyId);
    cgrNode ptype=cgrGetNode(node,cgrKeySignal);
    cgrNode dtype=cgrGetNode(node,cgrKeyType);
    cgrNode ini=cgrGetNode(node,cgrKeyIni);
    ovhdlIdList(work,id);
    fprintf(work->fp,":");
    ovhdlPortType(work,ptype);
    ovhdlDataType(work,dtype);
    ovhdlIni(work,ini);
    return 0;
}
/*------------------------------------------------------------------
    variable declaration list
-------------------------------------------------------------------*/
static int ovhdlVarDeclList(oVhdlWork work,cgrNode node){
    cgrNode body=node;
    while(body){
        osyscIndent(work->fp,work->indent);
        ovhdlVarDecl(work,body);
        if(body->next) fprintf(work->fp,";\n");
        body=body->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    port
-------------------------------------------------------------------*/
static int ovhdlPort(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"port (\n");
    upIndent(ovhdlVarDeclList,work,body);
    fprintf(work->fp,");\n");
    return 0;
}
/*------------------------------------------------------------------
    generic
-------------------------------------------------------------------*/
static int ovhdlGeneric(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"generic (\n");
    upIndent(ovhdlVarDeclList,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,");\n");
    return 0;
}


/*------------------------------------------------------------------
    entity_body
-------------------------------------------------------------------*/
static int ovhdlEntityBody(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_port    :ovhdlPort(work,node);   break;
    case VH_generic :ovhdlGeneric(work,node);break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    entity
-------------------------------------------------------------------*/
static int ovhdlEntity(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"entity %s is\n",name);
    while(body){
        upIndent(ovhdlEntityBody,work,body);
        body=body->next;
    }
    fprintf(work->fp,"end entity %s;\n\n",name);
    return 0;
}
/*------------------------------------------------------------------
    alias
-------------------------------------------------------------------*/
static int ovhdlAlias(oVhdlWork work,cgrNode node){
    char* name=cgrGetStr(node,cgrKeyId);
    cgrNode ope=cgrGetNode(node,cgrKeyVal);
    cgrNode opt=cgrGetNode(node,cgrKeyOpt);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"alias %s is ",name);
    ovhdlOpe(work,ope);
    if(opt){
        fprintf(work->fp,"[");
        ovhdlIdList(work,opt);
        fprintf(work->fp,"]");
    }
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    variable
-------------------------------------------------------------------*/
static int ovhdlVariable(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"variable ");
    ovhdlVarDecl(work,body);
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    signal
-------------------------------------------------------------------*/
static int ovhdlSignal(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"signal ");
    ovhdlVarDecl(work,body);
    fprintf(work->fp,";\n");
    return 0;
}

/*------------------------------------------------------------------
    type
-------------------------------------------------------------------*/
static int ovhdlType(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"type %s is (",name);
    ovhdlIdList(work,body);
    fprintf(work->fp,");\n");
    return 0;

    return 0;
}

/*------------------------------------------------------------------
    subtype
-------------------------------------------------------------------*/
static int ovhdlSubtype(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode type=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"subtype %s is ",name);
    ovhdlDataType(work,type);
    if(body) ovhdlRange(work,body);
    fprintf(work->fp,";\n");
    return 0;
}

/*------------------------------------------------------------------
    const
-------------------------------------------------------------------*/
static int ovhdlConst(oVhdlWork work,cgrNode node){
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"const ");
    ovhdlVarDecl(work,body);
    fprintf(work->fp,";\n");
    return 0;
}
/*------------------------------------------------------------------
    component
-------------------------------------------------------------------*/
static int ovhdlCompo(oVhdlWork work,cgrNode node){
    char*name=cgrGetStr(node,cgrKeyId);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"component %s\n",name);
    while(body){
        upIndent(ovhdlEntityBody,work,body);
        body=body->next;
    }
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end component;\n");
    return 0;
}
/*------------------------------------------------------------------
    function declaration
-------------------------------------------------------------------*/
static int ovhdlFuncDecl(oVhdlWork work,cgrNode node){
    char*   name =cgrGetStr(node,cgrKeyId);
    cgrNode ftype=cgrGetNode(node,cgrKeyType);
    cgrNode args=cgrGetNode(node,cgrKeyArgs);
    cgrNode retn=cgrGetNode(node,cgrKeyProcType);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    switch(ftype->type){
    case VH_func      : fprintf(work->fp,"function ");break;
    case VH_procedure : fprintf(work->fp,"procedure ");break;
    default : break;
    }
    fprintf(work->fp,name);
    if(args){
        fprintf(work->fp,"(\n");
        upIndent(ovhdlVarDeclList,work,args);
        fprintf(work->fp,")");
    }
    if(retn) ovhdlDataType(work,retn);

    return 0;
}
/*------------------------------------------------------------------
    function
-------------------------------------------------------------------*/
static int ovhdlFunc(oVhdlWork work,cgrNode node){
    char*   name =cgrGetStr(node,cgrKeyId);
    cgrNode ftype=cgrGetNode(node,cgrKeyType);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    if(!node) return 0;
    ovhdlFuncDecl(work,node); 
    if((!body) && (!decl)) fprintf(work->fp,";\n");
    fprintf(work->fp," is\n");
    upIndent(ovhdlDeclare,work,decl);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"begin\n");
    upIndent(ovhdlCfg,work,body);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"end ");
    switch(ftype->type){
    case VH_func      : fprintf(work->fp,"function ");break;
    case VH_procedure : fprintf(work->fp,"procedure ");break;
    default : break;
    }
    fprintf(work->fp,"%s;\n\n",name);

    return 0;
}
/*------------------------------------------------------------------
    util declare
-------------------------------------------------------------------*/
static int ovhdlDeclare(oVhdlWork work,cgrNode node){
    cgrNode decl=node;
    while(decl){
        switch(decl->type){
        case VH_variable  : ovhdlVariable(work,decl);break;
        case VH_subtype   : ovhdlSubtype(work,decl); break;
        case VH_type      : ovhdlType(work,decl);    break;
        case VH_alias     : ovhdlAlias(work,decl);   break;
        case VH_component : ovhdlCompo(work,decl);   break;
        case VH_const     : ovhdlConst(work,decl);   break;
        case VH_func      : ovhdlFunc(work,decl);    break;
        case VH_signal    : ovhdlSignal(work,decl);  break;
        default : break;
        }
        decl=decl->next;
    }
    return 0;
}
/*------------------------------------------------------------------
    architecture
-------------------------------------------------------------------*/
static int ovhdlArchit(oVhdlWork work,cgrNode node){
    char* name=cgrGetStr(node,cgrKeyId);
    cgrNode enti=cgrGetNode(node,cgrKeyPar);
    cgrNode decl=cgrGetNode(node,cgrKeyDeclared);
    cgrNode body=cgrGetNode(node,cgrKeyVal);
    if(!node) return 0;
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"architecture %s of ",name);
    ovhdlUseId(work,enti);
    fprintf(work->fp," is\n");
    upIndent(ovhdlDeclare,work,decl);
    osyscIndent(work->fp,work->indent);
    fprintf(work->fp,"begin\n");
    upIndent(ovhdlCfg,work,body);
    fprintf(work->fp,"end architecture %s;\n\n",name);
    return 0;
}
/*------------------------------------------------------------------
    top declare
-------------------------------------------------------------------*/
static int ovhdlTopDecl(oVhdlWork work,cgrNode node){
    if(!node) return 0;
    switch(node->type){
    case VH_entity    : ovhdlEntity(work,node);  break;
    case VH_archit    : ovhdlArchit(work,node);  break;
    case VH_library   : ovhdlLib(work,node);     break;
    case VH_use       : ovhdlUse(work,node);     break;
    case VH_alias     : ovhdlAlias(work,node);   break;
    case VH_variable  : ovhdlVariable(work,node);break;
    case VH_subtype   : ovhdlSubtype(work,node); break;
    case VH_type      : ovhdlType(work,node);    break;
    case VH_component : ovhdlCompo(work,node);   break;
    case VH_const     : ovhdlConst(work,node);   break;
    case VH_func      : ovhdlFunc(work,node);    break;
    default : break;
    }
    return 0;
}
/*------------------------------------------------------------------
    top
-------------------------------------------------------------------*/
static int ovhdlTop(oVhdlWork work,cgrNode top){
    cgrNode dec=top;
    while(dec){
        //### checking lib or source
        ovhdlTopDecl(work,dec);
        dec=dec->next;
    }
    return 0;
}

/*------------------------------------------------------------------
    output vhdl file
-------------------------------------------------------------------*/
int ovhdl(char*fname,cgrNode top,VasOptHandle opt){
    struct _oVhdlWork work;

    //init
    work.indent=0;
    work.fname=fname;
    work.opt=opt;
    work.fp=fopen(fname,"w");
    if(!work.fp){
        cmsgEcho(__sis_msg,0,fname);
        return -1;
    }
    //start output
    ovhdlTop(&work,top);

    //close
    fclose(work.fp);
    return 0;
}


