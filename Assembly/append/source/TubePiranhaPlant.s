repl_02142EC8_ov_20:	
	MOV     R4, R0
	MOV     R0, #4
	STRB    R0, [R4,#0x144]
	STRB    R0, [R4,#0x500]
	MOV     R0, R4
	BX 		LR

repl_02142944_ov_20:
	MOV     R4, R0
	MOV     R0, #0
	STRB    R0, [R4,#0x144]
	STRB    R0, [R4,#0x500]
	MOV     R0, R4
	BX 		LR
	

