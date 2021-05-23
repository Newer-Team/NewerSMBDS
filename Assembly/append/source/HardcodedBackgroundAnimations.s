repl_020BCE4C_ov_00:            @ R1 = Tileset ID
    LDR     R2, =0x208B168      @ Pointer to block 1
    LDR     R2, [R2]
    LDRH    R0, [R2,#6]         @ Get Top BG ID
    LDRH    R2, [R2,#18]        @ Get Bottom BG ID
    CMP     R0, #7              
    BNE     .TryForSlot31
    CMP     R2, #7
    BNE     .TryForSlot31
    MOV     R2, #1
    B       .TurnOn             @ If both BG IDs = 7, turn on lava waterfall animations.
    
.TryForSlot31:
    CMP     R0, #31     
    BNE     .NotLava
    CMP     R2, #31     
    BNE     .NotLava
    MOV     R2, #1
    B       .TurnOn             @ If both BG IDs = 31, turn on lava waterfall animations.
    
.NotLava:                       @ Well, the BG IDs don't match, but let's check if we can enable ice animations.
@   CMP     R1, #3              @ For tileset ID 3...
@   MOVEQ   R2, #2
@   BEQ     .TurnOn
@   BNE     .Exit
@
@   CMP     R1, #17             @ And for tileset ID 17.
@   MOVEQ   R2, #2
@   BEQ     .TurnOn
    B       .Exit
    
.TurnOn:
    LDR     R0, =0x20CAC7C      
    STRB    R2, [R0]

.Exit:
    B       0x20BCE84           @ Exit.
    
