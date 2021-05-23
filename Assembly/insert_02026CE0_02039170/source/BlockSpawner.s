repl_0209C1B8_ov_00:
    STR     R3, [SP,#4]
    CMP     R0, #0x29
    BXNE    LR
    LDR     R0, =0x0208B19C
    LDR     R0, [R0]    
    STMFD   SP!, {R1-R5,LR}
    MOV     R3, #0x1000
    MOV     R5, #0
    LDR     R4, [R2,#0x4]
    LDRB    R1, [R0,#0x12]
    CMP     R1, #0x0
    BEQ     .YOffs
    CMP     R1, #0x7F
    BLLS    .positive
    LDRB    R1, [R0,#0x12]
    CMP     R1, #0x7F
    BLHI    .negative
    STR     R4, [R2,#0x4]
.YOffs:
    LDRB    R1, [R0,#0x13]
    CMP     R1, #0x0
    ITTT    NE   
    LDREQB  R1, [R0,#0x1B]
    CMPEQ   R1, #0x0
    BEQ     .return
    BL      getPtrToPlayerActor
    LDRB    R5, [R0, #0x76C]
    CMP     R5, #0x15
    MOVEQ   R5, #1
    CMP     R5, #0x10
    MOVEQ   R5, #1
    LDR     R0, =0x0208B19C
    LDR     R0, [R0]
    LDR     R4, [R2,#0x8]
    CMP     R5, #1
    LDREQB  R1, [R0,#0x1B]
    CMPEQ   R1, #0x0
    BEQ     .return
    CMP     R5, #1
    SUBNE   R4, R4, #0x12000
    ADDEQ   R4, R4, #0x12000
    CMP     R5, #1
    BEQ     .down

.up:
    LDRB    R1, [R0,#0x13]
    CMP     R1, #0x7F
    BLLS    .positive
    LDRB    R1, [R0,#0x13]
    CMP     R1, #0x7F
    BLHI    .negative
    STR     R4, [R2,#0x8]
    B       .return
    
.down:
    LDRB    R1, [R0,#0x1B]
    CMP     R1, #0x7F
    BLLS    .positive
    LDRB    R1, [R0,#0x1B]
    CMP     R1, #0x7F
    BLHI    .negative
    STR     R4, [R2,#0x8]
    
.return:
    LDMFD   SP!, {R1-R5,LR}
    LDR     R1, [R0,#0x14]
    LDRH    R0, [R0,#0x10]
    BX      LR
    
.positive:
    MUL     R1, R3, R1
    ADD     R4, R4, R1
    BX      LR

.negative:
    MVN     R1, R1
    LSL     R1, #0x18
    LSR     R1, #0x18
    SUB     R1, #1
    MUL     R1, R3, R1
    SUB     R4, R4, R1
    BX      LR

repl_020D0468_ov_0A:
    LDR   R0, =0x0208B19C
    LDR   R0, [R0]
    LDRH  R0, [R0,#0x1C]
    BX    LR
    
repl_020D0420_ov_0A:
    STR   R2, [SP,#0x18]
    LDR   R0, =0x0208B19C
    LDR   R0, [R0]
    LDRH  R0, [R0, #0x18]
    CMP   R0, #0x76
    BEQ   0x020D0424
    BNE   0x020D043C