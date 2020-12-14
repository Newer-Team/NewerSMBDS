#include "game.h"
#include "registers.h"
#include "Electrocutor.h"
#include <math.h>
#include "sfx.h"
#include "VariousFunctions.h"

int Electrocutor_loadFiles()
	{
		loadFileByExtId_3dModel(2666-0x83, 0);
		return 1;
	}

void Electrocutor_setup(Electrocutor* zap)
	{		
		u8* Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(2666-0x83));
		
		getTextureParams(Texture, 0, &zap->NormalTex.texparam);
		getTexturePalBase(Texture, 0, &zap->NormalTex.palbase);
		getTextureParams(Texture, 1, &zap->ElectroTex.texparam);
		getTexturePalBase(Texture, 1, &zap->ElectroTex.palbase);
		
		zap->Tex = zap->NormalTex;
		
		zap->SizeX = 1 + (zap->spriteData & 0xF);							// Nybble 12.
		zap->SizeY = 1 + ((zap->spriteData >> 4) & 0xF);					// Nybble 11.
		zap->style = ((zap->spriteData >> 8) & 0x1);						// Nybble 10.
		
		if ((zap->spriteData >> 12) & 0x1)	zap->position.x += 0x8000;		// Nybble 9.
		if ((zap->spriteData >> 12) & 0x2)	zap->position.y += 0x8000;		// Nybble 9.
		
		zap->position.y -= 0x10000;											// Top right corner of box = sprite box in NSMBe
		zap->position.z = -20000;
		
		Vec2 Dist;	
		
		Dist.x = 0x20 * (zap->SizeX + 1);
		Dist.y = 0x20 * (zap->SizeX + 1);
		
		zap->DrawDist = Dist;
		
		Dist.x = Dist.y = 0x3A0;
		
		zap->ExistDist = Dist;
	
		Dist.x = 0x20;
		Dist.y = 0;
		
		zap->DeleteDist = Dist;
		
		zap->electrified = false;
		zap->PlayingSND = 0;
		
		int SizeXAddition = 0;
		int SizeYAddition = 0;
		
		if (!((zap->spriteData >> 16) & 0x1)) SizeXAddition = 0x6000;		// Nybble 8.
		if (!((zap->spriteData >> 16) & 0x2)) SizeYAddition = 0x6000;		// Causes the physics to only stick out in one direction
			
		u32 Electrocutor_physics[] = 										// Physics stick out a bit so Mario can actually touch the hurtbox
			{
				0x8000 + (0x8000 * (zap->SizeX)),		
				-0x1800 - (0x8000 * (zap->SizeY-1)),	
				SizeXAddition + (0x8000 * (zap->SizeX+1)),	
				SizeYAddition + (0x8000 * (zap->SizeY+1)),	
				0x910004, 
				0x0,
				0x0,
			};		

		initActivePhysics(&zap->activephysics, zap, Electrocutor_physics, 0);
		registerCollider(&zap->activephysics);			
			
		u32 Electrocutor_amp_physics[] = 										// Physics stick out a bit so Mario can actually touch the hurtbox
			{
				0x8000 + (0x8000 * (zap->SizeX)),		
				-0x1800 - (0x8000 * (zap->SizeY-1)),	
				0x8000 * (zap->SizeX+1),	
				0x8000 * (zap->SizeY+1),	
				0x910004, 
				0x0,
				(u32)&ElectrocutorCollision,
			};	
		
		initActivePhysics(&zap->activephysics2, zap, Electrocutor_amp_physics, 0);
		registerCollider(&zap->activephysics2);
		
		// Only uses SolidPhysics if the box is shifted by half tile; else, spawns solid tiles.
		// Makes the boxes more efficient, since SolidCollider doesn't have to be tracked on all those non-shifted boxes.
		// Still made it possible to have an unshifted one use the Solid Physics; set the shift nybble to 4.
		
		if (zap->spriteData >> 12 & 0xF)
			{
				s32 Solid_physics[] = 												
					{
						0x0,								// Top Right
						0x10000,							// Bottom Left
						(0x10000 * (zap->SizeX+1)),			// Top Right
						-(0x10000 * (zap->SizeY)),			// Bottom Right
						0,									// Top Callback
						0, 									// Bottom Callback
						0,									// Side Callback
						0,	
						0,
						0,
					};			
					
				InitSolidCollision(&zap->solidcollision, zap, Solid_physics, 0, 0, 0);
				registerSolidCollider(&zap->solidcollision);
			}
		else 
			{
				u32 xPos = (zap->position.x>>12) / 16; 
				u32 yPos = ((-zap->position.y-0x8000)>>12) / 16;
				
				int i = 0;
				int row = 0;
				
				while (true)	
					{
						changeTileAny(ptrToLevelActor, (i+xPos)*16, (row+yPos)*16, 8);
						i++;
						if (!(i % (zap->SizeX+1))) 
							{
								row++;
								i = 0;
							}
						if (row > zap->SizeY) break;
					}	
			}

	}

void Electrocutor_update_state0(Electrocutor* zap)
	{	
		if (zap->electrified)
			{
				if (!zap->PlayingSND)
					{
						zap->PlayingSND = 20;
						PlaySNDEffect(SE_PLY_TOUCH_BIRI, &zap->position);
					}
				else zap->PlayingSND--;
				
				zap->Tex = zap->ElectroTex;
				
				Vec3 ParticlePosition;
				
				s32 TopOfBox = zap->position.y + 0x10000;
				
				ParticlePosition.x = zap->position.x;
				ParticlePosition.y = TopOfBox;
				ParticlePosition.z = zap->position.z;
				
				ParticlePosition.x = zap->position.x + ((RNG(zap->SizeX+2))* 0x10000);
				ParticlePosition.y = TopOfBox - ((RNG(zap->SizeY+2))* 0x10000);
				SpawnAndEndParticle(40, 102, &ParticlePosition, 0, 0, 0, 0);
			}
			
		else zap->Tex = zap->NormalTex;	

		if (!zap->ElectroTimer)
			{
				zap->electrified = false;
				zap->activephysics.callback = 0;
			}
		else zap->ElectroTimer--;
		
		DeleteIfOutOfRange(zap, 0);
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

void Electrocutor_draw(Electrocutor* zap)
	{
		DrawPolygon(zap, 0, 0, 0);														// TR Corner
		DrawPolygon(zap, zap->SizeX*0x10000, 0, 0x100);									// TL Corner
		
		if (!zap->style)
			{
				DrawPolygon(zap, 0, -zap->SizeY*0x10000, 0x200);								// BR Corner Lava
				DrawPolygon(zap, zap->SizeX*0x10000, -zap->SizeY*0x10000, 0x300);				// BL Corner Lava
				for (int i = 0; i < zap->SizeX; i++)
					{
						DrawPolygon(zap, (0x10000+(i*0x10000)), -zap->SizeY*0x10000, 0x500);	// Bottom Edge Lava
					}
			}
		else 
			{
				DrawPolygon(zap, 0, -zap->SizeY*0x10000, 0x900);								// BR Corner Normal
				DrawPolygon(zap, zap->SizeX*0x10000, -zap->SizeY*0x10000, 0xA00);				// BL Corner Normal
				for (int i = 0; i < zap->SizeX; i++)
					{
						DrawPolygon(zap, (0x10000+(i*0x10000)), -zap->SizeY*0x10000, 0xB00);	// Bottom Edge Normal
					}
			}			
			
		for (int i = 0; i < zap->SizeX; i++)
			{
				DrawPolygon(zap, (0x10000+(i*0x10000)), 0, 0x400);						// Top Edge
			}
			
		for (int i = 0; i < zap->SizeY; i++)
			{
				DrawPolygon(zap, 0, -(0x10000+(i*0x10000)), 0x600);						// Right Edge
			}
			
		for (int i = 0; i < zap->SizeY; i++)
			{
				DrawPolygon(zap, zap->SizeX*0x10000, -(0x10000+(i*0x10000)), 0x700);	// Left Edge
			}
		
		int row = 0;
		int i = 0;
		
		while (true)																	// Filler
			{
				DrawPolygon(zap, ((i*0x10000)+0x10000), ((-row*0x10000)-0x10000), 0x800);
				i++;
				if (!(i % (zap->SizeX-1))) 
					{
						row++;
						i = 0;
					}
				if (row > zap->SizeY-1) break;
			}
	}
	
void DrawPolygon(Electrocutor* zap, int Xoffset, int Yoffset, int TextureOffset)
{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = zap->position.x + Xoffset;
		REG_MTX_TRANSLATE = zap->position.y + Yoffset;
		REG_MTX_TRANSLATE = zap->position.z;

		REG_MTX_SCALE = 0x10000;
		REG_MTX_SCALE = 0x10000;
		REG_MTX_SCALE = 0;

		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = zap->Tex.texparam;
		REG_PAL_BASE = zap->Tex.palbase;

		REG_COLOR= 0x7FFF;

		REG_GFX_BEGIN = 1;
		
		texcoord2(0x100, TextureOffset);
		vec3(0x1000, 	0x1000, 	0x0);

		texcoord2(0, TextureOffset);
		vec3(0x0000,	0x1000, 	0x0);

		texcoord2(0, 0x100+TextureOffset);
		vec3(0x0000, 	0x0000,		0x0);

		texcoord2(0x100, 0x100+TextureOffset);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;
}

void ElectrocutorCollision(ActivePhysics* a, ActivePhysics* b)
	{
		Electrocutor* zap = (Electrocutor*) a->owner;
		EnemyClassObject* Enemy = (EnemyClassObject*) b->owner;
		
		if (Enemy->ClassID == 110) 
		{
			zap->electrified = true;
			zap->ElectroTimer = 5;
			zap->activephysics.callback = (u32*)0x02098C78;
		}
		return;
	}