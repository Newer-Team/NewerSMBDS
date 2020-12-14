repl_02165B5C_ov_36:
repl_021659A8_ov_36:
	STMFD   	SP!, {LR}
	LDR 		R0, =0x2088BFC
	LDR 		R0,[R0]
	CMP 		R0,#0x0
	LDREQ 		R0, =0x9A7
	LDMEQFD   	SP!, {PC}
	CMP 		R0,#0x6
	LDREQ 		R0, =0x9A6
	LDMEQFD   	SP!, {PC}
	CMP 		R0,#0x7
	LDREQ 		R0, =0x9C6
	LDMEQFD   	SP!, {PC}
	LDR 		R0, =0x725
	LDMFD   	SP!, {PC}
	
repl_0216578C_ov_36:
	MOV     	R1, #0x280000
	LDRH        R12, [R5, #0xC]
	CMP         R12, #0x118 		
	BXNE        LR
	LDR         R12, [R5, #0x8]
	LSR         R12, R12, #0x8
	ANDS        R12, R12, #0xF
	MOVNE       R1, #0x0
	BX			LR