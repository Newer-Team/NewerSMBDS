.globl DrawAModel  @R0 = ModelAddress, R1 = Vec3 Position, R2 = S16Vec3 Rotation, R3 = Vec3 Scale
DrawAModel:
	STMFD   SP!, {R4, LR}
	SUB     SP, SP, #0x10

	@ Push variables onto stack
	STMIA 	SP, {R0-R3}
	
	@ Set Positions
	LDR		R0, =0x0208B6E8
	MOV     R4, R1
	ADD     R4, #4
	LDMIA	R4, {R1-R3}	
	BL		SetModelPos

	@ Set Rotation
	LDR		R0, =0x0208B6E8
	LDR     R4, [SP,#8]
	LDRSH   R1, [R4,#0x4]
	BL      SetModelRotX
	
	LDR     R0, =0x0208B6E8
	LDR     R4, [SP,#8]
	LDRSH   R1, [R4,#0x6]
	BL      SetModelRotY
	
	LDR     R0, =0x0208B6E8
	LDR     R4, [SP,#8]
	LDRSH   R1, [R4,#0x8]
	BL      SetModelRotZ
	
	@ Copying 48 bytes from 208B6E8 to ModelAddress + 0x60
	LDR     LR, =0x0208B6E8
	LDR     R12, [SP]
	ADD     R12, #0x60
	LDMIA   LR!, {R0-R3}
	STMIA   R12!,{R0-R3}
	LDMIA   LR!, {R0-R3}
	STMIA   R12!,{R0-R3}
	LDMIA   LR,  {R0-R3}
	STMIA   R12, {R0-R3}

	LDR     R0, [SP]
	LDR     R1, [SP,#0xC]
	ADD     R1, #4
	LDR     R2, [R0]
	LDR     R2, [R2,#0xC]
	BLX     R2
	
	MOV     R0, #1

	ADD     SP, SP, #0x10
	LDMFD   SP!, {R4,PC}
	