@ Increase alloc size to make room for custom data
repl_02140D88_ov_1C:
    LDR     R0, =0x5D0
    BX      LR


@ Redirect Final Boss Flag to custom field & set substate to 0
repl_0213C48C_ov_1C:
    STR     R2, [R4,#0x5C4]     @ Store Final Boss Flag in custom Field
    MOV     R2, #0
    STR     R2, [R4,#0x574]     @ Write 0 (not final boss) into original Field
    STR     R2, [R4,#0x5CC]     @ Set jrStateSub to 0
    BX      LR


@ ExecuteState0 hook
nsub_0213C0FC_ov_1C:
    BL      BowserJr_ExecuteSubstate
    CMP     R0, #0
    ADDNE   R0, R4, #0x500      @ Continue with normal onExecute
    BNE     0x0213C100
    MOV     R0, #1              @ Return
    ADD     SP, SP, #4
    LDMFD   SP!, {R4,R5,PC}
    


@ Don't freeze if for Final Boss
nsub_0213F4B0_ov_1C:
    LDR     R0, [R5,#0x5C4]
    CMP     R0, #0x0
    MOVEQ   R0, R4              @ Freeze Mario as normal
    BEQ     0x0213F4B4
    ADD     R0, R5, #0x500      @ Don't freeze and continue
    MOV     R2, #0x1
    STRH    R2, [R0,#0xAC]
    B       0x0213F5D4

@ Don't stop music if for Final Boss
nsub_0213F380_ov_1C:
    LDR     R0, [R5,#0x5C4]
    CMP     R0, #0x1
    BEQ     0x0213F5E0
    MOV     R0, #0x1E
    B       0x0213F384


@ BowserJrBoss_ShellCoverEnd
nsub_0213DB18_ov_1C:
    LDR     R1, [R0,#0x5C4]
    CMP     R1, #0x1

    STMNEFD SP!, {R4-R6,LR}
    BNE     0x0213DB1C

    LDR     R1, [R0, #0x5C8]
    LDRB    R1, [R1, #0x55E]
    CMP     R1, #0x2
    BXHI    LR

    STMFD   SP!, {R4-R6,LR}
    B       0x0213DB1C


@ Only inform Bowser after being damaged
repl_02140420_ov_1C:
	ADD     R2, R2, #0x30
	STMFD   SP!, {R0, R1}
    LDR     R0, [R5,#0x5C4]
    CMP     R0, #0x0

    LDMEQFD   SP!, {R0, R1}
	BXEQ    LR
    
    MOV     R0, #0x3
    LDR     R1, [R5, #0x5C8]
    STRB    R0, [R1, #0x55E]

    LDMFD   SP!, {R0, R1}
	BX      LR


@ You are fucking stupid, Nintendo...
nsub_02140404_ov_1C:
    LDR     R2, [R5,#0x5C4]
    CMP     R2, #0x0
    MOVNE   R2, #0x3
    LDRNE   R3, [R5, #0x5C8]
    STRNEB  R2, [R3, #0x55E]
    BNE     0x02140414

    CMP     R1, #0x0
    B       0x02140408

nsub_02140C40_ov_1C:
    STMFD   SP!, {R1, LR}

    LDR     R1, [R0,#0x5C4]
    CMP     R1, #0x0
    MOVNE   R0, #0x2
    LDMNEFD SP!, {R1, PC}

    ADD     R0, R0, #0x500
    LDRH    R0, [R0,#0x88]
    CMP     R0, #0x30
    MOVCC   R0, #0
    LDMCCFD SP!, {R1, PC}

    CMP     R0, #0x60
    MOVCC   R0, #1
    MOVCS   R0, #2

    LDMFD   SP!, {R1, PC}

@nsub_02140040_ov_1C:
@    MOV     R5, #0
@    LDR     R1, [R4,#0x5C4]
@    CMP     R1, #0x1
@    BNE     0x02140044
@    MOV     R0, #0x1
@    B       0x02140048


@ Groundpound deals normal damage 1
@ If Jr wasn't hurt before

nsub_0214000C_ov_1C:
    @ Return if is not for final boss
    LDR     R5, [R4,#0x5C4]
    CMP     R5, #0x1
    STRNEH  R2, [R1,#0x88]
    BNE     0x02140010

    @ Deal Damage
    LDRH    R5, [R1,#0x88]
    ADD     R5, R5, #0x30
    STRH    R5, [R1,#0x88]

    @ Inform Bowser
    MOV     R5, #0x3
    LDR     R3, [R4, #0x5C8]
    STRB    R5, [R3, #0x55E]

    B       0x02140010


@ Groundpound deals normal damage 2

@ Duplicate code by Ninty.
@ This way I only have to do stuff once :)
nsub_02140078_ov_1C:
    B       0x021400C0

nsub_021400C4_ov_1C:
    LDR     R5, [R4,#0x5C4]
    CMP     R5, #0x1

    MOV     R5, #1
    BNE     0x021400C8

    ADD     R0, R4, #0x500
    LDRH    R1, [R0,#0x88]
    ADD     R1, R1, #0x30   @ Normal Damage
    STRH    R1, [R0,#0x88]

    MOV     R0, #0x3
    LDR     R1, [R4, #0x5C8]
    STRB    R0, [R1, #0x55E]

    B       0x021400E8


@ Shell powerup cannot flip Jr.
@ This breaks the first final boss phase otherwise...
nsub_0213FF30_ov_1C:
    LDR     R3, [R4,#0x5C4]
    CMP     R3, #0x1
    BLNE    0x02140CAC  @ Flip Jr.
    B       0x0213FF34


@ Shells don't flip Jr when being in cover
@ Needed as the sequence breaks otherwise
nsub_02140160_ov_1C:
    MOV     R4, R0

    LDR     R0, [R4,#0x5C4]
    CMP     R0, #0x1
    BNE     0x02140164

    LDR     R0, [R4, #0x5C8]
    LDRB    R0, [R0, #0x55E]
    CMP     R0, #0x3

    BCC     0x02140164

    ADD     SP, SP, #8
    LDMFD   SP!, {R4,PC}


@ Bowser Jr. Death
@ Don't freeze stuff if for Final Boss
nsub_0213D194_ov_1C:
    BNE     0x0213D3BC

    LDR     R0, [R5,#0x5C4]
    CMP     R0, #0x0
    BNE     0x0213D3BC

    B       0x0213D198


@ Bowser Jr Death Bounce
@ Don't trigger ending cutscene and kill Bowser Jr. if for Final Boss
nsub_0213D2A4_ov_1C:
    LDR     R0, [R5,#0x5C4]
    CMP     R0, #0x1

    LDRNE   R0, [R5,#0x574]
    BNE     0x0213D2A8

    @ Set Jr State to 4 aka dead :P
    MOV     R0, #0x4
    LDR     R1, [R5, #0x5C8]
    STRB    R0, [R1, #0x55E]

    MOV     R0, R5
    BL      0x0209D988
    B       0x0213D3BC


@ Bowser Jr. Exit Level
@ Don't exit the level and delete Bowser Jr. if for Final Boss
nsub_0213CAB0_ov_1C:
    LDR     R4, [R0,#0x5C4]
    CMP     R4, #0x0

    MOVEQ   R4, R0
    BEQ     0x0213CAB4

    @ R.I.P. - Until the next game Jr.
    BL      Base__deleteIt
    B       0x0213CB24



@ Bowser Jr. Substate Addr
hook_02140C9C_ov_1C:
    @LDR     R0,=#0xC03C0
    @ADD     R1, R1, R0
    @LDR     R0, [R4,#0x504]
    @print   "Execute State: %r1% %r0%\r\n"
    bx      lr


@ Damage modifiers

@ No fireball damage
nsub_021401EC_ov_1C:
    MOV     R4, R0
    LDR     R0, [R4,#0x5C4]
    CMP     R0, #0x0
    BEQ     0x021401F0
    B       0x021402A8
	
@ Do not start his shitty music
repl_0213F558_ov_1C:
    LDR     R0, [R4,#0x5C4]
    CMP     R0, #0x0
    BEQ     0x02011F04
	BXNE    LR


@ (Not related to final boss)
@ Make it so that he only growls (?) once before grabbing
@ Peach and running away during the post-castle-battle cutscenes;
@ this allows us to replace SE_EMY_KPJR_GAU with a single sound
@ twice as long as retail's.
repl_02142440_ov_28:
    BX LR
