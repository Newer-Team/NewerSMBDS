repl_021561F8_ov_36:
	STMFD   SP!, {R0-R2, LR}
	LDR		R2, =CameraControllerPtr
	LDR     R1, [R2]
	CMP     R1, #0
	BEQ     .Exit
	MOV     R0, #0
	STR     R0, [R2]
	MOV     R0, R1
	BL		Base__deleteIt
.Exit:
	LDMFD   SP!, {R0-R2, LR}
	LDR     R2, [R4,#0x404]
	BX      LR

repl_02156454_ov_36:
	STMFD   SP!, {R0-R5, LR}
	LDR		R2, =CameraControllerPtr
	LDR     R1, [R2]
	CMP     R1, #0
	BEQ     .Exit_
	MOV     R0, #0
	STR     R0, [R2]
	MOV     R0, R1
	BL		Base__deleteIt
.Exit_:
	LDMFD   SP!, {R0-R5, LR}
	STRB    R2, [R5,#0x11F]
	BX      LR

	
repl_0215648C_ov_36:
repl_02156234_ov_36:
	BX		LR
	