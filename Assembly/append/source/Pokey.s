nsub_0213DBA8_ov_17:					@ Model Loading
	STMFD   SP!, {LR}
	SUB     SP, SP, #4									
	LDR     R0, =0x5CC					@ Snow Pokey Head
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x5CB					@ Snow Pokey Body
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9B3					@ Bomb Pokey Head
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9B4					@ Bomb Pokey Body
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9B5					@ Sand Pokey Head
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9B6					@ Sand Pokey Body
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}

repl_0213DB14_ov_17:					@ Using the head model
	LDR 	R0, [R5, #8]
	LSR     R0, R0, #8
	AND 	R0, R0, #0xF
	CMP     R0, #1
	LDRLT   R0, =0x5CC 
	LDREQ	R0, =0x9B6
	LDRGT   R0, =0x9B4
	BX 		LR

repl_0213DB54_ov_17:					@ Using the body model
	LDR 	R7, [R5, #8]
	LSR     R7, R7, #8
	AND 	R7, R7, #0xF
	CMP     R7, #1
	LDRLT   R7, =0x5CB 
	LDREQ   R7, =0x9B5
	LDRGT   R7, =0x9B3
	BX 		LR
	
repl_0213C8D4_ov_17:					@ Check if Bomb Pokey. If so, if hit with fireball, explode.
	STMFD   SP!, {R0}
	LDR 	R0, [R4, #8]
	LSR     R0, R0, #8
	AND 	R0, R0, #0xF
	CMP     R0, #2
	LDMFD   SP!, {R0}
	BEQ		.Selfsplode
	BNE 	0x213C8F8
	BX      LR   
	
.Selfsplode:							
	STMFD 	SP!, {R0-R9,LR}
	LDR     R0, [R4,#0x8]
	AND     R0, R0, #0xF				@ Get Pokey Segments number.
	ADD     R0, #2						@ Adding 2, since Pokey has 2 segments by default.
	LDR     R1, =Tempstack				@ Save Pokey Segments number.
	STRB    R0, [R1]
	
.ExplodeLoop:							@ Spawn the explosion particles. 
	LDR 	R2, [R4, #0x60]				@ Load the X position.
	LDR 	R3, [R4, #0x64]				@ Load the Y position.
	LDR 	R5, [R4, #0x68]
	LDR 	R0, =Tempstack
	LDRB 	R0, [R0]
	LDR 	R1, =0x10000
	SUB     R3, R1
	MUL 	R1, R0						@ Multiply Pokey Segment number by tile
	ADD     R3, R1						@ Add result to Y position
	LDR     R1, =Vec3_vtable
	STMIA   R1, {R1, R2, R3, R5}
	MOV 	R0, #0x16
	BL  	SpawnParticle	
	LDR 	R0, =0x69
	LDR     R1, =Vec3_vtable
	BL  	PlaySNDEffect
	LDR     R1, =Tempstack
	LDRB    R0, [R1]
	SUB		R0, #1						@ Decrement Pokey Segment number
	STRB    R0, [R1]
	CMP     R0, #0x0				
	BGT     .ExplodeLoop				@ If Pokey Segment number hasn't reached 0 yet, go back into loop
	
@Tile Breaking Begins Here
	LDR     R2, [R4,#0x8]				@ Loading amount of Pokey segments
	AND     R2, R2, #0xF
	ADD     R2, #4						@ Adding 4 to it because Pokey is 2 segments by default, and we want to break a tile above and below it
	LDR     R1, =Tempstack
	STRB    R2, [R1]
	LDR     R1, =Xoffs
	MOV     R0, #0x0					@ Resetting X offset to 0
	STRB    R0, [R1]

.BreakingLoop:
	LDR    	R1, =Tempstack
	LDRB   	R2, [R1]
	MOV    	R3, #0x10	
	LDR    	R0, [R4, #0x60]				@ Loading X position
	LSR    	R0, #0xC
	SUB    	R0, R0, #0x20				@ Substracting 2 tiles from X position, since we want to break two tiles on each side
	LDR    	R1, =Xoffs
	LDRB   	R1, [R1]
	ADD    	R0, R0, R1					@ Adding X position offset
	LDR    	R1, [R4, #0x64]				@ Loading Y position
	MVN    	R1, R1
	LSR    	R1, #0xC
	ADD    	R1, #0x20					@ Adding two tiles to Y position (i.e starting two tiles lower)
	MUL    	R5, R2, R3					@ Multiplying Pokey segments + 2 by 16 pixels
	SUB    	R1, R1, R5					@ Substracting Y position offset calculated off spritedata
.DetermineTile:
	STMFD   SP!, {R0-R3}
	BL     	getTileBehaviorAtPos2
	BL      ParticlesForTile
	LDR     R5, =Tile
	STRB    R0, [R5]
	CMP     R0, #0xFFFFFFFF
	LDMFD   SP!, {R0-R3}
	BLNE    .Break
.AfterBreak:
	LDR     R1, =Tempstack	
	LDRB    R0, [R1]
	SUB     R0, #1						@ Decrement Y position offset multiplier
	STRB    R0, [R1]		
	CMP     R0, #0			
	BGT     .BreakingLoop				@ If Y position multiplier is higher than 0, go back into loop
	LDR   	R1, =Xoffs	
	LDRB    R0, [R1]
	CMP     R0, #0x40		
	BEQ     .SpawnCoin					@ If X offset has reached the fifth column of tiles, then tile breaking is done
	ADD     R0, R0, #0x10				@ Otherwise, add a tile to X offset
	STRB    R0, [R1]
	LDR     R1, =Tempstack
	LDR     R0, [R4,#0x8]
	AND     R0, R0, #0xF
	ADD     R0, #4						@ ...Reset Y position multiplier back to Pokey segments + 2
	STRB    R0, [R1]
	B		.BreakingLoop				@ ...and go back into the loop
	
.Break:
	STMFD   SP!, {R0,R1}
	MOV     R2, R1						@ Move the already calculated Y tile position to Register 2
	MOV     R1, R0						@ Move the already calculated X tile position to Register 1
	LDR     R0, =ptrToLevelActor
	LDR     R0, [R0]
	MOV     R3, #0x0					@ Spawning tile 0 (empty)
	BL     	changeTileAny
	LDMFD   SP!, {R0,R1}
.SpawnParticles:	
	SUB     SP, SP, #8
	MOV     R6, R4
	MOV     R2, R1						@ Move the already calculated Y tile position to Register 3
	MOV     R1, R0						@ Move the already calculated X tile position to Register 2	
	LSL     R1, R1, #0xC				@ Undo the LSR, since createActor uses Vec3 values directly
	LSL     R2, R2, #0xC				
	MVN     R2, R2						@ Undo the logical NOT, since createActor uses Vec3 values directly
	ADD     R2, #0x10000
	LDR     R3, [R4, #0x68]
	LDR		R0, =Vec3_vtable
	STMIA   R0, {R0, R1, R2, R3}
	LDR 	R1, =0x00000030				@ Nybble 3 = Type, Nybble 7 = Direction.
	LDR     R2, =Tile					
	LDRB    R2, [R2]
	LSL     R2, #0x14					@ Load the precalculated particle type and shift it so it gets placed into the proper nybble
	ADD     R1, R2, R1
	MOV     R2, R0
	MOV 	R0, #0xF1
	MOV 	R3, #0x0
	MOV 	R4, #0x0
	MOV 	R5, #0x0
	STMIA   SP, {R4, R5}
	BL  	createActor
	ADD     SP, SP, #8
	LDR 	R0, =0x177					@ Play the regular tile breaking sound.
	LDR 	R1, =Vec3_vtable
	BL  	PlaySNDEffect
	MOV     R4, R6						@ Restore sprite address to R4.
	B       .AfterBreak
	
.SpawnCoin:
	SUB     SP, SP, #8
	MOV 	R0, R4
	BL  	Base__deleteIt				@ Delete the Pokey
	LDR 	R0, =0x42
	LDR 	R1, =0x00000005				@ Type = "Hops"
	LDR     R2, [R4, #0x60]
	SUB     R2, #0x7000					@ Shift the X spawn position by ~half a tile left
	STR     R2, [R4, #0x60]
	LDR     R2, [R4, #0x64]
	ADD     R2, #0x10000				@ Shift the Y spawn position by a tile upward
	STR     R2, [R4, #0x64]
	ADD 	R4, #0x5C					@ Since the Pokey is gone, we don't need it's pointer anymore
	MOV 	R2, R4
	MOV 	R3, #0x0
	MOV 	R4, #0x0
	MOV 	R5, #0x0
	STR     R5, [SP]	
	STR     R5, [SP, #0x4]				
	BL  	createActor
	ADD     SP, SP, #8
	LDMFD 	SP!, {R0-R9,PC}
	
	
repl_0213C8AC_ov_17:					@ Check if Bomb Pokey. If so, if hit with fireball, spawn many points.		
	STMFD   SP!, {R0}
	LDR 	R0, [R4, #8]			
	LSR     R0, R0, #8
	AND 	R0, R0, #0xF
	CMP     R0, #2
	LDMFD   SP!, {R0}
	BEQ		.SplodePoints
	MOVNE   R0, R4
	BXNE    LR
	
.SplodePoints:							@ Save Pokey Segments number.
	LDR     R0, [R4,#0x8]
	AND     R0, R0, #0xF
	ADD     R0, R0, #2					@ Adding 2 to it, since Pokey has 2 segments by default.
	LDR     R1, =Tempstack
	STRB    R0, [R1]
	
.PointsExplosionLoop:
	MOV     R0, R4
	MOV     R2, #0						@ X Offset	
	MOV     R3, #0x10000				@ Y Offset (1 tile)
	STMFD   SP!, {R0}
	LDR     R0, =Tempstack
	LDRB    R0, [R0]
	MUL     R1, R0, R3					@ Multiply Y offset by number of Pokey segment.
	SUB     R1, #0x10000				@ Decrease result by a tile.
	MOV     R3, R1
	MOV     R1, #0x8					@ Start points chain at 10 points
	ADD     R1, R1, R0					@ Increase it based on amount of Pokey segments
	CMP     R1, #0xA					@ But if it gets above 0xA, reset to 0 (Because 0xA = 1000 and we want 100)
	SUBGE   R1, #0xA
	LDMFD   SP!, {R0}
	STR     R2, [SP]					@ Needs to have 0 in SP?
	BL      GivePointsOfSomeKind
	LDR     R1, =Tempstack
	LDRB    R0, [R1]
	SUB     R0, R0, #1				    @ Decrement number of Pokey segments.	
	STRB    R0, [R1]			
	CMP     R0, #0					
	BNE     .PointsExplosionLoop		@ Go back into loop if Pokey segment number hasn't reached 0 yet.	
	B 		0x213C8C4		
	
repl_0213C8C8_ov_17:					@ Switch the sound played upon killing a Pokey with fireballs if it's a Bomb Pokey
	STMFD   SP!, {R0}	
	LDR 	R0, [R4, #8]
	LSR     R0, R0, #8
	AND 	R0, R0, #0xF
	CMP     R0, #2						
	LDMFD   SP!, {R0}
	LDREQ   R0, =0x14E
	LDRNE   R0, =0x70
	BX      LR
	
repl_0213C888_ov_17:					@ Do not spawn a second coin if Pokey killed by fireball headshot.
	STMFD   SP!, {R0}
	LDR 	R0, [R4, #8]
	LSR     R0, R0, #8
	AND 	R0, R0, #0xF
	CMP     R0, #2
	LDMFD   SP!, {R0}
	BLNE    createActor
	MOVEQ   R0, #0
	B       0x0213C88C
	

.data
.balign 4
Tempstack:
		.byte 0x00
Xoffs:
		.byte 0x00
Tile:
		.byte 0x00