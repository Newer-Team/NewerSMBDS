Thundercloud_ctor:	
	stmfd sp!, {r4,lr}
	mov r0, #0x400
	add r0, r0, #60
	bl allocFromGameHeap
	movs r4, r0
	beq ThunderCloud_ctor_end
	bl EnemyActor_ctor
	ldr r0, =ThunderCloud_vtable
	str r0, [r4]
ThunderCloud_ctor_end:
	mov r0, r4
	ldmfd sp!, {r4,pc}
	
ThunderCloud_null:
	bx lr
	
ThunderCloud_init:
	stmfd sp!, {r4,lr}
	mov r4, r0

	@ is that really needed? dunno.
	ldr r2, =mainHeapHandle
	ldr r2, [r2]
	mov r1, #0x40
	ldr r3, [r0]
	ldr r3, [r3,#0x34]
	blx r3
	
	mov r0, r4
	bl ThunderCloud_setup
	
	mov r0, #1
	ldmfd sp!, {r4,pc}
	
ThunderCloud_beforeDraw:
	MOV  	R0, #1
	BX		LR

ThunderCloud_CollisionSomething:
	STMFD   SP!, {R4,LR}
	MOV		R4, R0
	LDR		R1, [R2,#4]			@Load player actor ptr
	
	LDR     R0, [R1,#0x778]		
	ANDS    R0, R0, #0x400000	@Check if Mario is in Shell
	BNE		ThunderCloud_CollisionSomething_kill
	
	LDRB    R0, [R1,#0x7AB]
	CMP		R0, #3				@Check if Mario is Mega
	BEQ		ThunderCloud_CollisionSomething_kill
	
	MOV		R0, #0
	LDMFD	SP!, {R4,PC}
	
ThunderCloud_CollisionSomething_kill:
	MOV		R0, R4
	BL		ThunderCloud_kill_respawn
	MOV		R0, #1
	LDMFD	SP!, {R4,PC}

	
ThunderCloud_vtable:
	.long ThunderCloud_init
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long ThunderCloud_draw
	.long ThunderCloud_beforeDraw @enemyActor_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long ThunderCloud_update_state0
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
	.long ThunderCloud_null
	.long 0x0209FAF4
	.long 0x0209FAAC
	.long 0x0209C974
	.long 0x020A012C
	.long 0x020A00AC
	.long 0x0209F574	@ sets state=9
	.long 0x0209F3D8
	.long 0x0209F354	@ sets state=0
	.long enemyActor_executeAllStates
	.long ThunderCloud_null			@ called when hit from below block (EnemyActor__virt41: 0x0209D988)	
	.long ThunderCloud_shell_kill		@ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
	.long ThunderCloud_null			@ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
	.long 0x0209D568			@ unknow callback
	.long 0x0209D43C			@ unknown callback
	.long ThunderCloud_kill_respawn	@ called when killed by Mega Mario (EnemyActor__virt47: 0x0209D158)
	.long 0x0209D158			@ unknown callback
	.long 0x0209D014			@ unknown callback
	.long ThunderCloud_null
	.long 0x0209CE08			@ unknown callback
	.long ThunderCloud_kill_respawn 	@ called when hit by Shell Mario (EnemyActor__virt51: 0x0209CBF8)
	.long 0x0209CAD0			@ unknown callback
	.long 0x02098F7C			@ unknown callback
	.long ThunderCloud_flagpole_kill	@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)
	.long 0x0209CD3C			@
	.long 0x0209CAC4			@
	.long ThunderCloud_null
	.long 0x02099168			@ called when colliding with player?
	.long 0x02098A94			@ThunderCloud_CollisionSomething
	.long ThunderCloud_null			@ called when sprite killed by bottom/block?
	.long 0x02098A10
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x0209A6D4