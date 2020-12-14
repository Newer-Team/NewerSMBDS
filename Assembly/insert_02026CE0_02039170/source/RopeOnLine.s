repl_02187EB0_ov_61:
	STMFD    	SP!, {R1, R2, LR}
	LDR     	R0, [R4, #8]
	LSR     	R0, #0xC
	AND     	R0, R0, #0xF
	CMP     	R0, #0
	MOVEQ     	R0, R4
	LDMEQFD 	SP!, {R1, R2, PC}
	MOV         R1, #0x200
	MUL         R1, R0
	LDR     	R2, [R4, #0x56C]
	LDR     	R0, [R4, #8]
	LSR     	R0, #0x8
	ANDS    	R0, R0, #0xF
	SUBNE     	R2, R1
	ADDEQ    	R2, R1
	STR     	R2, [R4, #0x56C]
	MOV     	R0, R4
	LDMFD    	SP!, {R1, R2, PC}
