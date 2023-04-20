ENTRY:
    GOT MAIN
TEXT:
    CONST1: 0x004
    CONST2: 0x002
    CONST3: 0x003
    CONST4: 0x001
    CONST6: 0x006
    AX_ADDR: ax    
    BX_ADDR: bx    
    CX_ADDR: cx    
    DX_ADDR: dx 
    DATA_RAM0: 0x80
    DATA_RAM1: 0x81
    DATA_RAM2: 0x82
MAIN:
    MOV AX_ADDR CONST1
    MOV BX_ADDR CONST2
    ADD ax bx
    MOV BX_ADDR CONST3
    MUL ax bx
    MOV BX_ADDR CONST4
    RSF ax bx
    MOV DATA_RAM0 ax
    MOV BX_ADDR ax
    SUB ax CONST3
    MOV DATA_RAM2 ax
    MOV CX_ADDR CONST6
    LOOP_START: JMP cx LOOP_BODY LOOP_END
    LOOP_BODY:
        SUB cx CONST4
        MOV CX_ADDR ax
        ADD bx ax
        MOV BX_ADDR ax
        GOT LOOP_START
    LOOP_END:
    MOV DATA_RAM1 bx
    HLT
  
