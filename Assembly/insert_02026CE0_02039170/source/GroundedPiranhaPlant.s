@Rotate model if Nybble 11 set
repl_0217A100_ov_51:
repl_0217AEFC_ov_51:
	LDR     R0, [R4, #0x8]
	LSR     R0, #0x4
	ANDS    R0, #0xF
	LDREQH  R0, [R4,#0xC]
	BXEQ    LR
	MOV     R0, #0x8000
	STRH    R0, [R4, #0xA4]
	LDRH    R0, [R4,#0xC]
	BX      LR

@No gravity if Nybble 11 set
repl_0217A3C0_ov_51:
repl_0217A2A4_ov_51:
repl_0217B074_ov_51:
repl_0217B17C_ov_51:
repl_0217B2AC_ov_51:
	STMFD   SP!, {LR}
	LDR     R0, [R4, #0x8]
	LSR     R0, #0x4
	ANDS    R0, #0xF
	LDMNEFD SP!, {PC}
	MOV     R0, R4
	BL      0x209C85C
	LDMFD   SP!, {PC}

	
@Move stem hitbox if Nybble 11 set:
repl_0217A178_ov_51:
	LDR     R0, [R4, #0x8]
	LSR     R0, #0x4
	ANDS    R0, #0xF
	MOVEQ   R3, #0x0
	BXEQ    LR
	LDR     R0, =0x54C
	LDR     R1, =0xFFFF4000
	STR     R1, [R4, R0]
	MOV     R3, #0
	BX      LR		
	
@Move stem hitbox if Nybble 11 set (Venus):
repl_0217AF60_ov_51:
	LDR     R0, [R4, #0x8]
	LSR     R0, #0x4
	ANDS    R0, #0xF
	MOVEQ   R3, #0x0
	BXEQ    LR
	LDR     R0, =0x4BC
	LDR     R1, =0xFFFF3000
	STR     R1, [R4, R0]
	MOV     R3, #0
	BX      LR	

@Patch used model
repl_0217A63C_ov_51:
repl_0217B508_ov_51:
	LDRB	R0, [R5, #0x8]
	AND 	R0, R0, #0x1
	CMP		R0,	#0x0
	LDREQ	R0, =0x5B2
	LDRNE	R0, =0x9A8
	BX		LR

@Load custom Model
repl_0217A6EC_ov_51:
repl_0217B574_ov_51:
	STMFD   SP!, {LR}
	LDR     R0, =0x9A8
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV		R0, #1
	LDMFD   SP!, {PC}


@Load modded Physics (Normal Piranha)
repl_0217A0D4_ov_51:
repl_0217A0EC_ov_51:
	LDRB	R2, [R5, #0x8]
	AND 	R2, R2, #0x1
	CMP		R2,	#0x0
	LDREQ	R2, =0x0217B650
	LDRNE	R2, =PiranhaPlantPhysics
	BX		LR

@Load modded Physics (Fire Piranha)
repl_0217AE8C_ov_51:
repl_0217AEE4_ov_51:
	LDRB	R2, [R4, #0x8]
	AND 	R2, R2, #0x1
	CMP		R2,	#0x0
	LDREQ	R2, =0x0217B650
	LDRNE	R2, =PiranhaPlantPhysics
	BX		LR

.data
.balign 4
PiranhaPlantPhysics: 
	.word 0x00000000	@X offset
	.word 0x00008000	@Y offset
	.word 0x00004000	@Width
	.word 0x00008000	@Height
	.word 0x00930005	@behavior?
	.word 0x0000FFFC	@flags
	.word 0x02098C78	@player/sprite collision callback
	
@Overlay 81/0x51 offset: 0x123E00