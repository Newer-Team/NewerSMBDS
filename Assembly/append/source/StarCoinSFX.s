
nsub_02006884_main: @ Checking for collection of the final uncollected SC
    STMFD   SP!, {R0-R12, LR}
    STMFD   SP!, {R0, LR}
    BLX     GetStarCoinsCollectedInSave
    LDMFD   SP!, {R1, LR}
    AND     R0, R0, #7
    ORR     R0, R0, R1
    LDR     R1, =midwayStarCoins
    LDRB    R1, [R1]
    ORR     R0, R0, R1
    CMP     R0, #7
    BEQ     starCoinSfx_return
    ORR     R0, R0, LR
    CMP     R0, #7
    MOVEQ   R0, #0x118
    MOVEQ   R1, #0
    BLEQ    PlaySNDEffect
    B       starCoinSfx_return

starCoinSfx_return:
    LDMFD   SP!, {R0-R12, LR}
    ANDS    R2, R0, LR @ the instruction we nsubbed away
    B       0x02006888

nsub_020068B0_main: @ Checking for collecting all three SCs in a single run
    LDRB    R12, [R1] @ the instruction we nsubbed away
    STMFD   SP!, {R0-R12, LR}
    CMP     R12, #7
    MOVEQ   R0, #0x118
    MOVEQ   R1, #0
    BLEQ    PlaySNDEffect
    LDMFD   SP!, {R0-R12, LR}
    B       0x020068B4
