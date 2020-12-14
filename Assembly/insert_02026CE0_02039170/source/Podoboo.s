repl_02147F14_ov_30:
	MOV     R4, R0
	LDR		R1, [R4, #8]
	LSR     R1, #0x8
	ANDS 	R1, R1, #0xF
	BXEQ    LR
	MOV     R1, #0x8000
	LDR     R0, [R4, #0x60]
	ADD     R0, R1
	STR     R0, [R4, #0x60]
	BX      LR

repl_02148688_ov_30:
	STMFD 	SP!, {R1,R2}
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x6
	LDR 	R1,=0x216FCF0
	MOVNE 	R2, #0x600
	ADDNE 	R2, $0x9A
	MOVEQ 	R2, #0x900
	ADDEQ 	R2, #0xA2
	STR 	R2, [R1]
	LDMFD 	SP!, {R1,R2}
	MOV 	R0,#0x15
	BX 		LR

repl_021481BC_ov_30:
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x6
	MOVEQ 	R0, #0x160
	ADDEQ 	R0, R0, #0xD
	MOVNE 	R0, #0x8C
	BX 		LR

repl_02148214_ov_30:
	LDRB 	R0,[R4,#0xD6]
	CMP 	R0,#0xFF
	MOVEQ 	R2,#0x2
	MOVNE 	R2,#0x0
	MOV 	R0,R4
	BX 		LR

repl_02147E1C_ov_30:
	STMFD 	SP!, {R0,R1}
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x6
	MOVEQ 	R1,#0
	STREQB 	R1, [R10,#0x4CE]
	STREQB 	R1, [R10,#0x4D0]
	STREQB 	R1, [R10,#0x4D2]
	LDMFD 	SP!, {R0,R1}
	AND 	R3, R4, #0xFF
	BX 		LR


repl_021480D0_ov_30:
	STMFD	SP!, {R1,LR}
	LDR		R1, [R4, #8]
	AND 	R1, R1, #0xFF
	CMP 	R1, #0
	STREQH 	R0,[R3,#0xCA]
	LDMEQFD SP!, {R1,PC}
	CMP 	R1, #1
	MOVGE 	R0,#0x5
	STRLEH 	R0,[R3,#0xCA]
	LDMLEFD SP!, {R1,PC}
	MUL 	R0, R1, R0
	STRH 	R0, [R3,#0xCA]
	LDMFD 	SP!, {R1,PC}

@DD760