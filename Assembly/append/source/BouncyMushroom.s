repl_0217E390_ov_5A:
    LDR     R0, [R5, #8]
    LSR     R0, R0, #0x8
    ANDS    R0, R0, #0xF
    LDREQ   R0, =0x535
    LDRNE   R0, =0x9CC
    BX      LR


nsub_0217E3B8_ov_5A:
    STMFD   SP!, {LR}
    SUB     SP, SP, #4
    LDR     R0, =0x535
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x9CC
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    MOV     R0, #1
    ADD     SP, SP, #4
    LDMFD   SP!, {PC}   
    
# Instead of checking if A button is pressed, check if it's held.
nsub_021836B8_ov_5A:
    BL 0x212C630
    B 0x021836BC