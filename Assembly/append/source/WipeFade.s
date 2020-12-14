repl_02013C08_main:
	LDR     R5, =Vec3_vtable
	CMP		R1, #8
	BXGE    LR
	CMP		R1, #2
	MOVNE 	R1, #0
	BX		LR