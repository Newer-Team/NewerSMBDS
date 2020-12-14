@Separates pipe sounds and powerup lose sounds

repl_0209E478_ov_00:
repl_0209E578_ov_00:
	LDR     R0, =0x174
	BX		LR
	
repl_0211E504_ov_0A:
repl_0211DC74_ov_0A:
repl_0210C588_ov_0A:
repl_0211E1CC_ov_0A:
repl_0211D738_ov_0A:
repl_0211D494_ov_0A:
repl_0211D140_ov_0A:
repl_0211DA8C_ov_0A:
repl_0211DF84_ov_0A:
repl_0211D8B4_ov_0A:
repl_0211E324_ov_0A:
repl_0210C3FC_ov_0A:
repl_0211C540_ov_0A:
repl_0211D2BC_ov_0A:
repl_0211DDC8_ov_0A:
	LDR		R1, =0x176
	BX		LR
	

@ Changes the logic used in choosing a jump sound
	
nsub_020FD0F0_ov_0A:
	ADD     R1, R0, #0x700
	LDRB   	R3, [R1,#0xAC]
	LDR		R1, =0x147
	CMP		R3, #3
	ADDEQ	R1, R3
	BEQ		.DoTheSound
	CMP		R3, #4
	ADDEQ	R1, #2
	BEQ		.DoTheSound
	ADD     R3, R0, #0xB00
	LDRB   	R3, [R3,#0xA0]	
	CMP		R3, #2
	ADDEQ	R1, #1
	
.DoTheSound:
	ADD     R2, R0, #0x5C
	LDR 	R12, =0x212B838
	BX		R12
	
@Makes Hammers play 1UP_DOWN sound 

repl_02120918_ov_0A:
	CMP		R2, #0x5
	MOVEQ	R1, #0x17C
	ADD     R2, R0, #0x5C
	BX		LR
	
	
@Disables Capture
nsub_0204F1DC_main:
	BX		LR