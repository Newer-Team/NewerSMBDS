repl_02187864_ov_64:
	STMFD   SP!, {R2,R3,LR}
	LDR 	R2, [R0, #8]
	LSR     R2, #0xC
	AND     R2, #0xF
	CMP     R2, #2
	LDMEQFD SP!,{R2,R3,PC}
	LDR 	R2, [R0, #8]
	LSR     R2, #4
	ANDS    R2, #0xF
	MOVEQ   R2, #0x200
	MOVNE   R3, #0x80
	SUBNE   R2, #1
	MULNE   R2, R3
	ADDNE   R2, #0x100
	LDR 	R3, [R0, #8]
	LSR     R3, #8
	ANDS    R3, #0xF
	ADDEQ   R1, R1, R2
	SUBNE   R1, R1, R2
	LDMFD   SP!, {R2,R3,PC}

repl_02187828_ov_64:
	BNE     0x218783C
	ADD   R1, R2, #1
	STRB  R1, [R0,#0x56C]
	STMFD   SP!, {R2}
	LDR 	R2, [R0, #8]
	LSR     R2, #4
	AND   	R2, #0xF
	LDR     R1, =Time
	LDRB    R1, [R1,R2]
	LDR 	R2, [R0, #8]
	LSR     R2, #8
	ANDS    R2, #0xF
	SUBNE   R1, #2
	LDMFD   SP!, {R2}
	STRB  	R1, [R0,#0x56E]
	B     	0x218789C

repl_021877A8_ov_64:
repl_02187760_ov_64:
	ADD     R0, R6, R0
	STMFD 	SP!, {R1}
	LDR     R1, =0xB
	STRB    R1, [R0,#0x55]
	LDMFD 	SP!, {R1}
	BX		LR

repl_02187898_ov_64:
	STMFD   SP!, {R0}
	LDR 	R0, [R0, #8]
	LSR     R0, #0xC
	ANDS   	R0, #0xF
	LDMFD 	SP!, {R0}
	BEQ     0x02187960
	BX      LR

.data
.balign 4
Time:
	.word   0x40578040
	.word   0x22262B35
	.word   0x17181A1D
	.word   0x00141516
	





