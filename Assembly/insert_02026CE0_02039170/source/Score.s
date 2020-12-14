nsub_020D8978_ov_09:
	LDR		R0, =0x216F8E8
	LDRH	R1, [R0, #4]
	ANDS	R1, R1, #0x100
	ADDNE	PC, PC, #0x8C
	LDR	R1, [R0]
	STR	R1, [R0, #-0xC0]
	STR	R1, [R0, #0x28]
	STR	R1, [R0, #-0x80]
	STR	R1, [R0, #-0x28]
	STR	R1, [R0, #-0x158]
	STR	R1, [R0, #0x20]
	STR	R1, [R0, #-0x70]
	STR	R1, [R0, #-0xF8]
	STR	R1, [R0, #-0x170]
	ADD	R0, R0, #4
	MOV	R1, #0
	SUB	R1, R1, #0x10000
	ADD	R1, #0x1000
	CMP	R2, #1
	ADDEQ	R1, #0x2000
	STR	R1, [R0, #-0xC0]
	ADD	R1, R1, #2
	STR	R1, [R0, #0x28]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0x80]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0x28]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0x158]
	ADD	R1, R1, #2
	STR	R1, [R0, #0x20]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0x70]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0xF8]
	ADD	R1, R1, #2
	STR	R1, [R0]
	ADD	R1, R1, #2
	STR	R1, [R0, #-0x170]
	BX	LR
	ADD	R0, R0, #5
	LDRB	R1, [R0]
	AND	R1, R1, #0x1F
	CMP	R2, #1
	ADDEQ	R1, R1, #0x20
	STRB	R1, [R0, #-0xC0]
	STRB	R1, [R0, #0x28]
	STRB	R1, [R0, #-0x80]
	STRB	R1, [R0, #-0x28]
	STRB	R1, [R0, #-0x158]
	STRB	R1, [R0, #0x20]
	STRB	R1, [R0, #-0x70]
	STRB	R1, [R0, #-0xF8]
	STRB	R1, [R0]
	STRB	R1, [R0, #-0x170]
	BX	LR