nsub_020201A0_main:
	STMFD   SP!, {R4,LR}
	LDR     R0, =CameraControllerPtr
	LDR     R0, [R0]
	CMP     R0, #0
	BLEQ    getPtrToPlayerActor
	MOV     R1, #0
	BL      setCameraToActor
	LDMFD   SP!, {R4,PC}

CameraController_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x3F4
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		CameraController_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =CameraController_vtable
	STR		R0, [R4]
CameraController_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
		
CameraController_OnCreate:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	LDR     R2, =CameraControllerPtr
	STR     R4, [R2]
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}
	
CameraController_Profile:
	.long CameraController_ctor
	.long 0x1460146
	.long 0x0
	.long 0x0
	.long 0x0
	
CameraController_dtor:
	STMFD   SP!, {LR}
	MOV		R4, R0
	LDR     R2, =CameraControllerPtr
	MOV     R0, #0
	STR     R0, [R2]
	MOV     R0, R4
	BL      0x20D5894
	LDMFD   SP!, {PC}
	
CameraController_vtable:
	.long CameraController_OnCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long CameraController_dtor
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0
	.long 0x0209CAC0	
	.long 0x0209CAC0
	.long 0x0209CAC0	
	.long 0x0209CAC0
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
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ unknown callback
	.long 0x0209CAC0			@ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)
	
.data
.balign 4
CameraControllerPtr:
	.word 0x0
