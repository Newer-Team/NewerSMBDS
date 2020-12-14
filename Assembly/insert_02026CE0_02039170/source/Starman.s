nsub_020A08C0_ov_00:
	LDRH	R3, [R0, #0xC]
	CMP	R3, #0x1F			@Check for Actor 31 (Item from Brick/Block)
	ADDNE	PC, PC, #0x38			@If the check fails go to the end of the function
	LDR	R1, [R0, #8]
	MOV	R3, R1, LSR #0x18
	CMP	R3, #0x30			@Check if Power-up is thrown sideways
	ADDNE	PC, PC, #0x28			@If the check fails go to the end of the function
	MOV	R1, R1, LSL #0x1B		@Pseudo-modulo 32
	MOV	R1, R1, LSR #0x1B		@"
	CMP	R1, #1				@Check if Power-up is Starman
	ADDNE	PC, PC, #0x18			@If the check fails go to the end of the function
	LDR	R3, =0x20D38C0
	LDRB	R1, [R0, #0x1C8]
	CMP	R1, #0				@Check if hitbox is set
	ADDNE	PC, PC, #4			@If yes then bypass address change
	CMP	R2, #0				@Check if Starman relative height is below 0
	LDRLT	R3, =0x20D3CBC			@If yes then change address in R3
	STR	R3, [R0, #0x53C]
	LDR	R3, [R0, #0xD4]
	LDR	R1, [R0, #0xE4]
	BX	LR