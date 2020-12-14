.global doCollision
doCollision:
	STMFD SP!, {R0-R9,LR}
	
	@ Set up same environment as in the shell callback
	MOV		R8, R0
	MOV		R7, R1
	LDR		R6, [R8,#4]
	LDR		R4, [R7,#4]
	
	@ If arg02 == 1, kill other actor
	AND		R2, #1
	MOV		R9, R2
	
	@ Set up some kill Vec2s. Seems like they control the flight curve when dying
	MOV		R0, R6
	MOV		R1, R4
	BL		setKillVecs
	MOV     R5, R0
	
	@ Set up some Vec2s
	ADD     R2, R8, #0x30
	LDR     R0, [R2,#4]
	ADD     R1, R7, #0x30
	STR     R0, [R6,#0x394]
	LDR     R0, [R2,#8]
	STR     R0, [R6,#0x398]
	LDR     R0, [R1,#4]
	STR     R0, [R6,#0x3A0]
	LDR     R0, [R1,#8]
	STR     R0, [R6,#0x3A4]
	
	@ Kill actor and hammer
	ADD     R1, R6, #0x5C
	MOV     R0, #0x70
	BL      PlaySNDEffect
	STRB    R5, [R6,#0x3EE]
	LDR     R1, [R6,#0x3B4]
	EOR     R0, R5, #1
	ORR     R1, R1, #0x40
	STR     R1, [R6,#0x3B4]
	CMP		R9, #1
	STREQB	R0, [R4,#0x3EE]
	ADD		R0, R6, #0x100
	LDRSB	R1, [R0,#0x1E]
	MOV     R0, R4
	STRB    R1, [R0,#0x11E]
	CMP		R9, #1
	LDREQ 	R0, [R4,#0x3B4]
	ORREQ  	R0, R0, #0x40
	STREQ	R0, [R4,#0x3B4]
	LDREQB  R0, [R8,#0xA6]
	ORREQ   R0, R0, #2
	STREQB  R0, [R8,#0xA6]
	LDMFD 	SP!, {R0-R9,PC}

repl_0210EA48_ov_0A:
	MOV     R0, R4
	ADD		R0, R0, #0x700
	ADD		R0, R0, #0x88
	LDRB	R0, [R0]
	CMP		R0, #33
	BEQ		0x210EA64
	CMP		R0, #32
	BEQ		0x210EA64	
	MOV		R0, R4
	B		0x210EA4C
	
repl_020210BC_main:
repl_02020C44_main:
	STMFD	SP!, {R0, R1, R4, LR}
	LDR		R0, =0x9C2
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	LDMFD	SP!, {R0, R1, R4, LR}
	MOV     R0, #1
	BX      LR

	
repl_0209D57C_ov_00:				// Fixes killing sharks
	BL      getPtrToPlayerActor
	B 		0x0209D580	
	
Hammer_ctor:	@ Replacing Actor 253 (Sprite 145)
	stmfd sp!, {r4,lr}
	mov r0, #0x400
	add r0, r0, #40
	bl allocFromGameHeap
	movs r4, r0
	beq Hammer_ctor_end
	bl EnemyActor_ctor
	ldr r0, =Hammer_vtable
	str r0, [r4]
Hammer_ctor_end:
	mov r0, r4
	ldmfd sp!, {r4,pc}
	

Hammer_null:
	bx lr
	
	
Hammer_init:
	stmfd sp!, {r4,lr}
	mov r4, r0
	ldr r2, =mainHeapHandle
	ldr r2, [r2]
	mov r1, #0x40
	ldr r3, [r0]
	ldr r3, [r3,#0x34]
	blx r3
	
	mov r0, r4
	bl Hammer_setup
	
	mov r0, #1
	ldmfd sp!, {r4,pc}
	

Hammer_sprite_collision:
	stmfd	sp!, {lr}
	MOV		R0, R2
	BL 		Base__deleteIt
	stmfd	sp!, {lr}

Hammer_mario_collision:
	stmfd	sp!, {lr}
	ldmfd	sp!, {pc}

Hammer_DieonFlagpole:
	stmfd	sp!, {lr}
	MOV     R4, R0
	LDR     R1, =Vec3_vtable
	LDR     R2, [R0, #0x5C]
	STR     R2, [R1, #0x0]
	LDR     R2, [R0, #0x60]
	STR     R2, [R1, #0x4]
	LDR     R2, [R0, #0x64]
	STR     R2, [R1, #0x8]
	LDR     R2, [R0, #0x68]
	STR     R2, [R1, #0xC]	
	MOV     R0, #0x3A
	BL      SpawnParticle
	MOV     R0, R4
	BL 		Base__deleteIt
	ldmfd	sp!, {pc}

repl_02117D18_ov_0A:					@ Change Goal Pole animation
	STMFD	SP!, {R0}
	BL      getPtrToPlayerActor
	ADD     R0, #0x700
	ADD     R0, #0xAB
	LDRB    R1, [R0]
	CMP     R1, #0x5
	LDMFD	SP!, {R0}
	MOVEQ   R1, #0xC00
	STREQ   R1, [SP]
	MOV     R1, #0x61
	ADDEQ   R1, #0x2
	B       0x2117D1C
	
repl_02117BD0_ov_0A:					@ Disable taking off hat
repl_02117C78_ov_0A:
	STMFD	SP!, {R0,R1,LR}
	BL      getPtrToPlayerActor
	ADD     R0, #0x700
	ADD     R0, #0xAB
	LDRB    R1, [R0]
	CMP     R1, #0x5
	LDMFD	SP!, {R0,R1,LR}
	BXEQ    LR
	BNE     0x21209F4

repl_0212D308_ov_0B:					@ No idea but it fixes the cap lava so whatever
	LDR     R0, =0x212E544
	BX      LR

repl_020DDF2C_ov_0A:					@ Blue Koopas don't drop Hammers
	BL 		0x20DDFAC
	BX 		LR

nsub_020DDE70_ov_0A:					@ Blue Koopas don't drop Hammers if GP'd
	B  		0x20DF754
	
repl_020D07B4_ov_0A:					@ Hammer blocks work properly, i.e if powerup is mini or none they'll just spit out a mushroom like they should
	STMFD	SP!, {LR}
	BL      getPtrToMarioOrLuigi    
	ADD     R0, R0, #0x700
	LDRB    R0, [R0, #0xAB]
	MOV     R1, R4
	AND     R4, #0xF
	CMP     R4, #0xB
	LDMNEFD SP!, {PC}
	CMP     R0, #4
	CMPNE   R0, #0
	ANDEQ   R1, #0xFFFFFFF0
	LDMFD 	SP!, {PC}
	
CustomHammerBrotherVirt49:
	LDR     R1, [R0, #8]
	ANDS    R1, R1, #0xF
	BEQ     .JustDie

	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR     R1, =Vec3_vtable
	LDR     R2, [R0, #0x60]
	STR     R2, [R1, #0x4]
	LDR     R2, [R0, #0x64]
	STR     R2, [R1, #0x8]
	LDR     R2, [R0, #0x68]
	STR     R2, [R1, #0xC]
	LDR     R2, =0x30000200
	STR     R2, [SP]
	MOV     R2, #0x6
	BL      spawnPowerups2
	ADD     SP, SP, #4
	LDMFD   SP!, {LR}

.JustDie:
	ADD     R0, R0, #0x120
	B 		0x20A4658
	BX 		LR

.CustomHammerBrotherVirt50:
	LDR     R1, [R0, #8]
	ANDS    R1, R1, #0xF
	BEQ     .JustGP

	STMFD   SP!, {R0,LR}
	BL      getPtrToPlayerActor
	ADD     R0, #0x700
	ADD     R0, #0xAB
	MOV     R1, #5
	STRB    R1, [R0]
	LDMFD   SP!, {R0,LR}
	
.JustGP:
	B		0x0209CE08
	BX      LR
	
.balign 4
Hammer_vtable:
	.long Hammer_init
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long Hammer_beforedelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Hammer_draw
	.long Hammer_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long Hammer_update_state0
	.long 0x0209AD1C
	.long enemyActor_executeState1
	.long enemyActor_executeState2
	.long enemyActor_executeState3
	.long enemyActor_executeState4
	.long enemyActor_executeState5
	.long enemyActor_executeState6
	.long enemyActor_executeState7
	.long enemyActor_executeState8
	.long enemyActor_executeState9
	.long Hammer_null
	.long 0x0209FAF4
	.long 0x0209FAAC
	.long 0x0209C974
	.long 0x020A012C
	.long 0x020A00AC
	.long 0x0209F574	@ sets state=9
	.long 0x0209F3D8
	.long 0x0209F354	@ sets state=0
	.long enemyActor_executeAllStates
	.long 0x0209D988
	.long 0x0209D920
	.long 0x0209D84C
	.long 0x0209D568
	.long 0x0209D43C
	.long 0x0209D2A0
	.long 0x0209D158
	.long 0x0209D014
	.long Hammer_null
	.long 0x0209CE08
	.long 0x0209CBF8
	.long 0x0209CAD0
	.long 0x02098F7C
	.long Hammer_DieonFlagpole	@ 0x02098E7C
	.long Hammer_null	@ Collision with lava if dead.
	.long 0x0209CAC4
	.long Hammer_null
	.long 0x02099168
	.long 0x02098A94
	.long Hammer_null 	@ collision with other sprite / not used with the koopa shell callbackS
	.long Hammer_mario_collision    @0x02098A10
	.long 0x0209C9D0	@ sets state=2
	.long 0x0209C994	@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x0209A6D4