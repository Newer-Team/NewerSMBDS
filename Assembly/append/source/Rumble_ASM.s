repl_02004F28_main:			@ Main Rumble Execute
	BL 		DoRumble
	B		0x2004EC8

nsub_020F96AC_ov_0A:		@ Screen Shakes
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #1
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	MUL     R4, R0, R4
	B		0x20F96B0

nsub_0212089C_ov_0A:		@ Powerup get and lose
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #1
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	LDRB    R1, [R0,#0xBC1]
	B		0x21208A0

nsub_02119714_ov_0A:		@ Dying
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #2
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	MOV     R0, #0x180000
	B		0x2119718

repl_02119A24_ov_0A:		@ Dying 2
repl_021198B0_ov_0A:		@ Dying 3
repl_02119B88_ov_0A:		@ Dying 4
repl_0211B488_ov_0A:		@ Dying 5
repl_0211C054_ov_0A:		@ Flagpole Fireworks
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #1
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	MOV     R0, R4
	BX 		LR

nsub_020F8710_ov_0A:		@ Pipe Cannon
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #2
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	LDR     R2, =0x2122630
	B 		0x20F8714


nsub_0217F250_ov_59:		@ Warp Cannon
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #4
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	LDR     R1, =0x544
	B 		0x0217F254

nsub_02112248_ov_0A:		@ Groundpounding
	STMFD   SP!, {R0-R12, LR}
	MOV		R0, #1
	BL 		Rumble
	LDMFD	SP!, {R0-R12, LR}
	LDR     R1, =0x14D
	B		0x211224C
	
	
@ Rumble Pak Menu Code

@ This is for right
repl_020C1998_ov_00:
LDR     R0, =RumblePakType		@ Check if Rumble Pak is in,
LDR     R3, [R0]
LDR     R1, =0x20CC2D0 			
CMP 	R3, #0
STREQB	R3, [R0]	
STREQ   R3, [R1]
BEQ		0x20C1C88				@ If not, go to heck.
LDR		R0, =RumbleForce
LDR     R2, [R1]
CMP     R2, #2
ADDNE   R2, R2, #1
MOVEQ   R2, #0
STR   	R2, [R0]
STR   	R2, [R1]
CMP		R2, #0
MOVNE   R0, #1
BLNE    Rumble
B		0x20C19BC

@ This is for right on touchscreen
repl_020C1C00_ov_00:
LDR     R0, =RumblePakType		@ Check if Rumble Pak is in,
LDR     R3, [R0]
LDR     R1, =0x20CC2D0 			
CMP 	R3, #0
STREQB	R3, [R0]	
STREQ   R3, [R1]
BEQ		0x20C1C88				@ If not, go to heck.
LDR		R0, =RumbleForce
LDR     R2, [R1]
CMP     R2, #2
ADDNE   R2, R2, #1
MOVEQ   R2, #0
STR   	R2, [R0]
STR   	R2, [R1]
CMP		R2, #0
MOVNE   R0, #1
BLNE    Rumble
B		0x20C1C3C


@ This is for left
repl_020C19E0_ov_00:
LDR     R0, =RumblePakType		@ Check if Rumble Pak is in,
LDR     R3, [R0]
LDR     R1, =0x20CC2D0 			
CMP 	R3, #0
STREQB	R3, [R0]	
STREQ   R3, [R1]
BEQ		0x20C1C88				@ If not, go to heck.
LDR		R0, =RumbleForce
LDR     R2, [R1]
CMP     R2, #0
SUBNE   R2, R2, #1
MOVEQ   R2, #2
STR   	R2, [R0]
STR   	R2, [R1]
CMP		R2, #0
MOVNE   R0, #1
BLNE    Rumble
B		0x20C19F4

repl_020C1B9C_ov_00:
LDR     R0, =RumblePakType		@ Check if Rumble Pak is in,
LDR     R3, [R0]
LDR     R1, =0x20CC2D0 			
CMP 	R3, #0
STREQB	R3, [R0]	
STREQ   R3, [R1]
BEQ		0x20C1C88				@ If not, go to heck.
LDR		R0, =RumbleForce
LDR     R2, [R1]
CMP     R2, #0
SUBNE   R2, R2, #1
MOVEQ   R2, #2
STR   	R2, [R0]
STR   	R2, [R1]
CMP		R2, #0
MOVNE   R0, #1
BLNE    Rumble
B		0x20C1BD8


@ During setup
nsub_020C1624_ov_00:
LDR     R4, =RumbleForce	@ Check current force type,
LDR   	R4, [R4]		
STR		R4, [R1]			@ set it.
B		0x20C1628

@ Skip setting stereo mode
repl_020C1C58_ov_00:
BX		LR