@ Change the sound played when jumping into a tower/castle
repl_020D5B78_ov_08:
    MOV     R0, #0x2E
    BX      LR
	
@ Remove the pipe travel sound.
repl_020D662C_ov_08: 
	BX LR

@ When a toad house appears, prevent it from playing SE_SYS_NEW_POINT_END
repl_020D2800_ov_08:
    BX      LR

@ Change the footstep sounds that play in each world
repl_020D6558_ov_08:
    LDR     R2, =NewWorldMapFootstepSFXs
    LDR     R0, [R0, #0x20]
    CMP     R0, #7
    BEQ     .CheckForXOnWorld8
.DoSound:
    MOV     R1, #0
    LDRB    R0, [R2,R0] 
    BL      PlaySNDEffect
    B       0x020D656C

@ W8 uses either of two sound effects for W8 footsteps, depending on camera X position
.CheckForXOnWorld8:
    LDR     R1, =0x20EE490
    LDR     R1, [R1]
    LDR     R3, =0x12D6B6
    CMP     R1, R3
    ADDGE   R0, #1
    B       .DoSound

.data
.balign 4
NewWorldMapFootstepSFXs:
.byte 0x13
.byte 0x12
.byte 0x13
.byte 0x13
.byte 0x15
.byte 0x12
.byte 0x13
.byte 0x14
.byte 0x12