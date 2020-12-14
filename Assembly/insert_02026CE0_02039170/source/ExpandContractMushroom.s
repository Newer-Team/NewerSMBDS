repl_021822A0_ov_5A:
	LDR		R0, [R6, #8]
	LSR     R0, #0xC
	ANDS    R0, #1
	LDREQ   R0, =0x544
	BXEQ    LR
	LDR     R0, [R6, #0x60]
	ADD     R0, #0x8000
	STR     R0, [R6, #0x60]
	LDR     R0, =0x544
	BX      LR