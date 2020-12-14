#include "Files.h"
	
@ Fixes pipes giving Mario shell animation
repl_0211DA0C_ov_0A:
	B		0x211DA3C
repl_0211D414_ov_0A:
	B		0x211D444
repl_021194FC_ov_0A:
	B		0x2119530
repl_0211D588_ov_0A:
	B		0x211D5B8
repl_0211CF90_ov_0A:
	B       0x211CFC0
	
@ Don't draw the wand if going through pipes
repl_0211DB60_ov_0A:
repl_0211E050_ov_0A:
	MOV     R2, #0
	STRB    R2, [R4, #0xBAF]
	ADD     R2, R3, #1
	BX      LR	
	
repl_0211D814_ov_0A:
repl_0211D9EC_ov_0A:
repl_0211E478_ov_0A:
repl_0211DD3C_ov_0A:
repl_0211DEF8_ov_0A:
repl_0211D3F4_ov_0A:
repl_0211D21C_ov_0A:
	MOV     R0, #0
	STRB    R0, [R4, #0xBAF]
	ADD     R0, R1, #1
	BX      LR	
	
repl_0210C52C_ov_0A:
repl_0210C3A4_ov_0A:
	MOV     R2, #0
	STRB    R2, [R4, #0xBAF]
	ADD     R2, R2, #1
	BX      LR	

repl_0211C470_ov_0A:
	MOV     R1, #0
	STRB    R1, [R4, #0xBAF]
	ADD     R1, R2, #1
	BX      LR		

repl_0211D568_ov_0A:
repl_0211CF70_ov_0A:
	MOV     R0, #0
	STRB    R0, [R5, #0xBAF]
	ADD     R0, R3, #1
	BX      LR	
	
repl_0211E298_ov_0A:
	MOV     R0, #0
	STRB    R0, [R5, #0xBAF]
	ADD     R0, R1, #1
	BX      LR		
	
@ Draw the wand if pipe has ended and Mario is Magic Mario.
repl_0211DCBC_ov_0A:
repl_0211D98C_ov_0A:
repl_0211DE98_ov_0A:
repl_0210C61C_ov_0A:
repl_0211E214_ov_0A:
repl_0211C580_ov_0A:
repl_0211D394_ov_0A:
repl_0211DE98_ov_0A:
	MOV     R0, #0x700
	ADD     R0, #0xAC
	LDRB    R0, [R4, R0]
	CMP     R0, #5
	MOVEQ   R0, #1
	STREQB  R0, [R4, #0xBAF]
	MOV     R0, R4
	BX      LR		
	
repl_0211D784_ov_0A:
repl_0211D18C_ov_0A:
	MOV     R0, #0x700
	ADD     R0, #0xAC
	LDRB    R0, [R5, R0]
	CMP     R0, #5
	MOVEQ   R0, #1
	STREQB	R0, [R5, #0xBAF]
	MOV     R0, R5
	BX      LR	
	
@ Fixes upward pipe hat sticking out
repl_0211E56C_ov_0A:
	MOV     R1, #0x8000
	BX      LR

@ Extending Mario actor
repl_02120E1C_ov_0A:
	 LDR     R0, =0xCF5
	 BX      LR
	
@ Saving the regular face model addresses for use later
repl_0212034C_ov_0A:
	MOV     R4, R0
	LDR     R0, [R4, #0xCEC]
	LDR     R1, [R4, #0xCF0]
	STR     R0, [R4, #0x308]
	STR     R1, [R4, #0x310]
	BX	    LR
	
@ Setup of Magic Mario models
nsub_021002EC_ov_0A:
	LDR     R0, [R5, #0x308]
	STR     R0, [R5, #0xCEC]
	LDR     R0, [R5, #0x310]
	STR     R0, [R5, #0xCF0]	
	
	LDR     R1, =0x2085A50
	LDRB    R1, [R1]
	LDR     R0, =KAMEK_HEAD_MODEL_M
	ADD     R0, R1
	BL		getPtrToLoadedFile
	MOV     R1, R0
	ADD     R0, R5, #0xB00
	ADD     R0, R0, #0xCC
	MOV     R2, #0
	BL      model3d_setup
	ADD     R0, R5, #0xB00
	ADD     R0, R0, #0xCC
	BL      model3d_init	
	
	LDR     R1, =0x2085A50
	LDRB    R1, [R1]	
	LDR     R0, =KAMEK_DEADHEAD_MODEL_M
	ADD     R0, R1
	BL		getPtrToLoadedFile
	MOV     R1, R0
	ADD     R0, R5, #0xC00
	ADD     R0, R0, #0x5C
	MOV     R2, #0
	BL      model3d_setup
	ADD     R0, R5, #0xC00
	ADD     R0, R0, #0x5C	
	BL      model3d_init		

	ADD     R0, R5, #0x700
	LDRB    R0, [R0, #0xAC]
	CMP     R0, #5

	ADD     R0, R5, #0xB00
	ADD     R0, R0, #0xCC
	ADD     R1, R5, #0xC00
	ADD     R1, R1, #0x5C	
	
	STREQ   R0, [R5, #0x308]
	STREQ   R1, [R5, #0x310]	
	
	LDR     R0, =isMvsLMode
	B       0x21002F0
	
@ Magic Mario transformation
repl_0211FDB0_ov_0A:
repl_0211FD70_ov_0A:
	STRB    R0, [R7,#0xBAF]
	STMFD   SP, {R1, R2}
	ANDS    R0, R0, #1
	MOVEQ   R1, #2
	MOVNE   R1, #0
	STRB    R1, [R7,#0xBB6]
	CMP     R1, #2
	LDREQ   R1, [R7, #0xCEC]
	LDREQ   R2, [R7, #0xCF0]
	ADDNE   R1, R7, #0xB00
	ADDNE   R1, R1, #0xCC
	ADDNE   R2, R7, #0xC00
	ADDNE   R2, R2, #0x5C
	STR     R1, [R7, #0x308]
	STR     R2, [R7, #0x310]
	LDMFD   SP, {R1, R2}
	BX      LR
	
@ Magic Mario lose transformation
repl_0211FD30_ov_0A:
	STRB    R1, [R7,#0xBAF]
	STMFD   SP, {R0, R1, R2}
	ANDS    R1, R0, #1
	MOVEQ   R1, #1
	MOVNE   R1, #0
	STRB    R1, [R7,#0xBB6]
	CMP     R1, #0
	LDR     R1, [R7, #0xCEC]
	LDR     R2, [R7, #0xCF0]
	ADDEQ   R1, R7, #0xB00
	ADDEQ   R1, R1, #0xCC
	ADDEQ   R2, R7, #0xC00
	ADDEQ   R2, R2, #0x5C
	STR     R1, [R7, #0x308]
	STR     R2, [R7, #0x310]
	LDMFD   SP, {R0, R1, R2}	
	B 		0x211FD44
	
@ Loading the wand model
repl_0212E100_ov_0B:
repl_0212E1AC_ov_0B:
	LDR     R0, =PLAYER_WAND
	BX      LR
	
repl_020D07B4_ov_0A:					@ Magic blocks work properly, i.e if powerup is mini or none they'll just spit out a mushroom like they should
	STMFD	SP!, {LR}
	BL      getPtrToMarioOrLuigi    
	ADD     R0, R0, #0x700
	LDRB    R0, [R0, #0xAB]
	MOV     R1, R4
	AND     R4, #0xF
	CMP     R4, #0xB
	LDMNEFD SP!, {PC}
	CMP     R0, #4
	CMPNE   R0, #0
	ANDEQ   R1, #0xFFFFFFF0
	LDMFD 	SP!, {PC}
	
	
repl_02117D18_ov_0A:					@ Change Goal Pole animation
	STMFD	SP!, {R0}
	BL      getPtrToPlayerActor
	ADD     R0, #0x700
	ADD     R0, #0xAB
	LDRB    R1, [R0]
	CMP     R1, #0x5
	LDMFD	SP!, {R0}
	MOVEQ   R1, #0xC00
	STREQ   R1, [SP]
	MOV     R1, #0x61
	ADDEQ   R1, #0x2
	B       0x2117D1C
	
repl_02117BD0_ov_0A:					@ Disable taking off hat
repl_02117C78_ov_0A:
	STMFD	SP!, {R0,R1,LR}
	BL      getPtrToPlayerActor
	ADD     R0, #0x700
	ADD     R0, #0xAB
	LDRB    R1, [R0]
	CMP     R1, #0x5
	LDMFD	SP!, {R0,R1,LR}
	BXEQ    LR
	BNE     0x21209F4

	
repl_0211BBE8_ov_0A:
repl_0211BCB4_ov_0A:
repl_0211CF80_ov_0A:
repl_0211D230_ov_0A:
repl_0211D404_ov_0A:
repl_0211D578_ov_0A:
	MOV     R1, #0
	BX		LR
	
repl_020210BC_main:
repl_02020C44_main:
	STMFD	SP!, {R0, R1, R4, LR}
	LDR		R0, =PLAYER_SPELL
	MOV     R1, #0
	BL		loadFileByExtId_3dModel
	
	LDR     R1, =0x2085A50
	LDRB    R1, [R1]
	LDR		R0, =KAMEK_HEAD_MODEL_M
	ADD     R0, R1
	MOV     R1, #1
	BL		loadFileByExtId_3dModel
	
	LDR     R1, =0x2085A50
	LDRB    R1, [R1]	
	LDR		R0, =KAMEK_DEADHEAD_MODEL_M
	ADD     R0, R1
	MOV     R1, #1
	BL		loadFileByExtId_3dModel	
	
	LDMFD	SP!, {R0, R1, R4, LR}
	MOV     R0, #1
	BX      LR