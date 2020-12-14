repl_0213ED20_ov_18:
	CMP     R0, #0
	MOVEQ   R2, #0xC0
	MOVNE   R2, #0x100
	LDR		R0, [R4, #8]
	LSR     R0, R0, #20
	CMP     R0, #0
	MOVNE   R2, R0
	ADD     R0, R4, #0x1C00
	LDRH    R1, [R0,#0xC0]
	B		0x213ED34