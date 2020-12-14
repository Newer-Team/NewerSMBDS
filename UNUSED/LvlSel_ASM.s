repl_020CC584_ov_06:			
	BL 		0x2005D04
	BL      clearDebugScreen

	MOV		R0, #1
	B		0x20CC588

repl_02011B74_main:
	BX      LR

repl_020CC718_ov_01:
	MOV     R0, #16
	BX      LR
	
repl_020A19DC_ov_00:
	LDR     R0, =State
	MOV     R1, #0
	STR     R1, [R0]
	MOV     R0, #16
	BX      LR