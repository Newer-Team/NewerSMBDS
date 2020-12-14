#include "auto_consts_files.h"

@--------SKIPS-----------

@ Skip loading Pa1 NCG
nsub_020BD004_ov_00:
    B       0x020BD008

@ Skip loading Pa1 NCL
nsub_020BCCF8_ov_00:
    MOV     R0, R5
    BL      0x020BCE40
    B       0x020BCD2C

@ Skip loading Pa1 NSC
nsub_020BB9CC_ov_00:
    B       0x020BB9D0

@ Skip loading Pa2 NCG and NCL
nsub_020AE024_ov_00:
    B       0x020AE058

@ Skip loading Pa2 NSC
nsub_020AE19C_ov_00:
    B       0x020AE1A0


@-------DIFFERENT LOADS-------

@ Pa1 UNT HD
nsub_020B3A34_ov_00:
    LDR     R0, =FOLDERADJ_TILESETS_PA1
    LDR     R1, =0x020CACBC
    LDRH    R1, [R1]
    MOV     R2, #8
    MUL     R2, R1
    ADD     R0, #3
    ADD     R0, R2
    B       0x020B3A48

@ Pa1 UNT
nsub_020B3A50_ov_00:
    LDR     R0, =FOLDERADJ_TILESETS_PA1
    LDR     R1, =0x020CACBC
    LDRH    R1, [R1]
    MOV     R2, #8
    MUL     R2, R1
    ADD     R0, #2
    ADD     R0, R2
    LDR     R1, [SP,#4]
    B       0x020B3A64

@ Pa1 CHK
nsub_020AF9A8_ov_00:
    LDR     R0, =FOLDERADJ_TILESETS_PA1
    LDR     R1, =0x020CACBC
    LDRH    R1, [R1]
    MOV     R2, #8
    MUL     R2, R1
    ADD     R0, R2
    LDR     R1, =tilesetBehaviorsPtr
    LDR     R1, [R1]
    BL      loadFileByExtId_LZ_Dest
    B       0x020AF9DC

@ Pa1 PNL
nsub_020BB994_ov_00:
    B 0x020BB9D0

@-------DIFFERENT FUNCTIONALITY-------

@ Disable drawing tiles that aren't Pa0
nsub_020AFEF0_ov_00:
    CMP     R5, #1024
    MOVGE   R5, #0
    MOV     R0, R10
    B       0x020AFEF4

nsub_020AFDE4_ov_00:
    CMP     R5, #1024
    MOVGE   R5, #0
    MOV     R0, R10
    B       0x020AFDE8

@ Disable original randomization
repl_020B556C_ov_00:
    BX LR

@ Disable loading background animations for 1-1
nsub_020B84CC_ov_00:
    BX LR

