
@nsub_0205ECF4_main:         @ Adjusts music ID to music ID * 2 [+ 1 if GBA RAM Pak is in].
@    LDR     R1, =sCurrent
@    STMFD   SP!, {R1}
@    LSL     R0, #1
@    LDR     R1, =PakRamIn
@    LDRB    R1, [R1]
@    CMP     R1, #1
@    ADDEQ   R0, #1
@    LDMFD   SP!, {R1}
@    B       0x0205ECF8
@
@nsub_02011A30_main:
@    STMFD   SP!, {R0}
@    LDR     R0, =PakRamIn
@    LDRB    R0, [R0]
@    CMP     R0, #0
@    LDMFD   SP!, {R0}
@    BNE     0x02011A48
@    CMPEQ   R0, #0
@    BEQ     0x02011A48
@    BLNE    0x0204DA18
@    B       0x02011A38

nsub_0205ECF4_main:                     @ TODO: remove support for sequenced music entirely
    LSL     R0, R0, #1
    ADD     R0, #1
    LDR     R1, =sCurrent
    B       0x0205ECF8

nsub_02011A30_main:
    B       0x02011A48

nsub_02009444_main:
    MOV     R0, #0xFFFFFFFF          @ Set DMA channel number for ROM. 0xFFFFFFFF to make it copy using CPU.
    B       0x02009448