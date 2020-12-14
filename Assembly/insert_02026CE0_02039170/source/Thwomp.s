@B3E80

nsub_0213E364_ov_18:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR     R0, =0x4F2
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x4F3
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	LDR     R0, =0x9CE
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}

repl_0213DD08_ov_18:
	LDR     R0, [R4, #8]
	ANDS    R0, R0, #0xF
	LDREQ   R0, =0x4F2
	LDRNE   R0, =0x9CE
	BX      LR