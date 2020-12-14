repl_020CCE1C_ov_01:
	STMFD   SP!, {R4,LR}
	LDR		R4, =0x2089524
	LDR     R0, =0x9D4
	BL      loadFileByExtId
	MOV     R1, R0
	MOV     R0, R4
	BL      NNS_G2dFontInitAuto
	MOV     R0, #1
	LDMFD   SP!, {R4,PC}

nsub_02014A58_main:
	BX 		LR	
