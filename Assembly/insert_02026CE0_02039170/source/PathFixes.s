repl_0201FC74_main:		@ Makes Worlds 3-3 and 8-5 be seen as 1 area levels for the progress path counter

BL 0x0201F590

LDR R7, =0x02088BFC
LDR R7, [R7]
CMP R7, #2
BEQ .True1
CMP R7, #7
BEQ .True1
BNE 0x201FC78

.True1:
LDR R7, =0x02085A9C
LDR R7, [R7]
CMP R7, #3
BEQ .True2
CMP R7, #5
BEQ .True2
BNE 0x201FC78


.True2:
MOV R0, #1
B 0x201FC78