repl_02133320_ov_11:
	LDR    	R0, [R5, #0x8]			
	ANDS    R0, #0xF
	LDREQ   R2, =0x2133964
	LDRNE 	R2, =MyPhysics
	BX 		LR

repl_02133350_ov_11:
	MOV     R1, #0x6000   
	BX		LR

repl_0213328C_ov_11:
	STMFD   SP!, {R0-R8,LR}
	MOV     R4, R0
	MOV     R7, #0					@ Y loop iterator
	MOV     R8, #0					@ X loop iterator

.NextOffset:	
	LDR    	R0, [R4, #0x60]			@ Load in X position
	LSR    	R0, #0xC
	SUB     R0, #0x8
	MOV     R2, #0x10				
	MUL     R2, R8					@ Calculate X breaking position offset = [tile] * [X loop iterator]	
	ADD     R0, R2, R0				@ Add offset to X position 

	LDR    	R1, [R4, #0x64]			@ Load in Y position
	MVN    	R1, R1
	LSR    	R1, #0xC
	MOV     R2, #0x10					
	MUL     R2, R7					@ Calculate Y breaking position offset = [tile] * [Y loop iterator]
	ADD     R1, R2					@ Add offset to Y position   
	
.TileChecking:
	STMFD   SP!, {R0,R1}
	BL     	getTileBehaviorAtPos2
	
	MOV    	R5, R0	
	LSR    	R0, #0x10
	AND    	R0, R0, #0xF
	CMP   	R0, #0x8				@ If explodable
	BEQ     .DetermineTile
	
	CMP   	R0, #0x5				@ If question block
	LDREQ   R1, =Particle
	MOVEQ   R0, #0x3					
	STREQB  R0, [R1]
	LDMEQFD SP!, {R0,R1} 
	BEQ     .Break
	
	MOV    	R0, R5
	LSR    	R0, #0x14
	AND    	R0, R0, #0xF
	CMP    	R0, #0x1				@ If brick
	LDREQ   R1, =Particle
	MOVEQ   R0, #0x0				
	STREQB  R0, [R1]
	LDMEQFD SP!, {R0,R1} 
	BEQ     .Break
	
.Next:
	LDMFD 	SP!, {R0,R1} 
	B       .CheckIfDone

.DetermineTile:
	MOV    	R0, R5
	AND    	R0, R0, #0xF
	CMP     R0, #0x1				@ If Stone Block, return
	BEQ     .Next	
	CMP     R0, #0x3				@ If Red Block, return
	BEQ     .Next	
	CMP     R0, #0x0
	MOVEQ   R0, #4					@ If used block, particle = 4, else particle = whatever is in the last nybble
	LDR     R1, =Particle			
	STRB    R0, [R1]
	LDMFD 	SP!, {R0,R1} 
	
.Break:	
	STMFD   SP!, {R0,R1}
	MOV     R2, R1					@ Move the already calculated Y tile position to Register 2
	MOV     R1, R0					@ Move the already calculated X tile position to Register 1
	LDR     R0, =ptrToLevelActor
	LDR     R0, [R0]
	MOV     R3, #0x0				@ Spawning tile 0 (empty)
	BL     	changeTileAny
	LDMFD   SP!, {R0,R1}
	
.SpawnParticles:
	MOV     R6, R4
	SUB     SP, SP, #8
	MOV     R2, R0					@ Move the already calculated X tile position to Register 2	
	MOV     R3, R1					@ Move the already calculated Y tile position to Register 3
	LSL     R2, R2, #0xC			@ Undo the LSR, since createActor uses Vec3 values directly
	LSL     R3, R3, #0xC				
	MVN     R3, R3					@ Undo the logical NOT, since createActor uses Vec3 values directly
	LDR		R0, =Vec3_vtable					
	STR     R2, [R0, #0x4] 			@ Store precalculated X position
	ADD     R3, #0x10000			@ Add a tile to precalculated Y position (looks better)
	STR     R3, [R0, #0x8] 			@ Store precalculated Y position + tile
	LDR     R1, [R4, #0x68]			@ Load Z position
	STR     R1, [R0, #0xC] 			@ Store Z position		
	LDR 	R1, =0x00000030			@ Spritedata
	LDR     R2, =Particle					
	LDRB    R2, [R2]
	LSL     R2, #0x14				@ Load the precalculated particle type and shift it so it gets placed into the proper nybble
	ADD     R1, R2, R1
	MOV     R2, R0	
	LDR 	R0, =0xF1				@ SpriteID
	MOV 	R3, #0x0
	MOV 	R4, #0x0
	MOV 	R5, #0x0
	STR     R5, [SP, #0x0]
	STR     R5, [SP, #0x4]			
	BL  	createActor
	ADD     SP, SP, #8
	LDR 	R0, =0x177				@ Play the regular tile breaking sound.
	MOV 	R1, #0x0
	BL  	PlaySNDEffect
	MOV     R4, R6					@ Restore sprite address to R4.	
	
.CheckIfDone:	
	CMP     R7, #7					@ If Y iterator has not reached 7
	ADDLT   R7, #1					@ add 1 to iterator
	BLT     .NextOffset				@ and loop back
	
	CMP     R8, #0					@ If X iterator is 0
	ADDEQ   R8, #1					@ Add 1 to it	
	MOVEQ   R7, #0					@ Reset Y iterator		
	BEQ    .NextOffset				@ And loop back
	
.Return:	
	LDMFD   SP!, {R0-R8,LR}
	MOV     R4, R0
	BX      LR
	
.data
.balign 4
Particle:
	.byte = 0x0
.balign 4
MyPhysics:
    .word 	0x00000000                
    .word  	0xFFFA0000              
    .word  	0x00004000              
    .word  	0x00060000              
    .word   0x00330004                  
    .word   0x00000000                      
    .word   EnemyKillingCollisionCallback