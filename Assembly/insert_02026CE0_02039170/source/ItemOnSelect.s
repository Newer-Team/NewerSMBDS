hook_0211F844_ov_0A:
	STMFD		SP!, {R0, R1, LR}
	LDR	    	R0, =0x04000130
	LDRB		R0, [R0]
	LSR	    	R0, #2
	ANDS		R0, #1
	LDMNEFD		SP!, {R0, R1, PC}
	LDR			R0, =0x0208B698
	LDR			R1, =0x009500DA
	STR			R1, [R0]
	LDR			R1, =0x09600D40
	STR			R1, [R0, #0x8]
	LDR			R1, =0x1
	STR			R1, [R0, #0x4]
	STR			R1, [R0, #0xC]
	LDMFD		SP!, {R0, R1, PC}