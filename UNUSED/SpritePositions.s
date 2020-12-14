
// ========================================= loadSprites

nsub_0201F7CC: // x
    MOV     R0, R0,LSL#12 // originally #16
    B       0x0201F7D0

nsub_0201F7DC: // y
    MOV     R0, R0,LSL#12 // originally #16
    B       0x0201F7E0

nsub_0201F908: // x
    LDRH    R1, [R10,#2]
    LSR     R1, #4
    B       0x0201F90C

nsub_0201F918: // y
    LDRH    R1, [R10,#4]
    LSR     R1, #4
    B       0x0201F91C

// ========================================= spawnSprite

nsub_0209BF64_ov_0_d: // x
    ADD     R1, R0, R1 // originally ",LSL#4"
    B       0x0209BF68

nsub_0209BF5C_ov_0_d: // y
    ADD     R2, R2, R3 // originally ",LSL#4"
    B       0x0209BF60

// ========================================= First xref to spawnSprite

nsub_0209B1D8_ov_0_d: // x
    LDRH    R1, [R7,#2]
    LSR     R1, #4
    B       0x0209B1DC

nsub_0209B1CC_ov_0_d: // y
    LDRH    R3, [R7,#4]
    LSR     R3, #4
    B       0x0209B1D0

nsub_0209BE18_ov_0_d: // x
    LDRH    R10, [R0,#2]
    LSR     R10, #4
    B       0x0209BE1C

nsub_0209BE20_ov_0_d: // y
    LDRH    R8, [R0,#4]
    LSR     R8, #4
    B       0x0209BE24

// ========================================= Second xref to spawnSprite

nsub_0209B46C_ov_0_d: // x
    LDRH    R2, [R9,#2]
    LSR     R2, #4
    B       0x0209B470

nsub_0209B484_ov_0_d: // y
    LDRH    R2, [R9,#4]
    LSR     R2, #4
    B       0x0209B488

nsub_0209B520_ov_0_d: // x
    LDRH    R2, [R9,#2]
    LSR     R2, #4
    B       0x0209B524

nsub_0209B544_ov_0_d: // y
    LDRH    R0, [R9,#4]
    LSR     R0, #4
    B       0x0209B548

// ========================================= Third xref to spawnSprite

nsub_0209B6D0_ov_0_d: // x
    LDRH    R2, [R8,#2]
    LSR     R2, #4
    B       0x0209B6D4

nsub_0209B6E8_ov_0_d: // y
    LDRH    R2, [R8,#4]
    LSR     R2, #4
    B       0x0209B6EC

// ========================================= Fourth xref to spawnSprite

nsub_0209BB84_ov_0_d: // x
    LDRH    R3, [R5,#2]
    LSR     R3, #4
    B       0x0209BB88

nsub_0209BB8C_ov_0_d: // y
    LDRH    R2, [R5,#4]
    LSR     R2, #4
    B       0x0209BB90

// (0209BE18/0209BE20 is already listed under the first xref)
