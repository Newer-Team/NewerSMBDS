Fireball_ctor:
	STMFD 	SP!, {r4,lr}
	MOV 	R0, #0x400
	ADD 	R0, R0, #60
	BL 		allocFromGameHeap
	MOVS 	R4, R0
	BEQ 	Fireball_ctor_end
	BL 		EnemyActor_ctor
	LDR 	R0, =Fireball_vtable
	STR 	R0, [R4]
Fireball_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
	
Fireball_null:
	BX		LR
	
Fireball_onCreate:
	STMFD 	SP!, {R4,LR}
	MOV    	R4, R0
	LDR 	R2, =mainHeapHandle
	LDR 	R2, [R2]
	MOV 	R1, #0x40
	LDR 	R3, [R0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV	 	R0, R4
	BL 		Fireball_setup
	MOV		R0, #1
	LDMFD 	SP!, {R4, PC}

Fireball_CollisionSomething:
	LDR		R0, [R2,#4]			@Load player actor ptr
	LDR     R0, [R0,#0x778]		
	ANDS    R0, R0, #0x400000	@Check if Mario is in Shell
	MOVEQ	R0, #0
	MOVNE	R0, #1
	BX		LR

Fireball_vtable:
	.long Fireball_onCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Fireball_draw
	.long Fireball_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long Fireball_update_state0
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
	.long Fireball_null
	.long 0x0209FAF4
	.long 0x0209FAAC
	.long 0x0209C974
	.long 0x020A012C
	.long 0x020A00AC
	.long 0x0209F574			@ sets state=9
	.long 0x0209F3D8
	.long 0x0209F354			@ sets state=0
	.long enemyActor_executeAllStates
	.long Fireball_null			@ called when hit from below block (EnemyActor__virt41: 0x0209D988)	
	.long Fireball_null			@ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
	.long Fireball_null			@ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
	.long 0x0209D568			@ unknow callback
	.long 0x0209D43C			@ unknown callback
	.long Fireball_null			@ called when killed by Mega Mario (EnemyActor__virt47: 0x0209D158)
	.long 0x0209D158			@ unknown callback
	.long 0x0209D014			@ unknown callback
	.long Fireball_null
	.long 0x0209CE08			@ called when beeing ground-pounded
	.long Fireball_null		 	@ called when hit by Shell Mario (EnemyActor__virt51: 0x0209CBF8)
	.long 0x0209CAD0			@ unknown callback
	.long 0x02098F7C			@ unknown callback
	.long Fireball_flag			@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)
	.long 0x0209CD3C			@
	.long 0x0209CAC4			@
	.long Fireball_null
	.long 0x02099168			@ called when colliding with player?
	.long Fireball_CollisionSomething	@ called when colliding with player. Returns weather to hurt Mario (0) or to pass trough (1). The function can kill the sprite as well.
	.long Fireball_null			@ called when sprite killed by bottom/block?
	.long 0x02098A10
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x0209A6D4