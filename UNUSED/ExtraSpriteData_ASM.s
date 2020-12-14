#define BLOCK_6_DATA_LENGTH 0x10

nsub_0209C030_ov_00:
    SUB     SP, SP, #4
    LDR     R3, [R7, #12]
    STR     R3, [SP, #8]
    MOV     R3, #0
    B       0x209C034
  
nsub_0209C054_ov_00:
    ADD     R2, SP, #12
    BL      createActorWithExtraArgs
    ADD     SP, SP, #4
    B       0x209C058
    
nsub_0201FBB4:
    LDR     R0, =0x0208B148
    LDR     R1, =0x0208B180
    LDR     R2, [R1]
    MOV     R4, #0
    STR     R4, [R0]
 
.loop:
    LDR     R3, [R2]
    CMP     R3, #0xFFFFFFFF
    ADDNE   R4, #1
    ADDNE   R2, #0x10
    BNE     .loop
    
    STR     R4, [R0]
    ADD     SP, SP, #4
    B       0x201FBB8
    
nsub_0201F958_main:
    ADD     R10, R10, #BLOCK_6_DATA_LENGTH
    B       0x201F95C
    
nsub_0201FA1C_main:
    ADD     R8, R8, #BLOCK_6_DATA_LENGTH
    B       0x201FA20
    
nsub_0201FA50_main:
    MOV     R0, #BLOCK_6_DATA_LENGTH
    B       0x201FA54

nsub_0209B2BC_ov_00:
    ADD     R7, R7, #BLOCK_6_DATA_LENGTH
    B       0x209B2C0
   
nsub_0209B734_ov_00:
    LDRH    R1, [R8,#BLOCK_6_DATA_LENGTH]!
    B       0x209B738
    
nsub_0209BD80_ov_00:
    ADD     R5, R5, #BLOCK_6_DATA_LENGTH
    B       0x209BD84    
   
nsub_0201F3FC_main:
    ADD     R5, R5, #BLOCK_6_DATA_LENGTH
    B       0x201F400
    
nsub_0209C2D0_ov_00:    
    LDRH    R1, [R2,#BLOCK_6_DATA_LENGTH]!
    B       0x209C2D4
    
nsub_02168C54_ov_36:
    LDRH    R8, [R5,#BLOCK_6_DATA_LENGTH]!
    B       0x2168C58
    
nsub_0209B630_ov_00:
    LDRH    R2, [R9,#BLOCK_6_DATA_LENGTH]!
    B       0x209B634
    
    
nsub_0201F9B4:
    LDR     R3, [R8]
    LDR     R4, [R8,#0x10]
    STR     R3, [R8,#0x10]
    STR     R4, [R8]
    LDR     R3, [R8,#0x04]
    LDR     R4, [R8,#0x14]
    STR     R3, [R8,#0x14]
    STR     R4, [R8,#0x04]
    LDR     R3, [R8,#0x08]
    LDR     R4, [R8,#0x18]
    STR     R3, [R8,#0x18]
    STR     R4, [R8,#0x08]
    LDR     R3, [R8,#0x0C]
    LDR     R4, [R8,#0x1C]
    STR     R3, [R8,#0x1C]
    STR     R4, [R8,#0x0C]
    LDRB    R0, [R7]
    LDRB    R12, [R7,#1]
    STRB    R12, [R7]
    STRB    R0, [R7,#1]
    B       0x0201FA14
  