repl_0216734C_ov_36:
	LDR     R0, [R4, #8]
	ANDS    R0, #0xF
	MOVEQ   R0, #0x70
	MOVNE   R0, #0x41
	BX      LR

nsub_0209E2F8_ov_00:
	CMP     R2, #3
	MOVEQ   R4, R2
	BEQ     0x209E2FC
	ANDS    R2, #3
	BEQ     0x209E2FC
	B       0x209E344

repl_0209E33C_ov_00:
	MOV     R1, R4
	BL      createActor
	B		0x209E3DC