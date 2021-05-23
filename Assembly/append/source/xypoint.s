repl_021622B8_ov_36:
    LDR     R0, [R4, #8]
    AND     R0, R0, #0xF
    CMP     R0, #0
    BLEQ    0x2020608
    BEQ     0x21622BC 
    CMP     R0, #2
    BLLT    .Normal
    BLEQ    .Precise
    CMP     R0, #3
    BLGE    .Event
    
.Normal:
    MOV     R0, #0
    BL      0x2020608
    LDRH    R1, [R0,#0x62]
    LDRH    R2, [R4,#0x62]
    CMP     R1, R2
    MOVNE   R0, #0
    LDMNEFD SP!, {R4,PC}
    LDRH    R1, [R0,#0x66]
    ADD     R1, R1, #2
    LDRH    R2, [R4,#0x66]
    CMP     R1, R2
    MOVLT   R0, #0
    LDMLTFD SP!, {R4,PC}
    MOVGE   R0, #1
    LDMFD   SP!, {R4,PC}

.Precise:
    MOV     R0, #0
    BL      0x2020608
    LDRH    R1, [R0,#0x62]
    LDRH    R2, [R4,#0x62]
    CMP     R1, R2
    MOVNE   R0, #0
    LDMNEFD SP!, {R4,PC}
    LDRH    R1, [R0,#0x66]
    ADD     R1, R1, #2
    LDRH    R2, [R4,#0x66]
    CMP     R1, R2
    MOVNE   R0, #0
    LDMNEFD SP!, {R4,PC}
    MOVEQ   R0, #1
    LDMFD   SP!, {R4,PC}

.Event:
    LDR     R1, =0x208AF3C
    LDR     R12, [R1]
    LDR     R0, [R4, #0x334]
    AND     R0, R12, R0
    CMP     R0, #0
    LDMEQFD SP!, {R4,PC}
    MOVNE   R0,#1
    LDMFD   SP!, {R4,PC}
    
repl_02162110_ov_36:
    MOV     R0, #0
    STMFD   SP!, {R0,R1,R2,LR}
    BL      getPtrToPlayerActor
    ADD     R0, #0x700
    ADD     R0, #0xAB
    LDRB    R1, [R0]
    CMP     R1, #0x0
    MOVEQ   R1, #1
    STRB    R1, [R0]
    LDMFD   SP!, {R0,R1,R2,LR}
    BX LR 

nsub_021624BC_ov_36:        
    BX      LR
    
nsub_021620C8_ov_36:
    BX      LR