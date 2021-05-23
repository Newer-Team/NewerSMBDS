nsub_021308AC_ov_0C:        @ Load score model depending on player
    LDR     R1, =0x2085A50
    LDR     R1, [R1]
    LDR     R0, =0x9F7
    ADD     R0, R1
    MOV     R1, #0
    BL      loadFileByExtId_3dModel
    MOV     R0, #1
    B       0x021308B0

nsub_0213066C_ov_0C:        @ Disable 1UP, write 30 coins as reward into struct
    STMFD   SP, {R0}
    MOV     R0, #0x1E
    STRB    R0, [R6, #0x75C] 
    LDMFD   SP, {R0}
    B       0x2130698
    
nsub_02130694_ov_0C:        @ Disable points, write coins based on height as reward into struct
    STMFD   SP, {R0, R1}
    ADD     R0, R6, #0x700
    LDRH    R0, [R0,#0x54]
    SUB     R0, #1
    LDR     R1, =Rewards
    ADD     R1, R1, R0
    LDRB    R0, [R1]
    STRB    R0, [R6, #0x75C] 
    LDMFD   SP, {R0, R1}
    B       0x2130698
    
repl_021308D0_ov_0C:        @ Alloc size increase for reward storage 
    LDR     R0, =0x760
    BX      LR

nsub_0212FC70_ov_0C:        @ Swap score draw into our custom draw]
@   STMFD   SP, {R0-R2}
@   LDR     R0, =0x1F0E816A
@   ADD     R0, R0, R0
@   LDR     R0, =garbageCollect
@   LDR     R1, [R0, #0x40]
@   LDR     R0, [R0, #0x2C]
@   CMP     R0, R1
@   BNE     0x20045EC
@   LDMFD   SP, {R0-R2}
    ADD     R0, R5, #0x700
    LDRB    R1, [R0,#0x54]
    CMP     R1, #9          @ If score = 1UP, score = 8000
    MOVEQ   R1, #8
    SUB     R1, #1          @ Because lowest score = 1, and we want to count from 0
    MOV     R0, R5
    BL      CoinScore_draw
    B       0x212FC74

.data
.balign 4
Rewards:
    .byte 0x01              @ 1
    .byte 0x02              @ 2
    .byte 0x05              @ 5
    .byte 0x0A              @ 10
    .byte 0x0F              @ 15
    .byte 0x14              @ 20
    .byte 0x19              @ 25
    .byte 0x1E              @ 30
    .byte 0x00