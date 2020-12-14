Midpoint_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x420
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ		Midpoint_ctor_end
	BL		EnemyActor_ctor
	LDR		R0, =Midpoint_vtable
	STR		R0, [R4]
	
Midpoint_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}

Midpoint_null:
	BX		LR
	
Midpoint_OnCreate:
	STMFD	SP!, {R4,LR}
	MOV		R4, R0
	LDR 	R2, =mainHeapHandle
	LDR	 	R2, [R2]
	MOV		R1, #0x40
	LDR	 	R3, [r0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV		R0, R4
	BL 		Midpoint_setup
	MOV 	R0, #1
	LDMFD 	SP!, {R4,PC}	
	
repl_0215E544_ov_36:
repl_0215EF90_ov_36:
	STMFD   SP!, {LR,R5-R8}
    BL      CheckTheFuckingToadHouses
    CMP     R5, #1
    BEQ     no_1
	LDR 	R5, =MidwayPointSecond
	LDRB    R5, [R5]
	MOV     R6, #0xFF
	CMP     R5, R6
	MOVNE   R3, R5
no_1:
	STRB    R3, [R1,#8]
	LDMFD   SP!, {LR,R5-R8}
	BX    	LR
	
repl_02006728_main:
	LDR 	R0, =MidwayPointSecond
	LDR 	R1, =MidwayAreaSecond
	MOV     R2, #0xFF
	STRB    R2, [R0]
	STRB    R2, [R1]
	ADD     SP, SP, #4
	BX 		LR
	
repl_0215EEF0_main:
	STMFD   SP!, {LR,R5,R6}
    BL      CheckTheFuckingToadHouses
    CMP     R5, #1
    BEQ     no_2
	LDR 	R5, =MidwayAreaSecond
	LDRB    R5, [R5]
	MOV     R6, #0xFF
	CMP     R5, R6
	MOVNE   R4, R5	
no_2:
	LDMFD   SP!, {LR,R5,R6}
	STR     R4, [R1]
	BX 		LR
    
CheckTheFuckingToadHouses:
    STMFD   SP!, {LR}
    LDR 	R5, =0x2085A9C
    LDR     R5, [R5]
    CMP     R5, #18
    MOVEQ   R5, #1
    LDMEQFD SP!, {PC}
    CMP     R5, #19
    MOVEQ   R5, #1
    LDMEQFD SP!, {PC}    
    CMP     R5, #23
    MOVEQ   R5, #1
    LDMEQFD SP!, {PC}
    MOV     R5, #0
    LDMFD   SP!, {PC}
	
Midpoint_vtable:
	.long Midpoint_OnCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Midpoint_draw
	.long Midpoint_beforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long Midpoint_update_state0
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
	.long Midpoint_null
	.long 0x0209C9D0			@ sets state=2
	.long 0x0209C994			@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x020E22B0