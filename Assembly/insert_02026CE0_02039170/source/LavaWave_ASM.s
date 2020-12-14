LavaWave_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x410
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		LavaWave_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =LavaWave_vtable
	STR		R0, [R4]
	
LavaWave_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
	
LavaWave_OnCreate:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV		R0, R4
	BL 		LavaWave_setup
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}	
	
LavaWave_Profile:
	.long LavaWave_ctor
	.long 0x117010F
	.long LavaWave_loadFiles
	.long 0x0
	.long 0x0


LavaWave_vtable:
	.long LavaWave_OnCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long LavaWave_draw
	.long LavaWave_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long LavaWave_update_state0
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
	.long 0x0209CAC0			@ called when hit from below block (EnemyActor__virt41: 0x0209D988)	
	.long 0x0209CAC0			@ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
	.long 0x0209CAC0			@ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ called when killed by Mega Mario (EnemyActor__virt47: 0x0209D158)
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ called when beeing jumped on
	.long 0x0209CAC0			@ called when beeing ground-pounded
	.long 0x0209CAC0			@ called when hit by Shell Mario
	.long 0x0209CAD0			@ unknown callback
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)