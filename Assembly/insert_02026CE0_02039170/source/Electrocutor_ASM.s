Electrocutor_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x600
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		Electrocutor_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =Electrocutor_vtable
	STR		R0, [R4]
	
Electrocutor_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}

Electrocutor_OnCreate:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV		R0, R4
	BL 		Electrocutor_setup
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}	
	
Electrocutor_delete:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	ADD     R0, R0, #0x400
	ADD     R0, R0, #0x1C
	BL      0x20AB350
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}
	
Electrocutor_dtor:
	STMFD   SP!, {R4,LR}
	LDR     R2, =Electrocutor_vtable
	MOV     R4, R0
	LDR     R1, =Vec3_vtable
	STR     R2, [R4]
	ADD     R0, R0, #0x400
	ADD     R0, R0, #0x1C
	STR     R1, [R0,#0x6C]
	BL      UnregisterSolidCollision
	
	MOV		R0, R4
	ADD		R0, R0, #0x400
	ADD		R0, R0, #0xC8
	BL      UnregisterActivePhysics
	
	LDR     R1, =0x20C4F04
	LDR     R0, =Vec2_vtable
	STR     R1, [R4]
	STR     R0, [R4,#0x39C]
	STR     R0, [R4,#0x390]
	STR     R0, [R4,#0x384]
	STR     R0, [R4,#0x378]
	LDR     R1, =Vec3_vtable
	STR     R0, [R4,#0x36C]
	STR     R1, [R4,#0x35C]
	STR     R1, [R4,#0x31C]
	MOV     R0, R4
	STR     R1, [R4,#0x30C]
	BL      0x20A11A8
	MOV     R0, R4
	LDMFD   SP!, {R4,PC}
	
Electrocutor_dtorFree:
	STMFD   SP!, {R4,LR}
	LDR     R2, =Electrocutor_vtable
	MOV     R4, R0
	LDR     R1, =Vec3_vtable
	STR     R2, [R4]
	ADD     R0, R0, #0x400
	ADD     R0, R0, #0x1C
	STR     R1, [R0,#0x6C]
	BL      UnregisterSolidCollision
	
	MOV		R0, R4
	ADD		R0, R0, #0x400
	ADD		R0, R0, #0xC8
	BL      UnregisterActivePhysics
	
	LDR     R1, =0x20C4F04
	LDR     R0, =Vec2_vtable
	STR     R1, [R4]
	STR     R0, [R4,#0x39C]
	STR     R0, [R4,#0x390]
	STR     R0, [R4,#0x384]
	STR     R0, [R4,#0x378]
	LDR     R1, =Vec3_vtable
	STR     R0, [R4,#0x36C]
	STR     R1, [R4,#0x35C]
	STR     R1, [R4,#0x31C]
	MOV     R0, R4
	STR     R1, [R4,#0x30C]
	BL      0x20A11A8
	MOV     R0, R4
	BL      freeToGameHeap
	MOV     R0, R4
	LDMFD   SP!, {R4,PC}
	

Electrocutor_vtable:
	.long Electrocutor_OnCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long Electrocutor_delete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Electrocutor_draw
	.long 0x209DA58
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long Electrocutor_dtor
	.long Electrocutor_dtorFree
	.long actor_getSomething
	.long actor_incrementSomething
	.long Electrocutor_update_state0
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
	.long 0x0209CAC0			@
	.long 0x0209CAC0			@
	.long 0x0209CAC0			@ BX LR
	.long 0x0209CAC0			@ called when colliding with player?
	.long 0x02098A94			@ called when colliding with player. Returns weather to hurt Mario (0) or to pass trough (1). The function can kill the sprite as well.
	.long 0x0209CAC0			@ called when sprite killed by bottom/block?
	.long 0x02098A10
	.long 0x0209CAC0			@ sets state=2
	.long 0x0209CAC0			@ sets state=3
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0