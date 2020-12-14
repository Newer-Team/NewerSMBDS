#include <nds.h>
#include <math.h>

#include "registers.h"
#include "mrsun.h"
#include "game.h"
#include "sfx.h"
#include "VariousFunctions.h"

void MrSun_loadFiles()
	{
		loadFileByExtId_3dModel(2646 - 0x83, 0);		// Sun/Moon NSBTX
		loadFileByExtId_3dModel(2644 - 0x83, 0);		// Fireball NSBTX
		loadFileByExtId_3dModel(2645 - 0x83, 0);		// Iceball NSBTX
	}

u32 MrSun_physics[] = 
	{
		0x20000,	// X offset
		0x20000,	// Y offset
		0x12000,	// Width
		0x12000,	// Height
		0x00930A04, // Behavior
		0x0400,		// Flags
		0x02098C78  // Collision callback
	};

void MrSun_setup(MrSun* sun)
{	
	sun->skin = sun->settings >> 16 & 0x1;
	u32 bodyTexID;
	if (sun->skin) bodyTexID = 2; else bodyTexID = 0;
	
	u8* texture = (u8*) NNS_G3dGetTex(getPtrToLoadedFile(2646 - 0x83));
	getTextureParams(texture, bodyTexID, &sun->bodyTex.texparam);
	getTexturePalBase(texture, bodyTexID, &sun->bodyTex.palbase);
	
	if (!sun->skin)
		{
			getTextureParams(texture, 1, &sun->shineTex.texparam);
			getTexturePalBase(texture, 1, &sun->shineTex.palbase);
		}
	
	initActivePhysics(&sun->activephysics, sun, MrSun_physics, 0);
	sun->activephysics.Shape = 1;
	registerCollider(&sun->activephysics);
	
	sun->position.x -= 0x18000;
	sun->position.y -= 0x28000;
	sun->position.z  = 0xF0000;
	
	sun->velocity.x = 0;
	sun->timer = 0;
	sun->shineTimer = 0;
	sun->baseline = sun->position.y;

	if ((sun->settings >> 8 & 0xFF) == 0)
		MrSun_begin_Follow(sun);
	else
		MrSun_begin_Wait(sun);
}

void MrSun_begin_Wait(MrSun* sun)
	{
		sun->hackyState = 0;
		sun->timer = 0;
		sun->velocity.x = 0;
		sun->position.y = sun->baseline;
	}

void MrSun_update_Wait(MrSun* sun)
	{	
		if (eventIsActivated(sun->settings >> 8 & 0xFF))
			{
				MrSun_begin_Follow(sun);
				return;
			}		
	}

void MrSun_begin_Respawn(MrSun* sun)
	{	
		UnregisterActivePhysics(&sun->activephysics);
		registerCollider(&sun->activephysics);
		
		sun->hackyState = 1;
		
		// Lazy.
		sun->position.y = 0;
		sun->position.x = 0;
		
		sun->timer = 0;
	}

void MrSun_update_Respawn(MrSun* sun)
	{
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		u32 mX = mA->xPos;
		
		if (sun->timer > 550)
			{
				sun->position.x = mX + 0x100000;
				sun->position.y = sun->baseline;
				MrSun_begin_Follow(sun);		
				return;
			}
		
		sun->timer++;
	}

void MrSun_begin_Follow(MrSun* sun)
	{
		sun->hackyState = 2;
		sun->timer = 0;
		sun->velocity.x = 0;
		sun->position.y = sun->baseline;
	}

void MrSun_update_Follow(MrSun* sun)
{	
	MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
	u32 mX = mA->xPos;
	
	if (sun->timer > 225)
		{
			u32 sX  = sun->position.x + 0x20000;
			
			// Do not Swoop from too far offscreen
			if (sX > mX + 0x100000 || sX < mX - 0x100000)
				{
					sun->timer -= 55;
				}
				else
				{
					if (RNG(5) < 3) MrSun_begin_AttackSwirl(sun);
					else MrSun_begin_ShootFireballs(sun);
					return;
				}
		}
	
	bool direction = (((sun->position.x) + 0x20000) < mX);
	
	s32 speedDelta;
	if (sun->skin == 0) speedDelta = 215; else speedDelta = 300;
		
	if (direction)
		{
			sun->velocity.x = sun->velocity.x + speedDelta;

			if (sun->velocity.x < 0) sun->velocity.x = sun->velocity.x + (speedDelta / 2);
			if (sun->velocity.x < (speedDelta * 60)) sun->velocity.x = sun->velocity.x + speedDelta;		
		}
	else
		{
			sun->velocity.x = sun->velocity.x - speedDelta;

			if (sun->velocity.x > 0) sun->velocity.x = sun->velocity.x - (speedDelta / 2);
			if (sun->velocity.x > (speedDelta * 60)) sun->velocity.x = sun->velocity.x - speedDelta;	
		}
	
	sun->position.x += sun->velocity.x;
	sun->timer++;
}

void MrSun_begin_AttackSwirl(MrSun* sun)
	{
		sun->hackyState = 3;
		sun->timer = 0;
		sun->spinX = sun->position.x - (32<<12);
		sun->spinY = sun->position.y - (32<<12);
	}

void MrSun_update_AttackSwirl(MrSun* sun)
	{	
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		u32 marioY = mA->yPos;

		if (sun->timer > 100)
			{
				if (marioY > sun->position.y - 0x14000)
				{
					MrSun_begin_Follow(sun);
				}
				else
				{
					sun->position.y = sun->baseline;
					MrSun_begin_Swoop(sun);
				}
				return;
			}

		float x = (float)sun->timer/7;
		
		float dist = -32.768*(float)sun->timer*(float)sun->timer + 3276.8*(float)sun->timer;

		u32 newX = (u32)((float)sun->spinX + dist*cos(x)) + (32<<12);
		u32 newY = (u32)((float)sun->spinY + dist*sin(x)) + (32<<12);
		
		sun->position.x = newX;
		sun->position.y = newY;
		
		sun->timer++;
	}

void MrSun_begin_Swoop(MrSun* sun)
	{
		sun->hackyState = 4;
		
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		
		u32 mX = mA->xPos;
		u32 mY = mA->yPos;
		
		u32 sX = sun->position.x;
		u32 sY = sun->position.y;
		
		sun->timer = 0;
		
		sun->swoopX = mX-(32<<12);
		sun->swoopY = mY-(32<<12);
		
		float x1, x2, x3, y1, y2, y3;
		
		x1 = (float)sX - sun->swoopX;
		x2 = 0;
		x3 = -x1;
		
		y1 = (float)sY - sun->swoopY;
		y2 = 0;
		y3 = y1;
		
		float denominator = (x1 - x2) * (x1 - x3) * (x2 - x3);
		sun->swoopA       = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denominator;
		sun->swoopB       = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denominator;
		sun->swoopC       = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denominator;
		
		sun->swoopSpeed = (int)x3*2/75;
		
		if (sun->skin == 1) sun->swoopSpeed *= 1.25;
	}

void MrSun_update_Swoop(MrSun* sun)
	{
		sun->position.x += sun->swoopSpeed;
		
		u32 sX = sun->position.x;
		
		float newY = (sun->swoopA*((float)sX - sun->swoopX)*((float)sX - sun->swoopX) + sun->swoopB*((float)sX - sun->swoopX) + sun->swoopC) + sun->swoopY;

		sun->position.y = (u32)newY;
		
		if (sun->position.y > sun->baseline)
			{
				sun->position.y = sun->baseline;
				MrSun_begin_Follow(sun);
				return;
			}
	}
	
void MrSun_begin_ShootFireballs(MrSun* sun)
	{
		sun->hackyState = 5;
		sun->timer = 0;
	}

void MrSun_update_ShootFireballs(MrSun* sun)
	{
		if (sun->timer == 60)
			{
			
			MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
			
			if (sun->position.x+ 0x20000 > mA->xPos + 0x100000 || sun->position.x+ 0x20000 < mA->xPos - 0x100000)
				{
					MrSun_begin_Swoop(sun);
					return;
				}
			else
				{
					Vec3 pos;
					pos.x = sun->position.x + 0x20000;
					pos.y = sun->position.y + 0x30000;
					pos.z = 0;
					
					PlaySNDEffect(SE_EMY_BUBBLE, &pos);
					
					u32 ballskin = sun->skin << 20; 
					
					createActor(251, 0xD230 + ballskin, &pos, 0, 0, 0);
					createActor(251, 0xB430 + ballskin, &pos, 0, 0, 0);
					createActor(251, 0x9630 + ballskin, &pos, 0, 0, 0);
					
					if (sun->skin) 
						{
							createActor(251, 0xE630 + ballskin, &pos, 0, 0, 0);
							createActor(251, 0x8230 + ballskin, &pos, 0, 0, 0);					
						}
				}
			}
		
		if (sun->timer >= 120)
			{
				MrSun_begin_Follow(sun);
				return;
			}
		
		sun->timer++;
	}

void MrSun_update_state0(MrSun* sun)
{
	if (eventIsActivated(sun->settings & 0xFF))
		{
			sun->velocity.x = 0;
			sun->velocity.y = 0;
			MrSun_kill(sun);
			return;
		}
	
	if (sun->skin == 0)
		{
			if (sun->hackyState < 3)
				sun->shineTimer++;
			if (sun->shineTimer > 157)
				sun->shineTimer = 0;
		}
	
	switch (sun->hackyState)
		{
			case 0: MrSun_update_Wait(sun); break;
			case 1: MrSun_update_Respawn(sun); break;
			case 2: MrSun_update_Follow(sun); break;
			case 3: MrSun_update_AttackSwirl(sun); break;
			case 4: MrSun_update_Swoop(sun); break;
			case 5: MrSun_update_ShootFireballs(sun); break;
			default: break;
		}
}

void MrSun_kill(MrSun* sun)
	{
		MrSun_Do_Die_Particles(sun);
		PlaySNDEffect(SE_EMY_KAME_KERU, &sun->position);		// Die Sound Effect (Used by KilledByFlagpole)
		Base__deleteIt(sun);
	}

void MrSun_kill_respawn(MrSun* sun)
	{
		MrSun_Do_Die_Particles(sun);
		GivePointsOfSomeKind(sun, 4, 0x20000, 0x20000, 0);
		PlaySNDEffect(SE_EMY_KAME_KERU, &sun->position);		// Die Sound Effect (Used by KilledByFlagpole)
		MrSun_begin_Respawn(sun);
	}

void MrSun_flagpole_kill(MrSun* sun)
	{
		MrSun_Do_Die_Particles(sun);
		Base__deleteIt(sun);
	}

void MrSun_shell_kill(MrSun* sun)
	{	
		MrSun_Do_Die_Particles(sun);
		
		
		Vec3 coinspawn;
		coinspawn.x = sun->position.x+0x8000;
		coinspawn.y = sun->position.y;
		coinspawn.z = 0;	
		
		// Lazy 
		
		for (int i = 0; i < 3; i++)
			{
				createActor(66, 5, &coinspawn, 0, 0, 0);
				coinspawn.x += 0x10000;
				if (i == 0) coinspawn.y += 0x10000;
				if (i == 1) coinspawn.y -= 0x10000;
			}

		MrSun_begin_Respawn(sun);
	}
	
void MrSun_Do_Die_Particles(MrSun* sun)
	{
		Vec3 ParticlePos;
		ParticlePos.x = sun->position.x + 0x20000;
		ParticlePos.y = sun->position.y + 0x1D000;
		ParticlePos.z = 0;
		
		PlaySNDEffect(SE_PLY_STAR_ATTACK, &sun->position);
		
		SpawnParticle(122, &ParticlePos);	// Poof particle
		SpawnParticle(73, &ParticlePos);	// Poof particle
		SpawnParticle(122, &ParticlePos);	// Poof particle
		SpawnParticle(65, &ParticlePos);	// Star explosion particle
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

void rotateArroundPointU32Sun(int x, int y, float angle)
	{
		float tempX = x - 0x2000;
		float tempY = y - 0x2000;
		
		float rotatedX = tempX*cos(angle) - tempY*sin(angle);
		float rotatedY = tempX*sin(angle) + tempY*cos(angle);
		
		int newX = (int)rotatedX + 0x2000;
		int newY = (int)rotatedY + 0x2000;
		
		vec3(newX, newY, 0);
	}

void MrSun_draw(MrSun* sun)
	{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		REG_MTX_TRANSLATE = sun->position.x;
		REG_MTX_TRANSLATE = sun->position.y;
		REG_MTX_TRANSLATE = sun->position.z+0x2000;
		
		REG_MTX_SCALE = 0x10000;
		REG_MTX_SCALE = 0x10000;
		REG_MTX_SCALE = 0x10000;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;
		
		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = sun->bodyTex.texparam;
		(*(vu32*) 0x40004AC) = sun->bodyTex.palbase;
		
		REG_COLOR = 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		
		texcoord2(0x400, 0);
		vec3(0x4000, 	0x4000, 	0x0);
		
		texcoord2(0, 0);
		vec3(0x0000,	0x4000, 	0x0);
		
		texcoord2(0, 0x400);
		vec3(0x0000, 	0x0000,		0x0);
		
		texcoord2(0x400, 0x400);
		vec3(0x4000,	0x0000,		0x0);
		
		REG_GFX_END = 0;
		
		if (!sun->skin)
		{
			NNS_G3dGeFlushBuffer();
			G3_LoadMtx43(0x02085B20);
			
			REG_MTX_TRANSLATE = sun->position.x;
			REG_MTX_TRANSLATE = sun->position.y;
			REG_MTX_TRANSLATE = sun->position.z;
			
			REG_MTX_SCALE = 0x10000;
			REG_MTX_SCALE = 0x10000;
			REG_MTX_SCALE = 0x10000;
			
			REG_MTX_CONTROL = 3;
			REG_MTX_IDENTITY = 0;
			REG_MTX_CONTROL = 2;
			
			REG_POLY_FORMAT = 0x001F3880;
			REG_TEXT_FORMAT = sun->shineTex.texparam;
			(*(vu32*) 0x40004AC) = sun->shineTex.palbase;
			
			REG_COLOR = 0x7FFF;
			
			REG_GFX_BEGIN = 1;
			
			float angle = sin((float)sun->shineTimer/25)/4;
			
			texcoord2(0x400, 0);
			rotateArroundPointU32Sun(0x4000, 0x4000, angle);
			
			texcoord2(0, 0);
			rotateArroundPointU32Sun(0x0000, 0x4000, angle);
			
			texcoord2(0, 0x400);
			rotateArroundPointU32Sun(0x0000, 0x0000, angle);
			
			texcoord2(0x400, 0x400);
			rotateArroundPointU32Sun(0x4000, 0x0000, angle);
			
			REG_GFX_END = 0;
		}
	}
