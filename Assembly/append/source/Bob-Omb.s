repl_0214D008_ov_2A:                    @ For Bomb Spike
    LDR     R0, [R4, #8]
    LSR     R0, R0, #8
    ANDS    R0, R0, #0xF
    MOVEQ   R3, #0xF0
    MOVNE   R3, #0x25
    MULNE   R3, R0
    BX      LR

repl_0214DDE8_ov_2A:
    CMP     R0, #0
    BNE     0x214DE00
    LDR     R0, [R4, #8]
    LSR     R0, #8
    ANDS    R0, #0xF
    BNE     0x214DE00
    BX      LR

repl_0214D564_ov_2A:                    @ Explosion hurtbox resize
    LDR     R0, [R4, #8]
    LSR     R0, #4
    ANDS    R0, #0xF
    MOV     R1, #0x12000
    BXEQ    LR
    CMP     R0, #4
    MOVGT   R0, #4  
    ADD     R0, #1
    MUL     R1, R0
    BX      LR

repl_0214CFD4_ov_2A:                    @ Model resize
    LDR     R1, [R4, #8]
    LSR     R1, #4
    ANDS    R1, #0xF
    MOV     R0, #0x1000
    BXEQ    LR
    CMP     R1, #4
    MOVGT   R1, #4
    ADD     R1, #1
    MUL     R0, R1
    BX      LR
    
repl_0214D028_ov_2A:                    @ Draw distance
    LDR     R1, [R4, #8]
    LSR     R1, #4
    AND     R1, #0xF
    CMP     R1, #4
    MOVGT   R1, #4
    MOV     R2, #0x10
    MUL     R2, R1
    BX      LR
    
repl_0214D36C_ov_2A:                    @ Animation speed
repl_0214DAD0_ov_2A:
repl_0214DB00_ov_2A:
repl_0214DB60_ov_2A:
repl_0214DDA0_ov_2A:
repl_0214DF24_ov_2A:
repl_0214E044_ov_2A:
    STMFD   SP!, {R0,R1,LR}
    LDR     R0, [R4, #8]
    LSR     R0, #4
    AND     R0, #0xF
    CMP     R0, #4
    MOVGT   R0, #4
    MOV     R1, #0x1200
    MOV     R3, #0x800
    MOV     R1, #0x100
    MUL     R1, R0
    SUB     R3, R1
    LDMFD   SP!, {R0,R1,PC}

repl_0214D054_ov_2A:                    @ Tile collisions
    STMFD   SP!, {R0, R2, LR}
    LDR     R1, =MyBobOmbTileCols
    LDR     R2, =0x6000
    LDR     R0, [R4, #8]
    LSR     R0, #4
    ANDS    R0, #0xF
    BEQ     .StoreTileCols
    CMP     R0, #4
    MOVGT   R0, #4  
    ADD     R0, #1
    MUL     R2, R0
.StoreTileCols:
    STR     R2, [R1, #0x4]
    STR     R2, [R1, #0x8]
    LDMFD   SP!, {R0, R2, PC}
 
repl_0214D0D8_ov_2A:                    @ Active Physics
repl_0214D12C_ov_2A:
    STMFD   SP!, {R0, R1, LR}
    LDR     R2, =MyBobOmbCols
    LDR     R1, =0x8000
    LDR     R0, [R4, #8]
    LSR     R0, #4
    ANDS    R0, #0xF
    BEQ     .StoreAP
    CMP     R0, #4
    MOVGT   R0, #4  
    ADD     R0, #1
    MUL     R1, R0
.StoreAP:
    STR     R1, [R2, #0x4]
    STR     R1, [R2, #0x8]
    STR     R1, [R2, #0xC]
    LDMFD   SP!, {R0, R1, PC}

repl_0214CD54_ov_2A:                    @ Resized bombs can't be picked up
    STMFD   SP!, {R0}
    LDR     R0, [R6, #8]
    LSR     R0, #4
    ANDS    R0, #0xF
    LDMFD   SP!, {R0}
    BLEQ    0x20A0048
    BLNE    0x209A80C
    B       0x214CD58
    
repl_0214D428_ov_2A:                    @ If resized, spawn more puff particles when exploding
    MOV     R10, R0
    LDR     R9, [R10, #8]
    LSR     R9, #4
    ANDS    R9, #0xF
    BXEQ    LR
    STMFD   SP!, {LR}
    CMP     R9, #4
    MOVGT   R9, #4      
    
.SpawnMoreParticles:
    LDR     R1, =Vec3_vtable
    LDR     R0, [R10, #0x60]
    MOV     R2, #0x10000
    ADD     R0, R2
    MUL     R2, R9
    SUB     R0, R2
    STR     R0, [R1, #4]
    LDR     R0, [R10, #0x64]
    ADD     R0, #0x8000
    STR     R0, [R1, #8]    
    LDR     R0, [R10, #0x68]
    STR     R0, [R1, #0xC]
    
    MOV     R0, #0x17
    BL      SpawnParticle
    
    LDR     R1, =Vec3_vtable
    LDR     R0, [R10, #0x60]
    ADD     R0, #0x10000
    STR     R0, [R1, #4]
    LDR     R0, [R10, #0x64]
    MOV     R2, #0x10000
    MUL     R2, R9
    ADD     R0, R2
    STR     R0, [R1, #8]    
    LDR     R0, [R10, #0x68]
    STR     R0, [R1, #0xC]
    
    MOV     R0, #0x17
    BL      SpawnParticle
    
    LDR     R1, =Vec3_vtable
    LDR     R0, [R10, #0x60]
    MOV     R2, #0x8000
    ADD     R0, R2
    MUL     R2, R9
    ADD     R0, R2
    STR     R0, [R1, #4]
    LDR     R0, [R10, #0x64]
    ADD     R0, #0x8000
    STR     R0, [R1, #8]    
    LDR     R0, [R10, #0x68]
    STR     R0, [R1, #0xC]
    
    MOV     R0, #0x17
    BL      SpawnParticle
    
    SUBS    R9, #1
    LDMEQFD SP!, {PC}
    BNE     .SpawnMoreParticles
    

repl_0214D578_ov_2A:                    @ Explosion tilebreak
    LDR     R7, [R10,#0x8]              @ Loading scale of Bob-omb
    LSR     R7, #4
    AND     R7, R7, #0xF
    CMP     R7, #4
    MOVGT   R7, #4
    MOV     R8, #3                      @ Establishing range
    MOV     R9, #4
    ADD     R8, R7
    ADD     R9, R7
    
.BreakingLoop:
    LDR     R0, [R10, #0x60]            @ Loading X position
    LSR     R0, #0xC
    CMP     R7, #0
    SUBEQ   R0, #0x20
    CMP     R7, #1
    SUBEQ   R0, #0x20
    CMP     R7, #2
    SUBEQ   R0, #0x10
    CMP     R7, #3
    SUBEQ   R0, #0x10
    MOV     R1, #0x10
    MUL     R1, R7
    SUB     R0, R1                      @ Setting start X position
    MOV     R1, #0x10                   @ Adding offset to X position
    MUL     R1, R9      
    ADD     R0, R1
    LDR     R1, [R10, #0x64]            @ Loading Y position
    MVN     R1, R1
    LSR     R1, #0xC
    SUB     R1, #0x20
    MOV     R2, #0x10
    MUL     R2, R7
    SUB     R1, R2                      @ Setting start Y position
    MOV     R2, #0x10
    MUL     R2, R8      
    ADD     R1, R2                      @ Adding offset to Y position
.DetermineTile:
    STMFD   SP!, {R0-R3}
    BL      getTileBehaviorAtPos2
    BL      ParticlesForTile
    LDR     R5, =Tile
    STRB    R0, [R5]
    CMP     R0, #0xFFFFFFFF
    LDMFD   SP!, {R0-R3}
    BLNE    .Break
.AfterBreak:
    SUBS    R9, #1
    BNE     .BreakingLoop
    SUBS    R8, #1
    MOVNE   R9, #4
    ADDNE   R9, R7
    BNE     .BreakingLoop
    MOV     R0, R10
    BL      Base__deleteIt
    B       0x214D70C

.Break:
    STMFD   SP!, {R0,R1}
    MOV     R2, R1                      @ Move the already calculated Y tile position to Register 2
    MOV     R1, R0                      @ Move the already calculated X tile position to Register 1
    LDR     R0, =ptrToLevelActor
    LDR     R0, [R0]
    MOV     R3, #0x0                    @ Spawning tile 0 (empty)
    BL      changeTileAny
    LDMFD   SP!, {R0,R1}
    
.SpawnParticles:    
    SUB     SP, SP, #8
    MOV     R2, R1                      @ Move the already calculated Y tile position to Register 2 
    MOV     R1, R0                      @ Move the already calculated X tile position to Register 1 
    LSL     R1, R1, #0xC                @ Undo the LSR, since createActor uses Vec3 values directly
    LSL     R2, R2, #0xC                
    MVN     R2, R2                      @ Undo the logical NOT, since createActor uses Vec3 values directly
    ADD     R2, #0x10000
    LDR     R3, [R10, #0x68]
    LDR     R0, =Vec3_vtable        
    STMIA   R0, {R0, R1, R2, R3}
    MOV     R6, R10             
    LDR     R1, =0x00000030             @ Nybble 3 = Type, Nybble 7 = Direction.
    LDR     R2, =Tile                   
    LDRB    R2, [R2]
    LSL     R2, #0x14                   @ Load the precalculated particle type and shift it so it gets placed into the proper nybble
    ADD     R1, R2, R1
    MOV     R2, R0
    MOV     R0, #0xF1
    MOV     R3, #0x0
    MOV     R4, #0x0
    MOV     R5, #0x0
    STMIA   SP, {R4, R5}        
    BL      createActor
    ADD     SP, SP, #8
    LDR     R0, =0x177                  @ Play the regular tile breaking sound.
    LDR     R1, =Vec3_vtable
    BL      PlaySNDEffect
    MOV     R10, R6                     @ Restore sprite address to R10.
    B       .AfterBreak

repl_0214D490_ov_2A:                    @ Use a different SFX ID during the final Bowser battle
    LDR     R0, [R10,#8]
    LSR     R0, #24
    AND     R0, #0xF
    CMP     R0, #0
    MOVEQ   R0, #0x69 
    MOVNE   R0, #0xD0   
    BX      LR

.data
.balign 4
MyBobOmbTileCols:
    .word 0x00000000
    .word 0x00006000
    .word 0x00006000
MyBobOmbCols:
    .word 0x00000000
    .word 0x00008000
    .word 0x00008000
    .word 0x00008000
    .word 0x00930004
    .word 0x0000FFFE
    .word 0x02098C78
    
Tile:
    .byte 0x0