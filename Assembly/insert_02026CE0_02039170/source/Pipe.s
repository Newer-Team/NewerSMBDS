nsub_021175B0_ov_0A:
	MOV	R12, LR			@Center Mario upon entering a vertical pipe
	MOV	R2, R12
	CMP	R3, R12
	BLT	0x21175C0
	B	0x21175E4
	
repl_0211D0A4_ov_0A:
	STMFD	SP!, {R0,LR}
	ADD	R1, PC, #0x30
	ADD	LR, PC, #0
	BX	R1
	LDR	R1, [R5, #0x64]
	ADD	R1, R1, R0		@Begin level from pipe at Start or Midway
	STR	R1, [R5, #0x64]
	LDMFD	SP!, {R0,PC}
	
repl_021189F8_ov_0A:
	STMFD	SP!, {LR}
	ADD	R1, PC, #0x10
	ADD	LR, PC, #0
	BX	R1
	LDR	R1, [SP, #0x10]
	ADD	R1, R1, R0		@Load new area from pipe entrance
	LDMFD	SP!, {PC}
	
@relative function:
	STMFD	SP!, {LR}
	LDR	R0, =0x20CA8FE
	LDRB	R1, [R0]		@Load current entrance (not always equal to Entrance ID) from placeholder entrance
	MOV	R0, #0x14
	MUL	R1, R0, R1
	LDR	R0, =0x208B144
	LDR	R0, [R0, #0x38]
	ADD	R0, R0, R1
	LDRB	R1, [R0, #0xE]
	MOV	R0, #0
	CMP	R1, #5			@Check if entrance is from left pipe
	ADDEQ	PC, PC, #0x20
	CMP	R1, #6			@Check if entrance is from right pipe
	ADDEQ	PC, PC, #0x18
	CMP	R1, #0x12		@Check if entrance is from left mini-pipe
	ADDEQ	PC, PC, #0x10
	CMP	R1, #0x13		@Check if entrance is from right mini-pipe
	ADDEQ	PC, PC, #8
	CMP	R1, #0x18		@Check if entrance is from left connected mini-pipe
	ADDEQ	PC, PC, #0
	CMP	R1, #0x19		@Check if entrance is from right connected mini-pipe
	MOVEQ	R0, #0x2000		@If one of the 6 conditions is true, elevate Mario by 1/8th of a tile
	LDMFD	SP!, {PC}
	
repl_0215E8E0_ov_36:
	LDRB	R1, [R0, #8]
	LDR	R2, =0x2085A30
	LDRB	R2, [R2]
	CMP	R2, #0xFF		@Check if midway is set
	LDRNEB	R1, [R0, #9]		@If yes then later overwrite Start entrance with Midway entrance
	BX	LR
	
repl_0201E998:
	LDR	R12, =0x208B090
	LDRB	R9, [R4, #0xF]
	TST	R9, #8			@Check if Connected Pipe flag is set
	LDRNEB	R9, [R4, #0xA]		@If true then load Connected Pipe ID for this entrance
	MOVEQ	R9, #0			@Else force Connected Pipe ID checker to 0
	STR	R9, [R12]		@Write Connected Pipe ID for easier Pipe ID checking (Mostly to write a Pipe ID of 0)
	LDR	R12, =0x20CA8FE
	STRB	R3, [R12]		@Set placeholder entrance for easier entrance checking upon entry
	LDRH	R9, [R4]
	BX	LR

repl_0201E668:
	LDR	R3, =0x20CA8FE
	STRB	R2, [R3]		@Set placeholder entrance for easier entrance checking upon exit
	MOV	R2, #0
	BX	LR
	
repl_02117308_ov_0A:
	STMFD	SP!, {R0,LR}
	LDRH	R0, [R7, #0xE]
	AND	R0, R0, #1
	CMP	R0, #1			@Check if the connected pipe exit is facing right or left
	MOVEQ	R0, #0x2000		@If true elevate Mario by 1/8th of a tile
	ADDEQ	R3, R3, R0
	STR	R3, [R6, #0x64]
	CMP	R4, #1
	LDMFD	SP!, {R0,PC}
	
nsub_0211761C_ov_0A:
	ADD	R2, R0, #0x700
	LDRB	R3, [R2, #0xB2]
	CMP	R3, #2			@Check if Mario goes inside a Pipe Cannon or World-Cannon
	ADDEQ	PC, PC, #0x64		@If true return
	ADD	R2, R2, #0x400
	LDRB	R2, [R2, #0xAD]
	CMP	R2, #1			@Check if Mario is not (completely) inside a pipe
	ADDEQ	PC, PC, #0xC		@If true jump to main calculation
	LDR	R2, =0x208B090
	LDRB	R2, [R2]
	CMP	R2, #0			@Check if Connected Pipe ID is set
	ADDNE	PC, PC, #0x44		@If true return
	LDR	R2, =0x20CA8FE
	LDRB	R3, [R2]		@Load current entrance from placeholder entrance
	MOV	R2, #0x14
	MUL	R3, R2, R3
	LDR	R2, =0x208B144
	LDR	R2, [R2, #0x38]
	ADD	R2, R2, R3
	LDRB	R3, [R2, #0xE]
	LDRH	R2, [R2, #2]		@Get entrance vertical coordinate
	CMP	R3, #0x10		@Check if Mario is gunning for a Mini-Pipe
	MOVGE	R3, #0x10		@If true then add 16 to get bottom position
	MOVLT	R3, #0x20		@Else add 32
	ADD	R2, R2, R3		@Calculate Mario bottom vertical position
	MOV	R2, R2, LSL #0xC	@Change the vertical value to the right format
	RSB	R2, R2, #0		@"
	STR	R2, [R0, #0x64]		@Store Mario vertical position
	MOV	R3, #0x2000
	STRH	R3, [R0, #0x64]		@Elevate further Mario by 1/8th of a tile
	LDR	R3, [R0, #0x930]
	B	0x2117620