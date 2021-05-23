nsub_0215E768_ov_36:
    LDRB    R3, [R1,#0xA]
    CMP     R3, #0xEA
    BNE     AllowItem
    
    LDR     R3,=0x0208B32C  @ Inventory Item 
    LDRB    R3, [R3]
    CMP     R3, #0x5
    BNE     AllowItem   

    MOV     R3, #0x1
    STRB    R3, [R1,#0xF]

AllowItem:
    LDRB    R1, [R1,#0xF]
    B       0x0215E76C
