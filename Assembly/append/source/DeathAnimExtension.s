
@ Retail uses 120 and 77
#define DEATH_SEQUENCE_LENGTH_1 160
#define DEATH_SEQUENCE_LENGTH_2 130


@ Death by falling
repl_02119778_ov_0A:
    LDR     R1, =#DEATH_SEQUENCE_LENGTH_1
    B       0x0211977C

@ Death by enemy / lava / poison water
repl_021171EC_ov_0A:
    LDR     R1, =#DEATH_SEQUENCE_LENGTH_2
    B       0x021171F0

@ If you ever want to change how long Mario flails his arms for
@ before flipping over and falling, here you go:
@ repl_02119C0C_ov_0A:
@     MOV     R3, #duration
@     B       0x02119C10