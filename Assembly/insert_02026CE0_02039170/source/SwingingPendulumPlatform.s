repl_0217E8C8_ov_58:
	LDR     R1, [R4,#8]
	STMFD   SP!, {R0, R1, LR}
	ANDS    R1, #0xF
	MOV     R0, #0x100
	MUL     R0, R1
	LDR     R1, [R4, #0x68]
	ADD     R0, R1
	STR     R0, [R4, #0x68]
	LDMFD   SP!, {R0, R1, PC}
	
	
@126FC0