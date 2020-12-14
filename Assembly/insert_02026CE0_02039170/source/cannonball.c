#include <nds.h>
#include <math.h>
#include "registers.h"
#include "cannonball.h"
#include "game.h"

u32 Cannonball_physics_small[] = 
{
	0x00000000,		// X offset
	0x00000000,		// Y offset
	0x00007000,		// Width
	0x00007000,		// Height
	0x00930008,		// behavior
	0x0000FFFC,		// flags
	0x02098C78 		// player/sprite collision callback
};

u32 Cannonball_physics_big[] = 
{
	0x00000000,		// X offset
	0x00000000,		// Y offset
	0x0000F000,		// Width
	0x0000F000,		// Height
	0x00930008,		// behavior
	0x0000FFFC,		// flags
	0x02098C78 		// player/sprite collision callback
};

// behavior:
// bit2 = react to non-player collision (enemies, block hit from below)
// bit16 = react to player collision
// bit17 = ???
// bit20 = die when hit from below
// bit21 = 
// bit22 = ???
// bit23 = react to fireballs

// flags:
// bit1 = die when receiving fireball
// bit10 = die when receiving koopa shell

int Cannonball_loadFiles()
{
	loadFileByExtId_3dModel(2631-0x83, 0);
	return 1;
}

void Cannonball_setup(Cannonball* cb)
{
	cb->position.x += 0x8000;
	cb->position.y += 0x8000;
	cb->position.z = 0x1000;
	
	cb->isBig = (cb->spriteData >> 16) & 0x1;

	u8* texture = (u8*) NNS_G3dGetTex(getPtrToLoadedFile(2631-0x83));
	
	getTextureParams(texture, 0, &cb->tex.texparam);
	getTexturePalBase(texture, 0, &cb->tex.palbase);
	
	
	int rot = ((cb->spriteData >> 8) & 0xFF) * 15 % 360;	
	float rotation = -rot + 90;	
	rotation = rotation*3.14/(float)180;
	
	float speed = (cb->spriteData & 0xFF) * 0x200;
	
	cb->velocity.x = (int)(speed*cos(rotation));
	cb->velocity.y = (int)(speed*sin(rotation));
	
	if (!cb->isBig)
		initActivePhysics(&cb->activephysics, cb, Cannonball_physics_small, 0);
	else
		initActivePhysics(&cb->activephysics, cb, Cannonball_physics_big, 0);
	registerCollider(&cb->activephysics);
	
}

void Cannonball_update_state0(Cannonball* cb)
{	
	cb->position.x += cb->velocity.x;
	cb->position.y += cb->velocity.y;
	
	MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
	
	int xoffs = cb->position.x - mA->xPos;
	int yoffs = cb->position.y - mA->yPos;
	
	// This needs tweaking maybe?
	if (xoffs > 0x100000 || xoffs < -0x100000 || yoffs > 0x100000 || yoffs < -0x100000)
		Base__deleteIt(cb);
}

bool Cannonball_beforeDraw(Cannonball* cb)
{
	// Do some calcs here if the CB is offscreen
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

void Cannonball_Virt61(Cannonball* cb)
{
	cb->position.z = 0x8000;
	asm("B 0x0215E0EC");
}



void G3_LoadMtx43(u32 unk);

void Cannonball_draw(Cannonball* cb)
{
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);
	
	s32 xOffs;
	s32 yOffs;
	
	if (!cb->isBig)
	{
		xOffs = 0x8000;
		yOffs = 0x8000;
	}
	else
	{
		xOffs = 0x10000;
		yOffs = 0x10000;
	}
	
	REG_MTX_TRANSLATE = cb->position.x - xOffs;
	REG_MTX_TRANSLATE = cb->position.y - yOffs;
	REG_MTX_TRANSLATE = cb->position.z;
	
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;
	
	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = cb->tex.texparam;
	REG_PAL_BASE = cb->tex.palbase;
	
	REG_COLOR = 0x7FFF;
	
	REG_GFX_BEGIN = 1;
	
	if (!cb->isBig)
	{
		texcoord2(0x200, 0);
		vec3(0x1000, 0x1000, 0x0);
		
		texcoord2(0, 0);
		vec3(0x0000, 0x1000, 0x0);
		
		texcoord2(0, 0x200);
		vec3(0x0000, 0x0000, 0x0);
		
		texcoord2(0x200, 0x200);
		vec3(0x1000, 0x0000, 0x0);
	}
	else
	{
		texcoord2(0x200, 0);
		vec3(0x2000, 0x2000, 0x0);
		
		texcoord2(0, 0);
		vec3(0x0000, 0x2000, 0x0);
		
		texcoord2(0, 0x200);
		vec3(0x0000, 0x0000, 0x0);
		
		texcoord2(0x200, 0x200);
		vec3(0x2000, 0x0000, 0x0);
	}
	
	REG_GFX_END = 0;
}
