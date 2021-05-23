repl_021111DC_ov_0A:
    STMFD       SP!, {R6-R7}
    ADD         R7, PC, #0x164
    ADD         LR, PC, #0
    BX          R7
    CMP         R1, R6
    ADDLT       PC, PC, #0x14
    CMP         R0, #0
    MOVLT       R0, R6
    RSBLT       R0, R0, #0
    MOVGE       R0, R6
    LDMFD       SP!, {R6-R7}
    B           0x21111F4
    LDMFD       SP!, {R6-R7}
    B           0x21111FC

repl_021114D8_ov_0A:
    STMFD       SP!, {R6-R7}
    MOV         R6, R5
    SUB         R6, #0x10
    NOP
    LDR         R6, [R6, #0x7B0]
    TST         R6, #0x80
    MOVEQ       R0, #0
    ADDNE       R0, R0, #0x80
    CMP         R0, #0
    ADDNE       PC, PC, #4
    LDMFD       SP!, {R6-R7}
    B           0x21114E0
    ADD         R7, PC, #0x100
    ADD         LR, PC, #0
    BX          R7
    NOP
    MOV         R7, R5
    SUB         R7, #0x10
    LDR         R1, [R7, #0xC4]
    LDR         R2, [R7, #0xC8]
    CMP         R1, R2
    MOVEQ       R1, #1
    MOVNE       R1, #0
    CMP         R2, #0
    RSBLT       R6, R6, #0
    CMP         R1, #0
    STRNE       R6, [R7, #0xC4]
    STR         R6, [R7, #0xC8]
    LDMFD       SP!, {R6-R7}
    B           0x2111528
    
repl_0211162C_ov_0A:
    TST         R2, #0x80           @Check if D-Pad_Down is pressed
    ADDNE       R0, R0, #0x80
    CMP         R0, #0
    BNE         0x2111634
    B           0x211169C
    
repl_02111874_ov_0A:
    STMFD       SP!, {R6-R7}
    ADD         R7, PC, #0xA0
    ADD         LR, PC, #0
    BX          R7
    MOV         R1, R6
    LDMFD       SP!, {R6-R7}
    B           0x2111878
    
repl_021122E8_ov_0A:
    STMFD       SP!, {R6-R7}
    ADD         R7, PC, #0x84
    ADD         LR, PC, #0
    BX          R7
    CMP         R0, #0
    MOVLT       R0, R6
    RSBLT       R0, R0, #0
    MOVGE       R0, R6
    LDMFD       SP!, {R6-R7}
    B           0x21122F8
    
repl_02112574_ov_0A:
    STMFD       SP!, {R6-R7}
    ADD         R7, PC, #0x5C
    ADD         LR, PC, #0
    BX          R7
    CMP         R0, #0
    MOVLT       R0, R6
    RSBLT       R0, R0, #0
    MOVGE       R0, R6
    LDMFD       SP!, {R6-R7}
    B           0x2112584
    
repl_02115D14_ov_0A:
    STMFD       SP!, {R6-R7}
    ADD         R7, PC, #0x68
    ADD         LR, PC, #0
    BX          R7
    CMP         R0, R6
    ADDGE       PC, PC, #4
    LDMFD       SP!, {R6-R7}
    B           0x2115D40
    LDMFD       SP!, {R6-R7}
    MOV         R0, R4
    SUB         R0, #0x10
    NOP
    LDR         R0, [R0, #0x7B0]
    TST         R0, #0x80
    BNE         0x2115D18
    B           0x2115D40
    
@Relative function
    STMFD       SP!, {LR}
    LDR         R7, =0x208B350      @Pointer for Starman duration counter
    LDR         R7, [R7]            @Get Starman remaining time
    CMP         R7, #0              @Check if Starman counter is 0
    MOVNE       R7, #4
    MOV         R6, R5
    SUB         R6, #0x10
    LDR         R6, [R6, #0x264]    @Get Mario's current action
    CMP         R6, #0              @Check if Mario is airborne
    MOVEQ       R7, #0
    LDR         R6, =0x212E344      @Pointer for running speed
    LDR         R6, [R6, R7]        @Get max speed
    LDMFD       SP!, {PC}
    
@Relative function 2 because meromero is really cool
    STMFD       SP!, {LR}
    LDR         R6, =0x203A784
    LDR         R7, =0x208B350      @Pointer for Starman duration counter
    LDR         R7, [R7]            @Get Starman remaining time
    CMP         R7, #0              @Check if Starman counter is 0
    MOVNE       R7, #4
    MOV         R6, R4
    SUB         R6, #0x10
    LDR         R6, [R6, #0x264]    @Get Mario's current action
    CMP         R6, #0              @Check if Mario is airborne
    MOVEQ       R7, #0
    LDR         R6, =0x212E344      @Pointer for running speed
    LDR         R6, [R6, R7]        @Get max speed
    LDMFD       SP!, {PC}
    
@ Mostly fixes bumping if shelled
repl_02104818_ov_0A:
    MOV     R4, R0
    STMFD   SP!, {R0-R4, R12, LR}
    LDR     R1, [R0, #0x7AB]
    CMP     R1, #5
    LDMNEFD SP!, {R0-R4, R12, PC}
    LDR     R1, [R0, #0x76C]
    CMP     R1, #0x6E
    LDMNEFD SP!, {R0-R4, R12, PC}
    MOV     R4, R0
    LDR     R0, =#337
    ADD     R1, R4, #0x5C
    BL      PlaySNDEffect
    LDR     R0, [R4, #0xB4]
    CMP     R0, #0
    MOVEQ   R0, R4
    BEQ     .GetHurt
    LDRB    R0, [R4, #0x2BD]
    CMP     R0, #0
    LDREQ   R0, =#-0x4000
    LDRNE   R0, =#0x4000
    STR     R0, [R4, #0xB4]
    LDMFD   SP!, {R0-R4, R12, LR}
    MOV     R1, #0
    B       0x2104878
    
.GetHurt:
    LDR     R12, [R0]
    LDR     R12, [R12,#0x64]
    MOV     R3, #0x4000
    BLX     R12 
    LDMFD   SP!, {R0-R4, R12, LR}
    MOV     R1, #0
    B       0x2104878
    
nsub_02110458_ov_0A:
    B       0x0211045C

nsub_02110480_ov_0A:
    B       0x02110484


    
.data
.balign 4
ScrollDataX:
    .word 0x00000000
ScrollDataY:    
    .word 0x00000000

