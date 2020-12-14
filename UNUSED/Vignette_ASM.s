#include "Files.h"

Vignette_ctor:
	STMFD 	SP!, {R4,LR}
	MOV		R0, #0x600
	BL		allocFromGameHeap
	MOVS	R4, R0
	BEQ 	Vignette_ctor_end
	BL 		EnemyActor_ctor
	LDR 	R0, =Vignette_vtable
	STR 	R0, [r4]
Vignette_ctor_end:
	MOV		R0, R4
	LDMFD 	SP!, {R4,PC}
	
Vignette_onCreate:
	STMFD 	SP!, {R4,LR}
	MOV    	R4, R0
	LDR 	R2, =mainHeapHandle
	LDR 	R2, [R2]
	MOV 	R1, #0x40
	LDR 	R3, [R0]
	LDR 	R3, [R3,#0x34]
	BLX 	R3
	MOV	 	R0, R4
	BL 		Vignette_setup
	MOV		R0, #1
	LDMFD 	SP!, {R4, PC}

Vignette_Profile:
	.long Vignette_ctor
	.long 0x144012B
	.long Vignette_loadFiles
	.long 0x0
	.long 0x0


.balign 4
Vignette_vtable:
	.long Vignette_onCreate
	.long actor_beforeCreate
	.long actor_afterCreate
	.long base_onDelete
	.long actor_beforeDelete
	.long actor_afterDelete
	.long enemyActor_execute
	.long enemyActor_beforeExecute
	.long enemyActor_afterExecute
	.long Vignette_Draw
	.long Vignette_BeforeDraw
	.long actor_afterDraw
	.long base_willBeDeleted
	.long base_moreHeapShit
	.long base_createHeap
	.long base_heapCreated
	.long 0x020D5894
	.long 0x020D58F0
	.long actor_getSomething
	.long actor_incrementSomething
	.long Vignette_State0
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
	.long 0x0209CAC0
	.long 0x0209FAF4
	.long 0x0209FAAC
	.long 0x0209C974
	.long 0x020A012C
	.long 0x020A00AC
	.long 0x0209F574	@ sets state=9
	.long 0x0209F3D8
	.long 0x0209F354	@ sets state=0
	.long enemyActor_executeAllStates
	.long 0x0209D988
	.long 0x0209D920
	.long 0x0209D84C
	.long 0x0209D568
	.long 0x0209D43C
	.long 0x0209D2A0
	.long 0x0209D158
	.long 0x0209D014
	.long 0x0209CAC0
	.long 0x0209CE08
	.long 0x0209CBF8
	.long 0x0209CAD0
	.long 0x02098F7C
	.long Vignette_DieOnFlagpole	@ 0x02098E7C
	.long 0x0209CAC0	@ Collision with lava if dead.
	.long 0x0209CAC4
	.long 0x0209CAC0
	.long 0x02099168
	.long 0x02098A94
	.long 0x0209CAC0 	@ collision with other sprite / not used with the koopa shell callbackS
	.long 0x0209CAC0    @ 0x02098A10
	.long 0x0209C9D0	@ sets state=2
	.long 0x0209C994	@ sets state=3
	.long 0x020A0268
	.long 0x020A0240
	.long 0x0209A6D4