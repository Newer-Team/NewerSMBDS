#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "Vignette.h"
#include "Files.h"

int Vignette_loadFiles()
	{
		loadFileByExtId_3dModel(2933-0x83, 0);
		return 1;
	}
    
    
void Vignette_setup(Vignette* vig)
{
	u8* texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(2933-0x83));
	getTextureParams(texture, 0, &vig->Texture.texparam);
	getTexturePalBase(texture, 0, &vig->Texture.palbase);

	vig->RotateTimer = 0;

	vig->DrawDist.x = 0xFFFFFFF;
	vig->DrawDist.y = 0xFFFFFFF;
	vig->ExistDist.x = 0xFFFFFFF;
	vig->ExistDist.y = 0xFFFFFFF;

	vig->scale.x = 0xA0000;
	vig->scale.y = 0x60000;
	vig->scale.z = 1;
}


void Vignette_State0(Vignette* vig)
{
	return;
}

void Vignette_DieOnFlagpole(Vignette* vig)
{
}

bool Vignette_BeforeDraw()
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

void Vignette_Draw(Vignette* vig)
{
    u16 *StageZoom = (u16*)(0x020CADB4); 
    u32* CameraObject = *((u32*)(0x20CAA38));
    s32 ScreenX = *(CameraObject + (0xC0 / 4)); 
    s32 ScreenY = *(CameraObject + (0xC4 / 4));     

	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = -ScreenX - 0x20000;
	REG_MTX_TRANSLATE = -ScreenY + 0x60000;
	REG_MTX_TRANSLATE = 0x300000;

	REG_MTX_SCALE = vig->scale.x;
	REG_MTX_SCALE = vig->scale.y;
	REG_MTX_SCALE = vig->scale.z;
	 
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;

	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = vig->Texture.texparam;
	REG_PAL_BASE = vig->Texture.palbase;

	REG_COLOR= 0x7FFF;

	REG_GFX_BEGIN = 1;
	texcoord2(0x800, 0);
	vec3(0x1000, 0x1000, 0x0);

	texcoord2(0, 0);
	vec3(0x0000,0x1000, 0x0);

	texcoord2(0, 0x800);
	vec3(0x0000, 0x0000, 0x0);

	texcoord2(0x800, 0x800);
	vec3(0x1000, 0x0000, 0x0);

	REG_GFX_END = 0;
    
 	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = -ScreenX + 0x80000;
	REG_MTX_TRANSLATE = -ScreenY + 0x60000;
	REG_MTX_TRANSLATE = 0x20000;

	REG_MTX_SCALE = vig->scale.x;
	REG_MTX_SCALE = vig->scale.y;
	REG_MTX_SCALE = vig->scale.z;
	 
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;

	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = vig->Texture.texparam;
	REG_PAL_BASE = vig->Texture.palbase;

	REG_COLOR= 0x7FFF;

	REG_GFX_BEGIN = 1;
	texcoord2(0, 0);
	vec3(0x1000, 0x1000, 0x0);

	texcoord2(0x800, 0);
	vec3(0x0000,0x1000, 0x0);

	texcoord2(0x800, 0x800);
	vec3(0x0000, 0x0000, 0x0);

	texcoord2(0, 0x800);
	vec3(0x1000, 0x0000, 0x0);

	REG_GFX_END = 0;
       
    NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = -ScreenX - 0x20000;
	REG_MTX_TRANSLATE = -ScreenY;
	REG_MTX_TRANSLATE = 0x20000;

	REG_MTX_SCALE = vig->scale.x;
	REG_MTX_SCALE = vig->scale.y;
	REG_MTX_SCALE = vig->scale.z;
	 
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;

	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = vig->Texture.texparam;
	REG_PAL_BASE = vig->Texture.palbase;

	REG_COLOR= 0x7FFF;

	REG_GFX_BEGIN = 1;
	texcoord2(0x800, 0x800);
	vec3(0x1000, 0x1000, 0x0);

	texcoord2(0, 0x800);
	vec3(0x0000,0x1000, 0x0);

	texcoord2(0, 0);
	vec3(0x0000, 0x0000, 0x0);

	texcoord2(0x800, 0);
	vec3(0x1000, 0x0000, 0x0);

	REG_GFX_END = 0; 
    
    
    

    NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = -ScreenX + 0x80000;
	REG_MTX_TRANSLATE = -ScreenY;
	REG_MTX_TRANSLATE = 0x20000;

	REG_MTX_SCALE = vig->scale.x;
	REG_MTX_SCALE = vig->scale.y;
	REG_MTX_SCALE = vig->scale.z;
	 
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;

	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = vig->Texture.texparam;
	REG_PAL_BASE = vig->Texture.palbase;

	REG_COLOR= 0x7FFF;

	REG_GFX_BEGIN = 1;
	texcoord2(0, 0x800);
	vec3(0x1000, 0x1000, 0x0);

	texcoord2(0x800, 0x800);
	vec3(0x0000,0x1000, 0x0);

	texcoord2(0x800, 0);
	vec3(0x0000, 0x0000, 0x0);

	texcoord2(0, 0);
	vec3(0x1000, 0x0000, 0x0);

	REG_GFX_END = 0; 
}