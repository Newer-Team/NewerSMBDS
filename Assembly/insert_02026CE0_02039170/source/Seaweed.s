repl_0213EF68_ov_19:
	STMFD   SP!, {R0, LR}
	MOV     R0, #0x100
	STR     R0, [R4,#0x374]
	STR     R0, [R4,#0x37C]
	STR     R0, [R4,#0x380]	
	MOV     R1, #0
	LDMFD   SP!, {R0, PC}		