#include "Enemy.h"
typedef struct
{
	// EnemyActor class stuff
	u32* vtable;
	
	u32 unk0;			// 4
	
	u32 spriteData;		// 8
	
	u8 unk1[8];			// C
	
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
	
	// 2A8 -- anotherCollider
	// @2BE -> view ID
	// @240 -> state
	
	u32 unk8[0x5];						// 2A8
	u8  unk9;							// 2BC
	u8  direction;  					// 2BD
	u16 ViewID;							// 2BE
	
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
	Vec2 DeleteDist;					// 384
	Vec2 Unk3Vec2;						// 390 vector copied here from ActivePhysics when a collision occurs
	u32  unk16[0x16];					// 39C
	
	TexInfo Tex;						// 3F4
	TexInfo NormalTex;					// 3FC
	TexInfo ElectroTex;					// 404
	
	u16 SizeX;							// 40C	
	u16 SizeY;							// 40E	
	int PlayingSND;						// 410
	int ElectroTimer;					// 414	
	bool electrified;					// 418	
	bool style;							// 419	
	u16 pad2;							// 41A
	
	SolidCollision solidcollision;		// 41C
	ActivePhysics activephysics2;

} Electrocutor;

void DrawPolygon(Electrocutor* zap, int Xoffset, int Yoffset, int TextureOffset);
void ElectrocutorCollision(ActivePhysics*, ActivePhysics*);