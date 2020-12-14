MinecartEnemy_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x800
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		MinecartEnemy_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =MinecartEnemy_vtable
	STR		R0, [R4]
	
MinecartEnemy_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
	
MinecartEnemy_OnCreate:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV		R0, R4
	BL 		MinecartEnemy_setup
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}	
	
MinecartEnemy_Profile:
	.long MinecartEnemy_ctor
	.short 0x0139
	.short 0x0139
	.long MinecartEnemy_loadFiles
	
MinecartEnemy_vtable:
	.long MinecartEnemy_OnCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long MinecartEnemy_draw
	.long MinecartEnemy_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long MinecartEnemy_StateIdle
	.long 0x0209AD1C
	.long MinecartEnemy_StateFalling
	.long MinecartEnemy_StateLanding
	.long MinecartEnemy_StateDead
	.long enemyActor_executeState4
	.long enemyActor_executeState5
	.long enemyActor_executeState6
	.long enemyActor_executeState7
	.long enemyActor_executeState8
	.long enemyActor_executeState9
	.long 0x0209D9FC
	.long 0x0209FAF4
	.long 0x0209FAAC
	.long 0x0209C974
	.long 0x020A012C
	.long 0x020A00AC
	.long 0x0209F574	@ sets state=9
	.long 0x0209F3D8
	.long 0x0209F354	@ sets state=0
	.long enemyActor_executeAllStates
	.long 0x0209D988			@ called when hit from below block (EnemyActor__virt41: 0x0209D988)	
	.long MinecartEnemy_JumpedOn @ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
	.long 0x0209D84C			@ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
	.long 0x0209D568			@ called when starred
	.long 0x0209D43C			@ called when killed by Mega Mario (EnemyActor__virt46: 0x0209D2A0)
	.long 0x0209D158			@ unknown callback
	.long 0x0209D158			@ unknown callback
	.long 0x0209D014			@ unknown callback
	.long MinecartEnemy_JumpedOn @ called when beeing jumped on
	.long 0x0209CE08			@ called when beeing ground-pounded
	.long MinecartEnemy_JumpedOn @ called when hit by Shell Mario
	.long 0x0209CAD0			@ unknown callback
	.long 0x02098F7C			@ unknown callback
	.long 0x02098E7C			@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)
	.long 0x0209CD3C			@
	.long 0x0209CAC4			@
	.long 0x0209CAC0			@ BX LR
	.long 0x02099168			@ called when colliding with player?
	.long 0x02098A94			@ called when colliding with player. Returns whether to hurt Mario (0) or to pass through (1). The function can kill the sprite as well.
	.long 0x02098A90			@ called when sprite killed by bottom/block?
	.long Cannonball_Virt61
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x020E22B0