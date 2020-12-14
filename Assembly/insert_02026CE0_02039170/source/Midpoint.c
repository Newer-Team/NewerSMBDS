#include "game.h"
#include "registers.h"
#include "Midpoint.h"
#include "VariousFunctions.h"
#include <math.h>
#include "sfx.h"

int MidpointPlayer = 0;

extern u8 MidwayPointSecond;
extern s8 MidwayAreaSecond;


int Midpoint_loadFiles()
	{
		int *world = (int*)(0x02088BFC);
        int *level = (int*)(0x02085A9C);
		
		int FileID = 2672 - 0x83 + *world;
		
		if ((*world == 7) && (*level > 4) && (*level != 0xE) && (*level != 0xD)) FileID+=1;
		
		loadFileByExtId_3dModel(FileID, 0);
		return 1;
	}

void Midpoint_setup(Midpoint* mid)
	{	
		int *world = (int*)(0x02088BFC);
        int *level = (int*)(0x02085A9C);
		
		mid->FileID =  2672 - 0x83 + *world;
		
		if ((*world == 7) && (*level > 4) && (*level != 0xE) && (*level != 0xD)) mid->FileID+=1;
		
		u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(mid->FileID));
		
		getTextureParams(texture, 0, &mid->Staketex.texparam);
		getTexturePalBase(texture, 0, &mid->Staketex.palbase);
		
		if (!getStartingFromMidway())
			{
				getTextureParams(texture, 3, &mid->Flagtex.texparam);
				getTexturePalBase(texture, 3, &mid->Flagtex.palbase);
				MidpointPlayer = 0;
			}
		else
			{
				getTextureParams(texture, MidpointPlayer+1, &mid->Flagtex.texparam);
				getTexturePalBase(texture, MidpointPlayer+1, &mid->Flagtex.palbase);
				mid->gotten = true;
			}	
			
		mid->position.y-=0x2C000; 
			
		if (((mid->spriteData>>0x10)&0x1)) mid->position.x+=0x8000;
		if (((mid->spriteData>>0x10)&0x2)) mid->position.y+=0x8000;
			
		int Rotation = ((mid->spriteData>>8)&0xFF);
		float StartRadians = Rotation*1.4*3.14/180;
			
		mid->rotation.z = -Rotation*128;

		int offsx = 0;
		int offsy = 0;
		int sizex = 0;
		int sizey = 0;
			
		if ((Rotation & 0x3F) == 0) 
			{
				int bit1 = Rotation >> 7;
				int bit2 = (Rotation >> 6) & 1;
				offsx = bit2 ? 0x24000 : 0x8000;
				offsy = bit2 ? 0x8000 : 0x24000;
				sizex = bit2 ? 0x19000 : 0x3000;
				sizey = bit2 ? 0x3000 : 0x19000;
				if (bit1) offsx = -offsx;
				if (bit1 ^ bit2) offsy = -offsy;
			} 

		else 
			{
				offsx = 0x0 + 151053.016388 * cos(atan(0x24000/0x8000) - StartRadians);
				offsy = 0x0 + 151053.016388 * sin(atan(0x24000/0x8000) - StartRadians);
				sizex = 0x10000;
				sizey = 0x18000;
			}
			
		u32 Midpoint_physics[] = 
			{
				offsx,	
				offsy,	
				sizex,	
				sizey,	
				0x00030004, 
				0x0400,
				(u32)&MidpointCollision,
			};			
			
		initActivePhysics(&mid->activephysics, mid, Midpoint_physics, 0);
		registerCollider(&mid->activephysics);
	}

void Midpoint_update_state0(Midpoint* mid)
	{	
        if (mid->ShakeTimer >= 0)
        {
        	if (mid->ShakeTimer == 360)
	        	{
	        		if (MidpointPlayer)
						PlaySNDEffect(SE_VOC_LU_JUMP_03, 0);
					else
						PlaySNDEffect(SE_VOC_MA_JUMP_03, 0);
	        	}
            int rotation = ((mid->spriteData >> 8) & 0xFF);
            mid->rotation.z = -rotation * 128 + cos(mid->ShakeTimer*3.14/180) * (mid->ShakeTimer * 3 / 2);
            mid->ShakeTimer -= 20;
        }
		return;
	}

bool Midpoint_beforeDraw(Midpoint* mid)
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

void Midpoint_draw(Midpoint* mid)
	{
	//Render Stake	
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = mid->position.x;
		REG_MTX_TRANSLATE = mid->position.y;
		REG_MTX_TRANSLATE = mid->position.z;	
				
		glRotateZi(mid->rotation.z);
	
		REG_MTX_SCALE = 0x36000;
		REG_MTX_SCALE = 0x40000;
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = mid->Staketex.texparam;
		REG_PAL_BASE = mid->Staketex.palbase;
		
		REG_COLOR= 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x300, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0x400);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x300, 0x400);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;
		
	//Render Flag

		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = mid->position.x;
		REG_MTX_TRANSLATE = mid->position.y;
		REG_MTX_TRANSLATE = mid->position.z-0x500;
		
		glRotateZi(mid->rotation.z);
		glTranslatef32(0x9000,0x28000,0);
	
		REG_MTX_SCALE = 0x22000;
		REG_MTX_SCALE = 0x13000;
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = mid->Flagtex.texparam;
		REG_PAL_BASE = mid->Flagtex.palbase;
		
		REG_COLOR = 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x200, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0x100);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x200, 0x100);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;		
	}
	
void MidpointCollision(ActivePhysics* a, ActivePhysics* b)
	{
		Midpoint* mid = (Midpoint*) a->owner;
		EnemyClassObject* Actor = (EnemyClassObject*) b->owner;
				
		if ((Actor->ClassID != 0x15) && (Actor->ClassID != 0x137)) return; 
					
		if (!mid->gotten)
			{
				int* CurrentAreaNum = (int*)0x2085A94;
				
				s8 area = (mid->spriteData >> 24 & 0xF) + *CurrentAreaNum;
				u8 entrance = (mid->spriteData >> 28 & 0xF);
				
				if (entrance)
				{
					MidwayAreaSecond = area;
					MidwayPointSecond = entrance;
				}
				else
				{
					MidwayAreaSecond = -1;
					MidwayPointSecond = -1;	
				}
				
				mid->gotten = true;
				mid->ShakeTimer = 720;
				Rumble(1);
				
				MidpointPlayer = (*(int*)(0x02085A50));
				
				u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(mid->FileID));
				getTextureParams(texture, MidpointPlayer+1, &mid->Flagtex.texparam);
				getTexturePalBase(texture, MidpointPlayer+1, &mid->Flagtex.palbase);
				
				float StartRadians = ((mid->spriteData>>8)&0xFF)*1.4*3.14/180;

				Vec3 particle;	

				particle.x = mid->position.x + 167084.6714214 * cos(atan(0x28000/0x8000) - StartRadians);
				particle.y = mid->position.y + 167084.6714214 * sin(atan(0x28000/0x8000) - StartRadians);
				particle.z = 0;

				SpawnParticle(0x41, &particle);

				*((u64*)(0x0208AF3C)) |= 1<<((mid->spriteData&0xFF)-1); 
			}
		return;
	}