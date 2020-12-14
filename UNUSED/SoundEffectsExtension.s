nsub_020123B8_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x020123C0
	
nsub_02012378_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x02012380

nsub_02012334_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x0201233C
	
nsub_020122F4_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x020122FC

nsub_020122B0_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x020122B8	
	
nsub_02012270_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x02012278
	
nsub_0201222C_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x02012234

nsub_020121F4_main:
	STMFD	SP!, {LR}
	BL		GetIDSFromEfficientTable
	LDMFD	SP!, {LR}
	B		0x020121FC	
	
GetIDSFromEfficientTable:
	STMFD	SP!, {R2-R5,LR}
	
	MOV     R3, R0
    MOV     R0, #0
    MOV     R1, #0

.loop:
    LDR     R2, =EfficientSFXTable
    LDRB    R2, [R2, R1]

    ADD     R0, R2
    CMP     R0, R3
    ADDLE   R1, #1
    BLE     .loop
	
    SUB     R0, R0, R2
	SUB     R0, R3, R0
	MOV     R2, R0
	MOV     R0, R1
	MOV     R1, R2
	

	
	LDMFD	SP!, {R2-R5, PC}
	
	
	
.data
.balign 4
EfficientSFXTable:
    .byte 18 @ SSAR_Levels_Misc
    .byte 34 @ SSAR_World_Maps
    .byte 10 @ SSAR_Bowser_Jr_Boss
    .byte 6 @ SSAR_Volcano_Eruptions
    .byte 7 @ SSAR_Mario_Misc_1
    .byte 11 @ SSAR_MvL_and_Credits
    .byte 8 @ SSAR_Electricity
    .byte 8 @ SSAR_Mega_Goomba
    .byte 2 @ SSAR_Tornado
    .byte 2 @ SSAR_Bullets_And_Bombs
    .byte 4 @ SSAR_Spiked_Balls
    .byte 15 @ SSAR_Enemies_Common
    .byte 2 @ SSAR_Lakitus
    .byte 1 @ SSAR_Squigglers
    .byte 17 @ SSAR_Castles
    .byte 12 @ SSAR_Water_Enemies
    .byte 5 @ SSAR_Moneybags
    .byte 13 @ SSAR_Ghost_Houses
    .byte 4 @ SSAR_Blockhoppers
    .byte 7 @ SSAR_Snow_Levels
    .byte 8 @ SSAR_Petey_Piranha
    .byte 5 @ SSAR_Mummipokey
    .byte 9 @ SSAR_Mario_Misc_2
    .byte 20 @ SSAR_Bowser_And_Dry_Bowser
    .byte 1 @ SSAR_MvL_Loading_Screen
    .byte 12 @ SSAR_Menus
    .byte 1 @ SSAR_Dorrie
    .byte 3 @ SSAR_Metal_Grates
    .byte 33 @ SSAR_Opening_Cutscene
    .byte 42 @ SSAR_Levels_Misc_1
    .byte 7 @ SSAR_Luigi_Misc_1
    .byte 57 @ SSAR_Levels_Misc_2
    .byte 9 @ SSAR_Luigi_Misc_2
    .byte 7 @ SSAR_Toad_Houses
    .byte 13 @ SSAR_Bowser_Jr
    .byte 5 @ SSAR_Pipe_Cannons
    .byte 3 @ SSAR_Bowser_Boss_Door
    .byte 9 @ SSAR_Ending_Cutscene
    .byte 9 @ SSAR_Bosses
    .byte 8 @ SSAR_Monty_Tank
    .byte 3 @ SSAR_Cork_Pipe
    .byte 0