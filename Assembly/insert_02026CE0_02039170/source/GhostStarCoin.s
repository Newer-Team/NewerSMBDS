repl_02154690_ov_36:
	LDR		  R2, [R5,#8]
	LSR 	  R2, R2, #28
	AND 	  R2, R2, #0xF
	CMP 	  R2, #1
	MOVEQ     R2, #0
	LDRNEB    R2, [R0]
	

repl_02154678_ov_36:
	CMP 	  R0, #0
	BXEQ	  LR
	LDR		  R0, [R5, #8]
	LSR 	  R0,R0,#28
	AND 	  R0, R0, #0xF
	CMP 	  R0, #1
	MOVEQ     R0, #0
	BX 		  LR

repl_02154B78_ov_36:
	LDR 	  R0, [R4, #8]
	LSR 	  R0,R0,#28
	AND 	  R0, R0, #0xF
	CMP 	  R0, #1
	LDR       R0,=0x0212945C
	LDRSH     R0, [R0,#(0x2129462 - 0x212945C)]
	RSBEQ     R0,#0x1
	BX 		  LR

repl_02154C54_ov_36:
	MOV 	R4, R0
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	BEQ     0x02154CA8
	BXNE 	LR	
	
repl_02154CB0_ov_36:	
	STMFD	SP!, {LR}
	BL	    0x02012398
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	LDREQ 	R0, =0xA2
	ADDEQ   R1, R4, #0x5C
	BLEQ    0x02012398
	LDMFD 	SP!, {PC}

repl_02154D08_ov_36:
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	LDRNE   R0, =0xF8
	LDREQ   R0, =0x1D
	BX  	LR
	
repl_02154D28_ov_36:
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	LDRNE   R0, =0xF9
	LDREQ   R0, =0xFA
	BX 	 	LR
	
repl_02154D34_ov_36:
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	LDRNE   R0, =0xFA
	LDREQ   R0, =0x6B
	BX  	LR

repl_02154D3C_ov_36:
	LDR     R0, [R4,#0x508]
	CMP 	R0, #0
	BXNE    LR 
	LDR 	R0, [R4, #8]
	LSR 	R0,R0,#28
	AND 	R0, R0, #0xF
	CMP 	R0, #1
	MOVLT   R0, #0
	BX  	LR