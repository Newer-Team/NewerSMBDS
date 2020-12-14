PeachCage_ctor:
	STMFD 	SP!, {R4,LR}
	LDR 	R0, =0x500
	BL 		allocFromGameHeap
	MOVS 	R4, R0
	BEQ 	PeachCage_ctor_end
	BL 		EnemyActor_ctor
	LDR 	R1, =PeachCage_vtable
	STR 	R1, [R4]

PeachCage_ctor_end:
	MOV 	R0, R4
	LDMFD 	SP!, {R4,PC}

PeachCage_null:
	BX LR

PeachCage_onCreate:
	STMFD	SP!, {R4,LR}
	MOV 	R4, R0
	LDR 	R2, =mainHeapHandle
	LDR 	R2, [R2]
	MOV 	R1, #0x40
	LDR 	R3, [R0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV 	R0, R4
	BL		PeachCage_setup
	MOV		R0, #1
	LDMFD	SP!, {R4,PC}

PeachCage_delete:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	BL		base_onDelete
	LDMFD	SP!, {R4,PC}

PeachCage_Profile:
	.long 	PeachCage_ctor
	.long 	0x1800180
	.long 	PeachCage_loadFiles

PeachCage_vtable:
	.long PeachCage_onCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long PeachCage_delete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long PeachCage_onDraw
	.long PeachCage_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long PeachCage_heapCreated
	.long PeachCage_dtor
	.long PeachCage_dtorFree
	.long actor_getSomething
	.long actor_incrementSomething
	.long PeachCage_IdleState
	.long 0x0209AD1C
	.long PeachCage_DoEndingState
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
	.long PeachCage_AllStates
	.long 0x0209D988			@ called when hit from below block (EnemyActor__virt41: 0x0209D988)
	.long 0x0209D920			@ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
	.long 0x0209D84C			@ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
	.long 0x0209D568			@ unknow callback
	.long 0x0209D43C			@ unknown callback
	.long 0x0209D158			@ called when killed by Mega Mario (EnemyActor__virt47: 0x0209D158)
	.long 0x0209D158			@ unknown callback
	.long 0x0209D014			@ unknown callback
	.long 0x0209CFC0			@ called when beeing jumped on
	.long 0x0209CE08			@ called when beeing ground-pounded
	.long 0x0209CBF8			@ called when hit by Shell Mario
	.long 0x0209CAD0			@ unknown callback
	.long 0x02098F7C			@ unknown callback
	.long 0x02098E7C			@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)
	.long 0x0209CD3C			@
	.long 0x0209CAC4			@
	.long 0x0209CAC0			@ BX LR
	.long 0x02099168			@ called when colliding with player?
	.long 0x02098A94			@ called when colliding with player. Returns weather to hurt Mario (0) or to pass trough (1). The function can kill the sprite as well.
	.long 0x02098A90			@ called when sprite killed by bottom/block?
	.long 0x0217390C			@ 0x02277F8C - 0x104680
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x020E22B0
	.long 0x020CECCC			@ BX LR
	.long 0x020CECC8			@ BX LR
