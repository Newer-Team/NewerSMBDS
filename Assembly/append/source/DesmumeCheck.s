.globl CheckIfDesmume
CheckIfDesmume:
	MRC P15, 0, R0, C1, C0, 0
	EOR R0, #1
	MCR P15, 0, R0, C1, C0, 0
	LDR R0, =0x0FF00000
	LDR R0, [R0]
	LDR R1, =0xFFFF0000
	LDR R1, [R1]
	CMP R0, R1
	MOVEQ R2, #1
	MOVNE R2, #0
	MRC P15, 0, R0, C1, C0, 0
	EOR R0, #1
	MCR P15, 0, R0, C1, C0, 0
	MOV R0, R2
	BX LR