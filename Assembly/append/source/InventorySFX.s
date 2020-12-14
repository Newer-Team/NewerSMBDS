
@ Reset the flag if SELECT isn't held
hook_020C03C8_ov_00:
    LDR     R0, =#0x02087650
    LDRH    R0, [R0]
    AND     R0, R0, #4
    CMP     R0, #4
    LDRNE   R0, =SelectHeldForInventoryEmptySfx
    MOVNE   R1, #0
    STRNEB  R1, [R0]
    LDR     R0, =#0x02087652
    LDRH    R0, [R0]
    AND     R0, R0, #4
    CMP     R0, #4
    LDREQ   R0, =SelectWasPressedRecently
    MOVEQ   R1, #1
    STREQB  R1, [R0]
    BX      LR

hook_020C076C_ov_00:
    LDR     R0, =SelectWasPressedRecently
    MOV     R1, #0
    STRB    R1, [R0]
    BX      LR

@ If the flag is true, return; else, play the sound and set the flag to true
@ Also verify that SELECT is actually held, because sometimes this crazy game
@ tries to play the sound *after* you let go of SELECT
repl_020C0698_ov_00:
    LDR     R2, =SelectHeldForInventoryEmptySfx
    LDRB    R3, [R2]
    CMP     R3, #0
    BNE     0x020C069C
    LDR     R2, =SelectWasPressedRecently
    LDRB    R3, [R2]
    CMP     R3, #1
    BNE     0x020C069C
    MOV     R3, #0
    STRB    R3, [R2]
    LDR     R2, =SelectHeldForInventoryEmptySfx
    MOV     R3, #1
    STRB    R3, [R2]
    BL      PlaySNDEffect
    B       0x020C069C


.data
SelectWasPressedRecently:
    .byte   0
SelectHeldForInventoryEmptySfx:
    .byte   0