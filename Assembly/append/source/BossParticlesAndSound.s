nsub_02022D98_main:
    LDR     R0, =0x208B19C  @ Pointer to block 14
    LDR     R0, [R0]
    LDR     R0, [R0, #0xF]
    CMP     R0, #9          
    MOVEQ   R0, #0x0
    BXEQ    LR
    CMP     R0, #2          @ If Bowser
    MOVLT   R0, #0x0        
    BXLT    LR
    BEQ     .Bowser

    LDR     R1, =0x2026C88  @ Particle files array
    CMP     R0, #3          @ If Giga Goomba
    ADDEQ   R0, R1, #0x10
    BXEQ    LR

    CMP     R0, #4          @ If Petey
    ADDEQ   R0, R1, #0x18
    BXEQ    LR

    CMP     R0, #5          @ If Mummipokey
    MOVEQ   R0, R1
    BXEQ    LR

    CMP     R0, #6          @ If Lakithunder
    ADDEQ   R0, R1, #0x28
    BXEQ    LR

    CMP     R0, #7          @ If Cheep Skipper
    ADDEQ   R0, R1, #0x8
    BXEQ    LR

    ADD     R0, R1, #0x20   @ Else, Monty Tank
    BX      LR

.Bowser:
    LDR     R0, =0x208B180  @ Pointer to block 7
    LDR     R1, =0x208B184  @ Pointer to block 8
    LDR     R0, [R0]
    LDR     R1, [R1]
    ADD     R1, #4          @ Remove padding

    STMFD   SP!, {R0,LR}
    SUB     R0, R1, R0      @ Get size of block 7
    LDR     R1, =0xC    
    BL      FX_Div          @ Get amount of sprites in level
    LDMFD   SP!, {R0,LR}

    MOV     R12, R1
    LDR     R3, =0x2026C80

.CheckWhichBowser:      @ Determine which Bowser sprite is present
    MOV     R1, R12
    MOV     R2, #0xC
    MUL     R1, R2, R1
    LDRH    R2, [R0,R1]
    CMP     R2, #0x3A       @If Bowser
    MOVEQ   R0, R3
    BXEQ    LR
    CMP     R2, #0x3F       @If Dry Bowser
    ADDEQ   R0, R3,#0x38    
    BXEQ    LR
    CMP     R12, #0
    SUBGT   R12, #1
    BGT     .CheckWhichBowser
    ADD     R0, R3,#0x40    @Else, Giant Bowser
    BX      LR

nsub_020AD01C_ov_00:    @Load the music if Tower, Tower2, Castle, but not on TS.
    LDR     R0, =TitleScrNumber
    LDR     R0, [R0]
    CMP     R0, #0
    MOVNE   R0, #0
    BXNE    LR
    LDR     R0, =0x02085A9C 
    LDR     R0, [R0]
    CMP     R0, #0xD
    BEQ     .true
    CMP     R0, #0xE
    BEQ     .true
    CMP     R0, #0x15
    BEQ     .true
    CMP     R0, #0x16
    BEQ     .true
    MOV     R0, #0
    BX      LR

.true:
    MOV   R0, #1
    BX    LR

