repl_021142E4_ov_0A:
	CMP     R4, #0
	BEQ     0x21147A4
	
	MOV     R0, #1
	STRB    R0, [R5, #0xCF4]
	
	LDRB    R0, [R5, #0xBA0]
	CMP     R0, #0
	BNE		.AlreadyJumping

	LDRB    R0, [R5, #0x7B7]
	CMP     R0, #0
	MOVEQ   R0, #0
	STREQB  R0, [R5, #0xBA0]
	BXEQ    LR
	LDRB    R0, [R5, #0xBA0]
	CMP     R0, #2
	ADDNE   R0, #1
	MOVEQ   R0, #0
	STRB    R0, [R5, #0xBA0]
	BX      LR

.AlreadyJumping:
	ADD     R0, #1
	CMP     R0, #2
	MOVGT   R0, #0
	STRB    R0, [R5, #0xBA0]
	BX      LR

repl_0211432C_ov_0A:
	LDRB    R1, [R5, #0xCF4]
	CMP     R1, #0
	MOVEQ   R1, R6
	MOVNE   R1, #1
	BX		LR		
	
repl_0211435C_ov_0A:
	MOV     R0, #0
	STRB    R0, [R5, #0xCF4]
	MOV     R0, R5
	BX      LR