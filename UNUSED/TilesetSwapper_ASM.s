#include "files.h"


	
repl_0215F0D0_ov_36:
	B		0x215F10C
	
	
@PA0---------------------------
@ Loading UNT
nsub_020B3A14_ov_00:
	BL		GetPa0Number
	MOV		R1, R0	
	LDR		R0, =Pa0TilesetTable
	LDR     R0, [R0,R1,LSL#2]
	ADD		R0, #4
	B		0x20B3A18
	
@ Loading UNTHD
nsub_020B3A08_ov_00:
	BL		GetPa0Number
	MOV		R1, R0	
	LDR		R0, =Pa0TilesetTable
	LDR     R0, [R0,R1,LSL#2]
	ADD		R0, #5
	B		0x20B3A0C
	

@ PA1---------------------------
@ Loading NCG
nsub_020BCFEC_ov_00:
	LDR		R0, =Pa1TilesetTable
	LDR     R0, [R0,R5,LSL#2]
	ADD     R0, #2
	B		0x20BD000
	
@ Loading NCL
nsub_020BCCE0_ov_00:
	LDR		R0, =Pa1TilesetTable
	LDR     R0, [R0,R6,LSL#2]
	ADD     R0, #6
	B		0x20BCCF4

@ Loading PNL
nsub_020BB9A8_ov_00:
	LDR		R0, =Pa1TilesetTable
	LDR     R0, [R0,R2,LSL#2]
	ADD     R0, #3
	B		0x20BB9BC	
	
@Loading CHK
nsub_020AF9BC_ov_00:
	LDR     R0, =Pa1TilesetTable
	LDR     R0, [R0,R2,LSL#2]
	ADD		R0, #1
	B		0x020AF9D0	

@Loading ANIM
nsub_020B84E4_ov_00:
	B		0x020B84F8
		
nsub_020B8514_ov_00:	
	LDR		R0, =Pa1TilesetTable
	LDR     R0, [R0,R4,LSL#2]
	B		0x20B851C	

@Loading UNT
nsub_020B3A54_ov_00:
	LDRH    R0, [R0]
	LDR     R2, =Pa1TilesetTable
	LDR     R1, [SP,#4]
	LDR     R0, [R2,R0,LSL#2]
	ADD		R0, #4
	B		0x020B3A64

@Loading UNT_HD
nsub_020B3A34_ov_00:
	LDR     R0, =Pa1TilesetTable
	LDR     R0, [R0, R2, LSL #2]
	ADD		R0, #5
	B		0x020B3A48
	
@PA2------------------------------------
@ Loading UNT
nsub_020B3A90_ov_00:
	BL		GetPa2Number
	MOV		R1, R0	
	LDR		R0, =Pa2TilesetTable
	LDR     R0, [R0,R1,LSL#2]
	ADD     R0, #4
	B		0x20B3A94
	
@ Loading UNTHD
nsub_020B3A88_ov_00:
	BL		GetPa2Number
	MOV		R2, R0					
	LDR		R0, =Pa2TilesetTable
	LDR     R0, [R0,R2,LSL#2]
	ADD     R0, #5
	B		0x20B3A8C	
	
	
GetPa0Number:
	LDR		R0, =0x208B168
	LDR     R0, [R0]
	LDRB    R0, [R0, #0x20]
	BX      LR
	
GetPa2Number:
	LDR		R0, =0x208B168
	LDR     R0, [R0]
	LDRB    R0, [R0, #0x21]
	BX      LR 

.data
.balign 4
JyotyuChk:
	.long 0x00000000
	
	