@ Making bound limited areas backgrounds scroll properly
repl_020B917C_ov_00:
    LDR     R3, =ViewStartAndEndForBoundsLimit
    LDR     R0, [R3]
    CMP     R0, #0
    LDREQ   R3, =0x20CAE60
    BX      LR
    
nsub_020BA8B4_ov_00:
    LDR     LR, =ViewStartAndEndForBoundsLimit
    LDR     R3, [R0]
    CMP     R3, #0
    LDREQ   LR, =0x20CAE60
    B       0x020BA8B8

@ Disabling music on level enter
nsub_02011F04_main:
    LDR     R1, =MusicOffMode
    LDRB    R1, [R1]
    CMP     R1, #0
    BXNE    LR
    STMFD   SP!, {LR}
    SUB     SP, SP, #4
    B       0x2011F0C
    
repl_020D0264_ov_0A:
    LDR     R2, =IsSpecialLevel
    LDRB    R2, [R2]
    CMP     R2, #0
    BEQ     0x2012398
    BX      LR  
    
repl_02114E58_ov_0A:        @ Disabling Mario Y rotation autoupdate for the ending scene
    STMFD   SP!, {R1}
    LDR     R1, =DisableMarioYRotationForce
    LDRB    R1, [R1]
    CMP     R1, #0
    LDMFD   SP!, {R1}
    STREQH  R0, [R4,#0xA2]
    BX      LR
    
repl_02114E44_ov_0A:
repl_02114E70_ov_0A:
    STMFD   SP!, {R1}
    LDR     R1, =DisableMarioYRotationForce
    LDRB    R1, [R1]
    CMP     R1, #0
    LDMFD   SP!, {R1}
    STREQH  R1, [R4,#0xA2]
    BX      LR
    
nsub_02013BDC_main:         @ Saving some more RAM...
    LDR     R0, =IsSpecialLevel
    LDRB    R0, [R0]
    CMP     R0, #0
    BXNE    LR
    STMFD   SP!, {LR}
    B       0x02013BE0
    
nsub_0200B7FC_main:         @ Load blank HUD if special level
    ADD     R1, R6, #0x6400000
    STMFD   SP!, {R5}
    LDR     R5, =IsSpecialLevel
    LDRB    R5, [R5]
    CMP     R5, #0
    BEQ     .Load
    LDR     R5, =0x789
    CMP     R0, R5
    BNE     .Load
    LDR     R0, =0xA00
    .Load:
    LDMFD   SP!, {R5}
    B       0x200B800

repl_020A31EC_ov_00:        @ Set VRAM Bank A and B to be the Texture Palette if this is a titlescreen
    LDR     R0, =TitleScrNumber
    LDR     R0, [R0]
    CMP     R0, #0
    MOVNE   R0, #0x3
    MOVEQ   R0, #0x6
    BX      LR
    
repl_020BDB08_ov_00:        @ Set VRAM Bank C to be SUB_BG if this is a titlescreen
    LDR     R0, =TitleScrNumber
    LDR     R0, [R0]
    CMP     R0, #0
    MOVNE   R0, #0x4
    MOVEQ   R0, #0x80
    BX      LR
    
repl_020BDB18_ov_00:        @ Set mode to 5 if this is a special level
    LDR     R0, =TitleScrNumber
    LDRB    R0, [R0]
    CMP     R0, #0
    MOVNE   R0, #0x5
    BX      LR
    
nsub_02010094_main:         @ Remove control from player
    LDR     R1, =ControlLock
    LDRB    R1, [R1]
    CMP     R1, #0
    BXNE    LR
    STMFD   SP!, {R4,LR}
    B       0x2010098
repl_0200A874_main:
    STMFD   SP!, {R2-R5}
    LDR     R2, =HammerThrowingDisabledByEndOfControlLock
    LDRB    R3, [R2]
    CMP     R3, #0
    SUBNE   R3, R3, #1
    STRB    R3, [R2]
    LDR     R2, =ControlLock
    LDRB    R3, [R2]
    CMP     R3, #0
    LDR     R4, =ControlLockWasEnabled
    MOVNE   R5, #1
    STRNEB  R5, [R4]
    LDREQB  R5, [R4]
    CMP     R3, #0
    CMPEQ   R5, #1
    ANDNE   R12, R0, R1
    MOVEQ   R12, #0
    STREQB  R12, [R4]
    LDREQ   R4, =HammerThrowingDisabledByEndOfControlLock
    MOVEQ   R5, #2 @ because this runs 2 times per frame
    STREQB  R5, [R4]
    LDMFD   SP!, {R2-R5}
    BX      LR
    
nsub_02044444_main:         @ Remove touchscreen control from player
    LDR     R1, =TouchscrControlLock
    LDRB    R1, [R1]
    CMP     R1, #0
    BXNE    LR
    STMFD   SP!, {R4-R10,LR}
    B       0x02044448  
    
repl_020069B4_main:         @ Spawn two players if this is a special level
    LDR     R2, =IsSpecialLevel
    LDRB    R2, [R2]
    CMP     R2, #0
    MOVNE   R2, #3 
    STRNE   R2, [R13, #0x28] 
    LDREQB  R2, [R13, #0x28]
    BX      LR

hook_02108E14:              @ Force same zone for two players if this is a special level 
    LDR     R3, =IsSpecialLevel
    LDRB    R3, [R3]
    CMP     R3, #0
    BXEQ    LR
    MOV     R3, #0  
    STRB    R3, [R1, #0x1E] 
    BX      LR
    
repl_020A2764_ov_00:        @ No timer on special level
    LDR     R3, =IsSpecialLevel 
    LDRB    R3, [R3]
    CMP     R3, #0
    BXNE    LR
    SUBEQ   R2, R2, #92
    BX      LR
    
nsub_020BF0C8_ov_00:        @ Disable bottom screen stuff on special level
    LDR     R1, =IsSpecialLevel
    LDRB    R1, [R1]
    CMP     R1, #0
    BXNE    LR
    STMFD   SP!, {R4,LR}
    B       0x20BF0CC
    
repl_0204ED94_main:         @ Load sound set 2E during titlescreens.
    LDR     R4, =TitleScrNumber
    LDR     R4, [R4]
    CMP     R4, #0  
    LDR     R1, [R1]
    MOVNE   R0, #0x2E
    BX      LR
    
nsub_020CE35C_ov_09:        @ Killing most of original TS oncreate
    STMFD   SP!, {R4-R6,LR}
    MOV     R5, R0
    ADD     R0, R5, #0xE8
    MOV     R4, R1
    BL      0x020CC6B4
    MOV     R0, R5
    BL      0x020CDD50
    BL      0x020D8978
    LDMFD   SP!, {R4-R6,PC}
    
nsub_020D339C_ov_09:        @ Hacky scene change to new TS
    STMFD   SP!, {LR}
    MOV     R0, #100
    MOV     R1, #100
    BL      causeSceneChange
    LDMFD   SP!, {PC}
    
repl_020D35C8_ov_09:        @ Killing original TS sound
repl_020D3798_ov_09:
    BX      LR
    
nsub_02013BA8_main:         @ Killing circle transition
    LDR     R12, =IsSpecialLevel
    LDRB    R12, [R12]
    CMP     R12, #0     
    BXNE    LR
    STMFD   SP!, {LR}
    B       0x02013BAC

nsub_020CF7BC_ov_09:        @ Killing original TS draw
    BX      LR
