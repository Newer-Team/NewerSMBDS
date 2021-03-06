.globl EnemyKillingCollisionCallback
EnemyKillingCollisionCallback:
	STMFD   SP!, {R4-R8,LR}
	SUB     SP, SP, #8
	MOV     R8, R0
	MOV     R7, R1
	LDR     R6, [R8,#4]
	LDR     R5, [R7,#4]
	MOV     R0, R6
	MOV     R1, R5
	BL      0x20A047C				
	ADD     R3, R8, #0x30
	LDR     R2, [R3,#4]
	ADD     R1, R7, #0x30
	STR     R2, [R6,#0x394]
	LDR     R2, [R3,#8]
	MOV     R4, R0
	STR     R2, [R6,#0x398]
	LDR     R0, [R1,#4]
	STR     R0, [R6,#0x3A0]
	LDR     R0, [R1,#8]
	STR     R0, [R6,#0x3A4]
	LDRB    R0, [R5,#0x11C]
	CMP     R0, #2
	BNE     .Label1				@ Mario

	LDRH    R0, [R7,#0x28]
	ANDS    R0, R0, #0x400
	BNE     .Label2				@ Enemies

	LDRH    R1, [R8,#0xA2]
	LDR     R0, =0xFFFE
	AND     R0, R1, R0
	MOV     R0, R0,LSL#16
	MOVS    R0, R0,LSR#16
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	LDRB    R0, [R8,#0xA5]
	AND     R0, R0, #1
	ANDS    R0, R0, #0xFF
	ADDEQ   SP, SP, #8
	LDMEQFD SP!, {R4-R8,PC}

	LDRH    R0, [R7,#0x2A]
	AND     R0, R0, #0x8000
	MOV     R0, R0,LSL#16
	MOVS    R0, R0,LSR#16
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	LDRB    R0, [R7,#0x25]
	CMP     R0, #0
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	ADD     R0, R6, #0x100
	LDRSB   R0, [R0,#0x1E]
	ADD     R0, R8, R0,LSL#2
	LDR     R1, [R0,#0x64]
	LDR     R0, [R0,#0x3C]
	CMP     R1, #0
	RSBLT   R1, R1, #0
	CMP     R0, #0
	RSBLT   R0, R0, #0
	CMP     R1, R0
	ADDLE   SP, SP, #8
	LDMLEFD SP!, {R4-R8,PC}

	STRB    R4, [R6,#0x2BD]
	LDR     R1, [R6,#0xD0]
	AND     R0, R1, #0x80000000
	MOV     R0, R0,LSR#31
	EORS    R0, R4, R0
	ADDEQ   SP, SP, #8
	LDMEQFD SP!, {R4-R8,PC}

	RSB     R0, R1, #0
	STR     R0, [R6,#0xD0]
	LDR     R1, [R6,#0xE0]
	LDR     R0, =0x16F
	RSB     R2, R1, #0
	ADD     R1, R6, #0x5C
	STR     R2, [R6,#0xE0]
	BL      PlaySNDEffect
	ADD     SP, SP, #8
	LDMFD   SP!, {R4-R8,PC}

.Label1:
	CMP     R0, #1
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	ADD     R0, R5, #0x100
	LDRSB   R2, [R0,#0x1E]
	LDR     R0, [R6,#0x64]
	MOV     R1, R8
	BL      0x2098DD8
	CMP     R0, #0
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	ADD     R0, R5, #0x100
	LDRSB   R0, [R0,#0x1E]
	ADD     R0, R6, R0,LSL#1
	ADD     R0, R0, #0x300
	LDRH    R1, [R0,#0xC6]
	CMP     R1, #0
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	LDRB    R1, [R6,#0x3F1]
	MOV     R2, R7
	CMP     R1, #0
	MOVNE   R1, #0xF
	STRNEH  R1, [R0,#0xC6]
	MOVEQ   R1, #5
	STREQH  R1, [R0,#0xC6]
	MOV     R0, R6
	LDR     R3, [R0]
	MOV     R1, R8
	LDR     R3, [R3,#0xEC]
	BLX     R3
	CMP     R0, #0
	BEQ     .Label1.1

	LDR     R0, [R8,#0x9C]
	ADD     SP, SP, #8
	STR     R0, [R6,#0x358]
	LDRB    R0, [R8,#0xA6]
	ORR     R0, R0, #2
	STRB    R0, [R8,#0xA6]
	LDMFD   SP!, {R4-R8,PC}

.Label1.1:
	LDRH    R0, [R7,#0xA0]
	AND     R0, R0, #2
	MOV     R0, R0,LSL#16
	MOVS    R0, R0,LSR#16
	ADDNE   SP, SP, #8
	LDMNEFD SP!, {R4-R8,PC}

	LDR     R0, [R5,#0x778]
	ANDS    R0, R0, #0x400000
	BEQ     .Label1.3

	MOV     R0, R6
	MOV     R1, R5
	BL      0x209CCD0
	ADD     SP, SP, #8
	CMP     R0, #0
	LDMFD   SP!, {R4-R8,PC}

.Label1.3:
	MOV     R2, #0
	MOV     R0, R5
	STR     R2, [SP]
	LDR     R4, [R0]
	MOV     R1, R6
	LDR     R4, [R4,#0x64]
	MOV     R3, #0x4000
	BLX     R4
	ADD     SP, SP, #8
	LDMFD   SP!, {R4-R8,PC}

.Label2:
	LDRB    R1, [R6,#0x3DD]
	LDRB    R0, [R5,#0x3DD]
	CMP     R1, R0
	ADDCC   SP, SP, #8
	LDMCCFD SP!, {R4-R8,PC}

	LDRH    R1, [R8,#0xA2]
	LDR     R0, =0xFFFE
	AND     R0, R1, R0
	MOV     R0, R0,LSL#16
	MOVS    R0, R0,LSR#16
	STRNEB  R4, [R6,#0x3EE]
	LDRNE   R0, [R6,#0x3B4]
	ORRNE   R0, R0, #0x40
	STRNE   R0, [R6,#0x3B4]
	EOR     R0, R4, #1
	STRB    R0, [R5,#0x3EE]
	ADD     R0, R6, #0x100
	LDRSB   R1, [R0,#0x1E]
	MOV     R0, R5
	BL      0x20A0B5C
	LDR     R0, [R5,#0x3B4]
	ORR     R0, R0, #0x40
	STR     R0, [R5,#0x3B4]
	LDRB    R0, [R8,#0xA6]
	ORR     R0, R0, #2
	STRB    R0, [R8,#0xA6]
	LDRB    R0, [R5,#0x3DE]
	CMP     R0, #2
	ADDEQ   SP, SP, #8
	LDMEQFD SP!, {R4-R8,PC}

	LDRB    R4, [R6,#0x3DD]
	CMP     R4, #8
	ADDCC   R0, R4, #1
	ANDCC   R4, R0, #0xFF
	@STRB    R4, [R6,#0x3DD]			@ Enemy combo counter.
	LDRB    R0, [R5,#0x3DE]
	CMP     R0, #0
	BNE     .Label2.1

	ADD     R0, R6, #0x100
	LDRSB   R5, [R0,#0x1E]
	MOV     R0, R6
	MOV     R1, R4
	MOV     R2, #0
	MOV     R3, #0x18000
	STR     R5, [SP]
	BL      GivePointsOfSomeKind
	B       .Label2.2

.Label2.1:
	CMP     R0, #1
	BNE     .Label2.2

	ADD     R0, R6, #0x100
	LDRSB   R5, [R0,#0x1E]
	MOV     R0, R6
	MOV     R1, R4
	MOV     R2, #0
	MOV     R3, #0x18000
	STR     R5, [SP]
	BL      0x209AA04

.Label2.2:
	CMP     R4, #8
	BCS     .Label2.3

	ADD     R0, R4, #0x70
	ADD     R1, R6, #0x5C
	BL      PlaySNDEffect
	ADD     SP, SP, #8
	LDMFD   SP!, {R4-R8,PC}

.Label2.3:
	ADD     R1, R6, #0x5C
	MOV     R0, #0x77 
	BL      PlaySNDEffect
	ADD     SP, SP, #8
	LDMFD   SP!, {R4-R8,PC}