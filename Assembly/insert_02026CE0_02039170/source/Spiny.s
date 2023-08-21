repl_0214F9E4_ov_2A:
repl_02150C60_ov_2A:
repl_02150638_ov_2A:
	STMFD 	SP!, {R1,LR}
	MOV     R0, #0x1B00
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDMFD 	SP!, {R1,PC}
	
nsub_0214F170_ov_2A:
	STMFD 	SP!, {R1}
	MOV     R3, #0x1B00
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R3, R1, R3
	LDMFD 	SP!, {R1}
	B		0x214F174
	
repl_02150F40_ov_2A:
	STMFD 	SP!, {R1,LR}
	MOV     R0, #0x2100
	LDR     R1, [R5, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDMFD 	SP!, {R1,PC}
	
repl_02151374_ov_2A:
	STMFD 	SP!, {R0,LR}
	MOV     R1, #0x2100
	LDR     R0, [R4, #8]
	MOV     R0, R0, LSR#16
	ANDS    R0, R0, #0xF
	ADDEQ   R0, #1
	MUL     R1, R0, R1
	LDMFD 	SP!, {R0,PC}

repl_0214E840_ov_2A:
	STMFD 	SP!, {R1,LR}
	LDR     R0, =0x1C59
	LDR     R1, [R7, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDMFD 	SP!, {R1,PC}
	
repl_021509C0_ov_2A:
	STMFD 	SP!, {R1,LR}
	LDR     R0, =0x1C59
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDMFD 	SP!, {R1,PC}
	
repl_02150184_ov_2A:
	STMFD 	SP!, {R0,LR}
	LDR     R1, =0x1C59
	LDR     R0, [R4, #8]
	MOV     R0, R0, LSR#16
	ANDS    R0, R0, #0xF
	ADDEQ   R0, #1
	MUL     R1, R0, R1
	LDMFD 	SP!, {R0,PC}
	
repl_0214F3F0_ov_2A:
	STMFD 	SP!, {R0,LR}
	MOV     R1, #0x11
	LDR     R0, [R4, #8]
	MOV     R0, R0, LSR#16
	ANDS    R0, R0, #0xF
	ADDEQ   R0, #1
	MUL     R1, R0, R1
	LDMFD 	SP!, {R0,PC}	

repl_0214F430_ov_2A:
repl_0214F56C_ov_2A:
	STMFD 	SP!, {R0-R2,LR}
	MOV     R0, #0x6000
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDR    	R2, =0x2151AAC
	STR    	R0, [R2, #0x54]
	STR    	R0, [R2, #0x5C]
	STR    	R0, [R2, #0x14]
	STR    	R0, [R2, #0x18]
	MOV     R0, #0x8000
	MUL     R0, R1, R0
	STR    	R0, [R2, #0x58]
	MOV     R0, #0xC000
	MUL     R0, R1, R0
	STR    	R0, [R2]
	LDR     LR, =0x2151AFC
	LDMFD 	SP!, {R0-R2,PC}
	
repl_021513A8_ov_2A:
	STMFD 	SP!, {R0-R2,LR}
	LDR    	R2, =0x2151AAC
	MOV     R0, #0x6000
	STR    	R0, [R2, #0x54]
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	STR    	R0, [R2, #0x5C]
	STR    	R0, [R2, #0x14]
	STR    	R0, [R2, #0x18]
	MOV     R0, #0x8000
	MUL     R0, R1, R0
	STR    	R0, [R2, #0x58]
	MOV     R0, #0xC000
	MUL     R0, R1, R0
	STR    	R0, [R2]
	LDR     LR, =0x2151AFC
	LDMFD 	SP!, {R0-R2,PC}

repl_02151420_ov_2A:
	STMFD 	SP!, {R1,LR}
	MOV     R0, #0x6000
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R0, R1, R0
	LDMFD 	SP!, {R1,PC}
	
repl_0214F164_ov_2A:
	STMFD 	SP!, {R1,LR}
	MOV     R12, #0x8000
	LDR     R1, [R0, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R12, R1, R12
	LDMFD 	SP!, {R1,PC}	
	
repl_0214FA18_ov_2A:
	STMFD 	SP!, {R1,LR}
	MOV     R3, #0x6000
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	ADDEQ   R1, #1
	MUL     R3, R1, R3
	LDMFD 	SP!, {R1,PC}

repl_0215179C_ov_2A:
repl_02151748_ov_2A:
	LDR     R0, =0x634
	BX      LR

repl_0214F38C_ov_2A:
	MOV     R4, R0
	STMFD 	SP!, {R1,LR}
	LDR     R1, [R4, #8]
	MOV     R1, R1, LSR#16
	ANDS    R1, R1, #0xF
	STRB    R1, [R4,#0x630]
	LDMFD 	SP!, {R1,PC}     	

# Groundpound, fireballs
nsub_0214E798_ov_2A:
	LDR     R1, [R0,#8]
	MOV     R1, R1, LSR#16
	AND     R1, R1, #0xF
	CMP     R1, #1
	LDRLEB  R2, [R0,#0x2BD]
	BLE   	0x209D84C
	LDR     R2, =#0x02000000
	CMP     R4, R2
	BLE	    .regular
	LDR     R2, =0x630
	LDRB    R1, [R4,R2]
	CMP     R1, #0
	LDREQB  R2, [R0,#0x2BD]
	BLEQ    .spawn
	BEQ		0x209D84C
	SUB     R1, #1
	STRB 	R1, [R4,R2]
	STMFD	SP!, {LR}
	LDR		R0, =0x70
	MOV		R1, #0
	BL		PlaySNDEffect
	LDMFD	SP!, {LR}
	BX      LR
	
# Fix for crashes in water; revert to regular behaviour
.regular:
	LDRB    R2, [R0,#0x2BD]
	LDR     R1, =0x20C4EC8
	LDR     R12, =0x209D84C
	MOV     R2, R2,LSL#1
	LDRSH   R1, [R1,R2]
	STRH    R1, [R0,#0xA2]
	BX      R12

.spawn:
	MOV		R6, R0
	STMFD	SP!, {R0-R5,LR}
	SUB     SP, SP, #8
	MOV		R1, #5
	LDR		R2, [R0,#0x60]
	SUB		R2, R2,	#0x10000
	STR		R2, [R0,#0x60]
	ADD		R2, R0,	#0x5C
	MOV		R3, #0
	MOV		R4, #0
	MOV		R5, #0
	STR		R5, [SP]
	STR		R5, [SP, #0x4]
	MOV		R0, #0x42
	BL		createActor
	MOV		R1, #5
	LDR		R2, [R6,#0x60]
	ADD		R2, R2,	#0x10000
	STR		R2, [R6,#0x60]
	ADD		R2, R6,	#0x5C
	MOV		R3, #0
	MOV		R4, #0
	MOV		R5, #0
	STR		R5, [SP]
	STR		R5, [SP,#0x4]
	MOV		R0, #0x42
	BL		createActor
	ADD     SP, SP, #8
	LDMFD	SP!, {R0-R5,PC} 