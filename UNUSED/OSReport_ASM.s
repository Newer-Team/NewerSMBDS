.globl SetupGraphicsforOSReports
SetupGraphicsforOSReports:
STMFD   SP!, {LR}
SUB     SP, SP, #4
BL      Setup2
LDR     R12, =0x4000304
LDR     R0, =0x4000050
LDRH    R3, [R12]
MOV     R2, #0
LDR     R1, =0x4001050
ORR     R3, R3, #1
STRH    R3, [R12]
STRH    R2, [R0]
MOV     R0, #0x40 
STRH    R2, [R1]
MOV     R0, #0x80 
BL      GX_SetBankForSubBG
MOV     R2, #0x4000000
LDR     R0, [R2]
LDR     R1, =0x4001000
BIC     R0, R0, #0x1F00
ORR     R0, R0, #0x200
STR     R0, [R2]
LDR     R0, [R1]
LDR     R2, =0x400000A
BIC     R0, R0, #0x1F00
ORR     R0, R0, #0x200
STR     R0, [R1]
LDRH    R0, [R2]
LDR     R1, =0x400100A
AND     R0, R0, #0x43
ORR     R0, R0, #0x400
LDRH    R0, [R1]
AND     R0, R0, #0x43
ORR     R0, R0, #0x400
STRH    R0, [R1]
BL      G2_GetBG1CharPtr
MOV     R1, R0
LDR     R0, =debugScreenCompressedGfx 
ADD     R1, R1, #0x400
BL      G2S_GetBG1CharPtr
ADD     R1, R0, #0x400
LDR     R0, =debugScreenCompressedGfx 
BL      MI_UncompressLZ16
LDR     R0, =0x2039838
MOV     R1, #0x1A0
MOV     R2, #0x60
LDR     R0, =0x2039838
MOV     R1, #0x1A0
MOV     R2, #0x60 
BL      GXS_LoadBGPltt
MOV     R2, #0x7C00
MOV     R0, #0x5000000
STRH    R2, [R0]
LDR     R1, =0x5000018
LDR     R0, =0x5000400
STRH    R2, [R1]
STRH    R2, [R0]
LDR     R0, =0x5000418
STRH    R2, [R0]
BL      G2S_GetBG1ScrPtr
LDR     R1, =bg1SubScrPtrForDebugScreen
STR     R0, [R1]
LDR     R1, =0x4001000
LDR     R0, [R1]
ORR     R0, R0, #0x10000
STR     R0, [R1]
ADD     SP, SP, #4
LDMFD   SP!, {PC}

.globl Setup2
Setup2:
STMFD   SP!, {R4-R10,LR}
SUB     SP, SP, #8
MOV     R0, #0
BL      GXS_SetGraphicsMode
LDR     R0, =0x4001000
LDR     R1, =0xFFCFFFEF
LDR     R2, [R0]
LDR     R5, =0x4001008
BIC     R2, R2, #0x1F00
STR     R2, [R0]
LDR     R2, [R0]
LDR     R4, =0x400100A
BIC     R2, R2, #0xE000
STR     R2, [R0]
LDR     R2, [R0]
LDR     LR, =0x400100C
AND     R1, R2, R1
STR     R1, [R0]
LDR     R1, [R0]
LDR     R12, =0x400100E
BIC     R1, R1, #0x60
STR     R1, [R0]
LDR     R1, [R0]
MOV     R2, #0
BIC     R1, R1, #0x800000
STR     R1, [R0]
LDRH    R0, [R5]
LDR     R9, =0x4001010
LDR     R8, =0x4001014
AND     R0, R0, #0x43
STRH    R0, [R5]
LDRH    R0, [R4]
LDR     R7, =0x4001018
LDR     R6, =0x400101C
AND     R0, R0, #0x43
STRH    R0, [R4]
LDRH    R3, [LR]
LDR     R0, =0x4001020
LDR     R1, =0x2026248

AND     R3, R3, #0x43
STRH    R3, [LR]
LDRH    R10, [R12]
MOV     R3, R2
AND     R10, R10, #0x43
STRH    R10, [R12]
LDRH    R10, [R5]
BIC     R10, R10, #0x40
STRH    R10, [R5]
LDRH    R10, [R4]
BIC     R10, R10, #0x40
STRH    R10, [R4]
LDRH    R10, [LR]
BIC     R10, R10, #0x40
STRH    R10, [LR]
LDRH    R10, [R12]
BIC     R10, R10, #0x40
STRH    R10, [R12]
LDRH    R10, [R5]
BIC     R10, R10, #3
STRH    R10, [R5]
LDRH    R5, [R4]
BIC     R5, R5, #3
STRH    R5, [R4]
LDRH    R4, [LR]
BIC     R4, R4, #3
STRH    R4, [LR]
LDRH    R4, [R12]
BIC     R4, R4, #3
STRH    R4, [R12]
STR     R2, [R9]
STR     R2, [R8]
STR     R2, [R7]
STR     R2, [R6]
STR     R2, [SP,#0]
STR     R2, [SP,#4]
BL      G2x_SetBGyAffine
MOV     R2, #0
STR     R2, [SP,#0]
STR     R2, [SP,#4]
LDR     R0, =0x4001030
LDR     R1, =0x2026248
MOV     R3, R2
BL      G2x_SetBGyAffine
LDR     R2, =0x4001048
LDR     R7, =0x400104A
LDRH    R0, [R2]
MOV     R1, #0
LDR     LR, =0x4001040
BIC     R0, R0, #0x3F
STRH    R0, [R2]
LDRH    R0, [R2]
LDR     R12, =0x4001044
LDR     R5, =0x4001042
BIC     R0, R0, #0x3F00
STRH    R0, [R2]
LDRH    R0, [R7]
LDR     R4, =0x4001046
LDR     R3, =0x400104C
BIC     R0, R0, #0x3F
STRH    R0, [R7]
LDRH    R6, [R7]
LDR     R2, =0x400104D
LDR     R0, =0x400106C
BIC     R6, R6, #0x3F00
STRH    R6, [R7]
STRH    R1, [LR]
STRH    R1, [R12]
STRH    R1, [R5]
STRH    R1, [R4]
STRB    R1, [R3]
STRB    R1, [R2]
BL      GXx_SetMasterBrightness_
ADD     SP, SP, #8
LDMFD   SP!, {R4-R10,PC}