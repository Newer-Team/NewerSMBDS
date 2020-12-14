#include "Enemy.h"

typedef struct
{
	// size: 0xF
	u16 classID;		// 0
	s8 offsetX;			// 2
	s8 offsetY;			// 3
    u32 spriteData;		// 4
    u16 particleID;		// 8
	s8 particleOffsetX;	// A
	s8 particleOffsetY;	// B
    u16 sfxID;			// C
	u16 pad;			// E
} ExtraPrizeSettigs;

typedef struct
{
	// EnemyActor class stuff
	u32* vtable;
	
	u32 unk0;			// 4
	
	u32 spriteData;		// 8
	
	u32 ClassID;		// C
	u8 unk1[4];			// D
	
	Base base;			// 14
	
	u32 unk2[2];		// 54
	
	Vec3 position;		// 5C -- inited from createSprite_pos
	Vec3 lastPosition;	// 6C -- inited from createSprite_pos
	Vec3 unkv2;			// 7C -- inited to 0,0,0
	Vec3 unkv3;			// 8C -- seems to be used as a velocity vector?
	S16Vec3 rotation;	// 9C -- inited from createSprite_unk
	S16Vec3 unkv5;		// A8 -- inited from createSprite_unk
	
	u32 unk3[6];		// B4
	
	Vec3 velocity;		// CC -- velocity
	Vec3 unkv7;			// DC
	Vec3 scale;			// EC -- inited from createSprite_arg0
	Vec3 unkv9;			// FC -- inited to 0,0,0
	Vec3 unkv10;		// 10C -- inited to 0x8000,0x8000,0x8000
	
	u8 unk4;			// 11C -- inited to 0
	u8 unk5;			// 11D -- inited to 1
	u8 unk6;			// 11E -- inited from createSprite_arg4
	u8 unk7;			// 11F -- inited to 0x80
	
	// 120 -- ActivePhysics
	ActivePhysics activephysics;
	
	// 1D0 -- Collider
	Collider collider;
	
	u32 unk8[0x5];						// 2A8
	u8  unk9;							// 2BC
	u8  direction;  					// 2BD
	u16 unk10;							// 2BE
	
	u32 unk11[0x1D];					// 2C0
	
	u8 EventFieldID2;					// 334
	u8 EventFieldID1; 					// 335
	u16 unk12;							// 336
	u32 unk13;							// 338
	u8 EventField2ID2;					// 33C
	u8 EventField2ID1; 					// 33D
	u16 unk14;							// 33E
	u32 state;							// 340
	
	u32 unk15[0xA];						// 344
	Vec2 ExistDist;						// 36C
	Vec2 DrawDist;						// 378
	Vec2 Unk2Vec2;						// 384
	Vec2 Unk3Vec2;						// 390 vector copied here from ActivePhysics when a collision occurs
	u32  unk16[0x16];					// 39C
	TexInfo tex;						// 3F4
	
	bool gotten;
	bool outofblock;
	u8 type;
	
	int ticker;
	int animtimer;
	
	s32 initialypos;	
} ExtraPrize;

void ExtraPrizeCollision(ActivePhysics*, ActivePhysics*);
void ExtraPrizeKoopaCollision(ExtraPrize* prz, EnemyClassObject* Shell);
void ExtraPrize_Get3UP(ExtraPrize*);	 
void ExtraPrize_GetPlusClock(ExtraPrize*,bool type);		
void ExtraPrize_GetDragonCoin(ExtraPrize*);	
void ExtraPrize_GetBigCoin(ExtraPrize*);
void ExtraPrize_Get3UP(ExtraPrize*);