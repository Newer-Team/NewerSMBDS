nsub_021729D4_ov_3C:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR     R0, =0x544
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9BD
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}	
	
repl_021729AC_ov_3C:
	LDR		R0, [R5, #8]
	MOV     R0, R0, LSR #8
	ANDS    R0, R0, #0xF
	LDREQ   R0, =0x544
	LDRNE   R0, =0x9BD
	BX		LR
	
repl_021730D8_ov_3C:
	LDR		R0, [R6, #8]
	MOV     R0, R0, LSR #8
	ANDS    R0, R0, #0xF
	LDREQ   R0, [SP,#0x8]
	LDRNE   R0, =0x87
	BX		LR