repl_020D3348_ov_0A:		@ Disable MvL coin counting.
	MOV		R0, #1
	BX		LR

repl_020D490C_ov_0A:	 	@ Fire Flowers replace Shells.
	ADD     R0, R8, #0x500
	LDRH    R0, [R0,#0xA0]
	CMP     R0, #1
	BNE     0x20D4964
	CMP     R4, #3
	BEQ     0x20D4934
	CMP     R4, #2
	BNE     0x20D494C
	BEQ     0x20D4934
	

repl_020D494C_ov_0A:
	STMFD   SP!, {R0-R3,LR}
	LDR     R0, =0x0208B32C
	LDRB    R1, [R0]
	CMP     R1, #5
	BEQ     .Return
	CMP     R4, #5
	BNE     .Return
	MOV     R0, R6
	MOV     R1, #3
	BL      0x20BFFDC
.Return:
	LDMFD   SP!, {R0-R3,LR}
	MOV     R0, R7
	B       0x20D4950
	
repl_020D48F4_ov_0A:
	STMFD   SP!, {R0-R3,LR}
	LDR     R0, =0x0208B32C
	LDRB    R1, [R0]
	CMP     R1, #5
	BEQ     .Return2
	CMP     R4, #2
	BNE     .Return2
	MOV     R0, R6 
	MOV     R1, #2
	BL      0x20BFFDC
.Return2:
	LDMFD   SP!, {R0-R3,LR}
	MOV     R0, R7
	B       0x20D48F8

repl_0212BA48_ov_0B:	
	CMP     R2, #0
	BEQ     0x212BA60
	CMP     R2, #1
	BEQ     0x212BA60
	CMP     R2, #5
	BEQ     0x212BA60
	CMP     R2, #4
	BEQ     0x212BA60
	BNE     0x212BABC

nsub_020D4A70_ov_0A:	
	STMFD   SP!, {R4,LR}
	MOV     R4, R1
	MOV     R1, #0
	CMP     R3, #0			@ If nothing is in storage
	MOVEQ   R1, #1			
	BEQ     0x20D4AF0		@ Replace item in storage
	CMP     R3, #5			@ If current item is a Mega Mushroom
	BEQ     0x020D4AF0		@ Don't replace anything
	CMP     R2, #5			@ If item gotten is Mega Mushroom
	MOVEQ   R1, #1
	BEQ     0x20D4AF0		@ Replace item in storage	
	CMP     R2, #1			@ If item gotten isn't a mushroom...
	BEQ		0x20D4AF0
	CMP     R2, R3			@ and item gotten isn't a duplicate
	BEQ		0x20D4AF0
	MOVNE   R1, #1
	B		0x20D4AF0		@ Replace item in storage




