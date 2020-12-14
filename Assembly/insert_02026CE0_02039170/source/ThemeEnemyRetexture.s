hook_02006984_main:
	STMFD   SP!, {R0,R1,R2,LR}
	LDR 	R1, =0x212EE94
	LDR 	R2, =0x6A9
	STRH 	R2, [R1]
	LDR 	R1, =0x212EED4
	LDR 	R2, =0x6AD
	STRH 	R2, [R1]
	BL 		.SantaGoombas
	BL 		.BoneGoombas
	BL 		.SpaceGoombas
	BL 		.GhostDoors
	LDMFD 	SP!, {R0,R1,R2,PC}

.SantaGoombas:
	LDR 	R1, =0x212EE94
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x4
	LDREQ 	R2, =0x9A3
	STREQH 	R2, [R1]
	BX 		LR

.BoneGoombas:
	LDR 	R1, =0x212EE94
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x3
	LDREQ 	R2, =0x9A1	
	STREQH 	R2, [R1]	
	BX 		LR
	
.SpaceGoombas:
	LDR 	R1, =0x212EE94
	LDR 	R0, =0x2088BFC
	LDR 	R0,[R0]
	CMP 	R0,#0x5
	LDREQ 	R2, =0x9B7
	STREQH 	R2, [R1]
	BX 		LR

.GhostDoors:
	LDR 	R1, =0x216FCB0
	LDR 	R0, =0x2085A9C
	LDR     R0, [R0]
	CMP     R0, #0x0C
	LDRNE   R2, =0x6A5
	LDREQ   R2, =0x9AC
	STRH 	R2, [R1]
	BX 		LR