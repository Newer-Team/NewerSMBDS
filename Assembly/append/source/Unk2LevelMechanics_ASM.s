repl_020D7AEC_ov_0A:
repl_020D7838_ov_0A:
	LDR		R0, =0x02088BFC
	LDR		R0, [R0]
	CMP		R0, #0
	LDREQ   R0, =0x9BA
	BXEQ    LR
	CMP		R0, #6
	LDRGE   R0, =0x9BC
	BXGE    LR
	LDR   	R0, =0x6EB
	BX      LR

repl_020D7AC8_ov_0A:
repl_020D7844_ov_0A:
	LDR		R0, =0x02088BFC
	LDR		R0, [R0]
	CMP		R0, #0
	LDREQ   R0, =0x9BB
	BXEQ    LR
	CMP		R0, #6
	LDRGE   R0, =0x9BE
	BXGE    LR
	LDR   	R0, =0x6EC
	BX      LR

repl_020D7AA4_ov_0A:	
repl_020D7850_ov_0A:
	LDR		R0, =0x02088BFC
	LDR		R0, [R0]
	CMP		R0, #0
	LDREQ   R0, =0x9BC
	BXEQ    LR
	CMP		R0, #6
	LDRGE   R0, =0x9BF
	BXGE    LR
	LDR   	R0, =0x6ED
	BX      LR
	

repl_020D6900_ov_0A:				@ Sandstorm Code
	LDR 	R1, =0x208B184 
	LDR 	R1, [R1] 
	LDRB 	R1, [R1, #0xC]
	CMP     R1, #0x2
	BLT     .Return
	BEQ     .InandOut
	CMP     R1, #0x3
	BEQ   	.Constant
	B       .Return

.Constant:	
	LDR     R1, =ticker
	LDR     R1, [R1]
	CMP     R1, #0x78
	MVNLE   R2, #0x0
	MOVLE   R1, R1, LSL #0x7
	SUBLE   R2, R2, R1
	STRLE   R2, [R0,#0x438]
	BLE     .Return
	CMP     R1, #0xE0
	BLE     .Return	
	CMP     R1, #0xF0
	LDRLE   R2, [R0,#0x438]
	MOVLE   R1, R1, LSL #0x2
	ADDLE   R2, R2, R1
	STRLE   R2, [R0,#0x438]
	B       .Return	

.InandOut:
	LDR     R1, =ticker
	LDR     R1, [R1]
	CMP     R1, #2
	BLT     0x020D6C58
	CMP     R1, #0x20
	LSR     R2, R1, #1
	STRLEB  R2, [R0,#0x47C]
	CMP     R1, #0x78
	MVNLE   R2, #0x0
	MOVLE   R1, R1, LSL #0x7
	SUBLE   R2, R2, R1
	STRLE   R2, [R0,#0x438]
	BLE     .Return
	CMP     R1, #0xE0
	BLE     .Return
	CMP     R1, #0xF0
	BGE     0x020D6C58
	MOV     R2, #0xF0
	SUB     R2, R1
	STRB  	R2, [R0,#0x47C]

.Return:
	MOV     R4, R0
	BX 		LR	
	
repl_020D70B0_ov_0A:				@	Reset ticker on level start
	LDR     R5, =0x00000000
	LDR     R6, =ticker
	STR     R5, [R6]
	MOV     R6, R0
	BX      LR
	
repl_020D67D4_ov_0A:				@	Reset ticker on level end
	LDR     R3, =0x00000000
	LDR     R4, =ticker
	STR     R3, [R4]
	MOV     R4, R0
	BX      LR

repl_020D7808_ov_0A:				@	Load correct file
	LDR		R0, =0x2088BFC
	LDR     R0, [R0]
	CMP     R0, #7
	LDRNE   R0, =0x6E8
	LDREQ   R0, =0x9B8
	BX      LR
	
nsub_020D7B74_ov_0A:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	LDR		R0, =0x2088BFC
	LDR     R0, [R0]
	CMP     R0, #7
	LDRNE   R0, =0x6E8
	LDREQ   R0, =0x9B8
	MOV     R1, #0
	BL      loadFileByExtId_3dModel
	MOV     R0, #1
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}	
	
repl_020E0770_ov_0A:				@	Goomba
	LDR     R6, =ticker
	LDR     R6, [R6]	
	CMP     R6, #0
	MOVEQ   R6, R0
	BXEQ    LR
	LDR     R1, =0x20CA850
	LDR     R1, [R1]
	CMP     R1, #0
	MOVNE   R6, R0
	BXNE    LR
	CMP     R6, #0xF0
	MOVGE   R6, R0
	BXGE    LR
	LDR		R6, =0x700
	LDR 	R1, [R0,#0x60]
	ADD     R1, R6
	STR     R1, [R0,#0x60]
	MOV     R6, R0
	BX      LR
	
repl_020DE3B4_ov_0A:				@	Koopa
	LDR     R6, =ticker
	LDR     R6, [R6]	
	CMP     R6, #0
	MOVEQ   R6, R0
	BXEQ    LR
	LDR     R1, [R0,#0x469]
	ANDS    R1, #0xF
	MOVNE   R6, R0
	BXNE    LR
	LDR     R1, =0x20CA850
	LDR     R1, [R1]
	CMP     R1, #0
	MOVNE   R6, R0
	BXNE    LR
	CMP     R6, #0xF0
	MOVGE   R6, R0
	BXGE    LR
	LDR		R6, =0x700
	LDR 	R1, [R0,#0x60]
	ADD     R1, R6
	STR     R1, [R0,#0x60]
	MOV     R6, R0
	BX      LR
	
repl_0214C760_ov_2A:				@	Bob-omb
	STMFD   SP!, {R1,LR}
	LDR     R4, =ticker
	LDR     R4, [R4]	
	CMP     R4, #0
	MOVEQ   R4, R0
	LDMEQFD SP!, {R1,PC}
	LDR     R1, =0x20CA850
	LDR     R1, [R1]
	CMP     R1, #0
	MOVNE   R4, R0
	LDMNEFD SP!, {R1,PC}
	CMP     R4, #0xF0
	MOVGE   R4, R0
	LDMGEFD SP!, {R1,PC}
	LDR		R4, =0x700
	LDR 	R1, [R0,#0x60]
	ADD     R1, R4
	STR     R1, [R0,#0x60]
	MOV     R4, R0
	LDMFD   SP!, {R1,PC}
	

