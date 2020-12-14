repl_0213C380_ov_1B:
	LDR	R0, [R4, #8]
	LDR	R12, [R4, #0x60]
	TST	R0, #0x100					@Check if Shifting flag is set
	SUBEQ	R12, R12, #0x8000		@If yes then shift Swooper 1/2 tile to the right
	STR	R12, [R4, #0x60]
	STR	R12, [R4, #0x70]
	BX	LR
	
repl_0213C39C_ov_1B:
	MOV	R1, #0x10000			@Dynamically change hitbox data for Big Swooper
	STR	R1, [R2, #8]
	STR	R1, [R2, #0xC]
	MOV	R1, #0
	STR	R1, [R2, #4]
	MOV	R1, R4
	BX	LR

repl_0213C1DC_ov_1B:
	LDR	R3, [SP, #0xC]
	TST	R4, #1					@Check if Big flag is set
	ADDEQ	PC, PC, #0x64		@If not R4=0 then return
	LDRH	R4, [R5, #0xC4]
	CMP	R4, #0x100				@Check if Swooper began its 2nd phase
	ADDNE	PC, PC, #0x58		@If not R4=0 then return
	LDR	R4, [R5, #0xD4]
	ADD	R4, R4, #0x1000
	CMP	R4, #0x1000				@Check if current Swooper frame is one of the 16 allowed frames
	ADDCS	PC, PC, #0x48		@If greater than or equal (unsigned) then R4=0 and return
	LDR	R4, [R5, #0xD0]
	CMP	R4, #0					@Check if Swooper goes right or left
	LDR	R4, [R5, #0x134]		@Load Hitbox Horizontal Shifting value
	SUBLT	R4, R4, #0x800		@If it goes left subtract 2048
	ADDGT	R4, R4, #0x800		@If it goes right add 2048
	ADDGE	PC, PC, #0x10
	RSB	R4, R4, #0
	CMP	R4, #0x8000				@Check if H.H.S. is less than -32768
	MOVGE	R4, #0x8000			@If true cap at -32768
	RSB	R4, R4, #0
	ADD	PC, PC, #4
	CMP	R4, #0x8000				@Check if H.H.S. is more than 32768
	MOVGE	R4, #0x8000			@If true cap at 32768
	STR	R4, [R5, #0x134]		@Store H.H.S.
	LDR	R4, [R5, #0x138]		@Load Hitbox Vertical Shifting value
	ADD	R4, R4, #0x1000			@Add 4096
	CMP	R4, #0x10000			@Check if H.V.S. is more than 65536
	MOVGE	R4, #0x10000			@If true cap at 65536
	STR	R4, [R5, #0x138]		@Store H.V.S.
	MOV	R4, #0
	BX	LR						@Return
