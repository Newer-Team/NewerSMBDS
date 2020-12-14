repl_021095CC_ov_0A:
	LDR     R0, =#0x24D
	LDRSB   R0, [R4,R0]
	LDRNE   R0, [R4,#0xD0]
	BXNE	LR
	LDR     R0, [R4,#0xD0]
	STMFD	SP!, {R0-R3,LR}
	LDR		R0, [R4, #0x60]
	LSR    	R0, #0xC	
	LDR    	R1, [R4, #0x64]
	MVN    	R1, R1
	LSR    	R1, #0xC
	BL     	getTileBehaviorAtPos2
	LDR		R1, =0x80003000
	CMP		R0, R1
	LDMNEFD	SP!, {R0-R3,PC}
	LDR    	R1, [R4, #0x64]
	SUB		R1, #0x300
	STR		R1, [R4, #0x64]
	LDMFD	SP!, {R0-R3,PC}
