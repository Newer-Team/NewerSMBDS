nsub_020AAB1C_ov_00:
    CMP     R1, #0
    BXEQ    LR
    LDR     R1, [R1]
    B       0x20AAB20