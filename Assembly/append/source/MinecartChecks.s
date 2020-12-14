repl_020A97B8_ov_00:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	MOVNE   R0, #0
	BLEQ    0x1FFE0E4
	B       0x20A97BC

	
nsub_020A9A58_ov_00:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	MOVNE   R0, #0
	BXNE    LR
	STMEQFD SP!, {R4-R9,LR}
	BLEQ    0x20A9A5C 

	
nsub_02100260_ov_0A:
	LDR 	R1, =MarioUsingCart
	LDRB	R1, [R1]
	CMP  	R1, #0	
	LDREQ 	R1, [R0,#0x9B0]
	BEQ   	0x2100264
	BXNE  	LR
	
nsub_02104334_ov_0A:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	STR     R12, [SP,#0]
	BLEQ    0x2043674
	B       0x210433C
	
nsub_02104354_ov_0A:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	STR     R12, [SP,#0]
	BLEQ    0x2043674
	B       0x210435C
	
nsub_020FF6E8_ov_0A:
	LDR 	R1, =MarioUsingCart
	LDRB	R1, [R1]
	CMP  	R1, #0	
	LDREQ   R1, [R0,#0x788]
	BEQ		0x20FF6EC
	BNE		0x20FF754
	
repl_02108A14_ov_0A:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	BLEQ    0x20AB010
	B       0x2108A18

nsub_021089AC_ov_0A:
	STMFD   SP!, {R0}
	LDR 	R0, =MarioUsingCart
	LDRB	R0, [R0]
	CMP  	R0, #0	
	LDMFD   SP!, {R0}
	STREQ   R0, [R9,#0x7D8]	
	BEQ 	0x21089B0
	BNE		0x21089E4