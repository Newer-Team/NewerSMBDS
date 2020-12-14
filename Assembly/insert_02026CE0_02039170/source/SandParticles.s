repl_02022F2C_main:
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x7
	LDREQ   R0, =MyTable
	LDRNE   R0, =0x2026C74
	BX      LR

.data 
.balign 4
	MyTable:
	.word 0x000009C1
	.word 0x000006CB
	.word 0x000006CC