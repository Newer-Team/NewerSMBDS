nsub_020A6E70_ov_00:
    STMFD   SP!, {R5}
    LDR     R5, =WavyLiquidSpawned
    LDRB    R5, [R5]
    CMP     R5, #0
    LDMFD   SP!, {R5}
    STMEQFD SP!, {R4-R6,LR}
    BEQ     0x20A6E74
    STMFD   SP!, {R1-R12,LR}
    BLNE    IsSpriteUnderWavyLiquid
    LDMFD   SP!, {R1-R12,PC}
    
# ------------ Barrels 
    
nsub_02186F50_ov_60:
    STMFD   SP!, {R0-R12, LR}
    MOV     R1, R4
    LDR     R0, [R1, #0x60]
    BL      WavyLiquidHeightAt
    STR     R0, [R4,#0x490]
    LDMFD   SP!, {R0-R12, LR}
    LDR     R0, =0x4AC
    RSB     R3, R12, #0
    MOV     R1, R4
    B       0x02186F70 
    
nsub_02186FE4_ov_60:
    MOV     R4, R0
    LDR     R0, [R4, #0x60]
    BL      WavyLiquidHeightAt
    STR     R0, [R4,#0x490]
    LDR     R1, =0x484
    B       0x02187000  
    
repl_021874C0_ov_60:
repl_021875F8_ov_60:
repl_02187AB4_ov_60:
    STMFD   SP!, {R0, R1-R12, LR}
    LDR     R0, [R4, #0x60]
    BL      WavyLiquidHeightAt
    MOV     R1, R0
    LDMFD   SP!, {R0, R1-R12, PC}
    
# ------------ End Barrels 