@Write BG 8192 bytes later in the VRAM
repl_020BD048_ov_00:
	ADD		R1, R3, #0xB000
	BX		LR
	
@Write FG 8192 bytes later in the VRAM
repl_020BD094_ov_00:
	ADD		R1, R3, #0x6000
	BX		LR

@Begin lecture of BG and FG nsc 2 rows later
repl_020BCBB4_ov_00:
	MOV		R5, #0
	MOV		R7, #0x400
	SUB		R7, R7, #1
	LDRH	R4, [R11]
	AND		R6, R4, R7
	CMP		R6, #0x100			@Take into account Tile-Share
	ADDCS	R4, R4, #0x80
	STRH	R4, [R11], #2
	ADD		R5, R5, #1
	CMP		R5, #0x2000
	SUBCC	PC, PC, #0x24
	LDR		R4, =0x20CAD5C
	LDR		R4, [R4]
	CMP		R4, #0
	ADDEQ	PC, PC, #0x18
	LDR		R5, [R4, #-0xC]
	ADD		R5, R4, R5
	LDRH	R6, [R4]
	ADD		R6, R6, #0x80
	STRH	R6, [R4], #2
	CMP		R4, R5
	SUBCC	PC, PC, #0x18
	ADD		SP, SP, #0x24
	LDMFD	SP!, {R4-R11, PC}
	
@TEN animations for some tilesets
repl_020B699C_ov_00:
repl_020B69BC_ov_00:
repl_020B6E98_ov_00:
	ADD		R1, R1, #0x2000
	MOV		R2, #0x180
	BX		LR
	
repl_020B6DB8_ov_00:
	ADD		R1, R1, #0x2000
	MOV		R2, #0x400
	BX		LR
	
repl_020B710C_ov_00:
repl_020B712C_ov_00:
repl_020B714C_ov_00:
repl_020B7168_ov_00:
repl_020B720C_ov_00:
repl_020B722C_ov_00:
repl_020B724C_ov_00:
repl_020B731C_ov_00:
repl_020B733C_ov_00:
repl_020B735C_ov_00:
repl_020B737C_ov_00:
repl_020B739C_ov_00:
repl_020B73B8_ov_00:
repl_020B744C_ov_00:
repl_020B746C_ov_00:
repl_020B748C_ov_00:
repl_020B751C_ov_00:
repl_020B753C_ov_00:
repl_020B755C_ov_00:
repl_020B757C_ov_00:
repl_020B7598_ov_00:
repl_020B762C_ov_00:
repl_020B764C_ov_00:
repl_020B766C_ov_00:
repl_020B768C_ov_00:
repl_020B76A8_ov_00:
repl_020B77E0_ov_00:
repl_020B7804_ov_00:
repl_020B7828_ov_00:
repl_020B78C4_ov_00:
repl_020B78E4_ov_00:
repl_020B7904_ov_00:
repl_020B7920_ov_00:
	ADD		R1, R1, #0x2000
	MOV		R2, #0x100
	BX		LR
	
@Change which Tile Number yields which texture for SubNohara
nsub_020B00EC_ov_00:
	CMP		R1, #0x540
	ADDCC	PC, PC, #0x54
	LDR		R2, =0x545
	CMP		R1, R2
	BHI		0x20B0184
	SUB		R3, R1, #0x540
	MOV		R0, #0x65
	CMP		R3, #1
	MOVEQ	R0, #0x68
	CMP		R3, #2
	MOVEQ	R0, #0x62
	CMP		R3, #3
	MOVEQ	R0, #0x67
	CMP		R3, #4
	MOVEQ	R0, #0x6A
	CMP		R3, #5
	MOVEQ	R0, #0x64
	MOV		R3, R0
	LDR		R0, =0x20CAD44
	LDR		R2, [R0]
	ADD		SP, SP, #4
	STRB	R3, [R2, LR]
	MOV		R0, R1
	LDMFD	SP!, {R4-R5, PC}
	CMP		R1, #0x530
	ADDCC	PC, PC, #0x2C
	LDR		R2, =0x539
	CMP		R1, R2
	BHI		0x20B0184
	SUB		R3, R1, #0x530
	ADD		R3, R3, #0x86
	AND		R3, R3, #0xFF
	LDR		R0, =0x20CAD44
	LDR		R2, [R0]
	ADD		SP, SP, #4
	STRB	R3, [R2, LR]
	MOV		R0, R1
	LDMFD	SP!, {R4-R5, PC}
	CMP		R1, #0x520
	BCC		0x20B0184
	LDR		R2, =0x529
	CMP		R1, R2
	BHI		0x20B0184
	SUB		R3, R1, #0x520
	ADD		R3, R3, #0x7A
	AND		R3, R3, #0xFF
	LDR		R0, =0x20CAD44
	LDR		R2, [R0]
	ADD		SP, SP, #4
	STRB	R3, [R2, LR]
	MOV		R0, R1
	LDMFD	SP!, {R4-R5, PC}
	
@Broken flagpole dynamic palette
repl_02130214_ov_0C:
	LDRH	R0, [R4, #0xC]
	CMP		R0, #0x24
	ADDNE	PC, PC, #0x24
	LDR		R0, =0x20CAD30
	LDR		R0, [R0]
	LDR		R1, =0x635B635A
	LDR		R2, =0x635D635C
	LDRB	R3, [R4, #0xB]
	ANDS	R3, R3, #0x10
	LDRNE	R1, =0x737B737A
	LDRNE	R2, =0x737D737C
	STR		R1, [R0, #0x174]
	STR		R2, [R0, #0x17C]
	LDR		R1, [R4, #0x64]
	BX		LR