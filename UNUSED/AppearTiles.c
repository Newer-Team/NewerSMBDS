#include <limits.h>
#include "game.h"
#include "registers.h"
#include "AppearTiles.h"


const int MODEL = 1938 - 0x83;
const int ANIM_MAX = 0x1000;

// Use these to adjust the distances the tiles will become visible at.
// "Outer" is the radius the tile will barely start to be visible at
// "Inner" is the radius the tile will be completely opaque at
#define MARIO_RADIUS_INNER   0x30000
#define MARIO_RADIUS_OUTER   0x50000
#define GOOMBA_RADIUS_INNER  0x20000
#define GOOMBA_RADIUS_OUTER  0x30000


int AppearTiles_loadFiles()
{
    loadFileByExtId_3dModel(MODEL, 0);
    return 1;
}


void AppearTiles_setup(AppearTiles* tiles)
{
    u8* Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(MODEL));
    getTextureParams(Texture, 0, &tiles->Tex.texparam);
    getTexturePalBase(Texture, 0, &tiles->Tex.palbase);

                                                     // TODO: I just copypasted this
                                                     // draw distance code from BowserHeadEntrance;
                                                     // we should pick actually good values here
    Vec2 Distances;
    Distances.x = 0x100;							 //// This needs to go into the spriteparams.
    Distances.y = 0x100;

    tiles->DrawDist = Distances;
	tiles->ExistDist = Distances;

    tiles->tilesToDraw = tiles->spriteData & 0xFFFF; // Nybbles 9-12 specify which
                                                     // tiles should be drawn:
                                                     // (Nybble  9 &)  8 4 2 1
                                                     // (Nybble 10 &)  8 4 2 1
                                                     // (Nybble 11 &)  8 4 2 1
                                                     // (Nybble 12 &)  8 4 2 1
													 
													 
													 //// This will be hard to get working with movable collision.

    // Run IdleState to prepare tiles->animPos before _Draw() runs
    AppearTiles_IdleState(tiles);
}


inline bool AppearTiles_ShouldDraw(AppearTiles* tiles, int tileNum)
{
    return (tiles->tilesToDraw >> (15 - tileNum)) & 1;
}


int AppearTiles_GetAnimPos(AppearTiles* tiles, int currentAnimPos, int tileNum, s32 x, s32 y, s32 minDist, s32 maxDist)
{
    int xOffset = tileNum % 4;
    int yOffset = tileNum / 4;

    // TODO: try to make this work without s64's

    s64 tileCenterX = tiles->position.x + 0x8000 + 0x10000 * xOffset;
    s64 tileCenterY = tiles->position.y - 0x8000 - 0x10000 * yOffset;

    s64 distX = ((s64)x) - tileCenterX;
    s64 distY = ((s64)y) - tileCenterY;
    s32 distance = sqrt64(distX * distX + distY * distY);

    int animPos = distance;
    if (animPos < minDist) animPos = minDist;
    if (animPos > maxDist) animPos = maxDist;
    animPos -= minDist;
    animPos = (animPos * ANIM_MAX) / (maxDist - minDist);
    animPos = ANIM_MAX - animPos;

    if (animPos > currentAnimPos)
        return animPos;
    return currentAnimPos;
}


void AppearTiles_IdleState(AppearTiles* tiles)
{
	DeleteIfOutOfRange(tiles, 0);
		
    MarioActor* mario = (MarioActor*)getPtrToPlayerActor();

    for (int i = 0; i < 16; i++) {
        if (!AppearTiles_ShouldDraw(tiles, i)) continue;

        int animPos = 0;

        // Check Mario pos
        // TODO: support for small or ducking mario (add 0x8000 to y instead of 0x10000)
        animPos = AppearTiles_GetAnimPos(tiles,
                                         animPos,
                                         i,
                                         mario->xPos,
                                         mario->yPos + 0x10000,
                                         MARIO_RADIUS_INNER,
                                         MARIO_RADIUS_OUTER);

        // Check Goomba positions
        EnemyClassObject* Goomba = (EnemyClassObject*) findActorByType(83, 0);			//// This is rather processor intensive and lags on hardware.
        while (Goomba != NULL) {														//// We only got away with it in Newer DS because there were max 3 lava waves on screen, and with this all of the 25 or so tiles
																						//// try to find it.
																						//// Instead maybe we should write a hypervisor sprite that searches for all sprites on screen (one, once per frame, you see) 
																						//// and sets up an array of radiuses that should be made visible?
            animPos = AppearTiles_GetAnimPos(tiles,
                                             animPos,
                                             i,
                                             Goomba->position.x,
                                             Goomba->position.y + 0x8000,
                                             GOOMBA_RADIUS_INNER,
                                             GOOMBA_RADIUS_OUTER);
            Goomba = (EnemyClassObject*) findActorByType(94, Goomba->base.stuff[6]);
        }

        tiles->animPos[i] = animPos;
    }
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


void AppearTiles_draw(AppearTiles* tiles)
{
    for (int i = 0; i < 16; i++) {
        if (!AppearTiles_ShouldDraw(tiles, i)) continue;
        int xOffset = i % 4;
        int yOffset = i / 4;
        AppearTiles_drawTile(tiles, xOffset, yOffset, tiles->animPos[i]);
    }
}


void AppearTiles_drawTile(AppearTiles* tiles, int xTileOffset, int yTileOffset, int animPos)			
{
    // Optimization: if we're not going to draw anything, just return now
    if (animPos == 0) return;

    // Rotate every other tile (checkerboard-style) in the opposite direction
    int tileX = (tiles->position.x >> 16) + xTileOffset;
    int tileY = (tiles->position.y >> 16) + yTileOffset;
    int rotDirection = (((tileX & 1) + (tileY & 1)) & 1) ? 1 : -1;

    // Figure out model parameters
    s16 rotation = (0x4000 * rotDirection * animPos) / ANIM_MAX;
    int scale = (0x10000 * animPos) / ANIM_MAX;
    int opacity = (31 * animPos) / ANIM_MAX;
    if (opacity > 31) opacity = 31;

    Vec3 Position = tiles->position;
    Position.x -= scale / 2;
    Position.y -= scale / 2;
    Position.x += 0x8000;
    Position.y -= 0x8000;
    Position.x += 0x10000 * xTileOffset;
    Position.y -= 0x10000 * yTileOffset;

    Vec3 Scale;
    Scale.y = scale;
    Scale.x = scale;
    
    Vec2 TR, TL, BL, BR;
    TR.x = BR.x = 0x100;
    TL.x = BL.x = 0;
    TR.y = TL.y = 0x100;
    BR.y = BL.y = 0;
    
    Vec3 RotTrans;
    RotTrans.x = Scale.x / 2;
    RotTrans.y = Scale.x / 2;

    AppearTiles_DrawAPolygon(tiles->Tex, Position, Scale, TR, TL, BL, BR, RotTrans, rotation, opacity);
}

//// NSML has a standardized function for this now
void AppearTiles_DrawAPolygon(TexInfo Texture, Vec3 Position, Vec3 Scale, Vec2 TopRight, Vec2 TopLeft, Vec2 BottomLeft, Vec2 BottomRight, Vec3 RotationTranslation, s16 Rotation, int Opacity)
{
    if (Opacity == 0) return;

    NNS_G3dGeFlushBuffer();
    G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = Position.x;
    REG_MTX_TRANSLATE = Position.y;
    REG_MTX_TRANSLATE = Position.z;

    glTranslatef32(RotationTranslation.x,RotationTranslation.y,RotationTranslation.z);
    glRotateZi(Rotation);
    glTranslatef32(-RotationTranslation.x,-RotationTranslation.y,-RotationTranslation.z);

    REG_MTX_SCALE = Scale.x;
    REG_MTX_SCALE = Scale.y;
    REG_MTX_SCALE = Scale.z;

    REG_MTX_CONTROL = 3;
    REG_MTX_IDENTITY = 0;
    REG_MTX_CONTROL = 2;

    REG_POLY_FORMAT = (0x3880 | (Opacity << 16));
    REG_TEXT_FORMAT = Texture.texparam;
    REG_PAL_BASE = Texture.palbase;

    REG_COLOR = 0x7FFF;

    REG_GFX_BEGIN = 1;
    texcoord2(TopRight.x, TopRight.y);
    vec3(0x1000,    0x1000,     0x0);

    texcoord2(TopLeft.x, TopLeft.y);
    vec3(0x0000,    0x1000,     0x0);

    texcoord2(BottomLeft.x, BottomLeft.y);
    vec3(0x0000,    0x0000,     0x0);

    texcoord2(BottomRight.x, BottomRight.y);
    vec3(0x1000,    0x0000,     0x0);

    REG_GFX_END = 0;
}


void* AppearTiles_dtor(AppearTiles* tiles)
{
    tiles->vtable = (u32*)0x20C4F04;

    Vec3 dtor = {0,0,0};
    Vec2 dtor2 = {0,0};

    tiles->Unk4Vec2 = dtor2;
    tiles->Unk3Vec2 = dtor2;
    tiles->DeleteDist = dtor2;
    tiles->DrawDist = dtor2;
    tiles->ExistDist = dtor2;
    tiles->unkv13 = dtor;
    tiles->unkv12 = dtor;
    tiles->unkv11 = dtor;
	
	base_onDelete(tiles);

    return tiles;
}


void* AppearTiles_dtorFree(AppearTiles* tiles)
{
    AppearTiles_dtor(tiles);
    freeToGameHeap(tiles);
    return tiles;
}
