repl_021775B8_ov_44:                    @ Heap
    LDR     R0, =0x588
    BX      LR
    
nsub_02176780_ov_44:                    @ LoadResource
    STMFD   SP!, {LR}
    SUB     SP, SP, #4
    MOV     R0, #0x590
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x591
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x59C
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x59A
    MOV     R1, #0
    BL      loadFileByExtId_3dModel 
    LDR     R0, =0x58F
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    LDR     R0, =0x59B
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    MOV     R0, #1
    ADD     SP, SP, #4
    LDMFD   SP!, {PC}   
    
repl_02176700_ov_44:                    @ Use the body model
    LDR     R0, [R5,#8]
    LSR     R0, #8
    ANDS    R0, R0,#0xF
    LDREQ   R0, =0x590  
    LDRGT   R0, =0x59C
    BX      LR

repl_0217670C_ov_44:                    @ Use the shell model
    LDR     R0, [R5,#8]
    LSR     R0, #8
    ANDS    R0, R0,#0xF
    LDREQ   R0, =0x58F  
    LDRGT   R0, =0x59B
    BX      LR
    
repl_02176740_ov_44:                    @ Use the animation
    LDR     R0, [R5,#8]
    LSR     R0, #8
    ANDS    R0, R0,#0xF
    LDREQ   R0, =0x591  
    LDRGT   R0, =0x59A
    BX      LR
    
repl_021764B8_ov_44:                    @ Initial resize
    LDR     R1, [R4,#8]
    LSR     R1, #4
    ANDS    R1, R1, #0xF
    MOVEQ   R1, #1
    MOV     R0, #0x35                   @ Set up draw distance
    MUL     R0, R1
    STR     R0, [R4,#0x370]
    STR     R0, [R4,#0x374]
    STR     R0, [R4,#0x37C] 
    STR     R0, [R4,#0x380]
    MOV     R0, #0x1000
    ADD     R1, #1
    STRB    R1, [R4,#0x584]             @ Set up number of fireballs required to kill
    SUB     R1, #1
    MUL     R1, R0
    MOV     R0, #0x2
    STRB    R0, [R4,#0x585]             @ Set up pipe generator mode timer
    BX      LR
    
repl_02176568_ov_44:                    @ Setup tile collisions
    STMFD   SP!, {R0, R1, R7, LR}
    LDR     R7, [R4,#0x8]
    LSR     R7, #4
    ANDS    R7, R7, #0xF
    MOVEQ   R7, #1
    LDR     R1, [R4,#0x8]
    LSR     R1, #8
    ANDS    R1, R1, #0xF
    LDREQ   R12, =0x02178F98
    LDREQ   R0, =0x6000
    MULEQ   R0, R7
    STREQ   R0, [R12,#0x4]  
    STREQ   R0, [R12,#0x8]
    LDRNE   R12, =KoopaTileCols
    LDRNE   R0, =0x6000
    MULNE   R0, R7
    STRNE   R0, [R12,#0x4]  
    LDRNE   R0, =0x5000
    MULNE   R0, R7
    STRNE   R0, [R12,#0x8]  
    LDMFD   SP!, {R0, R1, R7, PC}

repl_021765C8_ov_44:                    @ Setup Active Physics
    LDR     R7, [R4,#0x8]
    LSR     R7, #4
    ANDS    R7, R7, #0xF
    MOVEQ   R7, #1
    
    LDR     R0, [R4,#0x8]
    LSR     R0, #8
    ANDS    R0, R0, #0xF
    BEQ     .Buzzy
    
    STMFD   SP!, {R0-R5}
    LDR     R0, =0x2178FB4
    LDR     R1, =0x0000A000
    LDR     R2, =0x00006000
    LDR     R3, =0x00009800 
    LDR     R4, =0x00F30004 
    LDR     R5, =0x0000FFFE 
    MUL     R1, R7
    MUL     R2, R7
    MUL     R3, R7
    STMIA   R0, {R0, R1, R2, R3, R4, R5}
    MOV     LR, R0
    LDMFD   SP!, {R0-R5}
    B       0x021765CC
    
.Buzzy:
    STMFD   SP!, {R0-R5}
    LDR     R0, =0x2178FB4
    LDR     R1, =0x00007800 
    LDR     R2, =0x00008000 
    LDR     R3, =0x00007800 
    LDR     R4, =0x00F30004  
    LDR     R5, =0x0000FFFC 
    MUL     R1, R7
    MUL     R2, R7
    MUL     R3, R7  
    STMIA   R0, {R0, R1, R2, R3, R4, R5}
    MOV     LR, R0
    LDMFD   SP!, {R0-R5}
    B       0x021765CC

repl_0217692C_ov_44:                @ Resize the Koopa shell and change collisions when stomped
repl_02175BA8_ov_44:
repl_02176650_ov_44:
repl_02176D10_ov_44:
    STMFD   SP!, {R0-R4,R7}
    LDR     R7, [R4,#0x8]
    LSR     R7, #4
    ANDS    R7, R7, #0xF    
    MOVEQ   R7, #1
    LDR     R0, [R4,#0x8]
    LSR     R0, #8
    ANDS    R0, R0, #0xF
    BEQ     .noshellresize  
    LDR     R0, =0x1800
    MUL     R0, R7
    STR     R0, [R4, #0xF0]
    STR     R0, [R4, #0xF4]
    STR     R0, [R4, #0xF8]
    
.noshellresize: 
    ADD     R0, R4, #0x134
    LDR     R1, =0x00000000 
    LDR     R2, =0x00006800
    LDR     R3, =0x00007000
    LDR     R4, =0x00006800
    MUL     R2, R7
    MUL     R3, R7
    MUL     R4, R7
    STMIA   R0, {R1,R2,R3,R4}
    LDMFD   SP!, {R0-R4,R7}
    MOV     R0, #2
    BX      LR
    
repl_02177298_ov_44:                @ Resize the Koopa shell and change collisions when sliding
    STMFD   SP!, {R0-R4,R7}
    LDR     R7, [R4,#0x8]
    LSR     R7, #4
    ANDS    R7, R7, #0xF    
    MOVEQ   R7, #1
    LDR     R0, [R4,#0x8]
    LSR     R0, #8
    ANDS    R0, R0, #0xF
    BEQ     .noshellresize2
    LDR     R0, =0x1800
    MUL     R0, R7
    STR     R0, [R4, #0xF0]
    STR     R0, [R4, #0xF4]
    STR     R0, [R4, #0xF8]
    
.noshellresize2:    
    ADD     R0, R4, #0x134
    LDR     R1, =0x00000000 
    LDR     R2, =0x00006800
    LDR     R3, =0x00007000
    LDR     R4, =0x00006800
    MUL     R2, R7
    MUL     R3, R7
    MUL     R4, R7
    STMIA   R0, {R1,R2,R3,R4}
    LDMFD   SP!, {R0-R4,R7}
    MOV     R2, #2
    BX      LR
    
repl_02177338_ov_44:                @ Resize the Koopa and change collisions when returning back to the walk state
    STMFD   SP!, {R0-R4,R7}
    LDR     R7, [R4,#0x8]
    LSR     R7, #4
    ANDS    R7, R7, #0xF    
    MOVEQ   R7, #1
    LDR     R0, [R4,#0x8]
    LSR     R0, #8
    ANDS    R0, R0, #0xF
    BEQ     .BuzzyWalkCol
    
.KoopaWalkCol:  
    MOV     R0, #0x1000
    MUL     R0, R7 
    STR     R0, [R4, #0xF0]
    STR     R0, [R4, #0xF4]
    STR     R0, [R4, #0xF8]
    ADD     R0, R4, #0x134
    LDR     R1, =0x00000000 
    LDR     R2, =0x0000A000
    LDR     R3, =0x00006000
    LDR     R4, =0x00009800
    MUL     R2, R7
    MUL     R3, R7
    MUL     R4, R7
    STMIA   R0, {R1,R2,R3,R4}
    LDMFD   SP!, {R0-R4,R7}
    MOV     R1, #0
    BX      LR  
    
.BuzzyWalkCol:
    ADD     R0, R4, #0x134
    LDR     R1, =0x00000000 
    LDR     R2, =0x00007800
    LDR     R3, =0x00008000
    LDR     R4, =0x00007800
    MUL     R2, R7
    MUL     R3, R7
    MUL     R4, R7
    STMIA   R0, {R1,R2,R3,R4}
    LDMFD   SP!, {R0-R4,R7}
    MOV     R1, #0
    BX      LR  
    

repl_02175D8C_ov_44:                @ Tilt differently if Koopas
    LDR     R1, [R4,#0x8]
    LSR     R1, #8
    ANDS    R1, R1, #0xF
    LDRSH   R1, [SP,#0x16]
    BEQ     0x2175D90
    LSR     R2, R1, #2
    ADD     R1, R2
    B       0x2175D90
        

repl_021767EC_ov_44:                @ Change animation speed when resized
repl_02176F38_ov_44:    
repl_0217734C_ov_44:
    STMFD   SP!, {R0, R2, LR}
    LDR     R0, [R4,#8]
    LSR     R0, #4
    ANDS    R0, R0,#0xF
    BNE     .Resized
    LDR     R3, [R4,#0x8]
    LSR     R3, #8
    ANDS    R3, R3, #0xF
    CMPEQ   R3, #0x0
    MOVEQ   R3, #0x1C00
    LDMEQFD SP!, {R0, R2, PC}

.Resized:
    LDR     R3, [R4,#0x8]
    LSR     R3, #8
    ANDS    R3, R3, #0xF
    MOVEQ   R3, #0x1C00
    MOVNE   R3, #0x0950
    MOV     R2, #0x90
    MUL     R2, R0
    SUB     R3, R2
    CMP     R3, #0x1C00
    MOVHI   R3, #0x200
    LDMFD   SP!, {R0, R2, PC}
    
    
repl_02176388_ov_44:                @ Prevent picking up of resized shells
    STMFD   SP!, {R7}
    LDR     R7, [R0,#0x8]
    LSR     R7, #4
    AND     R7, R7, #0xF
    CMP     R7, #1
    LDMFD   SP!, {R7}
    BLE     0x20A0048
    BGT     0x209F574
    BX      LR
    
CustomBuzzyBeetleVirt43:
    STMFD   SP!, {LR}
    LDR     R1, [R0,#8]
    MOV     R1, R1, LSR #8
    ANDS    R1, R1, #0xF
    BLEQ    .returnf
    
    LDR     R1, [R0,#8]
    MOV     R1, R1, LSR #4
    AND     R1, R1, #0xF
    CMP     R1, #1
    BLLE    .returnf    
    
    LDRB    R1, [R0,#0x584]
    SUB     R1, R1, #1
    STRB    R1, [R0,#0x584]
    CMP     R1, #0
    BEQ     .dead

    LDR     R0, =0x70
    MOV     R1, #0
    BL      PlaySNDEffect
    LDMFD   SP!, {PC}   

.dead:
    STMFD   SP!, {R0-R6,LR}
    MOV     R6, R0
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
    MOV     R0, R6
    LDMFD   SP!, {R0-R6,LR}
    
.returnf:   
    BL      0x0209D84C
    LDMFD   SP!, {PC}
    
    
repl_021771B4_ov_44:                @ Shell slide breaker code
    STMFD  SP!, {R0-R9,LR}
    MOV     R4, R0
    LDR     R7, [R4,#0x8]           @ If not resized, just use the normal code.
    LSR     R7, #4
    AND     R7, R7, #0xF
    CMP     R7, #1
    BLE    .Return
    SUB     R7, #1
    MOV     R8, R7                  @ R8 = X offset multiplier
    MOV     R7, #0                  @ R7 = Y offset multiplier, loop iterator
    
.NextYOffset:   
    MOV     R9, #0
    LDR     R0, [R4, #0x60]         @ Load in X position
    LSR     R0, #0xC
    MOV     R1, #0x12               
    MUL     R1, R8                  @ Calculate X breaking position offset = [1,5 tile] * [Enemy scale - 1]
    LDRB    R2, [R4,#0x2BD]         @ Load direction 
    CMP     R2, #0              
    ADDEQ   R0, R1                  @ If going right, add offset,
    SUBNE   R0, R1                  @ If going left, subtract offset
    LDR     R1, [R4, #0x64]         @ Load in Y position
    MVN     R1, R1
    LSR     R1, #0xC
    MOV     R2, #0x10                   
    MUL     R2, R7                  @ Calculate Y breaking position offset = [tile] * [loop iterator]
    SUB     R1, R2                  @ Subtract offset from Y position
    SUB     R1, #0xF
    STMFD   SP!, {R0,R1}
    BL      getTileBehaviorAtPos2
    MOV     R3, R0
    LSR     R0, #0x10
    AND     R0, R0, #0xF
    CMP     R0, #0x8
    MOVEQ   R0, R3
    ANDEQ   R0, R0, #0xF            @ If tile is used block
    CMPEQ   R0, #0
    LDMEQFD SP!, {R0,R1}
    BLEQ    .Break                  @ Branch to tile destroyer
    MOV     R0, R3
    LSR     R0, #0x14
    AND     R0, R0, #0xF
    CMP     R0, #0x1                @ If tile at calculated position is a brick
    LDMEQFD SP!, {R0,R1}
    MOVEQ   R9, #1
    BLEQ    .Break                  @ Branch to tile destroyer
    LSR     R3, #0x10
    AND     R3, R3, #0xF
    CMP     R3, #0x2                @ If tile at calculated position is a coin
    LDMEQFD SP!, {R0,R1}
    BLEQ    .GetCoins               @ Collect it
    CMP     R7, R8                  @ If iterator has not reached [Enemy Scale - 1]
    ADDLT   R7, #1                  @ add 1 to iterator
    LDMLTFD SP!, {R0,R1}
    BLT     .NextYOffset            @ and loop back 
    LDMFD   SP!, {R0,R1}    
    BL      .Return

.Break: 
    STMFD   SP!, {R0,R1}
    MOV     R2, R1                  @ Move the already calculated Y tile position to Register 2
    MOV     R1, R0                  @ Move the already calculated X tile position to Register 1
    LDR     R0, =ptrToLevelActor
    LDR     R0, [R0]
    MOV     R3, #0x0                @ Spawning tile 0 (empty)
    BL      changeTileAny
    LDMFD   SP!, {R0,R1}
    
.SpawnParticles:
    MOV     R6, R4
    SUB     SP, SP, #8
    MOV     R2, R1                  @ Move the already calculated Y tile position to Register 2
    MOV     R1, R0                  @ Move the already calculated X tile position to Register 1 
    LSL     R1, R1, #0xC            @ Undo the LSR, since createActor uses Vec3 values directly
    LSL     R2, R2, #0xC                
    MVN     R2, R2                  @ Undo the logical NOT, since createActor uses Vec3 values directly
    ADD     R2, #0x8000             @ Add half a tile to precalculated Y position (looks better)
    LDR     R3, [R4, #0x68]         @ Load Z position
    LDR     R0, =Vec3_vtable        
    STMIA   R0, {R0, R1, R2, R3}
    MOV     R2, R0                  
    CMP     R9, #1
    LDRNE   R1, =0x00600030         @ Spritedata if used block
    LDREQ   R1, =0x00000030         @ Spritedata if brick
    LDR     R0, =0xF1               @ SpriteID
    MOV     R3, #0x0
    MOV     R4, #0x0
    MOV     R5, #0x0    
    STMIA   SP, {R4, R5}    
    BL      createActor
    ADD     SP, SP, #8
    LDR     R0, =0x177              @ Play the regular tile breaking sound.
    LDR     R1, =Vec3_vtable
    BL      PlaySNDEffect
    MOV     R4, R6                  @ Restore sprite address to R4. 
    CMP     R7, R8                  @ If iterator has not reached [Enemy Scale - 1]
    ADDLT   R7, #1                  @ add 1 to iterator
    BLT     .NextYOffset            @ and loop back
    
.Return:    
    LDMFD   SP!, {R0-R9,LR}
    MOV     R4, R0
    BX      LR
    
.GetCoins:  
    MOV     R2, R1                  @ Move the already calculated Y tile position to Register 2
    MOV     R1, R0                  @ Move the already calculated X tile position to Register 1
    LDR     R0, =ptrToLevelActor
    LDR     R0, [R0]
    MOV     R3, #0x0                @ Spawning tile 0 (empty)
    BL      changeTileAny
    MOV     R0, #0
    BL      getCoin
    LDR     R0, =0x16C              @ Play coin sound
    MOV     R1, #0x0
    BL      PlaySNDEffect
    CMP     R7, R8                  @ If iterator has not reached [Enemy Scale - 1]
    ADDLT   R7, #1                  @ add 1 to iterator
    BLT     .NextYOffset            @ and loop back
    B       .Return
    
repl_02177438_ov_44:                @Pipe generator mode
    LDR     R0, [R4,#0x8]
    LSR     R0, #0xC
    AND     R0, R0, #0xF
    CMP     R0, #0
    MOVEQ   R0, #1
    BXEQ    LR

    LDRB    R0, [R4,#0x585]
    CMP     R0, #0
    MOVEQ   R0, #1
    BXEQ    LR
    
    LDR     R0, [R4,#0xD4]
    CMP     R0, #0
    BEQ     .PGenMove
    BNE     .DontMove
    
.PGenMove:
    LDRB    R0, [R4,#0x585]
    SUB     R0, #1
    STRB    R0, [R4,#0x585]
    LDRB    R0, [R4,#0x2BD]         
    CMP     R0, #0  
    MOV     R0, #0x900
    MVNNE   R0, R0
    STR     R0, [R4,#0xD0]
    MOV     R0, #1
    BX      LR
    
.DontMove:
    MOV     R0, #0x0
    STR     R0, [R4,#0xD0]
    MOV     R0, #1
    BX      LR

.data
.balign 4
KoopaTileCols: 
    .word   0x00000000 
    .word   0x00006000 
    .word   0x00005000 
    .word   0x00000001
    .word   0xFFFFD000
    .word   0x0000C000
    .word   0x00000000
