FinalBoss_Profile:
	.long FinalBoss_ctor
	.short 0x14B
	.short 0x14B
	.long FinalBoss_loadFiles


FinalBoss_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x880		@ TODO: Get final size of this guy.
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		FinalBoss_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =FinalBoss_vtable
	STR		R0, [R4]

	ADD     R0, R4, #0x3F4
	BL      modelAnim3d_ctor

	ldr		R0, =0x49C
	add     R0, R4, R0
	bl      modelAnim3d_ctor
	

FinalBoss_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}


FinalBoss_init:
	STMFD	SP!,{R4,LR}
	MOV 	R4, R0

	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [R0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3

	MOV 	R0, R4
	BL   	FinalBoss_onCreate

	LDMFD 	SP!, {R4,PC}

FinalBoss__null:
	BX		LR

FinalBoss_vtable:
    .long FinalBoss_init                @ 00
	.long actor_beforeCreate            @ 01
	.long actor_afterCreate             @ 02
	.long base_onDelete                 @ 03
	.long actor_beforeDelete            @ 04
	.long actor_afterDelete             @ 05
	.long enemyActor_execute            @ 06 Always execute
	.long enemyActor_beforeExecute      @ 07
	.long enemyActor_afterExecute       @ 08
	.long FinalBoss_onDraw              @ 09 Draws the actor
	.long FinalBoss_beforeDraw     		@ 10 Returns whether to draw the actor (1) or not (0)
	.long actor_afterDraw               @ 11
	.long base_willBeDeleted            @ 12
	.long base_moreHeapShit             @ 13
	.long base_createHeap               @ 14
	.long FinalBoss_heapCreated         @ 15
	.long FinalBoss_dtor 				@ EnemyActor__dtor              @ 16
	.long FinalBoss_dtorFree			@ EnemyActor__dtorFree          @ 17
	.long Actor__getXPos                @ 18 Unused, most likely from early development stages
	.long Actor__addToXPos              @ 19 Unused, most likely from early development stages
	.long FinalBoss_executeState0	    @ 20
	.long EnemyActor__isInvisible       @ 21
	.long EnemyActor__executeState1     @ 22
	.long EnemyActor__executeState2     @ 23
	.long EnemyActor__executeState3     @ 24
	.long EnemyActor__executeState4     @ 25
	.long EnemyActor__executeState5     @ 26
	.long EnemyActor__executeState6     @ 27
	.long EnemyActor__executeState7     @ 28
	.long EnemyActor__executeState8     @ 29
	.long EnemyActor__executeState9     @ 30
	.long EnemyActor__virt31            @ 31 Unknown
	.long EnemyActor__virt32            @ 32 Unknown
	.long EnemyActor__virt33            @ 33 Unknown
	.long EnemyActor__virt34            @ 34 Unknown
	.long EnemyActor__virt35            @ 35 Unknown
	.long EnemyActor__virt36            @ 36 Unknown
	.long EnemyActor__virt37            @ 37 Unknown, sets state to 9
	.long EnemyActor__virt38            @ 38 Unknown
	.long EnemyActor__virt39	        @ 39 Unknown, sets state to 0
	.long enemyActor_executeAllStates   @ 40 A state that's always run.
	.long FinalBoss__null            	@ 41 Called when hit from below block
	.long FinalBoss__null            	@ 42 Called when receiving a koopa shell
	.long EnemyActor__virt43		    @ 43 Called when receiving a fireball
	.long FinalBoss__null				@ 44 Called when touched with star powerup
	.long FinalBoss__null            	@ 45 Called when touched with mega powerup
	.long EnemyActor__virt46            @ 46 Unknown callback
	.long EnemyActor__virt47            @ 47 Unknown callback
	.long EnemyActor__virt48	        @ 48 Unknown callback
	.long EnemyActor__virt49            @ 49 Called when beeing jumped on
	.long EnemyActor__virt50            @ 50 Called when beeing ground-pounded
	.long FinalBoss__null            	@ 51 Called when hit with shell powerup
	.long EnemyActor__virt52	        @ 52 Unknown callback
	.long EnemyActor__virt53            @ 53 Unknown callback
	.long EnemyActor__DieFromFlagpole   @ 54 Called when player touches flagpole
	.long EnemyActor__virt55	        @ 55 Unknown
	.long EnemyActor__virt56	        @ 56 Unknown
	.long EnemyActor__virt57	        @ 57 Unknown
	.long EnemyActor__virt58            @ 58 Called when colliding with player
	.long EnemyActor__virt59            @ 59 Called when colliding with player. Returns whether to hurt Mario (0) or to pass through (1). The function can kill the sprite as well.
	.long EnemyActor__virt60            @ 60 Called when sprite killed by bottom/block?
	.long EnemyActor__virt61            @ 61 Unknown, something after being jumped on
	.long EnemyActor__virt62            @ 62 Unknown, sets state to 2
	.long EnemyActor__virt63            @ 63 Unknown, sets state to 3
	.long EnemyActor__virt64            @ 64 Unknown
	.long EnemyActor__virt65            @ 65 Unknown
	.long EnemyActor__virt66            @ 66 Unknown
