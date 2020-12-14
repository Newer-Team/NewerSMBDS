repl_0214715C_ov_2A:
MOV 	R1, #0
BX 		LR

repl_02149898_ov_2a:
AND     R0, R5, #0x1
CMP     R0, #1
MOVEQ   R0, #7
MOVNE   R0, #5
BX      LR

repl_021498A0_ov_2a:
STRB  	R0, [R6,#0x2BD]
BX      LR

repl_02149A1C_ov_2a:
STMFD   SP!, {R0-R2}
LDR     R0, [R6,#0x8]
AND     R0, R0, #0xF
CMP     R0, #3
LDMEQFD SP!, {R0-R2}
LDREQB  R0, [R6,#0x2BD]
BXEQ    LR
CMP     R0, #2
LDMLTFD SP!, {R0-R2}
LDRLTB  R0, [R6,#0x2BD]
BXLT    LR
MOVEQ   R0, #0x2000
STREQ   R0, [R6,#0x134]
LDMEQFD SP!, {R0-R2}
BEQ	 	0x2149A38
LDR     R1, =NewBillCollisions
CMP     R0, #8
LDRLT   R0, [R1]
STRLT   R0, [R6,#0x140]
LDMLTFD SP!, {R0-R2}
BLT	 	0x2149A40
CMP     R0, #0xA
LDRLT   R0, [R1, #0x4]
STRLT   R0, [R6,#0x140]
LDMFD   SP!, {R0-R2}
B	 	0x2149A40

repl_02149920_ov_2A:
repl_02149740_ov_2A:
LDR     R1, [R6,#0x8]
AND     R1, R1, #0xF
CMP     R1, #0x4
LDRLT   R2, =0x21518C0
BXLT    LR
CMP     R1, #0x6
LDRLT   R2, =NewBanzaiCollisions
BXLT    LR
LDR     R2, =NewTiltedCollisions
BX    	LR

repl_02149960_ov_2A:
LDR     R1, [R6,#0x8]
AND     R1, R1, #0xF
CMP     R1, #2
LDRLTB  R1, [R6,#0x2BD]
MOVEQ   R1, #0
CMP     R1, #3
MOVEQ   R1, #1
BX      LR

repl_02149E98_ov_2A:	@Disable triangle collision on tilted ones
LDR     R0, [R4,#0x8]
AND     R0, R0, #0xF
CMP     R0, #2
LDRLTB  R1, [R4,#0x2BD]
BXLT    LR
MOVEQ   R1, #0
BXEQ    LR
CMP     R0, #3
MOVEQ   R1, #1
MOVNE   R1, #2
BX      LR

repl_021499DC_ov_2A:
LDR     R1, [R6,#0x8]
AND     R1, R1, #0xF
CMP     R1, #0x4
LDRLT   R1, [R6,#0x5A4]
BXLT    LR
B       0x2149A40


repl_02149530_ov_2A:
LDR     R2, [R4,#0x68]
STMFD   SP!, {R0,R1}
LDR 	R1, [R4,#0x8] 
LSR 	R1, #0x8 
AND 	R1, R1, #0xF 
MOV     R0, #0xF000
MUL     R0, R1
ADD     R2, R2, R0
LDMFD   SP!, {R0,R1}
BX      LR

repl_0214976C_ov_2A:
MOV     R0, #0x50
BX      LR

repl_02149764_ov_2A:
MOV     R1, #0x40
BX      LR
	
.data
.balign 4
NewBillCollisions:
.word 0x00009000
.word 0x00008000
.balign 4
NewBanzaiCollisions:
.word 0x00000000
.word 0x00000000
.word 0x00017000
.word 0x0001E000
.word 0x00930008
.word 0x0000FFFC
.word 0x02098C78
.balign 4
NewTiltedCollisions:
.word 0x00000000
.word 0x00000000
.word 0x0001B000
.word 0x00018000
.word 0x00930008
.word 0x0000FFFC
.word 0x02098C78
