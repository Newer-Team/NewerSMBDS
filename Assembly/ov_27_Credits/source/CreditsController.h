#include "Enemy.h"

typedef struct
{
	u8 Length;
	u8 CommandID;
} Command;

typedef struct
{
	Command Base;
	u16 Frames;
} DelayCommand;

typedef struct
{
	u8 Length;
	u8 CommandID;
	u16 Area;
	u16 Entrance;
	u8 BgIDTop;
	u8 BgIDBottom;
	u8 TilesetID;
	bool EndingScene;
} SwitchSceneCommand;

typedef struct
{
	Command Base;
	u8 textlen;
	char Text[];
} SetTextCommand;

typedef struct
{
	Command Base;
	s32 PosX;
	s32 PosY;
} PlacePeachCommand;

typedef struct
{
	Command Base;
	u16 FileID;
	u8  Slot;
} FileLoadOperationCommand;

typedef struct
{
	Command Base;
	u8  Slot;
} FileUnloadOperationCommand;


typedef struct
{
    u32* vtable;
    
    u32 unk0;           // 4
    
    u32 spriteData;     // 8
    
    u16 ClassID;        // C
    u8 unk1[6];         // D
    
    Base base;          // 14
    
    u32 unk2[2];        // 54
    
    Vec3 position;      // 5C -- inited from createSprite_pos
    Vec3 lastPosition;  // 6C -- inited from createSprite_pos
    Vec3 unkv2;         // 7C -- inited to 0,0,0
    Vec3 unkv3;         // 8C -- seems to be used as a velocity vector?
    S16Vec3 rotation;   // 9C -- inited from createSprite_unk
    S16Vec3 unkv5;      // A8 -- inited from createSprite_unk
    
    u32 unk3[6];        // B4
    
    Vec3 velocity;      // CC -- velocity
    Vec3 unkv7;         // DC
    Vec3 scale;         // EC -- inited from createSprite_arg0
    Vec3 unkv9;         // FC -- inited to 0,0,0
    Vec3 unkv10;        // 10C -- inited to 0x8000,0x8000,0x8000
    
    u8 unk4;            // 11C -- inited to 0
    u8 unk5;            // 11D -- inited to 1
    u8 unk6;            // 11E -- inited from createSprite_arg4
    u8 unk7;            // 11F -- inited to 0x80
    
    // 120 -- ActivePhysics
    ActivePhysics activephysics;
    
    // 1D0 -- Collider
    Collider collider;
    
    u32 unk8[0x5];                      // 2A8
    u8  unk9;                           // 2BC
    u8  direction;                      // 2BD
    u16 ViewID;                         // 2BE
    
    u32 unk11[0x13];                    // 2C0
	
	Vec3 unkv11;						// 30C
	Vec3 unkv12;						// 31C

	u32 unkm12[0x2];					// 32C
    
    u8 EventFieldID2;                   // 334
    u8 EventFieldID1;                   // 335
    u16 unk12;                          // 336
    u32 unk13;                          // 338
    u8 EventField2ID2;                  // 33C
    u8 EventField2ID1;                  // 33D
    u16 unk14;                          // 33E
    u32 state;                          // 340
    
    u32 unk15[0x6];                     // 344
	
	Vec3 unkv13;						// 35C

    Vec2 ExistDist;                     // 36C
    Vec2 DrawDist;                      // 378
    Vec2 DeleteDist;                    // 384
    Vec2 Unk3Vec2;                      // 390 vector copied here from ActivePhysics when a collision occurs
	Vec2 Unk4Vec2;						// 39C
	
    u32  unk16[0x11];                   // 3A8
    u8   unk17;                         // 3EC
    u8   unk18;                         // 3ED
    u8   unk19;                         // 3EE
    u8   unk20;                         // 3EF
    u8   unk21;                         // 3F0

	TexInfo TexLogo;
	TexInfo TexHeader;
	TexInfo TexBody;
	TexInfo TheEndText;
	TexInfo DarkTex;
	TexInfo CoinTex;
	
	EnemyClassObject* ActivePlayer;
	EnemyClassObject* InactivePlayer;
	
	u32* BottomNSCPtr;
	u32* CommandFilePtr;
	Command* CurrentCommandPtr;
	
	u32 ExtraFileIDs[6];
	
	int ScreenBrightness;
	int CurrentCommand;
	int LogoFade;
	int LogoFall;
	int HeaderFade;
	int BodyFade;
	int CoinFade;
	int DarknessFade;
	int TheEndBounceIn;
	int Desmume;
	int BottomFallTimer;
	int BottomFallTimer2;
	int FireworksTimer;
	int AnimationTimer;
	int CollectedCoins;
	bool MakingFireworks;
	
	char HeaderText[255];
	char BodyText[255];
	
} CreditsController;

void Command_SetNext(CreditsController* credits);
void Command_Delay(CreditsController* credits);
void Command_SwitchScene(CreditsController* credits);
void Command_FadeInLogo(CreditsController* credits);
void Command_DropLogo(CreditsController* credits);
void Command_FadeToBlack(CreditsController* credits);
void Command_FadeFromBlack(CreditsController* credits);
void Command_FadeToWhite(CreditsController* credits);
void Command_FadeFromWhite(CreditsController* credits);
void Command_ShowText(CreditsController* credits);
void Command_HideText(CreditsController* credits);
void Command_SetHeaderText(CreditsController* credits);
void Command_ShowHeaderText(CreditsController* credits);
void Command_HideHeaderText(CreditsController* credits);
void Command_SetBodyText(CreditsController* credits);
void Command_ShowBodyText(CreditsController* credits);
void Command_HideBodyText(CreditsController* credits);
void Command_DisablePlayerControl(CreditsController* credits);
void Command_EnablePlayerControl(CreditsController* credits);
void Command_EnableLowGravityPhysics(CreditsController* credits);
void Command_DisableLowGravityPhysics(CreditsController* credits);
void Command_UnlockInactiveCharacter(CreditsController* credits);
void Command_SetPlayersFacingScreen(CreditsController* credits);
void Command_PlacePeach(CreditsController* credits);
void Command_PlayCharacterWinAnimations(CreditsController* credits);
void Command_BeginFireworks(CreditsController* credits);
void Command_EndFireworks(CreditsController* credits);
void Command_ShowDarknessOverlay(CreditsController* credits);
void Command_ShowTheEnd(CreditsController* credits);
void Command_HideTheEnd(CreditsController* credits);
void Command_ShowCoinCounter(CreditsController* credits);
void Command_HideCoinCounter(CreditsController* credits);
void Command_LoadFile(CreditsController* credits);
void Command_UnloadFile(CreditsController* credits);
void Command_ExitStage(CreditsController* credits);

void DoFireworks(CreditsController* credits);
void Player_InactiveState(EnemyClassObject* Player);
void Player_WinningState(EnemyClassObject* Player);
void LookUp(CreditsController* credits);
void Letter_Draw(CreditsController* credits, s32 DrawX, s32 DrawY, bool IsHeader, char ASCII);
void Logo_Draw(CreditsController* credits, s32 DrawX, s32 DrawY); 
void TheEnd_Draw(CreditsController* credits, s32 DrawX, s32 DrawY); 
void CoinScore_Draw(CreditsController* credits, s32 DrawX, s32 DrawY, int num);
void DarknessFade_Draw(CreditsController* credits, s32 DrawX, s32 DrawY);
