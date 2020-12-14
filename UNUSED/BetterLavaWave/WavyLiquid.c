#include "game.h"
#include "registers.h"
#include "WavyLiquid.h"
#include <math.h>
#include "OSReport.h"

extern Vec3 WavyLiquidPosition;
extern Vec3 WavyLiquidScale;
extern int WavyLiquidFrame;
extern int WavyLiquidSpaceBetween;
extern bool WavyLiquidSpawned;

int WavyLiquid_loadFiles()
	{
		for (int i=0; i<12; i++)
		{
			loadFileByExtId_3dModel(2654-0x83 + i, 0);
		}
		return 1;
	}

void WavyLiquid_setup(WavyLiquid* wave)
	{
		wave->ExistDist.x = 0xFFFF;
		wave->ExistDist.y = 0xFFFF;
        wave->frame = 0;
		
		wave->scale.x = 1 + (wave->spriteData & 0xF);				        // Nybble 12.
		wave->scale.y = 1 + ((wave->spriteData >> 4) & 0xF);		        // Nybble 11.
		wave->speed = ((wave->spriteData >> 8) & 7);				        // Nybble 10.
		wave->speedsign = ((wave->spriteData >> 8) & 8);                    // Nybble 10, top.
        wave->spacebetween = ((wave->spriteData >> 20) & 0xF) * 0x10000;	// Nybble 7 

        wave->position.x = (0x10000 * wave->scale.x) + (16 * (*(((u16*)getPtrToViewById(wave->ViewID,0)) + 1)));		// Moves the mode 0 wave to beginning of the screen.
        wave->position.y -= 0x10000;												                                    // So bottom of box in NSMBE = bottom of liquid
        wave->position.z = 0x300000;										                                            // Goes above everything
		wave->initialX = wave->position.x;
   
        u8* LiquidType = (u8*)0x20CACE0;
        int Type = ((wave->spriteData >> 16) & 0xF);
            
        if (Type == 1) 
            *LiquidType = 2;           // Nybble 8 = 1, poison.
        else if (Type == 2) 
            *LiquidType = 4;           // Nybble 8 = 2, lava.
        else
            *LiquidType = 0;           // Else, water.
            
        WavyLiquidPosition = wave->position;
        WavyLiquidScale = wave->scale;
        WavyLiquidFrame = wave->frame;
        WavyLiquidSpawned = true;
	}

void WavyLiquid_update_state0(WavyLiquid* wave)
	{
		if (wave->speedsign)
            wave->position.x += wave->speed * 0x250;
		else
            wave->position.x -= wave->speed * 0x250;

		wave->frame++;
		if (wave->frame >= 96) wave->frame = 0;
        
        WavyLiquidPosition = wave->position;
        WavyLiquidFrame = wave->frame;
	}

bool WavyLiquid_beforeDraw(WavyLiquid* wave)
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

void WavyLiquid_UpdateTextureParams(WavyLiquid* wave, int File, int frame)
	{
		u8* texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(File));
		getTextureParams(texture, 0, &wave->WaveTex.texparam);
		getTexturePalBase(texture, 0, &wave->WaveTex.palbase);		
	}

void WavyLiquid_draw(WavyLiquid* wave)
	{
        u32 *CameraObject = *((u32**)(0x20CAA38));
        s32 ScreenX = -(*(CameraObject + (0xC0 / 4)));
        s32 Xoffs = ScreenX - wave->position.x;
        u16 *StageZoom = (u16*)(0x020CADB4);
        int Size = (0x10000 * wave->scale.x) + wave->spacebetween;
        
        s32 XPosOfFirstWaveOnScreen = (Xoffs/Size - 1) * Size;
        XPosOfFirstWaveOnScreen += wave->position.x;
        
		Vec3 Scale; 
		Scale.x = 0x8000 * wave->scale.x;
		Scale.y = 0x10000 * wave->scale.y;
		Scale.z = 0;
     
        Vec3 Pos;
        Pos.x = XPosOfFirstWaveOnScreen;
        Pos.y = wave->position.y;
        Pos.z = wave->position.z;
        
        u32 ScreenRangeX = ((*StageZoom) / 0x100);
        int frame = wave->frame/8;

        for (int i = 0; i < 1 + ((ScreenRangeX + 4) / (wave->scale.x /2)); i++)
        {
            WavyLiquid_UpdateTextureParams(wave, (2654 + frame -0x83), 0);
            WavyLiquid_DrawPolygon(wave, Pos, Scale, 0x400, 0, 0, 0x800);
            Pos.x += (0x8000 * wave->scale.x);
            WavyLiquid_UpdateTextureParams(wave, (2654 + ((12 - frame) % 12) -0x83), 0);
            WavyLiquid_DrawPolygon(wave, Pos, Scale, 0, 0x400, 0, 0x800);
            Pos.x += wave->spacebetween + (0x8000 * wave->scale.x);
        }
	}


void WavyLiquid_DrawPolygon(WavyLiquid* wave, Vec3 Position, Vec3 Scale, int TexXA, int TexXB, int TexYA, int TexYB)
	{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = Position.x;
		REG_MTX_TRANSLATE = Position.y;
		REG_MTX_TRANSLATE = Position.z;

		REG_MTX_SCALE = Scale.x;
		REG_MTX_SCALE = Scale.y;
		REG_MTX_SCALE = 0;
 
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (12 << 16));
		REG_TEXT_FORMAT = wave->WaveTex.texparam;
		REG_PAL_BASE = wave->WaveTex.palbase;

		REG_COLOR= 0x7FFF;

		REG_GFX_BEGIN = 1;
		texcoord2(TexXA, TexYA);
		vec3(0x1000, 	0x1000, 	0x0);

		texcoord2(TexXB, TexYA);
		vec3(0x0000,	0x1000, 	0x0);

		texcoord2(TexXB, TexYB);
		vec3(0x0000, 	0x0000,		0x0);

		texcoord2(TexXA, TexYB);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;
	}

void* WavyLiquid_dtor(WavyLiquid* wave)
    {
        wave->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        wave->Unk4Vec2 = dtor2;
        wave->Unk3Vec2 = dtor2;
        wave->DeleteDist = dtor2;
        wave->DrawDist = dtor2;
        wave->ExistDist = dtor2;
        wave->unkv13 = dtor;
        wave->unkv12 = dtor;
        wave->unkv11 = dtor;
		
        WavyLiquidPosition = dtor;
        WavyLiquidScale = dtor;
        WavyLiquidFrame = -1;
        WavyLiquidSpaceBetween = -1;
        WavyLiquidSpawned = false;
		
        return wave;
    }

void* WavyLiquid_dtorFree(WavyLiquid* wave)
    {
        WavyLiquid_dtor(wave);
		freeToGameHeap(wave);
        return wave;
    }
	