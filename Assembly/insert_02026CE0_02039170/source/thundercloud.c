#include <nds.h>
#include <math.h>
#include "registers.h"
#include "thundercloud.h"
#include "game.h"
#include "sfx.h"
#include "VariousFunctions.h"

u32 ThunderCloud_physics[] = 
{
	0x10000,	
	0x10000,	
	0x0C000,	
	0x0C000,	
	0x00D30004, 
	0x0400,
	0x02098C78  
};

bool Thundercloud_loadFiles()
	{
		loadFileByExtId_3dModel(2620 - 0x83, 0);
		return 1;
	}

void ThunderCloud_setup(ThunderCloud* tc)
{
	tc->mode = tc->spriteData & 0x1;
	
	if (getUnk2ForView(tc->ViewID) < 4) tc->brightness = 0;
	else tc->brightness = -(getUnk1ForView(tc->ViewID));
		

	u8* texture = (u8*) NNS_G3dGetTex(getPtrToLoadedFile(2620-0x83));
	
	getTextureParams(texture, 0, &tc->cloudTex.texparam);
	getTexturePalBase(texture, 0, &tc->cloudTex.palbase);

	getTextureParams(texture, 1, &tc->lightningTex.texparam);
	getTexturePalBase(texture, 1, &tc->lightningTex.palbase);
	
	initActivePhysics(&tc->activephysics, tc, ThunderCloud_physics, 0);
	registerCollider(&tc->activephysics);
	
	
	if (!(tc->spriteData >> 4 & 0x1)) tc->position.x -= 0x8000;
	if (tc->spriteData >> 4 & 0x2) tc->position.y -= 0x8000;
	tc->position.y -= 0x18000;
	tc->position.z = 0x200000;
	
	tc->cloudScaleX = 0x10000;
	tc->cloudScaleY = 0x10000;
	
	tc->lightningY = 0;
	
	tc->velocity.x = 0;
	tc->baseline = tc->position.y;
	tc->timer = 0;
	
	if (tc->mode == 0) ThunderCloud_begin_Follow(tc);
	else
	{
		tc->hackyState = 0;
		tc->timer = ((tc->spriteData & 0xFF0000) >> 16) * 60 - ((tc->spriteData & 0xFF00) >> 8) * 60;
	}
}

// Wait State

void ThunderCloud_begin_Wait(ThunderCloud* tc)
{
	tc->hackyState = 0;
	tc->timer = 0;
	tc->velocity.x = 0;
}

void ThunderCloud_update_Wait(ThunderCloud* tc)
{	
	u32 tx = tc->timer%60;
	u32 ty = tc->timer%40;
	
	tc->cloudScaleX = (u32)(sin(tx/9.55-1.57) * 3000 + 3000) + 0x10000;
	tc->cloudScaleY = (u32)(sin(ty/6.37-1.57) * 2400 + 2400) + 0x10000;
	tc->lightningY = (s32)(sin(ty/6.37) * 0x3000);
	
	tc->timer++;
	MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
	u32 mX = mA->xPos;
	u32 tX = tc->position.x + 0x20000;
	if (tc->timer > 60 * ((tc->spriteData & 0xFF0000) >> 16))
	{
		tc->timer = 0;
		if (tX > mX + 0x80000 || tX < mX - 0x80000) ThunderCloud_begin_Wait(tc);
		else
			{
				ThunderCloud_begin_Lightning(tc);
				tc->cloudScaleX = 0x10000;
				tc->cloudScaleY = 0x10000;
				tc->lightningY = 0;
			}
	}
}


// Respawn State

void ThunderCloud_begin_Respawn(ThunderCloud* tc)
{
	UnregisterActivePhysics(&tc->activephysics);
	registerCollider(&tc->activephysics);
	tc->hackyState = 1;
	tc->position.y = 0;
	tc->position.x = 0;
	tc->timer = 0;
}

void ThunderCloud_update_Respawn(ThunderCloud* tc)
{	
	if (tc->timer > 550)
	{
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		u32 mX = mA->xPos;
		
		tc->timer = 0;
		
		tc->position.x = mX + 0x100000;
		tc->position.y = tc->baseline;
		ThunderCloud_begin_Follow(tc);		
		return;
	}
	
	tc->timer++;
}


// Follow State

void ThunderCloud_begin_Follow(ThunderCloud* tc)
{
	tc->hackyState = 2;
	tc->timer = 0;
	tc->velocity.x = 0;
}

void ThunderCloud_update_Follow(ThunderCloud* tc)
{	
	MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
	u32 mX = mA->xPos;
	
	if (tc->timer > 240)
	{
		u32 tX = tc->position.x + 0x20000;
		
		// Do not do a Lighning from too far
		if (tX > mX + 0x50000 || tX < mX - 0x50000)
		{
			tc->timer -= 120;
		}
		else
		{
			ThunderCloud_begin_Lightning(tc);
			return;
		}
	}
	
	bool direction = (((tc->position.x) + 0x10000) < mX);
	
	s32 speedDelta = 225;
		
	if (direction)
	{
		tc->velocity.x = tc->velocity.x + speedDelta;

		if (tc->velocity.x < 0) tc->velocity.x = tc->velocity.x + (speedDelta / 2);
		if (tc->velocity.x < (speedDelta * 60)) tc->velocity.x = tc->velocity.x + speedDelta;		
	}
	else
	{
		tc->velocity.x = tc->velocity.x - speedDelta;

		if (tc->velocity.x > 0) tc->velocity.x = tc->velocity.x - (speedDelta / 2);
		if (tc->velocity.x > (speedDelta * 60)) tc->velocity.x = tc->velocity.x - speedDelta;	
	}
	
	tc->position.x += tc->velocity.x;
	
	tc->timer+=((tc->spriteData & 0xFF0000) >> 16);
	
	u32 tx = tc->timer%60;
	u32 ty = tc->timer%40;
	u32 ly = tc->timer%80;
	
	tc->cloudScaleX = (u32)(sin(tx/9.55-1.57) * 3000 + 3000) + 0x10000;
	tc->cloudScaleY = (u32)(sin(ty/6.37-1.57) * 2400 + 2400) + 0x10000;
	tc->lightningY = (s32)(sin(ly/12.74) * 0x3000);
}


// Lightning State

void ThunderCloud_begin_Lightning(ThunderCloud* tc)
{
	tc->cloudScaleX = 0x10000;
	tc->cloudScaleY = 0x10000;
	tc->lightningY = 0;
	tc->timer = 0;
	tc->hackyState = 3;
}

void ThunderCloud_update_Lightning(ThunderCloud* tc)
{	
	tc->timer++;

	s32 calcTimer = tc->timer;
	if (tc->timer > 60) calcTimer -= 30;
	else if (tc->timer > 30) calcTimer = 30;
	
	if (tc->timer == 30) ThunderCloud_summonLightning(tc);
	
	if (tc->mode == 0)
		{
			if ((tc->timer > 30) && (tc->timer < 40))
				{
					setBrightness(1,7);
				}
			if ((tc->timer == 41))
				{
					setBrightness(1,0);
				}
		}
	
	s32 sq = (s32)((float)(calcTimer-30)*3.7);
	
	tc->cloudScaleX = - (sq*sq) + 0x13000;
	tc->cloudScaleY = tc->cloudScaleX;
	
	if (tc->timer >= 90)
	{
		tc->cloudScaleX = 0x10000;
		tc->cloudScaleY = 0x10000;
		
		if (tc->mode == 0) ThunderCloud_begin_Follow(tc);
		else ThunderCloud_begin_Wait(tc);
	}
}


void ThunderCloud_update_state0(ThunderCloud* tc)
{	
	if (eventIsActivated(tc->spriteData >> 24))
	{
		tc->velocity.x = 0;
		tc->velocity.y = 0;
		ThunderCloud_kill(tc);
		return;
	}

	switch (tc->hackyState)
	{
		case 0: ThunderCloud_update_Wait(tc); break;
		case 1: ThunderCloud_update_Respawn(tc); break;
		case 2: ThunderCloud_update_Follow(tc); break;
		case 3: ThunderCloud_update_Lightning(tc); break;
		default: break;
	}
}

void ThunderCloud_kill(ThunderCloud* tc)
{
	SpawnDeathParticles(tc);
	PlaySNDEffect(SE_EMY_KAME_KERU, &tc->position);		// Die Sound Effect (Used by KilledByFlagpole)
	Base__deleteIt(tc);
}

void ThunderCloud_kill_respawn(ThunderCloud* tc)
{	
	if (tc->mode == 1) ThunderCloud_kill(tc);

	SpawnDeathParticles(tc);
	
	GivePointsOfSomeKind(tc, 4, 0x10000, 0x10000, 0);
	
	PlaySNDEffect(SE_EMY_KAME_KERU, &tc->position);		// Die Sound Effect (Used by KilledByFlagpole)
	
	if (tc->mode == 1)
		Base__deleteIt(tc);
	else
		ThunderCloud_begin_Respawn(tc);
}

void ThunderCloud_flagpole_kill(ThunderCloud* tc)
{
	SpawnDeathParticles(tc);
	EnemyActor__DieFromFlagpole(tc);
}

void ThunderCloud_shell_kill(ThunderCloud* tc)
{	
	if (tc->mode == 1) ThunderCloud_kill(tc);
	SpawnDeathParticles(tc);
	ThunderCloud_begin_Respawn(tc);
}

void ThunderCloud_summonLightning(ThunderCloud* tc)
{
	Vec3 pos = tc->position;
	pos.x += 0x10000;
	
	createActor(0x61, 0x00000001, &pos, 0, 0, 0);
	asm("STR     R0, [R0,#0x3FC]");		// really, why do this in C?
}


void SpawnDeathParticles(ThunderCloud* tc)
{
	Vec3 Pos;
	Pos.x = tc->position.x + 0x10000;
	Pos.y = tc->position.y + 0x10000;
	Pos.z = tc->position.z;
	
	SpawnParticle(135, &Pos);
	SpawnParticle(136, &Pos);
	SpawnParticle(19, &Pos);
	
	setBrightness(1,tc->brightness);
	
	return;
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


void ThunderCloud_draw(ThunderCloud* tc)
{
	// Render Cloud
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);
	
	REG_MTX_TRANSLATE = tc->position.x + 0x10000 - tc->cloudScaleX;
	REG_MTX_TRANSLATE = tc->position.y + 0x10000 - tc->cloudScaleY;
	REG_MTX_TRANSLATE = tc->position.z;
	
	REG_MTX_SCALE = tc->cloudScaleX;
	REG_MTX_SCALE = tc->cloudScaleY;
	REG_MTX_SCALE = 0x10000;
	
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;
	
	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = tc->cloudTex.texparam;
	REG_PAL_BASE = tc->cloudTex.palbase;
	
	REG_COLOR= 0x7FFF;
	
	REG_GFX_BEGIN = 1;
	
	texcoord2(0x200, 0);
	vec3(0x2000, 	0x2000, 	0x0);
	
	texcoord2(0, 0);
	vec3(0x0000,	0x2000, 	0x0);
	
	texcoord2(0, 0x200);
	vec3(0x0000, 	0x0000,		0x0);
	
	texcoord2(0x200, 0x200);
	vec3(0x2000,	0x0000,		0x0);
	
	REG_GFX_END = 0;
	
	
	// Render Lightning
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);
	
	REG_MTX_TRANSLATE = tc->position.x + 0x8000;
	REG_MTX_TRANSLATE = tc->position.y + 0xA000 + tc->lightningY;
	REG_MTX_TRANSLATE = tc->position.z;
	
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;
	
	REG_POLY_FORMAT= 0x001F3880;
	REG_TEXT_FORMAT = tc->lightningTex.texparam;
	REG_PAL_BASE = tc->lightningTex.palbase;
	
	REG_COLOR = 0x7FFF;
	
	REG_GFX_BEGIN = 1;
	
	texcoord2(0x100, 0);
	vec3(0x1000, 	0x2000, 	0x0);
	
	texcoord2(0, 0);
	vec3(0x0000,	0x2000, 	0x0);
	
	texcoord2(0, 0x200);
	vec3(0x0000, 	0x0000,		0x0);
	
	texcoord2(0x100, 0x200);
	vec3(0x1000,	0x0000,		0x0);
	
	REG_GFX_END = 0;
}
