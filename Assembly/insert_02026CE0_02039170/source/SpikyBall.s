@149BA0

repl_0218CFD8_ov_79:
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x14
	ANDS    R0, R0, #0xF
	LDREQ   R0, =Vec3_vtable
	BEQ     0x0218CFDC
	ADD     SP, SP, #0x28
	LDRB    R1, [R4,#0x2BD]
	EOR     R1, #1
	STRB    R1, [R4,#0x2BD]
	MOV     R0, #0
	STR     R0, [SP]
	LDRB    R3, [R4,#0x2BD]
	MOV     R0, R4
	LDR     R12, [R0]
	MOV     R2, #0x300
	LDR     R1, =0x20C4ED4
	MOV     R3, R3,LSL#1
	LDRSH   R1, [R1,R3]
	LDR     R12, [R12,#0xF8]
	RSB     R3, R2, #0
	MOV     R2, #0x3000
	BLX     R12
	ADD     SP, SP, #8
	LDMFD   SP!, {R4,PC}
	
nsub_0218D250_ov_79:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR     R0, =0x55D
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x60E
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9CD
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}	
	
nsub_0218DCBC_ov_79:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR     R0, =0x4BD
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x60E
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9CD
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}
	
repl_0218D220_ov_79:
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x18
	AND     R0, R0, #0xF
	CMP     R0, #1
	LDRLT   R0, =0x55D
	LDREQ   R0, =0x60E
	LDRGT   R0, =0x9CD
	BX      LR

repl_0218DC8C_ov_79:
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x18
	AND     R0, R0, #0xF
	CMP     R0, #1
	LDRLT   R0, =0x4BD
	LDREQ   R0, =0x60E
	LDRGT   R0, =0x9CD
	BX      LR

nsub_0218CAF4_ov_79:
	LDR     R2, [R4,#8]
	LSR     R2, R2, #0x18
	ANDS    R2, R2, #0xF
	MOV     R2, #0x1000
	BEQ     0x0218CAF8
	STRH    R2, [R4,#0xA0]
	MOV     R2, #0xE00
	B		0x0218CAF8
	
repl_0218D5D0_ov_79:
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x18
	ANDS    R0, R0, #0xF
	MOV     R0, #0x1000
	BXEQ    LR
	STRH    R0, [R4, #0xA0]
	MOV     R0, #0x2000
	BX		LR
	
SpikyBallVirt44:
	MOV     R9, R0
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x18
	AND     R0, R0, #0xF
	CMP     R0, #2
	MOVLT   R0, R9
	BLT     0x209D568
	BL      getPtrToPlayerActor
	SUB     SP, SP, #8
	MOV     R5, R0	
	LDR     R0, =Vec3_vtable
	LDR     R1, [R5,#0x60]
	LDR     R2, [R5,#0x64]
	LDR     R3, [R5,#0x68]
	ADD     R2, #0x10000
	STMIA   R0, {R0, R1, R2, R3}
	LDR     R1, [R4,#8]
	LSR     R1, R1, #0x14
	AND     R1, R1, #0xF00
	MOV     R2, R0
	MOV     R3, #0
	MOV     R4, #0
	MOV     R5, #0
	STMIA   SP, {R4, R5}
	MOV     R0, #0x43
	BL      createActor
	MOV     R0, R9
	ADD     SP, SP, #8
	B       0x209D568
	
SpikyBallVirt46:
	MOV     R9, R0
	LDR     R0, [R4,#8]
	LSR     R0, R0, #0x18
	AND     R0, R0, #0xF
	CMP     R0, #2
	MOVLT   R0, R9
	BLT     0x209D2A0
	BL      getPtrToPlayerActor
	SUB     SP, SP, #8
	MOV     R5, R0	
	LDR     R0, =Vec3_vtable
	LDR     R1, [R5,#0x60]
	LDR     R2, [R5,#0x64]
	LDR     R3, [R5,#0x68]
	ADD     R2, #0x10000
	STMIA   R0, {R0, R1, R2, R3}
	LDR     R1, [R4,#8]
	LSR     R1, R1, #0x14
	AND     R1, R1, #0xF00
	MOV     R2, R0
	MOV     R3, #0
	MOV     R4, #0
	MOV     R5, #0
	STMIA   SP, {R4, R5}
	MOV     R0, #0x43
	BL      createActor
	MOV     R0, R9
	ADD     SP, SP, #8
	B       0x209D2A0
	
