repl_0216341C_ov_36:
    LDR     R0, =0x20CADB4
    LDR     R0, [R0]
    CMP     R0, #0x1000
    LDREQ   R0, [R5,#0x60]
    MOVNE   R0, #0
    BX      LR