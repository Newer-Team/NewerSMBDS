Itembox_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x520
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ 	Itembox_ctor_end
	BL 		Class2123ee0_ctor
	LDR 	R0, =Itembox_vtable
	STR 	R0, [r4]
Itembox_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
	
Itembox_null:
	BX		LR
	
Itembox_init:
	STMFD	SP!,{R4,LR}
	MOV 	R4, R0
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV     R1, #0
	STRB    R1, [R4,#0x4B8]	
	MOV     R2, #0xFF
	STRB    R2, [R4,#0x4B9]
	MOV 	R0, R4
	BL   	Itembox_setup
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}
	
Itembox_vtable:
	.long Itembox_init
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Itembox_Draw
	.long Itembox_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long Itembox_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long Itembox_update_state0
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
	.long Itembox_null
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x020E22B0
	.long 0x020CECCC			@ BX LR
	.long 0x020CECC8			@ BX LR