ChainChompVirt50:					@No-log chain chomps don't die on groundpounds 
	STMFD   SP!, {LR}
	SUB     SP, SP, #0x4
	ADD     R0, #0x100
	LDRSB   R0, [R0,#0x1E]
	BL      getPtrToMarioOrLuigi
	LDR     R12, [R0]
	LDR     R12, [R12,#0x64]
	MOV     R3, #0x4000
	BLX     R12
	ADD     SP, SP, #0x4
	LDMFD   SP!, {PC}


repl_02172EC0_ov_39:				@Spawning chomp with resizer set
	LDR     R8, [R0, #0x8]
	LSR     R8, #0x4
	AND     R8, #0xF
	ADD     R5, R0, #0x470
	BX 		LR

repl_02172EF8_ov_39:
	BX		LR
	
repl_02172F10_ov_39:
	MOV     R3, #0x0
	STR     R3, [SP]
	STR     R3, [SP, #4]
	B		0x2172F18		

repl_02174050_ov_39:				@Resizing Chomp
	LDR     R2, [R4, #0x8]
	ANDS    R2, #0xF
	MOV     R1, #0x1000
	BXEQ    LR
	ADD     R2, #1
	MUL     R1, R2
	BX      LR
	
repl_02174094_ov_39:				@Resizing Active Physics
	STMFD   SP!, {R0,R1,R3}
	LDR     R0, =0x02175514
	LDR     R1, =0x0000C000
	LDR     R2, =0x0000C000
	LDR     R3, [R4, #0x8]
	AND     R3, #0xF
	ADD     R3, #1
	MUL		R1, R3
	MUL     R2, R3
	STMIA   R0, {R1, R2}
	LDR     R2, =0x0217550C
	LDMFD   SP!, {R0,R1,R3}
	BX      LR
	
repl_02173570_ov_39:				@ Resize chainlinks
	LDR     R1, [R1]
	MOV     R2, #0x90
	LDR     R3, [R7, #0x8]
	ANDS    R3, #0xF	
	BXEQ    LR
	MUL     R2, R3
	ADD     R1, R2
	BX      LR

repl_02173E20_ov_39:				@Model position is higher if resizer is set
	LDR     R3, [R5, #0x8]
	ANDS    R3, #0xF
	LDR     R2, [R5,#0x64]
	BXEQ    LR
	MOV     R0, #0x10000
	MUL     R0, R3 
	ADD     R2, R0
	BX      LR
	
nsub_02174F2C_ov_39:				@ Offset active physics left or right depending on Y rotation and scale
	STMFD   SP!, {R1,R2,R5}
	LDR     R2, [R0, #0x8]
	AND     R2, #0xF
	MOV     R1, #0xC000
	MUL    	R1, R2
	LDRH    R2, [R0, #0xA3]
	CMP     R2, #0x4000
	SUBGT   R5, R1
	ADDLE   R5, R1
	STR     R5, [R0,#0x134]
	LDMFD   SP!, {R1,R2,R5}
	B       0x2174F30

nsub_02174F4C_ov_39:				@ Offset active physics up depending on X rotation and scale
	STMFD   SP!, {R2,R5,R6}
	LDR     R5, [R0, #0x8]
	AND     R5, #0xF
	LDRH    R6, [R0, #0xA0]
	CMP     R2, #0x0
	MOVEQ   R6, #0x10000
	MOVNE   R6, #0x18000
	MUL     R6, R5
	ADD     R2, R6
	STR     R2, [R0,#0x138]
	LDMFD   SP!, {R2,R5,R6}
	B		0x2174F50

repl_02173428_ov_39:				@ Setup chainlink position change
	SUB     R8, R7, #0x3F4
	SUB     SP, SP, #8
	LDR     R1, [R8, #0x8]
	ANDS    R1, #0xF
	MOVEQ   R2, #0x0
	STREQ   R2, [SP]
	STREQ   R2, [SP,#4]
	ADDEQ   R8, R7, #0x3B8
	BXEQ    LR
	MOV     R2, #0x10000
	MUL     R2, R1
	STR     R2, [SP]
	MOV     R2, #0x2500
	MUL     R2, R1
	STR     R2, [SP, #4]
	ADD     R8, R7, #0x3B8
	BX      LR

repl_02173434_ov_39:				@ Offset chainlinks 
	LDR		R2, [R0,#4]
	LDR     R3, [SP]
	ADD     R2, R3
	LDR     R0, [SP, #4]
	SUB     R3, R0
	STR     R3, [SP]
	ADD     R0, R6, #0x1C
	BX		LR

repl_02173484_ov_39:
	ADD     SP, SP, #0xC
	BX LR

