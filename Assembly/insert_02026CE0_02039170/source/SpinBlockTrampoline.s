repl_020DA118_ov_0A:
	LDR		R0, [R4, #0x8]
	ANDS    R0, #0x1
	LDREQ   R0, [R4,#0x44C]
	BXEQ    LR
	LDR     R0, [R4, #0x60]
	ADD     R0, #0x8000
	STR     R0, [R4, #0x60]
	LDR   	R0, [R4,#0x44C]
	BX    	LR	