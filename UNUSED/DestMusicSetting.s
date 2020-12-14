nsub_021195EC_ov_0A:
    STMFD   SP!, {R0-R12}
    MOV     R0, R1
    BL      0x201E400
    CMP     R0, #0    
    LDMEQFD SP!, {R0-R12}
    MOVEQ   R4, R0
    BEQ     0x21195F0
    LDRB    R1, [R0,#0x10]
    CMP     R1, #0    
    LDMEQFD SP!, {R0-R12}
    MOVEQ   R4, R0
    BEQ     0x21195F0
    LDR     R2, =0x02088B70
    LDRB    R2, [R2]
    SUB     R1, #1
    CMP     R1, R2
    LDR     R0, =ReloadMusicCheck
    MOVEQ   R1, #1
    MOVNE   R2, #2
    STRB    R1, [R0]
    LDMFD   SP!, {R0-R12}
    MOV     R4, R0
    B       0x21195F0
    
nsub_0201E788_ov_0A:
    STMFD   SP!, {R0-R12}   

    LDR     R0, =ReloadMusicCheck
    LDRB    R1, [R0]
    MOV     R2, #0
    STRB    R2, [R0]
    
    CMP     R1, #0
    LDMEQFD SP!, {R0-R12}
    LDREQ   R0, =0x208B0F4
    BEQ     0x201E78C
    
    CMP     R1, #1
    LDMFD   SP!, {R0-R12}
    BEQ     0x201E7B8
    BNE     0x201E7B0   
    
    
    
.data
.balign 4

ReloadMusicCheck:
.byte 0x0