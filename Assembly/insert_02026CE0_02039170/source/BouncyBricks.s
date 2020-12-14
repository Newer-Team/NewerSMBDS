repl_02185914_ov_63: 
STMFD	SP!, {R0,R1,LR}
BL      getPtrToPlayerActor
LDR     R0, [R0, #0x77E]
AND     R0, #2
CMP		R0, #0
MOVEQ     R12, #0x10
MOVNE     R12, #0x55
LDMFD	SP!, {R0,R1,LR}
BX LR

