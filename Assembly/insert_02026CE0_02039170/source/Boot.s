repl_020CCDD4_ov_01:
    STMFD   SP!, {R1-R3, LR}
    LDR     R0, =0x208FBB8
    LDR     R1, [R0]
    MOV     R0, #0x7
    BL      NNSi_SndArcLoadGroup
    MOV     R0, #0x16C
    LDMFD   SP!, {R1-R3, PC}