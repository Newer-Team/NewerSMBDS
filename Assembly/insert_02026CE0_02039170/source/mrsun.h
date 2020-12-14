#include "Enemy.h"

typedef struct
{
	// EnemyActor class stuff
	u32* vtable;		// 0
	
	u32 unk0;			// 4
	
	u32 settings;		// 8
	
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
	
	u32 unk8[0x53];		// 2A8
	
	// 394: vector copied there from ActivePhysics when a collision occurs
	
	// All of our custom Sprite 100 stuff
	TexInfo bodyTex;	// 3F4
	TexInfo shineTex;	// 3FC
	u32 hackyState;		// 404 // fix those offsets
	u32 timer;			// 408
	u32 shineTimer;		// 40C
	u32 baseline;		// 410
	float swoopA;		// 414
	float swoopB;		// 418
	float swoopC;		// 41C
	int swoopSpeed;		// 420
	u32 swoopX;			// 424
	u32 swoopY;			// 428
	u32 spinX;			// 42C
	u32 spinY;			// 430
	u8 skin;			// 434
	u8 pad[3];
} MrSun;


// MrSun Funcs
void MrSun_begin_Wait(MrSun* sun);
void MrSun_update_Wait(MrSun* sun);
void MrSun_begin_Respawn(MrSun* sun);
void MrSun_update_Respawn(MrSun* sun);
void MrSun_begin_Follow(MrSun* sun);
void MrSun_update_Follow(MrSun* sun);
void MrSun_begin_AttackSwirl(MrSun* sun);
void MrSun_update_AttackSwirl(MrSun* sun);
void MrSun_begin_Swoop(MrSun* sun);
void MrSun_update_Swoop(MrSun* sun);
void MrSun_begin_ShootFireballs(MrSun* sun);
void MrSun_update_ShootFireballs(MrSun* sun);
void MrSun_kill(MrSun* sun);
void MrSun_kill_respawn(MrSun* sun);
void MrSun_Do_Die_Particles(MrSun* sun);
