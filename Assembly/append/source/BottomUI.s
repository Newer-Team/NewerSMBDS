@ d_2d_UI_O_1P_course_select NCG
repl_020CFF28_ov_08:
	LDR R0, =0x2088BDC
	LDR R0, [R0,#0x38]
	LDR R6, =0x20CC0B4
	STRB R0, [R6]
	LDR R6, =0xB9B
	ADD R0, R0, R6
	BX  LR 
	
@ d_2d_UI_O_1P_course_select NCL
repl_020CFFD8_ov_08:
	LDR	R0, =0x20CC0B4
	LDRB R0, [R0]
	LDR R1, =0xBA5
	ADD R0, R0, R1
	BX  LR
	
@ UI_O_1P_course_select_o_ud NCL	
repl_020BD430_ov_00:
	LDR	R0, =0x20CC0B4
	LDRB R0, [R0]
	LDR R1, =0xBAF
	ADD R0, R0, R1
	MOV R1, #1
	BX  LR
	
@ Don't need this anymore... (Original NCG/NCL load)
nsub_020C0998_ov_00:
	B	0x20C09A8
	
nsub_020C0A50_ov_00:
	B	0x20C0A70
	
@ New d_2d_UI_O_1P_game_in_b_d load

nsub_020BE234_ov_00:
	STMFD   SP!, {LR}
	SUB     SP, SP, #4
	
@ NCL
	LDR		R0, =0x20CC0B4		@ Currently Selected BG
	LDRB 	R6, [R0]
	LDR 	R1, =0xBAA
	ADD 	R0, R1, R6
	LDR     R1, =0x5000400
	BL      loadFileByExtId_LZ_Dest

@ NCG
	BL      G2S_GetBG0CharPtr
	MOV     R1, R0
	LDR 	R0, =0xBA0
	ADD 	R0, R0, R6
	BL      loadFileByExtId_LZ_Dest
	
@ NSC
	BL      G2S_GetBG3ScrPtr
	LDR     R2, =0x20C2D94
	MOV     R1, R0
	LDR     R0, [R2,R6,LSL#2]
	BL      loadFileByExtId_LZ_Dest
	ADD     SP, SP, #4
	LDMFD   SP!, {PC}

	
