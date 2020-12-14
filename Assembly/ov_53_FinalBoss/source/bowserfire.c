#include <math.h>

#include "registers.h"
#include "bowserfire.h"
#include "game.h"

void BowserFire_loadFiles()
{
	loadFileByExtId_3dModel(2706 - 0x83, 0);		// BowserFire NSBTX
}

u32 BowserFire_physics[] = 
{
	0x8000,		// X offset
	0x8000,		// Y offset
	0x5000,		// Width (from center to edge)
	0x5000,		// Height (from center to edge)
	0x130A04,	// behavior
	0x0000,		// flags
	0x02098C78 	// player/sprite collision callback
};

void BowserFire_setup(BowserFire* bf)
{
	ExtId fileId = 2706 - 0x83;

	void* file = getPtrToLoadedFile(fileId);
	void* texture = NNS_G3dGetTex(file);
		
	getTextureParams(texture, 0, &bf->tex.texparam);
	getTexturePalBase(texture, 0, &bf->tex.palbase);
	
	initActivePhysics(&bf->activephysics, bf, BowserFire_physics, 0);
	registerCollider(&bf->activephysics);

	bf->timer = RNG(0x1000);
	bf->timer2 = 0;
}

void BowserFire_update_state0(BowserFire* bf)
{
	u32 xPos = bf->position.x >> 12; 
	u32 yPos = (-bf->position.y) >> 12;
	int tilebehaviour = getTileBehaviorAtPos2(xPos + 8, yPos);

	if ((!(tilebehaviour >> 0x10 & 0x2))  && (tilebehaviour))
		bf->timer2++;
	else
		bf->position.y -= 0x1800;
	
	bf->timer++;

	Vec3 particlePos = bf->position;
	particlePos.x += 0x8000;
	particlePos.y += 0x8000;

	// Smoke trail while falling
	if ((bf->timer2 == 0) && (bf->timer % 4 == 0))
		SpawnAndEndParticle(16, 130, &particlePos, 0, 0, 0, 0);

	// Sparks
	if (bf->timer % 3 == 0)
		SpawnParticle(121, &particlePos);

	// Disappear with poof
	if (bf->timer2 > 180)
	{
		SpawnParticle(0xAC, &particlePos);
		SpawnParticle(0xAD, &particlePos);
		Base__deleteIt(bf);
	}
}

bool BowserFire_beforeDraw(BowserFire* bf)
{
	return 1;
}

static inline void vec3(int x, int y, int z)
{
	REG_VERTEX16 = (y << 16) | (x & 0xFFFF);
	REG_VERTEX16 = (z & 0xFFFF);
}

static inline void texcoord2(int x, int y)
{
	REG_TEXT_COORD = (y << 16) | (x & 0xFFFF);
}

void G3_LoadMtx43(u32 unk);

void BowserFire_draw(BowserFire* bf)
{
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

	REG_MTX_TRANSLATE = bf->position.x + (cosLerp(bf->timer * 0x1400) * 5) / 4;
	REG_MTX_TRANSLATE = bf->position.y;
	REG_MTX_TRANSLATE = bf->position.z;
		
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
		
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;
		
	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = bf->tex.texparam;
	(*(vu32*) 0x40004AC) = bf->tex.palbase;
		
	REG_COLOR = 0x7FFF;
		
	REG_GFX_BEGIN = 1;

	texcoord2(0x200, 0);
	vec3(0x1000, 0x1000, 0x0);
		
	texcoord2(0, 0);
	vec3(0,	0x1000, 0x0);
	
	texcoord2(0, 0x200);
	vec3(0, 0, 0x0);
		
	texcoord2(0x200, 0x200);
	vec3(0x1000, 0, 0x0);
		
	REG_GFX_END = 0;
}
