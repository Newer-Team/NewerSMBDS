repl_0217B0E8_ov_52:
LDR     R0, =0x9A9
MOV     R1, #0
BL      loadFileByExtId_3dModel
LDR     R0, =0x9AA
MOV     R1, #0
BL      loadFileByExtId_3dModel
MOV     R0, #1
BX      LR

repl_0217B038_ov_52:
LDR 	R0, [R5, #8]
AND 	R0, R0, #0xF
CMP     R0, #0
LDREQ   R0, =0x506
CMP     R0, #1
LDREQ   R0, =0x9A9
CMP     R0, #2
LDREQ   R0, =0x9AA
BX		LR

repl_0217B23C_ov_52:
LDR 	R0, [R4, #8]
AND 	R0, R0, #0xF
CMP     R0, #0
LDREQ   R0, =0x9C
LDREQ 	R1, [R4, #8]
ANDEQ	R1, R1, #0x10
MOVEQ	R1, R1, lsr#4
CMP     R0, #1
LDREQ	R1, [R4, #8]
LSREQ   R1, #0x4
ANDEQ	R1, R1, #0xF
LSLEQ   R1, #0x8
LDREQ   R0, =0x30000100
ADDEQ   R1, R0
LDREQ   R0, =0x23
CMP     R0, #2
LDREQ   R0, =0x67
LDREQ   R1, =0x00100001
BL      0x20A0B64
LDR 	R3, [R4, #8]
AND 	R3, R3, #0xF
CMP     R3, #1
BNE     0x0217B240
LDR     R3, =0xA3
ADD     R1, R0, R3
LDRB    R2, [R4, R3]
CMP     R2, #0xC0 
ADDEQ   R2, #0x20
SUBNE   R2, #0x20
STRB    R2, [R1]
B       0x0217B240

repl_0217B078_ov_52:
LDR 	R0, [R5, #8]
AND 	R0, R0, #0xF
CMP     R0, #0
LDREQ	R0, =0x55C
CMP     R0, #1
LDREQ	R0, =0x4D1
CMP     R0, #2
LDREQ	R0, =0x55D
BX LR

repl_0217B630_ov_52:
LDR 	R0, [R4, #8]
AND 	R0, R0, #0xF
CMP     R0, #0
LDREQ   R0, =0x400
CMP     R0, #1
LDREQ   R0, =0xF00
CMP     R0, #2
LDREQ   R0, =0xE20
BX LR

repl_0217B29C_ov_52:
repl_0217B108_ov_52:
MOV     R4, R0
ADD     R1, R4, #0x500
LDRSB   R2, [R1,#0x9C]
CMP     R2, #0
MOVEQ   R0, R4
BXEQ    LR
LDR 	R0, [R4, #8]
AND 	R0, R0, #0xF
CMP     R0, #1
LDREQ   R0, [R4,#0x554]
SUBEQ   R0, R0, #0x8000
STREQ   R0, [R4,#0x554]
LDREQ   R0, [R4,#0x558]
SUBEQ   R0, R0, #0x12000
STREQ   R0, [R4,#0x558]
MOV     R0, R4
BX      LR

repl_0217B144_ov_52:
LDR 	R1, [R4, #8]
AND 	R1, R1, #0xF
CMP     R1, #1
LDR  	R1, [R4,#0x554]
ADDEQ   R1, R1, #0x8000
BX      LR

repl_0217B3E4_ov_52:
LDR 	R1, [R4, #8]
AND 	R1, R1, #0xF
CMP     R1, #1
LDR     R1, [R4,#0x544]
SUBEQ   R1, R1, #0x9000
SUBGT	R1, R1, #0x2000
BX      LR


repl_0217B450_ov_52:
LDR 	R0, [R4, #8]
AND 	R0, R0, #0xF
CMP     R0, #1
LDR     R0, [R4,#0x544]
SUBEQ   R0, R0, #0x9000
SUBGT	R0, R0, #0x2000
BX      LR

repl_0217B458_ov_52:
LDR 	R0, [R4, #8]
AND 	R0, R0, #0xF
CMP     R0, #2
LDR     R0, [R4,#0x548]
SUBEQ   R0, R0, #0x9000
BX      LR