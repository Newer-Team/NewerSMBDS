repl_0209EED8_ov_00:	@SFX Player on tile break
	LDR     R0, =0x2000000
	CMP     R10, R0
	LDRLT   R0,  =0x177
	BXLT    LR
	LDR     R0, =0x23D0900
	CMP     R10, R0	
	LDRGT   R0,  =0x177
	BXGT    LR	
	LDR 	R0, [R10, #8]
	LSR 	R0,R0,#24
	AND 	R0, R0, #0xF
	BL 		.SetSFX
	B 		0x0209EEDC

repl_02156BDC_ov_36:	@SFX Player on tile spawn
	LDR 	R0, [R10, #8]
	LSR 	R0,R0,#24
	AND 	R0, R0, #0xF
	CMP 	R0, #0
	LDREQ 	R0, [SP, #0x54]
	BXEQ 	LR
	BNE 	.SetSFX
	BX 		LR

repl_02156BB8_ov_36:	@Particle Spawner on tile spawn
	LDR 	R0, [R10, #8]
	LSR 	R0,R0,#20
	AND 	R0, R0, #0xF
	CMP 	R0, #0
	LDREQ 	R0, [SP,#0x48]
	BXEQ 	LR
	BLNE 	.SetParticle
	B 		0x2156BD4

.SetSFX:
	STMFD	SP!, {R1,LR}
	LSL 	R0, R0, #1
	LDR 	R1, =SoundArray
	ADD 	R0, R1, R0
	LDRH 	R0, [R0]
	LDMFD 	SP!, {R1,LR}
	BX 		LR

.SetParticle:
	STMFD	SP!, {R1,LR}
	SUB 	R0, R0, #1
	LSL 	R0, R0, #0x1
	LDR 	R1, =ParticleArray
	ADD 	R0, R1, R0
	LDRH 	R0, [R0]
	LDMFD	SP!, {R1,PC}

.data
.balign 4
SoundArray: 
	.hword 0x0177	@ Default breaking sound.
	.hword 0x0068	@ Banzai Bill
	.hword 0x0177	@ Used by fucking broozers
	.hword 0x00A2	@ Boo
	.hword 0x0004
	.hword 0x0005  		
	.hword 0x0006
	.hword 0x0007
	.hword 0x0008
	.hword 0x0009
	.hword 0x000A
	.hword 0x000B
	.hword 0x000C
	.hword 0x000D
	.hword 0x000E
	.hword 0x000F
	
.balign 4
ParticleArray:
	.hword 0x0016 @ Red smoke
	.hword 0x0017 @ Red smoke 2
	.hword 0x0019 @ Red firework
	.hword 0x006B @ White puff of smoke	
	.hword 0x0005 
	.hword 0x011D @ Big water splash upward
	.hword 0x007B @ Big orange firework
	.hword 0x0008 
	.hword 0x0009 
	.hword 0x000A
	.hword 0x000B
	.hword 0x000C
	.hword 0x0000
	.hword 0x0000
	.hword 0x0000