
@ Replaces Profile ID 153
@ Overlay 71/0x47 offset: 0x1186C0
@02179710_ov_47: WoodBowser_ctor
@02179718_ov_47: WoodBowser_loadFiles

@ Replaces profile ID 251 (Rotating Spiked ? duplicate)
0216D9AC_ov_36: Fireball_ctor
0216D9B4_ov_36: Fireball_loadFiles

@ Replaces profile ID ???
0216FF58_ov_36: Midpoint_ctor
0216FF60_ov_36: Midpoint_loadFiles

@ Replaces Profile ID 106 (Fire Snake duplicate)
0213ED38_ov_17: MrSun_ctor
0213ED40_ov_17: MrSun_loadFiles

@ Replaces Profile ID 255 with the Cannonball
0212533C_ov_0A: Cannonball_ctor
02125344_ov_0A: Cannonball_loadFiles

@ Replaces profile 131, sprite 5
02039E40: LavaWave_Profile
020C22C2_ov_00: 0x131

@ Replaces Profile ID 167
02125F28_ov_0A: Electrocutor_ctor
02125F30_ov_0A: Electrocutor_loadFiles

@ Replaces Profile ID 298
02124E70_ov_0A: MovementController_ctor

@ Replaces Profile ID 299
02124E88_ov_0A: MusicSwitcher_ctor

@ Replaces Profile ID ???
0216D9CC_ov_36: Hammer_loadFiles
0216D9C4_ov_36: Hammer_ctor

@ Chain chomps die on hammers
0217551C_ov_39: 0x00F30004

@ Fire snakes die on hammers
0213EBC4_ov_17: 0x00F30004
0213EBA8_ov_17: 0x00F30004

@ Makes hat use texture 3 if hammer suit is used
0212E534_ov_0B: 0x00200000
0212E538_ov_0B: 0x00200040

@ Hammer Brother Vtable Changes
02175568_ov_38: CustomHammerBrotherVirt49
0217556C_ov_38: CustomHammerBrotherVirt50

@ Thunderstrike Vtable Changes
02133DE4_ov_11: 0x0209CFC0

@ Thundercloud
0218FD88_ov_76: Thundercloud_ctor
0218FD90_ov_76: Thundercloud_loadFiles

@ No log chain chomps don't die on GP
0217582C_ov_39: ChainChompVirt50

@ Podoboos don't die on GP
02148B24_ov_30: ChainChompVirt50

@ Golden Chomp Star Coin
0218E06C_ov_79: SpikyBallVirt44
0218E194_ov_79: SpikyBallVirt44
0218E078_ov_79: SpikyBallVirt46
0218E19C_ov_79: SpikyBallVirt46

@ --------

@ Buzzy Beetle Vtable Changes
02179240_ov_44: CustomBuzzyBeetleVirt43

@ Itembox
02185480_ov_5A: Itembox_loadFiles
02185478_ov_5A: Itembox_ctor

@ Splunkin Vtable Changes
02179940_ov_47: CustomSplunkinVirt43
0217995C_ov_47: CustomSplunkinVirt50

@ Changes W2 Tower file.
020E87F4_ov_08: 0x000009C5

@ W6 Tree
020E88E8_ov_08: 0x000009CF
020E88EC_ov_08: 0x0000066F

@ Background changes
020C97D4_ov_00: 0x000007B0
020C97D8_ov_00: 0x000007AA
020C97DC_ov_00: 0x000007AD
020C9904_ov_00: 0x000007B1
020C9908_ov_00: 0x000007AB
020C990C_ov_00: 0x000007AE
020C9DC4_ov_00: 0x000007AF
020C9DC8_ov_00: 0x000007A9
020C9DCC_ov_00: 0x000007AC

@Bottom UI NSCs, CourseIn
020E2D44_ov_08: 0x00000BB9
020E2D48_ov_08: 0x00000BBA
020E2D4C_ov_08: 0x00000BBB
020E2D50_ov_08: 0x00000BBC
020E2D54_ov_08: 0x00000BBD

@Bottom UI NSCs, GameIn
020C2D94_ov_00: 0x00000BB4
020C2D98_ov_00: 0x00000BB5
020C2D9C_ov_00: 0x00000BB6
020C2DA0_ov_00: 0x00000BB7
020C2DA4_ov_00: 0x00000BB8

@ Not loading unnecessary files
021222EC_ov_0A: 0x6F8   
021222F8_ov_0A: 0x6F8
02122304_ov_0A: 0x6F8
02122310_ov_0A: 0x6F8
0212231C_ov_0A: 0x6F8
02122328_ov_0A: 0x6F8
02122334_ov_0A: 0x6F8
02122340_ov_0A: 0x6F8
02122358_ov_0A: 0x6F8
0212237C_ov_0A: 0x6F8
021223A0_ov_0A: 0x6F8  

@ Replaces profile 0x132, sprite 6
02039E44: Tripleblock_Profile
020C22C4_ov_00: 0x132

@ Replaces profile 0x147, sprite 7
02039E98: TextureGraphicSprite_Profile
020C22C6_ov_00: 0x147

@ Replaces profile 0x146, no sprite.
02039E94: CameraController_Profile

@ Replaces profile 0x134, sprite 8
02039E4C: Titlescreen_Profile
020C22C8_ov_00: 0x134

@ Replaces profile 0x135, sprite 9
02039E50: CreditsController_Profile
020C22CA_ov_00: 0x135

@ Replaces profile 0x136, sprite 10
02039E54: PeachCage_Profile
020C22CC_ov_00: 0x136

@ Replaces profile 0x137, sprite 11
02039E58: Minecart_Profile
020C22CE_ov_00: 0x137

@ Replaces profile 0x139, sprite 12
02039E60: MinecartEnemy_Profile
020C22D0_ov_00: 0x139

@ Replaces profile 0x148, sprite 13
02039E9C: OpeningScene_Profile
020C22D2_ov_00: 0x148

@ Replaces profile 0x149, sprite 14
02039EA0:  BowserHeadEntrance_Profile
020C22D4_ov_00: 0x149

020C53B4_ov_00: 0x0
020C53B8_ov_00: 0x100012
020C53BC_ov_00: 0xFFF8FFF7
020C53C0_ov_00: 0
020C53C4_ov_00: 0x2000

@ Replaces profile 0x14A, sprite 15
02039EA4:  KingBoo_Profile
020C22D6_ov_00: 0x14A

020C53C8_ov_00: 0x0
020C53CC_ov_00: 0x100012
020C53D0_ov_00: 0xFFF8FFF7
020C53D4_ov_00: 0
020C53D8_ov_00: 0x2000

@ Replaces profile 0x14B, sprite 16
02039EA8:  FinalBoss_Profile
020C22D8_ov_00: 0x14B

020C53DC_ov_00: 0x80008
020C53E0_ov_00: 0x40004
020C53E4_ov_00: 0
020C53E8_ov_00: 0x400040
020C53EC_ov_00: 0x467F2

@ Replaces profile 0x14C, sprite 17
02039EAC:  BowserFire_Profile
020C22DA_ov_00: 0x14C

@ Replaces profile 0x14D, sprite 18
02039EB0:  ExtraPrize_Profile
020C22DC_ov_00: 0x14D

020C5404_ov_00: 0
020C5408_ov_00: 0x40004
020C540C_ov_00: 0
020C5410_ov_00: 0x400040
020C5414_ov_00: 0x467F2


@ Actors free: 0x1, 0x2, 0x6, 0x7, 0xA, 0xF, 0x10, 0x11, 331 - 384

020C9694_ov_00: 0xBBE
020C9C84_ov_00: 0xBBF
020C9EE4_ov_00: 0xBC0
020C8334_ov_00: 0xBC1
020C8204_ov_00: 0xBC2
020C80D4_ov_00: 0xBC3

020C9698_ov_00: 0xBC4
020C9C88_ov_00: 0xBC5
020C9EE8_ov_00: 0xBC6
020C8338_ov_00: 0xBC7
020C8208_ov_00: 0xBC8
020C80D8_ov_00: 0xBC9

@ Jr. Paths.
020E6360_ov_08: World6Path
020E648C_ov_08: World8Path

@ Making W8-T2 be 8-A.
020E7478_ov_08: 0x090000A9
020E747C_ov_08: 0x9

@ Fixing 1-A unlock zooming too far.
020E831C_ov_08: 0xD000D

@Fixing 5-3 unlock zooming too far
020E8A44_ov_08: 0x80019

@Fixing 5-GH unlock zooming too far
020E8A5C_ov_08: 0x140009