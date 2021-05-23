repl_0212C214_ov_0B:
    LDRSB   R0, [R0,#0x1E]
    LDR     R2, =ForcePlayerPowerupTo
    LDR     R3, [R2]
    CMP     R3, #0xFFFFFFFF
    MOVNE   R1, R3
    MOV     R3, #0xFFFFFFFF
    STR     R3, [R2]
    B       0x212C218
