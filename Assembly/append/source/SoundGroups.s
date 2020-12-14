
#define TOTAL_SOUND_GROUPS #0x3B

repl_02011BB4_main:
    CMP     R4, TOTAL_SOUND_GROUPS
    BX      LR
nsub_02011B38_main:
    MOV     R3, TOTAL_SOUND_GROUPS
    B       0x02011B3C
repl_02012730_main:
    MOV     R1, TOTAL_SOUND_GROUPS
    BX      LR