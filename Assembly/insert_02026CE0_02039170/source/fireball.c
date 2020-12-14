#include <nds.h>
#include <math.h>

#include "registers.h"
#include "fireball.h"
#include "game.h"
#include "sfx.h"

void Fireball_loadFiles()
	{
		loadFileByExtId_3dModel(2644 - 0x83, 0);		// Fireball NSBTX
		loadFileByExtId_3dModel(2645 - 0x83, 0);		// Iceball NSBTX
	}

u32 Fireball_physics[] = 
	{
		0x0000,		// X offset
		0x0000,		// Y offset
		0x4000,		// Width
		0x4000,		// Height
		0x130A04,	// behavior
		0x0000,		// flags
		0x02098C78 	// player/sprite collision callback
	};

void Fireball_setup(Fireball* fb)
	{
		fb->skin = (fb->spriteData >> 20) & 0x1;
		
		ExtId fileId = 2644 - 0x83 + fb->skin % 2;

		void* file = getPtrToLoadedFile(fileId);
		void* texture = NNS_G3dGetTex(file);
		
		getTextureParams(texture, 0, &fb->tex.texparam);
		getTexturePalBase(texture, 0, &fb->tex.palbase);

		int rot = ((fb->spriteData >> 8) & 0xFFF) % 360;	
		float rotation = -rot + 90;	
		rotation = rotation * 3.14159265358979 / 180;
		fb->rot = rotation;

		fb->bouncedLastFrame = false;
		
		float speed = (fb->spriteData & 0xFF) * 0x200;
		
		fb->velocity.x = (int)(speed * cos(rotation));
		fb->velocity.y = (int)(speed * sin(rotation));
		
		fb->bouncing = ((fb->spriteData >> 24) & 0xF);
		fb->scale.x = 1 + ((fb->spriteData >> 28) & 0xF);

		initActivePhysics(&fb->activephysics, fb, Fireball_physics, 0);
		registerCollider(&fb->activephysics);
		
		fb->activephysics.sizex *= fb->scale.x;
		fb->activephysics.sizey *= fb->scale.x;

		rotateAroundPoint( 0x800,  0x800, rotation, &fb->drawCoords[0], &fb->drawCoords[1]);
		rotateAroundPoint(-0x800,  0x800, rotation, &fb->drawCoords[2], &fb->drawCoords[3]);
		rotateAroundPoint(-0x800, -0x800, rotation, &fb->drawCoords[4], &fb->drawCoords[5]);
		rotateAroundPoint( 0x800, -0x800, rotation, &fb->drawCoords[6], &fb->drawCoords[7]);
	}

void Fireball_update_state0(Fireball* fb)
	{
		fb->timer++;
		if (fb->timer > 15)
			fb->timer = 0;
	
		fb->position.x += fb->velocity.x;
		fb->position.y += fb->velocity.y;

		u32 xPos = (fb->position.x >> 12) / 16; 
		u32 yPos = ((-fb->position.y) >> 12) / 16;
		
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		
		int xoffs = fb->position.x - mA->xPos;
		int yoffs = fb->position.y - mA->yPos;
		
		if (xoffs > 0x100000 || xoffs < -0x100000 || yoffs > 0x100000 || yoffs < -0x100000)
			Base__deleteIt(fb);

		int tilebehaviour = getTileBehaviorAtPos2(xPos*16, yPos*16);
		if (!fb->bouncing)		
			{
				if ((!(tilebehaviour >> 0x10 & 0x2)) && (tilebehaviour))
					{
						if (!fb->skin)
							{
								SpawnParticle(0xAC, &fb->position);
								SpawnParticle(0xAD, &fb->position);
							}
							else SpawnParticle(122, &fb->position);
							Base__deleteIt(fb);
					}
			}
		else
			{
				if (((!(tilebehaviour >> 16 & 0x2)) && (tilebehaviour)))
					{
						// Detect pass-through
						if (fb->velocity.y > 0)
						{
							if (tilebehaviour >> 24 & 0x80) return;
						}

						if (fb->bouncedLastFrame)
						{
							if (!fb->skin)
								{
									SpawnParticle(0xAC, &fb->position);
									SpawnParticle(0xAD, &fb->position);
								}
								else SpawnParticle(122, &fb->position);
								Base__deleteIt(fb);
						}
				
						fb->rot = -fb->rot;
						fb->velocity.y = -fb->velocity.y;
						PlaySNDEffect(SE_OBJ_FIREBALL_DISAPP, &fb->position);
						
						rotateAroundPoint( 0x800,  0x800, fb->rot, &fb->drawCoords[0], &fb->drawCoords[1]);
						rotateAroundPoint(-0x800,  0x800, fb->rot, &fb->drawCoords[2], &fb->drawCoords[3]);
						rotateAroundPoint(-0x800, -0x800, fb->rot, &fb->drawCoords[4], &fb->drawCoords[5]);
						rotateAroundPoint( 0x800, -0x800, fb->rot, &fb->drawCoords[6], &fb->drawCoords[7]);
						fb->bouncedLastFrame = true;
					}
				else
					{
						fb->bouncedLastFrame = false;
					}
			}

	}
	
void Fireball_flag(Fireball* fb)
	{
		if (!fb->skin)
			{
				SpawnParticle(0xAC, &fb->position);
				SpawnParticle(0xAD, &fb->position);
			}
		else SpawnParticle(122, &fb->position);
		Base__deleteIt(fb);
	}

void rotateAroundPoint(int x, int y, float angle, int* oX, int* oY)
	{	
		*oX = (int)((float)x*cos(angle) - (float)y*sin(angle));
		*oY = (int)((float)x*sin(angle) + (float)y*cos(angle));
	}

bool Fireball_beforeDraw(Fireball* fb)
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

void Fireball_draw(Fireball* fb)
	{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = fb->position.x;
		REG_MTX_TRANSLATE = fb->position.y;
		REG_MTX_TRANSLATE = fb->position.z;
		
		REG_MTX_SCALE = 0x10000 * fb->scale.x;
		REG_MTX_SCALE = 0x10000 * fb->scale.x;
		REG_MTX_SCALE = 0x10000 * fb->scale.x;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;
		
		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = fb->tex.texparam;
		(*(vu32*) 0x40004AC) = fb->tex.palbase;
		
		REG_COLOR = 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		
		int framePos = (fb->timer/4) * 0x100;
		
		texcoord2(0x100, framePos);
		vec3(fb->drawCoords[0], fb->drawCoords[1], 0x0);
		
		texcoord2(0, framePos);
		vec3(fb->drawCoords[2],	fb->drawCoords[3], 0x0);
		
		texcoord2(0, framePos + 0x100);
		vec3(fb->drawCoords[4], fb->drawCoords[5], 0x0);
		
		texcoord2(0x100, framePos + 0x100);
		vec3(fb->drawCoords[6], fb->drawCoords[7], 0x0);
		
		REG_GFX_END = 0;
	}
