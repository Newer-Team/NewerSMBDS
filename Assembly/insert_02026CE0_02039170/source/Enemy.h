#ifndef ENEMY_H
#define ENEMY_H

typedef struct
{
	u32* vtable;
	u32 stuff[15];
} Base;

typedef struct
{
	u32* vtable;
	s32 x, y;
} Vec2;

typedef struct
{
	u32* vtable;
	s32 x, y, z;
} Vec3;

typedef struct
{
	u32* vtable;
	s16 x, y, z;
	u16 padding;
} S16Vec3;

typedef struct
{
	s32 x, y, width, height;
} Rect2D;

typedef struct
{
	// size: 0xB0
	u32* vtable;
	void* owner;		// 4
	u32 unk0;			// 8
	u32 unk1;			// C
	u32* unk2;			// 10
	u32 offsetx;		// 14
	u32 offsety;		// 18
	u32 sizex;			// 1C
	u32 sizey;			// 20
	u32 behaviour;		// 24
	u32 flags;			// 28
	u32* callback;      // 2C
	Vec2 unkv1;			// 30 
	s32 XContactPos;	// 3C
	u32 unk3[9];		// 40
	s32 YContactPos;	// 64
	u32 unk4[9];		// 68
	u32 unk5[5];		// 8C
	u16 unk6;			// A0
	u16 collisionFlags;	// A2
	u16 unk7;			// A4
	u16 someFlags;		// A6
	u16 DeadMaybe;		// A8
	u8 Shape;			// AA  0 = Square, 1 = Circle, 2 = Trapezoid, 3 = Trapezoid 2
	u8 unk9;			// AB
	u32 unk10;			// AC
	
} ActivePhysics;

typedef struct
{
	// size: unknown
	void* owner;			// 0
	u8 unk1[0x24];		  	// 4
	u32 left;				// 28
	u32 bottom;				// 2C
	u32 unk2;				// 30
	u32 right;        		// 34
	u32 top;	        	// 38
	u8 unk3[0x2E];			// 3C
	u16 type;           	// 6A
	u32 justincase[0x10]; 	// 6C
} SolidCollision;

typedef struct
{
	u32 flags; 			  // 0. 0 = point, 1 = line
	u32 XPos;			  // 4
	u32 YPos;			  // 8
	u32 Unused;		  	  // C
	
}	ColliderPointHorizontal;

typedef struct
{
	u32 flags; 			  // 0. 0 = point, 1 = line
	u32 DistFromCenter;	  // 4
	u32 YPos;			  // 8. Positive goes up
	u32 Unused;	  // C
	
}	ColliderPointVertical;

typedef struct
{
	u32 flags; 			  // 0. 0 = point, 1 = line
	u32 XPos;			  // 4
	u32 XPos2;			  // 8
	u32 YPos;		  	  // C
	
}	ColliderLineHorizontal;

typedef struct
{
	u32 flags; 			  // 0. 0 = point, 1 = line
	u32 YPos1;	  		  // 4. Positive goes up
	u32 YPos2;			  // 8. Positive goes up
	u32 DistFromCenter;	  // C
	
}	ColliderLineVertical;

typedef struct
{
	u32* vtable;
	void* owner;				// 4
	void*  bottomstruct;		// 8
	void*  topstruct;    		// C
	void*  sidestruct;			// 10
	
	u32 unk0[0x10];				// 14
	
	Vec3 unkv1;					// 54
	Vec2 unkv2;					// 64
	Vec2 unkv3;					// 70
	
	u32 unk1[0x17];				// 7C
	
} Collider;

typedef struct {
	Vec3 position;
	S16Vec3 rotation;
} PositionInfo;

typedef struct
{
	u32 texparam;
	u32 palbase;
} TexInfo;

typedef struct
{
	// EnemyActor class stuff
	u32* vtable;
	
	u32 unk0;			// 4
	
	u32 spriteData;		// 8
	
	u16 ClassID;		// C
	u8 unk1[6];			// D
	
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
	u32  unk16[0x14];					// 39C
	u8   unk17;							// 3EC
	u8   unk18;							// 3ED
	u8   unk19;							// 3EE
	u8   unk20;							// 3EF
	u8   unk21;							// 3F0
	u32  SpriteSpecificData1;			// 3F4	
	u8   SpriteSpecificData2;			// 3F8
	u8   unk22[100];					// 3FC

} EnemyClassObject;

#endif // ENEMY_H