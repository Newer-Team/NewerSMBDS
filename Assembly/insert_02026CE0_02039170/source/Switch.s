repl_02169BAC_ov_36:
MOV     LR, #4
STR     LR, [SP]
MOV     LR, #0
B       0x2169BB0

repl_0213A27C_ov_0D:
MOV     R3, #4
B 		0x0213A280

repl_02169F48_ov_36:
MOV		R0, R2
ANDS    R0, #1
LDRNE   R0, [R5,#0x60]
ADDNE   R0, R0, #0x8000
STRNE   R0, [R5,#0x60]
MOV		R0, R2
ANDS    R0, #2
LDRNE   R0, [R5,#0x64]
ADDNE   R0, R0, #0x8000
STRNE   R0, [R5,#0x64]
MOV		R0, R2
ANDS    R0, #4
LDRNE   R0, [R5,#0x64]
ADDNE   R0, R0, #0xA000
STRNE   R0, [R5,#0x64]
ANDS    R2, #8
LDRNE   R0, [R5,#0x64]
ADDNE   R0, R0, #0xC000
STRNE   R0, [R5,#0x64]
B		0x2169F58