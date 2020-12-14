repl_02186E08_ov_62:
	LDRB    R0, [R5,#0x11D]
	CMP		R0, #1
	BNE		0x2186E0C
	LDR     R0, =0x9C4
	ADD     R0, R5, R0
	BL		0x20AB3B4	
	B		0x2186E0C

nsub_02187110_ov_62:
	STRH    R2, [R0,#0x8A]
	LDR     R0, =0x9C4
	ADD     R0, R4, R0
	BL		0x20AB3B4
	LDMFD   SP!, {R4,PC}

repl_02187794_ov_62:
repl_02186EDC_ov_62:
repl_02185DD4_ov_62:
repl_0218641C_ov_62:
	LDR		R0, =0x9F9
	BX		LR

repl_02185DFC_ov_62:
repl_02186428_ov_62:
repl_021877A0_ov_62:
repl_02186F04_ov_62:
	LDR		R0, =0x9FA
	BX		LR

