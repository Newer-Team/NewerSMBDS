#include "auto_consts_files.h"

repl_0205F0E4:
    B    0x205F0F0

repl_0205F0F8:
    ADD  R1, R6, #0x7C
    LDR  R0,  =0x02096114
    STR  R0, [R1]
    LDR  R0, =0x86
    STR  R0, [R1, #4]
    BX   LR







@hksf_0205EE8C:
@    MOV R1, R6
@    BL  hkxsf_0205EE8C
@    BX  LR