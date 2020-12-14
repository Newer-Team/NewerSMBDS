repl_0212D7B8_ov_0B:
	CMP     R0, #0
	BEQ 	0x212D760
	B       0x212D7C8

repl_0212D718_ov_0B:
	LDR     R10, =0x0208B364
	LDRB    R10, [R10]
	CMP     R10, #99
	MOV     R10, R0
	LDR     R7, =0x0212E584
	LDR     R0, =0x6D9
	ADDEQ   R0, #1
	STR     R0, [R7]
	STR     R0, [R7, #0x4]
	LDR     R0, =0x6D3
	ADDEQ   R0, #1
	STR     R0, [R7, #0x10]	
	STR     R0, [R7, #0x14]	
	BX		LR
	
repl_02020D0C_main:
repl_02020E3C_main:
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99
	LDR     R0, =0x6D9
	ADDEQ   R0, #1
	BX      LR

repl_0202089C_main:
repl_020209CC_main:
	LDR     R0, =0x0208B364
	LDRB    R0, [R0]
	CMP     R0, #99
	LDR     R0, =0x6D3
	ADDEQ   R0, #1
	BX      LR

repl_020207C4_main:
	BX		LR

repl_0212DB3C_ov_0B:
	MOV     R0, #1
	BX      LR