nsub_02020608_main:
    STMFD   SP!, {LR}
    CMP     R0, #1
    BLGT    0x20205EC
    LDMGTFD SP!, {PC}
    CMP     R0, #0
    BLLT    0x20205EC
    LDMLTFD SP!, {PC}   
    LDR     R1, =0x208B35C
    LDR     R0, [R1,R0,LSL#2]
    LDMFD   SP!, {PC}