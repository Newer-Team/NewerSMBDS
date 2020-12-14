@Sub Sprite Ext Palette can only be mapped into Bank I
@But Bank I already hosts Sub Sprite
@Change this!
repl_020CFD08_ov_08:
	MOV	R0, #1				@Force Main BG to be mapped into Bank A
	BX	LR
	
repl_020CFD38_ov_08:
	MOV	R0, #8				@Force Sub Sprite to be mapped into Bank D
	BX	LR

repl_020CFD40_ov_08:
	STMFD	SP!, {LR}
	MOV	R0, #0x100
	BL	.SetBankForSubOBJExtPal
	LDR	R1, =0x2085B18
	LDMFD	SP!, {PC}
	
@repl_020BD2A0_ov_00:
@	LDR     R0, =512
@	BL		allocFromGameHeap
@	MOV     R1, R0
@	LDR     R0, =1927
@	STMFD   SP!, {R1}
@	BL 		loadFileByExtId_Dest
@	LDMFD   SP!, {R1}
@	STMFD   SP!, {R0}
@	MOV     R0, R1
@	LDR 	R1, =0x5000200
@	LDR     R2, =512
@	BL      0x02066F28
@	LDMFD   SP!, {R0}
@	BL 		freeToGameHeap	
@	B 		0x20BD2B8
	

repl_020D0194_ov_08:
	STMFD	SP!, {LR}
	BL	0x20099C8
	MOV	R0, #0x780
	MOV	R1, #0
	MOV	R2, R1
	BL	.PrepareSubOBJExtPal
	LDMFD	SP!, {PC}

.PrepareSubOBJExtPal:
	STMFD	SP!, {R4-R6, LR}
	MOV	R6, R1
	CMP	R2, #0
	BEQ	.LoadFileByExtId
	BL	0x200869C
	MOV	R5, R0
	B	.Begin
.LoadFileByExtId:
	BL	0x20087F0
	MOV	R5, R0
.Begin:
	CMP	R5, #0
	LDMEQFD	SP!, {R4-R6, PC}
	LDR	R0, =0x208B72C
	MOV	R1, R5
	LDR	R0, [R0]
	BL	0x2044CC8
	MOV	R4, R0
	BL	.BeginLoadSubOBJExtPal
	MOV	R0, R5
	MOV	R1, R6
	MOV	R2, R4
	BL	.LoadSubOBJExtPal
	BL	.EndLoadSubOBJExtPal
	MOV	R0, R5
	BL	0x20087C0
	LDMFD	SP!, {R4-R6, PC}

repl_020CDD9C_ov_09:
	LDR	R0, =0x216E72C
	ADD	R0, R0, #1
	MOV	R2, #0
.LoopPaletteMode:
	LDRB	R1, [R0]
	ANDS	R1, R1, #0x20
	LDREQB	R1, [R0]
	ADDEQ	R1, R1, #0x20
	STREQB	R1, [R0], #8
	ADD	R2, R2, #1
	CMP	R2, #0x69
	BCC	.LoopPaletteMode
	LDMFD	SP!, {PC}

repl_020CDD8C_ov_09:
	STMFD	SP!, {R0}
	LDR		R0, =0x216E730
	LDRH	R1, [R0]
	LDR		R2, =0x3FF
	AND		R1, R1, R2
	CMP		R1, #0x48
	BNE		.Return
	MOV		R1, #0
.LoopTileNumber:
	LDRH	R2, [R0]
	STMFD	SP!, {R1}
	MOV		R1, R2, LSL #0x16
	MOV		R1, R1, LSR #0x15
	AND		R2, R2, #0xC00
	ADD		R2, R2, R1
	LDMFD	SP!, {R1}
	STRH	R2, [R0], #8
	ADD		R1, R1, #1
	CMP		R1, #0x69
	BCC		.LoopTileNumber
	
.Return:
	LDMFD	SP!, {R0}
	MOV		R1, #0
	BX		LR
	
repl_020D0D58_ov_08:
	MOV	R0, #0xF7			@Common coefficient for un-greying map-icons
	STR	R0, [SP, #0x1C]
	STR	R0, [SP, #0x20]
	STR	R0, [SP, #0x24]
	STR	R0, [SP, #0x28]
	BX	LR

repl_020D0E88_ov_08:
repl_020D0E90_ov_08:
repl_020D0E98_ov_08:
repl_020D0EA0_ov_08:
	LDR	R2, [SP, #0x1C]
	ADD	R2, R2, R8			@Add World number (R8) to map-icon coefficient (R2)
	BX	LR
	
repl_020D0F08_ov_08:
	MOV	R7, #0xF7			@Coefficient for un-greying roads
	BX	LR
	
.SetBankForSubOBJExtPal:
	STMFD	SP!, {LR}
	SUB	SP, SP, #4
	LDR	R1, =0x2094248
	MVN	R12, R0
	LDRH	R2, [R1, #0x18]
	LDRH	R3, [R1]
	CMP	R0, #0
	STRH	R0, [R1, #0x18]
	ORR	R2, R3, R2
	AND	R2, R12, R2
	STRH	R2, [R1]
	BEQ	.AlternateMethod
	CMP	R0, #0x100
	BNE	.VRAMCountLCDC
	LDR	R3, =0x4001000
	LDR	R0, =0x4000249
	LDR	R2, [R3]
	MOV	R1, #0x83
	ORR	R2, R2, #0x80000000
	STR	R2, [R3]			@Enable Sub Sprite Ext Palette by setting [B]DISPCNT bit 31 at 1
	STRB	R1, [R0]			@Set Bank I (0x4000249) as Sub Sprite Ext Palette (0x83)
	B	.VRAMCountLCDC
.AlternateMethod:
	LDR	R1, =0x4001000
	LDR	R0, [R1]
	BIC	R0, R0, #0x80000000
	STR	R0, [R1]			@Disable Sub Sprite Ext Palette by setting [B]DISPCNT bit 31 at 0
.VRAMCountLCDC:
	LDR	R0, =0x2094248
	LDRH	R0, [R0]
	BL	0x2061D7C
	ADD	SP, SP, #4
	LDMFD	SP!, {LR}
	BX	LR
	
.BeginLoadSubOBJExtPal:
	STMFD	SP!, {LR}
	SUB	SP, SP, #4
	BL	0x2061164
	LDR	R1, =VariableSubOBJExtPal
	STR	R0, [R1]
	ADD	SP, SP, #4
	LDMFD	SP!, {LR}
	BX	LR
	
.LoadSubOBJExtPal:
	STMFD	SP!, {R4-R5, LR}
	SUB	SP, SP, #0xC
	LDR	R3, =0x2085714
	MVN	R12, #0
	LDR	LR, [R3]
	MOV	R5, R0
	MOV	R4, R1
	MOV	R3, R2
	CMP	LR, R12
	BEQ	.AlternateMethod2
	LDR	R2, =0x68A0000
	MOV	R12, #0
	STR	R12, [SP, #0]
	MOV	R0, LR
	MOV	R1, R5
	ADD	R2, R4, R2
	STR	R12, [SP, #4]
	BL	0x2066A60
	ADD	SP, SP, #0xC
	LDMFD	SP!, {R4-R5, LR}
	BX	LR
.AlternateMethod2:
	LDR	R1, =0x68A0000
	ADD	R1, R4, R1
	BL	0x2066EAC
	ADD	SP, SP, #0xC
	LDMFD	SP!, {R4-R5, LR}
	BX	LR

.EndLoadSubOBJExtPal:
	STMFD	SP!, {LR}
	SUB	SP, SP, #4
	LDR	R0, =0x2085714
	MVN	R1, #0
	LDR	R0, [R0]
	CMP	R0, R1
	BEQ	.AfterDMA
	BL	0x1FFA358
.AfterDMA:
	LDR	R0, =VariableSubOBJExtPal
	LDR	R0, [R0]
	BL	.SetBankForSubOBJExtPal
	LDR	R0, =VariableSubOBJExtPal
	MOV	R1, #0
	STR	R1, [R0]
	ADD	SP, SP, #4
	LDMFD	SP!, {LR}
	BX	LR
	
.data
.balign 4
VariableSubOBJExtPal:
	.word 0x0