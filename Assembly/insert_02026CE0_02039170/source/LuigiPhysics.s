repl_0215EAD4_ov_36:
	STMFD	SP!, {R2-R9, LR}
	MOV		R0, R5, LSL R9
	LDR		R2, =0x2085A50
	LDRB	R2, [R2]
	LDR		R3, =0x212E2D8
	CMP		R2, #1
	LDRNE	R4, =0x3A80
	MOVNE	R5, #0x90
	MOVNE	R6, #0xB0
	MOVNE	R7, #0x80
	MOVNE	R8, #0x52
	MOVNE	R9, #0x180
	LDREQ	R4, =0x4010
	MOVEQ	R5, #0x60
	MOVEQ	R6, #0x88
	MOVEQ	R7, #0x64
	MOVEQ	R8, #0x30
	MOVEQ	R9, #0x70
	STR	R4, [R3]
	STR	R4, [R3, #0x40]
	STR	R4, [R3, #0x80]
	STR	R5, [R3, #0xE0]
	STR	R5, [R3, #0x100]
	STR	R6, [R3, #0xE4]
	STR	R6, [R3, #0x104]
	STR	R7, [R3, #0xE8]
	STR	R7, [R3, #0x108]
	STR	R8, [R3, #0xEC]
	STR	R8, [R3, #0x10C]
	STR	R9, [R3, #0xFC]
	LDR	R3, =0x216F8E8
	LDRH	R4, [R3, #4]
	ANDS	R4, R4, #0x100
	BNE	.AlternateCalculation
	LDR	R4, [R3]
	STR	R4, [R3, #-0xC0]
	STR	R4, [R3, #0x28]
	STR	R4, [R3, #-0x80]
	STR	R4, [R3, #-0x28]
	STR	R4, [R3, #-0x158]
	STR	R4, [R3, #0x20]
	STR	R4, [R3, #-0x70]
	STR	R4, [R3, #-0xF8]
	STR	R4, [R3, #-0x170]
	ADD	R3, R3, #4
	MOV	R4, #0
	SUB	R4, R4, #0x10000
	ADD	R4, #0x1000
	CMP	R2, #1
	ADDEQ	R4, #0x2000
	STR	R4, [R3, #-0xC0]
	ADD	R4, R4, #2
	STR	R4, [R3, #0x28]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0x80]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0x28]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0x158]
	ADD	R4, R4, #2
	STR	R4, [R3, #0x20]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0x70]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0xF8]
	ADD	R4, R4, #2
	STR	R4, [R3]
	ADD	R4, R4, #2
	STR	R4, [R3, #-0x170]
	B	.Exit
.AlternateCalculation:
	ADD	R3, R3, #5
	LDRB	R4, [R3]
	AND	R4, R4, #0x1F
	CMP	R2, #1
	ADDEQ	R4, R4, #0x20
	STRB	R4, [R3, #-0xC0]
	STRB	R4, [R3, #0x28]
	STRB	R4, [R3, #-0x80]
	STRB	R4, [R3, #-0x28]
	STRB	R4, [R3, #-0x158]
	STRB	R4, [R3, #0x20]
	STRB	R4, [R3, #-0x70]
	STRB	R4, [R3, #-0xF8]
	STRB	R4, [R3]
	STRB	R4, [R3, #-0x170]
.Exit:
	LDMFD	SP!, {R2-R9, PC}	