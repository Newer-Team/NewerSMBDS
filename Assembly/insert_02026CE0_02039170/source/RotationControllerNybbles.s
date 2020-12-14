repl_020D9CC0_ov_0A:		// Coin
	STMFD 	SP!, {R0}
	LDR     R0, [R4,#0x8]
	LSR     R0, #0x10		// Nybble 8
	ANDS    R0, #1
	LDMFD 	SP!, {R0}
	STMFD 	SP!, {LR}
	BLEQ    0x020AF844
	LDMFD 	SP!, {PC}
	
repl_020AB2D4_ov_00:
	LDR     R1, [R4]
	LDR     R0, =0x2000000
	CMP     R1, R0
	LDMLTFD SP!, {R4,PC}
	LDR     R0, =0x23D0900
	CMP     R1, R0	
	LDMGTFD SP!, {R4,PC}
	MOV     R0, R4
	BX      LR
	
repl_02154CA4_ov_36:		// Star Coin
	STMFD 	SP!, {R0}
	LDR     R0, [R4,#0x8]
	LSR     R0, #0x18		// Nybble 6
	ANDS    R0, #1
	LDMFD 	SP!, {R0}
	STMFD 	SP!, {LR}
	BLEQ    0x020CDB80
	LDMFD 	SP!, {PC}
	
repl_02175C64_ov_47:		// Boo
	STMFD 	SP!, {R0}
	LDR     R0, [R4,#0x8]
	LSR     R0, #0x8		// Nybble 10
	ANDS    R0, #1
	LDMFD 	SP!, {R0}
	STMFD 	SP!, {LR}
	BLEQ    0x02177394
	LDMFD 	SP!, {PC}
	
repl_02147E98_ov_30:		// Podoboo
	STMFD 	SP!, {R0}
	LDR     R0, [R10,#0x8]
	LSR     R0, #0xC		// Nybble 9
	ANDS    R0, #0xF
	BEQ     .Original
	CMP     R9, R0
	LDMFD 	SP!, {R0}
	BX      LR
	
.Original:
	LDMFD SP!, {R0}
	CMP   R9, #8
	BX    LR