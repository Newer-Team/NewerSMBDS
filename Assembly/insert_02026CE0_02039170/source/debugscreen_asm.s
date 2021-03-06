#include "Version.h"

@ Disable key combination that shows the debug screen
repl_02005B58:
	MOV R5, #5
	
	@Stop all DMAs
	@This avoids garbled graphics on debugscreen because of HDMA
	LDR r12, =0x040000B8
	STR R5, [R12]
	STR R5, [R12, #0xC]
	STR R5, [R12, #0x18]
	STR R5, [R12, #0x24]

	bx lr

@ Calling showDebugScreen twice? No thank you.
repl_02005C20:
	BX LR

@ Do not display stack trace by default
repl_02005C30:
	BX LR

@ Go to custom debug menu
repl_02005C2C:
	PUSH {LR}
	BL _Z9debugMenuv
	POP {PC}

@ Replace crash dump label (Normally BUILDTIME)
repl_02005EC4:
	LDR R2, =crashDumpLabel
	BX LR

@ Normally BUILDTIME is loaded here
crashDumpLabel:
  .ascii "NewerDS v" GAME_VERSION " Crash Dump"
  
  
nsub_02008058:
	LDRB    R0, [R7,#0x5BC]
	TST     R0, #2
	BEQ     .return

	LDR     R0, =0x4001050
	MOV     R1, #63
	MOV     R2, #0
	BL      0x020622EC

.return:
	LDRB    R0, [R7,#0x5BC]
	AND     R0, R0, #1
	B       0x0200805C