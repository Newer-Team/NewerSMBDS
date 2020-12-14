repl_020133F4_main:
BX LR

repl_02024B96_main:
BX LR

repl_020256F4_main:
BX LR

repl_02001868_main:
BX LR

repl_020CC5D8_ov_01:
STMFD   SP!, {R0, R1, LR}
LDR 	R0, =0x2065F10
LDR 	R0, [R0]
LDR 	R1, =0x203A784
STR 	R0, [R1]
LDMFD   SP!, {R0, R1, PC}
