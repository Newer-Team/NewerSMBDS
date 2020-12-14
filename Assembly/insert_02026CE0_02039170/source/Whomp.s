repl_0214329C_ov_22:
	LDR     R0, [R4,#8]
	STMFD   SP!, {R0, LR}
	LSR     R0, #0x8
	ANDS    R0, #1
	LDMEQFD SP!, {R0, PC}
	LDR     R0, [R4,#0x60] 
	ADD     R0, #0x8000
	STR     R0, [R4,#0x60]
	LDMFD   SP!, {R0, PC}
	
repl_0214369C_ov_22:
	MOV     R1, #0x40
	STR     R1, [R4,#0x380]
	MOV     R1, #0x20
	BX      LR