nsub_021144D8_ov_0A:		@ jump_e
	CMP		 R1, #0x53
	LDRNE    R0, [R2,#4]
	BNE      0x21144DC
	STMFD   SP!, {R0-R5, LR}
	BL      0x200E6F4
	LDR     R1, =0x7FFF
	MOV     R2, #3
	AND     R0, R0, R1
	MUL     R1, R0, R2
	MOV     R0, R1,LSR#15
	AND     R4, R0, #0xFF
	CMP     R4, #1
	LDMFD   SP!, {R0-R5, LR}
	MOVEQ   R1, #0x54
	LDR     R0, [R2,#4]
	B       0x21144DC
	
nsub_02113DF4_ov_0A:		@ Luigi fall
	MOV     R2, #0x800
	
	LDR 	R0, =0x2085A50
	LDRB 	R0, [R0]
	CMP		R0, #1
	BNE 	0x2113DF8		@ If not Luigi...
	
	CMP     R1, #6
	BNE 	0x2113DF8		@ If not doing the fall animation...
	
	LDR     R0, =0x688
	ADD     R1, R4, R0
	MOV     R0, R4
	LDR	    R1, [R1]
	CMP     R1, #0
	MOVNE   R1, #6	
	BNE 	0x2113DF8		@ If holding something...
	
	MOV     R1, #106	
	MOV     R2, #0x3000
	B 		0x2113DF8

nsub_02114134_ov_0A:		@ Luigi fall block/gp cancel
	LDR 	R1, =0x2085A50
	LDRB 	R1, [R1]
	CMP		R1, #1	
	MOVNE   R1, #6
	BNE     0x2114138       @ If not Luigi...

	LDR     R0, =0x688 
	ADD     R1, R4, R0
	MOV     R0, R4
	LDR		R1, [R1]
	CMP     R1, #0
	MOVNE   R1, #6
	BNE     0x2114138       @ If holding something...
	
	MOV     R1, #0x3000
	STR     R1, [SP,#0]
	MOV	    R1, #106
	B       0x2114138
	
nsub_021144F4_ov_0A:		@ Luigi jump
	LDR 	R2, =0x2085A50
	LDRB 	R2, [R2]
	CMP		R2, #1	
	MOV	    R2, #0
	BNE 	0x21144F8		@ If not Luigi...
	
	CMP     R1, #5
	BNE  	0x21144F8		@ If not doing the jump animation...

	LDR     R2, =0x688 
	ADD     R1, R5, R2
	MOV	    R2, #0
	LDR     R1, [R1]
	CMP     R1, #0
	MOVEQ   R1, #105
	MOVNE   R1, #5
	B 		0x21144F8
	