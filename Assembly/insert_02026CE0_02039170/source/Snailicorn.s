repl_02141F74_ov_24:
	LDR 	R1, [R4,#8]
	ANDS    R1, R1, #0xF	
	LDR     R5, =0x2143584
	LDR     R0, =0x00008000
	STREQ   R0, [R5, #4]
	BEQ     .TileCols
	MOV     R2, #0x1200
	MUL     R2, R1
	ADD   	R0, R2
	STR     R0, [R5, #4]

.TileCols:
	CMP		R1, #0
	MOVEQ	R1, #0x1000
	BXEQ    LR
	ADD     R1, #1
	MOV     R0, #0x200
	MUL     R0, R1
	MOV     R1, #0x1000
	ADD     R1, R0, R1
	BX 		LR
	
repl_0214239C_ov_24:
repl_021423D0_ov_24:
repl_02142464_ov_24:
repl_02142CB0_ov_24:
repl_02142ECC_ov_24:
	STMFD   SP!, {R0,R1,LR}
	LDR 	R1, [R4,#8]
	ANDS    R1, R1, #0xF	
	MOV     R3, #0x800
	LDMEQFD SP!, {R0,R1,PC}
	MOV   	R0, #0x50
	MUL     R0, R1
	SUB     R3, R0
	LDMFD 	SP!, {R0,R1,PC}
