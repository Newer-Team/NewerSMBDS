nsub_020DB4C0_ov_0A:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4

@ If World 5, load snowy ones
	LDR		R0, =0x02088BFC
	LDR     R1, =0x02085A9C
	LDR     R1, [R1]
	LDR		R0, [R0]
	CMP		R0, #4
	LDRNE	R0, =0x596
	BNE		.NotW5
	CMP     R1, #0xC		@ Except for ghost houses

	LDRNE   R0, =0x9FB
	LDREQ	R0, =0x596
.NotW5:
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1	
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}
	
nsub_020DB4A8_ov_0A:
	LDR		R1, =0x02088BFC
	LDR     R12, =0x02085A9C
	LDR		R1, [R1]
	LDR		R12, [R12]
	CMP		R1, #4	
	LDRNE   R1, =0x596
	BNE     .NotW5_
	CMP   	R12, #0xC
	LDRNE   R1, =0x9FB
	LDREQ	R1, =0x596
.NotW5_:
	LDR     R12, =0x20DB1B4
	ADD     R0, R0, #0x3F4
	BX      R12
	
@ Three Platform Rickshaw
nsub_0217E304_ov_56:
	LDR		R0, =0x02088BFC
	LDR		R0, [R0]
	CMP		R0, #4	
	LDRNE   R0, =0x588
	LDREQ	R0, =0x9FB
	B		0x217E308	
	
nsub_0217E2C8_ov_56:
	LDR		R1, =0x02088BFC
	LDR		R1, [R1]
	CMP		R1, #4	
	LDRNE   R1, =0x588
	LDREQ	R1, =0x9FB
	B		0x217E2CC