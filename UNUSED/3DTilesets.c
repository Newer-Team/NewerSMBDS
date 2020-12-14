#include <limits.h>
#include "game.h"
#include "Enemy.h"
#include "registers.h"
#include "auto_consts_files.h"

#include "TexDraw.h"
#include "3DTilesets.h"
#include "OSReport.h"
#include "OSPanic.h"

u8* Pa1_Texture;
u8* NRGT_File;

#define PARALLAX_SCALE 1.023

void DrawTileAtPosition(int TilesetID, int TileNumber, int x, int y, u8 xsize, u8 ysize, int zOrder, int parallaxX)
{
    Vec3 Pos;
    Pos.x = x << 12;
    Pos.y = (-y << 12) - (0x10000 * ysize);
    Pos.z = 0x10000 + zOrder * 0x1000;

    Vec3 Scale;
    Scale.x = 0x10000 * xsize;
    Scale.y = 0x10000 * ysize;
    Scale.z = 1;

    Vec2 TR, TL, BL, BR;
    TL.x = BL.x = 0;
    TR.x = BR.x = 0x100 * xsize;
    TR.y = TL.y = 0;
    BR.y = BL.y = 0x100 * ysize;

    Vec3 RotTrans;
    RotTrans.x = 0;
    RotTrans.y = 0;

    TexInfo Pa1_Info;
    getTextureParams(Pa1_Texture, TileNumber * 2, &Pa1_Info.texparam);
    getTexturePalBase(Pa1_Texture, TileNumber * 2, &Pa1_Info.palbase);

    DrawPolygon(Pa1_Info, Pos, Scale, 0, RotTrans, TR, TL, BL, BR, 31, 0);

    // ---
    zOrder += 512;

    Pos.x = (x << 12) - ((parallaxX << 12) * (PARALLAX_SCALE - 1));
    Pos.y = (-y << 12) - (0x10000 * ysize);
    Pos.z = 0x10000 + zOrder * 0x1000;

    Scale.x = 0x10000 * xsize * PARALLAX_SCALE;
    Scale.y = 0x10000 * ysize;
    Scale.z = 1;

    TL.x = BL.x = 0;
    TR.x = BR.x = 0x100 * xsize;
    TR.y = TL.y = 0;
    BR.y = BL.y = 0x100 * ysize;

    RotTrans.x = 0;
    RotTrans.y = 0;

    getTextureParams(Pa1_Texture, TileNumber * 2 + 1, &Pa1_Info.texparam);
    getTexturePalBase(Pa1_Texture, TileNumber * 2 + 1, &Pa1_Info.palbase);

    DrawPolygon(Pa1_Info, Pos, Scale, 0, RotTrans, TR, TL, BL, BR, 31, 0x100);
}


int GetMap16IndexAtPosition(int x, int y)
{
    u16** LevelTilemapPointers = (u16**)(0x020CAFE0);

    int BlockID = getTilemapBlockForPos(ptrToLevelActor, x, y);
    u16* BlockData = LevelTilemapPointers[BlockID];
    u16 TileData = BlockData[((x & 0xF0) >> 4) + (y & 0xF0)];

    if (TileData == 1)
        return 0;
    else
        return TileData;
}


bool canExtendInto(TileSettings *a, TileSettings *b)
{
    if (a->TilesetNumber != b->TilesetNumber) return false;

    if (b->Disabled)
    {
        // B is a see-through tile, so only let A extend into it
        // if it's the exact same type of tile
        return a->Index == b->Index;
    }

    int nrgtTableLen = NRGT_File[5];

    int aGroup = 0, bGroup = 0;
    int aIndex = a->Index, bIndex = b->Index;
    if (aIndex < nrgtTableLen)
        aGroup = NRGT_File[5 + aIndex];
    if (bIndex < nrgtTableLen)
        bGroup = NRGT_File[5 + bIndex];

    if (aGroup && bGroup)
        return aGroup == bGroup;
    return aIndex == bIndex;
}

TileSettings TSetArray[24][24];
int LastScreenX = 0x7FFFFFFF;
int LastScreenY = 0x7FFFFFFF;

void hook_020BB438_ov_00()
{
    u16 *StageZoom = (u16*)(0x020CADB4);
    u32 *CameraObject = *((u32**)(0x20CAA38));
    s32 ScreenX = *(CameraObject + (0xC0 / 4));
    s32 ScreenY = *(CameraObject + (0xC4 / 4));
    u32 xPos = (-ScreenX >> 12) / 16;
    u32 yPos = (ScreenY >> 12) / 16 - 13;

    u32 ScreenRangeX = ((*StageZoom) / 0x100);
    u32 ScreenRangeY = 0;

    if (ScreenRangeX < 16)
    {
        ScreenRangeX = 18;
        ScreenRangeY = 16;
    }
    else
    {
        ScreenRangeY = 2 + ((ScreenRangeX * 3) / 4);
        ScreenRangeX += 2;
    }

    if (LastScreenX != xPos || LastScreenY != yPos)
    {
        LastScreenX = xPos;
        LastScreenY = yPos;

        // Getting all the tile rows
        for (int j = 0; j < ScreenRangeY; j++)
        {
            for (int i = 0; i < ScreenRangeX; i++)
            {
                s32 x = (xPos + i) * 16;
                s32 y = (yPos + j) * 16;

                int map16Index = GetMap16IndexAtPosition(x, y);

                int TilesetNumber = 0;
                int Index = 0;

                if (map16Index < 256)
                {
                    TilesetNumber = 0;
                    Index = 0;
                }
                else if (map16Index < 3328)
                {
                    TilesetNumber = 1;
                    Index = map16Index - 256;
                }
                else
                {
                    TilesetNumber = 2;
                    Index = map16Index - 3328;
                }

                TileSettings *me = &TSetArray[i][j];
                me->Index = Index;
                me->TilesetNumber = TilesetNumber;
                me->Width = 1;
                me->Height = 1;
                me->Disabled = false;
            }
        }

        // Merging
        for (int j = 0; j < ScreenRangeY; j++)
        {
            for (int i = 0; i < ScreenRangeX; i++)
            {
                TileSettings *me = &TSetArray[i][j];

                if (!me->TilesetNumber || me->Disabled)
                    continue;

                // Extend rightward as far as possible
                for (int r = i + 1; r < ScreenRangeX + 1; r++)
                {
                    TileSettings *other = &TSetArray[r][j];

                    if (!canExtendInto(me, other))
                        break;

                    me->Width++;
                    if (me->Index == other->Index)
                        other->Disabled = true;
                }

                // Extend downward as far as possible
                for (int q = j + 1; q < ScreenRangeY + 1; q++)
                {
                    bool canExtendDown = true;

                    for (int p = i; p < i + me->Width; p++)
                    {
                        if (!canExtendInto(me, &TSetArray[p][q]))
                        {
                            canExtendDown = false;
                            break;
                        }
                    }

                    if (!canExtendDown) break;

                    me->Height++;
                    for (int p = i; p < i + me->Width; p++)
                    {
                        if (me->Index == TSetArray[p][q].Index)
                            TSetArray[p][q].Disabled = true;
                    }
                }

                i += me->Width - 1;
            }
        }
    }

    // Drawing
    int z = 0;
    int centerX = (-ScreenX >> 12) + ScreenRangeX * 8;
    for (int j = 0; j < ScreenRangeY; j++)
    {
        for (int i = 0; i < ScreenRangeX; i++)
        {
            if (!TSetArray[i][j].TilesetNumber || TSetArray[i][j].Disabled)
               continue;

            s32 x = (xPos + i) * 16;
            s32 y = (yPos + j) * 16;

            DrawTileAtPosition(TSetArray[i][j].TilesetNumber,
                               TSetArray[i][j].Index,
                               x,
                               y,
                               TSetArray[i][j].Width,
                               TSetArray[i][j].Height,
                               z++,
                               centerX - x);
        }
    }
}

//u16 timeElapsed;

// void hook_02004F10()
// {
//     // pre vblank
//     u16 timeElapsed = NSMB_TimerTick(2);
//     NSMB_TimerStop(2);

//     OS_Report_Num("Frame length:", timeElapsed);
// }

// void hook_02004F14()
// {
//     // post vblank
//     NSMB_TimerStart(2, ClockDivider_64, 0);
// }

// void hook_020B0360_ov_00()
// {
//     NSMB_TimerStart(2, ClockDivider_64, 0);
// }

// void hook_020B04A4_ov_00()
// {
//     timeElapsed = NSMB_TimerTick(2);
//     NSMB_TimerStop(2);
// }

void hook_020BB974_ov_00()
{
    u16* Pa1Number = (u16*)0x020CACBC;
    int texFileID = FOLDERADJ_TILESETS_PA1 + (8 * (*Pa1Number)) + 6;
    loadFileByExtId_3dModel(texFileID, 0);
    Pa1_Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(texFileID));

    int nrgtFileID = FOLDERADJ_TILESETS_PA1 + (8 * (*Pa1Number)) + 4;
    NRGT_File = (u8*)loadFileByExtId_LZ(nrgtFileID);
    if (*((u32*)NRGT_File) != 0x5447524E) // "NRGT"
        OS_Panic_With_Reason("NRGT magic incorrect");
}


void randomizeTilemapFromNRND(u16* tilemap, u8* nrndFile)
{
    int firstRandomizedGroupEnd = *((u16*)(nrndFile + 0x4));
    int secondRandomizedGroupStart = *((u16*)(nrndFile + 0x6));
    int secondRandomizedGroupEnd = *((u16*)(nrndFile + 0x8));
    u8* firstTable = nrndFile + 0xA;
    u8* secondTable = nrndFile + 0xA + firstRandomizedGroupEnd;
    int lastTableOffs = 0xA + firstRandomizedGroupEnd + secondRandomizedGroupEnd - secondRandomizedGroupStart;
    if (lastTableOffs % 2 == 1) lastTableOffs++;
    u16* lastTable = (u16*)(nrndFile + lastTableOffs);

    for (int i = 0; i < 256; i++) {
        u16 tile = tilemap[i];
        if (tile < 256) continue;
        tile -= 256;
        if (tile >= 768) continue;

        int indexInLastTable;
        if (tile < firstRandomizedGroupEnd)
        {
            indexInLastTable = firstTable[tile];
        }
        else if (secondRandomizedGroupStart <= tile && tile < secondRandomizedGroupEnd)
        {
            indexInLastTable = secondTable[tile - secondRandomizedGroupStart];
        }
        else continue;

        int numChoices = (lastTable[indexInLastTable] >> 12) + 2;
        int choice = RNG(numChoices);

        if (choice != numChoices - 1) {
            tilemap[i] = lastTable[indexInLastTable + choice] & 0xFFF;
        }
    }
}


void hook_020B3C38_ov_00()
{
    u16* Pa1Number = (u16*)0x020CACBC;
    int nrndFileID = FOLDERADJ_TILESETS_PA1 + (8 * (*Pa1Number)) + 5;
    u8* nrnd = (u8*)loadFileByExtId_LZ(nrndFileID);
    if (*((u32*)nrnd) != 0x444E524E) // "NRND"
        OS_Panic_With_Reason("NRND magic incorrect");

    u16** LevelTilemapPointers = (u16**)(0x020CAFE0);

    for (int i = 0; i < 512; i++) {
        u16* tilemap = LevelTilemapPointers[i];

        // TODO: find a more robust solution, or prove that this solution works in general
        if (tilemap == NULL) continue;

        randomizeTilemapFromNRND(tilemap, nrnd);
    }

    freeToCurrentHeapIfNotNull(nrnd);
}
