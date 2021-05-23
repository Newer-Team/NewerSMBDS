repl_021790C8_ov_47:                    @ Load the models
    STMFD   SP!, {LR}
    LDR     R0, =0x02088BFC
    LDR     R0, [R0]
    CMP     R0, #6  
    BEQ     .world7only
    LDR     R0, =0x9F6                  @ Goomba model
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    MOV     R0, #1
    LDR     R0, =0x582                  @ Goomba animation
    MOV     R1, #0
    BL      loadFileByExtId_3dModel 
    B       .exit
    .world7only:
    LDR     R0, =0x9FC                  @ Shy Guy
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x9FD                  @ Shy Guy animation
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x9FE                  @ Fly Guy
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x9FF                  @ Fly Guy animation
    MOV     R1, #0
    BL      loadFileByExtId_3dModel 
    .exit:
    MOV     R0, #1
    LDMFD   SP!, {PC}
    
repl_02178FD0_ov_47:                    @ Set model depending on type
    LDR     R0, [R5,#8]
    MOV     R0, R0, LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    LDREQ   R0, =0x565
    CMP     R0, #1
    LDREQ   R0, =0x9FC
    CMP     R0, #2
    LDREQ   R0, =0x9F6
    CMP     R0, #3
    LDREQ   R0, =0x9FE
    BX      LR
    
repl_02178FDC_ov_47:                    @ Set different animation depending on type
    LDR     R0, [R5,#8]
    MOV     R0, R0, LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    LDREQ   R0, =0x564
    CMP     R0, #1
    LDREQ   R0, =0x9FD
    CMP     R0, #2
    LDREQ   R0, =0x582
    CMP     R0, #3
    LDREQ   R0, =0x9FF
    BX      LR
    
repl_02178ADC_ov_47:                    @ Set different sound upon death depending on type
    LDR     R0, [R6,#8]
    MOV     R0, R0, LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    MOVEQ   R0, #0xA4
    CMP     R0, #1
    MOVEQ   R0, #0x70   
    CMP     R0, #2
    MOVEQ   R0, #0xA4   
    CMP     R0, #3
    MOVEQ   R0, #0x70   
    BX      LR

repl_02178A18_ov_47:                    @ Death particle 1
    LDR     R0, [R4,#8]
    MOV     R0, R0, LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    MOVGT   R0, #0xF5 
    MOVEQ   R0, #0x4C 
    BX      LR

repl_02178A30_ov_47:                    @ Death particle 2
    LDR     R0, [R4,#8]
    MOV     R0, R0, LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    MOVGT   R0, #0xF6 
    MOVEQ   R0, #0x4D 
    BX      LR

repl_02178A3C_ov_47:                    @ Death particle 3
    LDR     R0, [R4,#8]
    MOV     R0, R0,LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #0
    MOVGT   R0, #0xF7 
    MOVEQ   R0, #0x4E 
    BX      LR

repl_0217869C_ov_47:                    @ Prevent changing of palette on Goombas and Shy Guys. 
    STMFD   SP!, {R0}
    LDR     R0, [R5,#8]
    MOV     R0, R0, LSR#16
    ANDS    R0, R0, #0xF
    LDMNEFD SP!, {R0}
    BXNE    LR
    LDRB    R0, [R5, #0x4C9]
    CMP     R0, #3                      @ Set palette to default if HP is 3
    LDMFD   SP!, {R0}
    LDREQ   R1, =0x021799B8
    LDREQH  R1, [R1]
    STRH    R1, [R0,#0x1C]
    BX      LR
    
repl_02178708_ov_47:                    @ Set HP
    LDR     R3, [R5,#8]
    MOV     R3, R3, LSR#16
    AND     R3, R3, #0xF
    CMP     R3, #0
    MOVNE   R1, #1
    BXNE    LR
    LDR     R3, [R5,#8]
    MOV     R3, R3, LSR#4
    AND     R3, R3, #0xF
    CMP     R3, #0x0
    MOVEQ   R1, #2
    CMP     R3, #0x1
    MOVGE   R1, #3
    BX      LR

repl_02178800_ov_47:                    @ Set fireball hit counter
    LDR     R0, [R4,#8]
    MOV     R0, R0, LSR#4
    AND     R0, R0, #0xF
    ADD     R0, R0, #1
    STR     R0, [R4,#0x4D8]
    MOV     R0, #1
    BX      LR
    
repl_021786EC_ov_47:                    @ Set starting scale according to nybble 11
    STMFD   SP!, {R0,LR}
    LDR     R0, [R4,#8]
    MOV     R0, R0, LSR#4
    AND     R0, R0, #0xF
    CMP     R0, #0
    ADDEQ   R0, #1
    MOV     R1, #0x1200
    MULGT   R1, R0
    LDMFD   SP!, {R0,PC}    
    

repl_02178658_ov_47:                    @ Set walking scale according to nybble 11
    STMFD   SP!, {R1,LR}
    LDR     R1, [R4,#8]
    MOV     R1, R1,LSR#4
    AND     R1, R1, #0xF
    CMP     R1, #0
    ADDEQ   R1, #1
    MOV     R0, #0x1200
    MULGT   R0, R1, R0
    LDMFD   SP!, {R1,PC}
    
nsub_021785B8_ov_47:                    @ Skip some nonsense
    BX      LR

repl_021787C0_ov_47:                    @ Set up collisions
    STMFD   SP!, {R0,R1,LR}
    LDR     R1, [R4,#8]
    MOV     R1, R1, LSR#4
    ANDS    R1, R1, #0xF
    ADDEQ   R1, R1, #1
    LDR     R2, =0x21792D4
    MOV     R0, #0x8000
    MUL     R0, R1
    STR     R0, [R2, #0x4]
    STR     R0, [R2, #0x8]
    STR     R0, [R2, #0xC]

    MOV     R0, #0x20                   @ Set up draw distance
    MUL     R0, R1
    STR     R0, [R4,#0x370]
    STR     R0, [R4,#0x374]
    STR     R0, [R4,#0x37C] 
    STR     R0, [R4,#0x380]
    LDMFD   SP!, {R0,R1,PC}
    
repl_0217879C_ov_47:                    @ Set up tile collisions
    STMFD   SP!, {R1-R3,LR}
    LDR     R1, [R4,#8]
    MOV     R1, R1,LSR#4
    AND     R1, R1, #0xF
    CMP     R1, #0
    ADDEQ   R1, R1, #1
    LDR     R0, =MyTileCollisions
    MOV     R2, #0x8000
    MUL     R2, R1, R2
    STR     R2, [R0, #0x4]
    STR     R2, [R0, #0x8]
    LDMFD   SP!, {R1-R3,PC}

repl_02178770_ov_47:                    @ Change animation speed depending on scale (bigger = slower)
    LDR     R0, [R4,#8]
    MOV     R0, R0, LSR#4
    AND     R0, R0, #0xF
    MOV     R0, R0, LSL#9
    MOV     R1, #0x1000
    SUB     R1, R1, R0
    CMP     R1, #0x1000
    MOVHI   R1, #0x200
    CMP     R1, #0x100
    MOVLS   R1, #0x200
    BX      LR

repl_02178A44_ov_47:                    @ Spawn Goombas if sprite is Goomba and resized.
    LDR     R0, [R4,#8]
    MOV     R0, R0,LSR#16
    AND     R0, R0, #0xF
    CMP     R0, #2
    MOVNE   R0, R4
    BXNE    LR
    LDR     R0, [R4,#8]
    MOV     R0, R0,LSR#4
    AND     R0, R0, #0xF
    CMP     R0, #1
    MOVLE   R0, R4
    BXLE    LR
    CMP     R0, #2
    BEQ     .SpawnGoomba
    SUB     R0, R0, #1
    MOV     R6, R0
    MOV     R0, #2

.SpawnMegaGoomba:
    STMFD   SP!, {R0-R4,LR}
    SUB     SP, SP, #8
    MOV     R1, #0x20000
    ADD     R1, R1, R6,LSL#4
    LDR     R2, [R4,#0x60]
    MOV     R3, R0
    ANDS    R3, R3, #1
    ADDEQ   R2, R2, R0,LSL#16
    SUBEQ   R2, R2, #0x30000
    SUBNE   R2, R2, R0,LSL#16
    ADDNE   R2, R2, #0x30000
    STR     R2, [R4,#0x60]
    LDR     R2, [R4,#0x64]
    ADD     R2, R2, #0x2000
    STR     R2, [R4,#0x64]
    LDR     R2, [R4,#0x68]
    ADD     R2, R2, R0,LSL#17
    STR     R2, [R4,#0x68]
    ADD     R2, R4, #0x5C       
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
    STR     R5, [SP]
    STR     R5, [SP,#4]
    MOV     R0, #0x7F 
    BL      createActor
    LDR     R1, [R0,#0xD4]
    ADD     R1, R1, #0x2000
    STR     R1, [R0,#0xD4]
    LDR     R1, [R0,#0x64]
    SUB     R2, R2, #0x2000
    STR     R1, [R0,#0x64]
    ADD     SP, SP, #8
    LDMFD   SP!, {R0-R4,LR}
    SUB     R0, R0, #1
    CMP     R0, #0
    BNE     .SpawnMegaGoomba
    BEQ     .Next

.SpawnGoomba:           
    STMFD   SP!, {R0-R4,LR}
    SUB     SP, SP, #8
    MOV     R6, R0
    MOV     R1, #1
    LDR     R2, [R4,#0x60]
    MOV     R3, R0
    ANDS    R3, R3, #1
    ADDEQ   R2, R2, R0,LSL#16
    SUBEQ   R2, R2, #0x30000
    SUBNE   R2, R2, R0,LSL#16
    ADDNE   R2, R2, #0x30000
    STR     R2, [R4,#0x60]
    LDR     R2, [R4,#0x64]
    ADD     R2, R2, #0x5000
    STR     R2, [R4,#0x64]
    ADD     R2, R4, #0x5C
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
    STR     R5, [SP]
    STR     R5, [SP,#4]
    MOV     R0, #0x53 
    BL      createActor
    MOV     R3, R6
    ANDS    R3, R3, #1
    LDR     R1, [R0,#0xD0]
    MOV     R2, #0x1500
    ADDNE   R1, R1, R2
    RSBEQ   R1, R2, #0
    STR     R1, [R0,#0xD0]
    LDR     R1, [R0,#0xD4]
    ADD     R1, R1, #0x2000
    STR     R1, [R0,#0xD4]
    LDR     R1, [R0,#0x64]
    SUB     R2, R2, #0x5000
    STR     R1, [R0,#0x64]
    ADD     SP, SP, #8
    LDMFD   SP!, {R0-R4,LR}
    SUB     R0, R0, #1
    CMP     R0, #0
    BNE     .SpawnGoomba

.Next:
    LDR     R0, [R4,#8]
    MOV     R0, R0,LSR#4
    AND     R0, R0, #0xF
    
    STMFD   SP!, {R4, LR}
    MOV     R0, #0x1E
    LDR     R1, =Vec3_vtable
    LDR     R2, [R4,#0x60]
    LDR     R3, [R4,#0x64]
    LDR     R4, [R4,#0x68]
    SUB     R2, R2, #0x10000
    STMIA   R1, {R1, R2, R3, R4}
    BL      SpawnParticle
    
    MOV     R0, #0x1F
    LDR     R1, =Vec3_vtable
    BL      SpawnParticle
    
    MOV     R0, #0x14
    LDR     R1, =Vec3_vtable
    LDR     R2, [R1, #8]
    ADD     R2, R2, #0x10000
    STR     R2, [R1, #8]
    BL      SpawnParticle
    
    MOV     R0, #0x6F
    LDR     R1, =Vec3_vtable
    BL      PlaySNDEffect
    LDR     R0, =0x14D
    LDR     R1, =Vec3_vtable
    BL      PlaySNDEffect
    
    LDMFD   SP!, {R4, LR}
    MOV     R0, R4
    BX      LR

repl_021790E8_ov_47:
    LDR     R0, =0x4DC
    BX      LR

CustomSplunkinVirt43:                       
    STMFD   SP!, {LR}
    LDR     R5, [R0,#8]
    MOV     R5, R5, LSR #4
    AND     R5, R5, #0xF
    CMP     R5, #1
    BLE     .returnf
    LDR     R5, [R0,#8]
    MOV     R5, R5, LSR #16
    AND     R5, R5, #0xF
    CMP     R5, #1
    BEQ     .returnf
    LDR     R1, [R0,#0x4D8]
    SUB     R1, R1, #1
    STR     R1, [R0,#0x4D8]
    CMP     R1, #0
    BEQ     .dead
    
    STMFD   SP!, {LR}
    LDR     R0, =0x70
    MOV     R1, #0
    BL      PlaySNDEffect
    LDMFD   SP!, {LR,PC}

.dead:
    MOV     R6, R0
    STMFD   SP!, {R0-R5,LR}
    SUB     SP, SP, #8
    MOV     R1, #5
    LDR     R2, [R0,#0x60]
    SUB     R2, R2, #0x10000
    STR     R2, [R0,#0x60]
    ADD     R2, R0, #0x5C
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
    STR     R5, [SP]
    STR     R5, [SP, #0x4]
    MOV     R0, #0x42
    BL      createActor
    MOV     R1, #5
    LDR     R2, [R6,#0x60]
    ADD     R2, R2, #0x10000
    STR     R2, [R6,#0x60]
    ADD     R2, R6, #0x5C
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
    STR     R5, [SP]
    STR     R5, [SP,#0x4]
    MOV     R0, #0x42
    BL      createActor
    ADD     SP, SP, #8
    LDMFD   SP!, {R0-R5,LR}

.returnf:   
    BL      0x209D84C
    LDMFD   SP!, {PC}

CustomSplunkinVirt50:           
    STMFD   SP!, {LR}
    LDR     R5, [R0,#8]
    MOV     R5, R5,LSR#16
    AND     R5, R5, #0xF
    CMP     R5, #2
    BNE     .return
    LDR     R6, [R0,#8]
    MOV     R6, R6, LSR#4
    AND     R6, R6, #0xF
    CMP     R6, #1
    BEQ     .return
    CMP     R6, #2
    ADDGT   R6, R6, #1
    MOV     R4, R0

.SpawnGPGoombas:
    STMFD   SP!, {R0-R4,LR}
    SUB     SP, SP, #8
    MOV     R1, #1
    LDR     R2, [R4,#0x60]
    MOV     R3, R6
    ANDS    R3, R3, #1
    SUBEQ   R2, R2, #0x15000
    ADDNE   R2, R2, #0x25000
    STR     R2, [R4,#0x60]
    LDR     R2, [R4,#0x64]
    ADD     R2, R2, #0x5000
    STR     R2, [R4,#0x64]
    ADD     R2, R4, #0x5C
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
    STR     R5, [SP]
    STR     R5, [SP,#0x4]
    MOV     R0, #0x53
    BL      createActor
    MOV     R3, R6
    ANDS    R3, R3, #1
    LDR     R1, [R0,#0xD0]
    MOV     R2, #0x1000
    ADDNE   R1, R1, R2
    RSBEQ   R1, R2, #0
    STR     R1, [R0,#0xD0]
    LDR     R1, [R0,#0xD4]
    ADD     R1, R1, #0x4000
    STR     R1, [R0,#0xD4]
    LDR     R1, [R0,#0x64]
    SUB     R2, R2, #0x5000
    STR     R1, [R0,#0x64]
    ADD     SP, SP, #8
    LDMFD   SP!, {R0-R4,LR}
    SUB     R6, R6, #1
    CMP     R6, #0
    BNE     .SpawnGPGoombas

    STMFD   SP!, {R0-R5,LR}
    LDR     R1, =Vec3_vtable
    LDR     R2, [R4,#0x60]
    SUB     R2, R2, #0x10000
    LDR     R3, [R4,#0x64]
    LDR     R5, [R4,#0x68]
    STMIA   R1, {R1, R2, R3, R5}
    LDR     R0, =0x151
    BL      PlaySNDEffect
    MOV     R0, #0x1E
    LDR     R1, =Vec3_vtable
    BL      SpawnParticle
    LDMFD   SP!, {R0-R5,LR}

.return:
    BL      0x0209CE08
    LDMFD   SP!, {PC}

.data
.balign 4
MyTileCollisions:
    .word 0x00000000
    .word 0x00008000
    .word 0x00008000