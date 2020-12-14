repl_021551C4_ov_36:
	MOV	R1, #1				@Mario will throw both Big and Small Springboard the same way
	BX	LR				@Also has the added benefit of despawning a Big Springboard when thrown into a pit

repl_0215524C_ov_36:
	CMP	R5, #0x10000
	MOVGE	R5, #0x10000
	MOVLT	R5, #0x8000
	STR	R5, [R4, #0x4D4]		@Equalize hitbox's length and height to the Springboard's
	STR	R5, [R4, #0x4D8]		@"
	LDR	R1, =0x216C030
	MOV	R12, #0
	STR	R12, [R1]			@Make Top collision for Big Springboard equal to 0
	STR	R12, [R1, #4]			@"
	STR	R12, [R1, #0x30]		@Make Bottom collision for Big Springboard equal to 0
	STR	R12, [R1, #0x34]		@"
	MOV	R12, #0x1000
	STR	R12, [R1, #0x10]		@Shave 1/32nd of Big Springboard height to get bottom Side collision
	STR	R12, [R1, #0x20]		@Shave 1/16th of Small Springboard height to get bottom Side collision
	MOV	R12, #0xF000
	STR	R12, [R1, #8]			@Top collision at 15/16th of a tile beginning from the bottom (like a Small Spgbrd.)
	STR	R12, [R1, #0x24]		@Shave 1/16th of Small Springboard height to get top Side collision
	MOV	R12, #0x10000
	STR	R12, [R1, #0x18]		@Distance between Right and Left collision is equal to Big Springboard length
	MOV	R12, #0x1F000
	STR	R12, [R1, #0x14]		@Shave 1/32nd of Big Springboard height to get top Side collision
	BX	LR