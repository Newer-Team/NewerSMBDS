nsub_0201980C_main:
	LDR     R2, [R0,#0x58]
	CMP     R2, #0x2000000
	MOVLT   R0, #0
	MOVLT   R0, #0
	B       0x2019810
	
repl_0212D270_ov_0B:
	MOV     R0, R3,LSL#2
	LDR     R4, [R2,#4]
	STMFD   SP!, {R0, R3, LR}
	
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99	
	LDMEQFD SP!, {R0, R3, LR}
	BEQ 	0x212D334	
	
	BL      getPtrToPlayerActor
	ADD     R3, R0, #0x700
	ADD     R3, #0xAC
	LDRB    R3, [R3]
	CMP     R3, #5
	LDMFD   SP!, {R0, R3, LR}
	BEQ 	0x212D334
	
	CMP     R3, #0
	BEQ     0x212D2E0
	CMP     R3, #2
	BEQ     0x212D2E0
	B		0x212D334
	
nsub_0212D3F0_ov_0B:
	STMFD   SP!, {R0, LR}
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99	
	LDMFD   SP!, {R0, LR}
	BEQ    	0x212D3F4
	
	CMP     R0, #0x2000000
	BLGT    0x205A010
	B      	0x212D3F4
	
repl_0212D3E4_ov_0B:
	STMFD   SP!, {R0, LR}
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99	
	LDMFD   SP!, {R0, LR}
	BEQ    	0x212D3E8
	CMP     R0, #0x2000000
	BLGT    0x205A2D8
	B      	0x212D3E8
	
repl_0212D3D4_ov_0B:
	STMFD   SP!, {R0, LR}
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99	
	LDMFD   SP!, {R0, LR}
	BEQ    	0x212D3D8
	CMP     R0, #0x2000000
	BLGT    0x205A2D8
	B      	0x212D3D8
	
repl_0212D3FC_ov_0B:
	STMFD   SP!, {R0, LR}
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99	
	LDMFD   SP!, {R0, LR}
	BEQ    	0x212D400
	CMP     R0, #0x2000000
	BLGT    0x2059FD0
	B      	0x212D400	