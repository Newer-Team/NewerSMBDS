repl_0215CAB0_ov_36:
repl_0215CAE8_ov_36:    
    LDR     R0, [R5, #8]
    LSR     R0, #28
    ANDS    R0, #0xF
    MOVNE   R2, R0
    MOV     R0, R5
    BX      LR
