nsub_02166F9C_ov_36:
	LDR     R0, =0x9AB
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, R4
	B		0x02166FA0

repl_02166DF0_ov_36:
	CMP     R0, #0x17
	LDRNE   R0, =0x601
	LDREQ   R0, =0x9AB
	BX      LR
