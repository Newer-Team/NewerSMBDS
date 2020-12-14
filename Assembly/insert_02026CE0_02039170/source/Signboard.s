nsub_02130E9C_ov_0C:
	STMFD	SP!, {LR}
	SUB		SP, SP, #4
	LDR     R0, =pointersToBlocks
	LDR     R0, [R0]
	LDRH    R0, [R0,#0xC]

	CMP     R0, #0x28
	BEQ     .LoadGhost
	CMP     R0, #0x17
	BEQ     .LoadGhost
	CMP     R0, #0x4A
	BEQ     .LoadGhost
	CMP     R0, #0x3C
	BEQ     .LoadGhost
	
	CMP     R0, #0x1D
	BEQ     .LoadUnderground
	CMP     R0, #0x1E
	BEQ     .LoadUnderground
	CMP     R0, #0x49
	BEQ     .LoadUnderground
	CMP     R0, #0x2C
	BEQ     .LoadUnderground

	CMP     R0, #0x2E
	BEQ     .LoadSnow
	CMP     R0, #0x24
	BEQ     .LoadSnow
	CMP     R0, #0x41
	BEQ     .LoadSnow

	LDR     R0, =0x615
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	LDR     R0, =0x616
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	B       .Return

.LoadGhost:
	LDR     R0, =0x9AD
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	LDR     R0, =0x9AE
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	B       .Return

.LoadUnderground:
	LDR     R0, =0x9AF
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	LDR     R0, =0x9B0
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	B       .Return

.LoadSnow:
	LDR     R0, =0x9B1
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	LDR     R0, =0x9B2
	MOV     R1, #0
	BL		loadFileByExtId_3dModel

.Return:
	MOV     R0, #1
	ADD		SP, SP, #4
	LDMFD	SP!, {PC}


repl_02130D20_ov_0C:
	STMFD	SP!, {R1,LR}

	LDR     R1, =pointersToBlocks
	LDR     R1, [R1]
	LDRH    R1, [R1,#0xC]

	LDR     R0, =0x615

	CMP     R1, #0x28
	LDREQ   R0, =0x9AD
	CMP     R1, #0x17
	LDREQ   R0, =0x9AD
	CMP     R1, #0x4A
	LDREQ   R0, =0x9AD
	CMP     R1, #0x3C
	LDREQ   R0, =0x9AD

	CMP     R1, #0x1D
	LDREQ   R0, =0x9AF
	CMP     R1, #0x1E
	LDREQ   R0, =0x9AF
	CMP     R1, #0x49
	LDREQ   R0, =0x9AF
	CMP     R1, #0x2C
	LDREQ   R0, =0x9AF

	CMP     R1, #0x2E
	LDREQ   R0, =0x9B1
	CMP     R1, #0x24
	LDREQ   R0, =0x9B1
	CMP     R1, #0x41
	LDREQ   R0, =0x9B1

	LDMFD	SP!, {R1,PC}




repl_02130D4C_ov_0C:
	STMFD	SP!, {R1,LR}
	
	LDR     R1, =pointersToBlocks
	LDR     R1, [R1]
	LDRH    R1, [R1,#0xC]

	LDR     R0, =0x616

	CMP     R1, #0x28
	LDREQ   R0, =0x9AE
	CMP     R1, #0x17
	LDREQ   R0, =0x9AE
	CMP     R1, #0x4A
	LDREQ   R0, =0x9AE
	CMP     R1, #0x3C
	LDREQ   R0, =0x9AE

	CMP     R1, #0x1D
	LDREQ   R0, =0x9B0
	CMP     R1, #0x1E
	LDREQ   R0, =0x9B0
	CMP     R1, #0x49
	LDREQ   R0, =0x9B0
	CMP     R1, #0x2C
	LDREQ   R0, =0x9B0

	CMP     R1, #0x2E
	LDREQ   R0, =0x9B2
	CMP     R1, #0x24
	LDREQ   R0, =0x9B2
	CMP     R1, #0x41
	LDREQ   R0, =0x9B2

	LDMFD	SP!, {R1,PC}