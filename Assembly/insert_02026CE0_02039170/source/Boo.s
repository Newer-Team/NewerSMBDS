repl_02175B94_ov_47:
    MOV     R12, #6
    STR     R12, [SP]
    MOV     R12, #0
    BX      LR
    
repl_02175F2C_ov_47:
repl_02175B74_ov_47:
    MOV     R1, #0x1000
    MOV     R3, #0x100
    LDR     R2, [R5, #8]
    LSR     R2, #4
    ANDS    R2, R2, #0xF
    BXEQ    LR
    ADD     R2, #1
    MUL     R2, R3
    ADD     R1, R2
    BX      LR
    
