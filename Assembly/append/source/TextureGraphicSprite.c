#include "game.h"
#include "registers.h"
#include "TextureGraphicSprite.h"

void TextureGraphicSprite_setup(TextureGraphicSprite* txtr)
{       
    txtr->Style = txtr->spriteData & 0xFF;                      // Nybbles 11-12.
    int ModelFile = 0;
    
    switch (txtr->Style)
    {
        case 0:                                                 // Minecart Loopedyloop
        {
            ModelFile = 2694;                   
            txtr->DrawDistanceAddition = 0x80000;
            break;
        }
        default: Base__deleteIt(txtr);
    }
    
    txtr->ModelPointer = (u32*)loadFileByExtId_3dModel(ModelFile - 0x83, 0);
    if ((txtr->spriteData>>8)&0xF) Base__deleteIt(txtr);        // Nybble 10, so you can safely load the model.
}
    
bool TextureGraphicSprite_CheckIfMarioInRange(TextureGraphicSprite* txtr)
{
    u16 *StageZoom = (u16*)(0x020CADB4);
    
    MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
    
    int xoffs = Player->xPos - txtr->position.x;
    int yoffs = Player->yPos - txtr->position.y;
    int DrawDistance = (*StageZoom * 0x100) + txtr->DrawDistanceAddition;
    
    if (xoffs > DrawDistance || xoffs < -DrawDistance || yoffs < -0x300000 || yoffs > 0x300000)  return false;
    
    else return true;
}

bool TextureGraphicSprite_beforeDraw(TextureGraphicSprite* txtr)
{
    return TextureGraphicSprite_CheckIfMarioInRange(txtr);
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

void TextureGraphicSprite_draw(TextureGraphicSprite* txtr)
{
    u8* texture = (u8*) NNS_G3dGetTex((u32*)txtr->ModelPointer);    

    switch (txtr->Style)
    {
        case 0:                                                     // Minecart Loopedyloop
        {       
            Vec3 Scale;
            Scale.x = 0x80000;
            Scale.y = 0xA0000;
            Scale.z = 0;
            
            Vec3 Position = txtr->position;
            Position.z = 0;
            Position.y -= 0x10000;
            
            // Right half
            getTextureParams(texture, 0, &txtr->Texture.texparam);
            getTexturePalBase(texture, 0, &txtr->Texture.palbase);
            TextureGraphicSprite_DrawAPolygon(txtr->Texture, Scale, Position, 0x800, 0x800);
            
            // Left half
            Position.z += 0x300000;
            getTextureParams(texture, 1, &txtr->Texture.texparam);
            getTexturePalBase(texture, 1, &txtr->Texture.palbase);
            TextureGraphicSprite_DrawAPolygon(txtr->Texture, Scale, Position, 0x800, 0x800);
            break;
        }   
        default: return;
    }   
}

void TextureGraphicSprite_DrawAPolygon(TexInfo texture, Vec3 Scale, Vec3 Position, int CoordX, int CoordY)
{
    NNS_G3dGeFlushBuffer();
    G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = Position.x;
    REG_MTX_TRANSLATE = Position.y;
    REG_MTX_TRANSLATE = Position.z; 

    REG_MTX_SCALE = Scale.x;
    REG_MTX_SCALE = Scale.y;
    REG_MTX_SCALE = Scale.z;
    
    REG_MTX_CONTROL = 3;
    REG_MTX_IDENTITY = 0;
    REG_MTX_CONTROL = 2;

    REG_POLY_FORMAT = 0x001F3880;
    REG_TEXT_FORMAT = texture.texparam;
    REG_PAL_BASE = texture.palbase;
    
    REG_COLOR= 0x7FFF;
    
    REG_GFX_BEGIN = 1;
    texcoord2(CoordX, 0);
    vec3(0x1000,    0x1000,     0x0);
        
    texcoord2(0, 0);
    vec3(0x0000,    0x1000,     0x0);
        
    texcoord2(0, CoordY);
    vec3(0x0000,    0x0000,     0x0);
        
    texcoord2(CoordX, CoordY);
    vec3(0x1000,    0x0000,     0x0);

    REG_GFX_END = 0;
}
    