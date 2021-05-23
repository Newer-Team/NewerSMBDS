#include "game.h"
#include "registers.h"
#include "Enemy.h"
#include "sfx.h"
#include "VariousFunctions.h"

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


void CoinScore_draw(EnemyClassObject* goal, int score)
{
    NNS_G3dGeFlushBuffer();
    G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = goal->position.x - 0x24000;
    REG_MTX_TRANSLATE = *((u32*)goal + 0x1CF);
    REG_MTX_TRANSLATE = goal->position.z + 0x200000;    

    REG_MTX_SCALE = 0x20000;
    REG_MTX_SCALE = 0x10000;
    REG_MTX_SCALE = 0;
    
    REG_MTX_CONTROL = 3;
    REG_MTX_IDENTITY = 0;
    REG_MTX_CONTROL = 2;
    
    int *player = (int*)(0x02085A50);
    
    int FileID = (2682 - 0x83) + *player;
    u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(FileID));
    
    TexInfo CoinScore;
    getTextureParams(texture, 0, &CoinScore.texparam);
    getTexturePalBase(texture, 0, &CoinScore.palbase);

    REG_POLY_FORMAT = 0x001F3880;
    REG_TEXT_FORMAT = CoinScore.texparam;
    REG_PAL_BASE = CoinScore.palbase;   
    
    REG_COLOR= 0x7FFF;
    
    REG_GFX_BEGIN = 1;
    texcoord2(0x200, 0x100 * score);
    vec3(0x1000,    0x1000,     0x0);
        
    texcoord2(0, 0x100 * score);
    vec3(0x0000,    0x1000,     0x0);
        
    texcoord2(0, (0x100 + 0x100 * score));
    vec3(0x0000,    0x0000,     0x0);
        
    texcoord2(0x200, (0x100 + 0x100 * score));
    vec3(0x1000,    0x0000,     0x0);

    REG_GFX_END = 0;
    
    if (*((u8*)goal + 0x75C))
    {
        getCoin(0);
        if ((*((u8*)goal + 0x75C) % 3) == 0) Rumble(1);
        PlaySNDEffect(SE_OBJ_GET_COIN, 0);
        AddPointsToScore(0, 200);
        *((u8*)goal + 0x75C) -= 1;
    }
    
}
    