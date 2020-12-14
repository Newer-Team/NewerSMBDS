repl_020AEDB8_ov_00:
repl_020BAA34_ov_00: 
repl_020BC1B4_ov_00:
repl_020AEA64_ov_00:
repl_020BC1F8_ov_00:
repl_020BC9C4_ov_00:
	MOV R0, #0
	BX LR
	
repl_020AEB8C_ov_00:
	BL      getUnk2ForView
	CMP     R0, #9
	MOVEQ   R0, #2
	MOVNE   R0, #0
	B       0x20AEB90