#include "game.h"
#include "Enemy.h"
#include "FileSel.h"
#include "VariousFunctions.h"
#include "sfx.h"
 
extern u8 lastslot;
 
FSEL* ThisFSEL = 0;
 
const u16* TopScreenTopLayer = BG_GFX_SUB + 0x8000;
const u16* TopScreenBotLayer = BG_GFX_SUB;
const u16* SubScreenTopLayer = BG_GFX + 0x8000;
const u16* SubScreenBotLayer = BG_GFX;
 
void ExecuteFSEL()
    {
        if (!ThisFSEL) InitFSEL();
 
        if ((!(pressedKeys & A)) && 
			(!(pressedKeys & B)) && 
			(!(pressedKeys & Y)) && 
			(!(pressedKeys & X)) && 
			(!(pressedKeys & RIGHT)) && 
			(!(pressedKeys & LEFT)) && 
			(!TouchDown)) ThisFSEL->FSELheld = false;

        if (ThisFSEL->FadeTimer)
        {
            ThisFSEL->FSELheld = true;
            ThisFSEL->FadeTimer--;
        }
 
        ScrollTopScreenBottomLayer();
 
        switch (ThisFSEL->currentScreen)
        {
            case 1:     EraseScreenCode(); break;
            case 2:     CopyScreenCode(); break;
            default:    MainScreenCode(); break;
        }
    }
 
void InitFSEL()
    {
        ThisFSEL = (FSEL*)allocFromCurrentHeap(sizeof(FSEL), 0);
 
        ThisFSEL->FSELheld = true;
        ThisFSEL->FSELSelection = 0;
        ThisFSEL->currentScreen = 0;
        ThisFSEL->currentSlot = lastslot;
        ThisFSEL->copyslot = ThisFSEL->currentSlot;
        ThisFSEL->Timer = 0;
        ThisFSEL->currentmessage = -1;
        ThisFSEL->yesnoselection = -1;
        ThisFSEL->FadeTimer = 30;
 
        vramDefault();
        SetupScreensForEnpgs();
 
        DrawEnpgToScreen(3068, 3, 0, true);
        DrawEnpgToScreen(3072, 3, 0, false);
        bgWrapOn(7);
 
        bgInitNerDS(2, BgType_Bmp8, BgSize_B8_256x256, 4, 0, true);
        bgInitNerDS(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0, false);
        bgInitNerDS(2, BgType_Bmp8, BgSize_B8_256x256, 4, 0, false);
 
        FadeIn(3, 0, 2);
 
        LoadMusicFile(0x1B, 0x2E);
		
		SwapASseq(0x1B, 2658);
		SwapASwar(0x15, 2659);
		SwapASbnk(0x14, 2660);
 
        PlayMusicFile(0x1B, 0);
 
        NNSi_SndArcLoadGroup(0x2E, *((u32**)(0x208FBB8)));
 
        for (int i=0; i<12; i++)
            {
                ThisFSEL->ENPGPointers[i] = (u8*) allocFromCurrentHeap(66048, 0);
                loadFileByExtId_LZ_Dest(3068 - 0x83 + i, ThisFSEL->ENPGPointers[i]);
            }
 
        for (int i=0; i<3; i++)
            {
                ThisFSEL->SaveFileInfos[i] = (SaveFileInfo*)allocFromCurrentHeap(sizeof(SaveFileInfo), 0);
                SetSaveInfo(i, ThisFSEL->SaveFileInfos[i]);
            }
 
        ArrangeAFileScreen(lastslot);
        ArrangeBottomScreen(lastslot);
    }
 
void SetSaveInfo(int slot, SaveFileInfo* Info)
    {
        loadSave2(slot, 0, &saveData);
 
        Info->Header = (u32)saveData.header;
        Info->Inited = (u8)saveData.inited;
        Info->World = (u8)saveData.world;
        Info->Lives = (u8)saveData.lives;
        Info->Coins = (u8)saveData.coins;
        Info->StarCoins = (u8)saveData.gottenstarcoins;
        Info->Score = (u32)saveData.Score;
 
        Info->CompletionBitmask = 0;
 
        int st = (u32)saveData.completion;
 
        if (st & 0x20) Info->CompletionBitmask |= 1;
        if (st & 0x1) Info->CompletionBitmask |= 2;
        if ((st & 0x780112) == 0x780112) Info->CompletionBitmask |= 4;
    }
 
void ScrollTopScreenBottomLayer()
    {
        // Wii U doesn't support scrolling backgrounds with bgScrollf(), so we have to do it manually.

        if (++ThisFSEL->ScrollTimer == 8)
        {
            ThisFSEL->ScrollTimer = 0;

            // Only the middle part of the BG needs to scroll
            for (int j = 27; j < 158; j++)
                {
                    // variable to store the byte that moves from each pair to the one prior
                    u8 saved_top8;

                    // Initialize
                    u16 first = ((u16*)BG_GFX_SUB)[j * 128];
                    u8 leftmost = first & 0xFF;
                    saved_top8 = first >> 8;

                    // Update first 127 pairs
                    for (int i = 0; i < 127; i++) {
                        u16 next16 = ((u16*)BG_GFX_SUB)[j * 128 + i + 1];
                        ((u16*)BG_GFX_SUB)[j * 128 + i] = saved_top8 | (next16 << 8);
                        saved_top8 = next16 >> 8;
                    }

                    // Update final pair
                    ((u16*)BG_GFX_SUB)[j * 128 + 127] = saved_top8 | (leftmost << 8);
                }
        }

        bgUpdate();
    }
 
void ArrangeAFileScreen(int slot)
    {
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 256, 26, 0, 0, 0, 0);                  // Redrawing Top Bar
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 256, 32, 0, 160, 0, 160);              // Redrawing Bottom Bar
 
        MIi_CpuClear32(0, (u16*)TopScreenTopLayer, 0x10000);                                                        // Clearing TopScreenTopLayer
 
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 56, 32, 16, 160, 0, 192);                // FILE
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 24, 32, 72, 160, 56 + (24*slot), 192);     // FILE #
 
        if (!ThisFSEL->SaveFileInfos[slot]->Inited)
            {
                copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 96, 64, 80, 64, 0, 80);            // "NEW"
                return;
            }
 
        else
        {
            int CharWidths[10] = {12, 12, 14, 14, 14, 14, 14, 14, 14, 14};
            int World = ThisFSEL->SaveFileInfos[slot]->World;
            int DigitHundreds = 0;
            int DigitTens = 0;
            int DigitOnes = 0;
 
        //region WORLD
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 64, 16, 32, 32, 0, 0);
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 16, 32, 0, 96, 0, 16);
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 48, 32, 16, 112, 16, 32);
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 16, 16, 0, 64, 16, 16);
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 16, 16, 0, 80, (16*(World % 4)), (64 + 16 * (World / 4)));
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 16, 80, 96, 48, (16 * (World % 4)), (96 + 80 * (World / 4)));
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 80, 64, 16, 48, (64 + 80 * (World / 4)), (64 * (World % 4)));
            copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 32, 32, 64, 112, 224, (32 * World));
            if (World == 4) copyVRAMTile(ThisFSEL->ENPGPointers[2], (u16*)TopScreenTopLayer, 32, 16, 32, 112, 32, 16);
 
 
        //region ICONS
            copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 112, 96, 112, 32, 112, 32);
 
        //region LIVES
            if (ThisFSEL->SaveFileInfos[slot]->Lives == 99)
                {
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 32, 32, 128, 32, 128, 0);
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 28, 20, 150, 44, 160, 156);
                }
 
            else
                {
                    DigitTens = ThisFSEL->SaveFileInfos[slot]->Lives / 10;
                    DigitOnes = ThisFSEL->SaveFileInfos[slot]->Lives % 10;
 
                    if (DigitTens) copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 32, 150, 32, (16 *  DigitTens), 144);
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 32, (150 + (((bool)DigitTens) * CharWidths[DigitTens])), 32, (16 *  DigitOnes), 144);
                }
 
        //region COINS
 
            DigitTens = ThisFSEL->SaveFileInfos[slot]->Coins / 10;
            DigitOnes = ThisFSEL->SaveFileInfos[slot]->Coins % 10;
 
            if (DigitTens) copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 32, 220, 32, (16 *  DigitTens), 144);
            copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 32, (220 + (((bool)DigitTens) * CharWidths[DigitTens])), 32, (16 *  DigitOnes), 144);
 
 
        //region STAR COINS
 
            if (ThisFSEL->SaveFileInfos[slot]->StarCoins == 240)
                {
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 32, 16, 160, 80, 160, 16);
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 40, 20, 182, 78, 160, 190);
                }
 
            else
                {
                    DigitHundreds = ThisFSEL->SaveFileInfos[slot]->StarCoins / 100;
                    DigitTens = (ThisFSEL->SaveFileInfos[slot]->StarCoins / 10) % 10;
                    DigitOnes = ThisFSEL->SaveFileInfos[slot]->StarCoins % 10;
 
                    int START_X = 182, add_x = 0;
 
                    if (DigitHundreds != 0)
                        {
                            copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 40, START_X + add_x, 68, (16 *  DigitHundreds), 180);
                            add_x += CharWidths[DigitHundreds];
                        }
 
                    if (add_x > 0 || DigitTens != 0)
                        {
                            copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 40, START_X + add_x, 68, (16 * DigitTens), 180);
                            add_x += CharWidths[DigitTens];
                        }
 
                    copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 40, START_X + add_x, 68, (16 * DigitOnes), 180);
 
                }
 
        //region SCORE
 
            int ScoreDigits[8]; // [most significant digit, second-most-sig. digit, ..., tens place, ones place]
            int CalcScore = ThisFSEL->SaveFileInfos[slot]->Score;
            int TotalScoreWidth = 0;
 
            // Find digits from right to left
            for (int i = 7; i >= 0; i--)
                {
                    ScoreDigits[i] = CalcScore % 10;
                    CalcScore /= 10;
                    TotalScoreWidth += CharWidths[ScoreDigits[i]];
                }
 
            int ScoreDrawX = 180 - (TotalScoreWidth / 2); // left edge
 
            // Draw from left to right
            for (int i = 0; i <= 7; i++)
                {
                    copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 16, 32, ScoreDrawX, 112, (16 * ScoreDigits[i]), 224);
                    ScoreDrawX += CharWidths[ScoreDigits[i]];
                }
        }
 
    }
 
void ArrangeCopyScreen(int slot, int XOffset)
    {
        int World = ThisFSEL->SaveFileInfos[slot]->World;
        int CharWidths[10] = {10, 10, 12, 12, 12, 12, 12, 12, 12, 12};
 
        int DigitHundreds = 0;
        int DigitTens = 0;
        int DigitOnes = 0;
 
        if (!XOffset) copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 48, 64, 112, 64, 160, 128);                // ARROW
 
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 56, 32, 16 + XOffset, 160, 0, 192);                // FILE
        copyVRAMTile(ThisFSEL->ENPGPointers[0], (u16*)TopScreenTopLayer, 24, 32, 72 + XOffset, 160, 56 + (24*slot), 192);   // FILE #
 
        if (!ThisFSEL->SaveFileInfos[slot]->Inited)
            {
                copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 96, 64, 8 + XOffset, 64, 0, 80);           // "NEW"
                return;
            }
 
        // WORLD
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 48, 6, 32 + XOffset, 26, 208, 128);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 48, 8, 32 + XOffset, 96, 208, 144);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 80, 64, 16 + XOffset, 32, (80 * (World % 3)), (64 * (World  / 3)));
 
 
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 30, 24, 0 + XOffset, 104, 160, 200);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 28, 24, 58 + XOffset, 104, 202, 200);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 30, 22, 24 + XOffset, 130, 200, 226);
 
 
        DigitTens = ThisFSEL->SaveFileInfos[slot]->Lives / 10;
        DigitOnes = ThisFSEL->SaveFileInfos[slot]->Lives % 10;
 
        if (DigitTens) copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, XOffset + 30, 106, (16 *  DigitTens), 238);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, (XOffset + 30 + (((bool)DigitTens) * CharWidths[DigitTens])), 106, (16 *  DigitOnes), 238);
 
        DigitTens = ThisFSEL->SaveFileInfos[slot]->Coins / 10;
        DigitOnes = ThisFSEL->SaveFileInfos[slot]->Coins % 10;
 
        if (DigitTens) copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, XOffset + 86, 106, (16 *  DigitTens), 238);
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, (XOffset + 86 + (((bool)DigitTens) * CharWidths[DigitTens])), 106, (16 *  DigitOnes), 238);
 
        DigitHundreds = ThisFSEL->SaveFileInfos[slot]->StarCoins / 100;
        DigitTens = (ThisFSEL->SaveFileInfos[slot]->StarCoins / 10) % 10;
        DigitOnes = ThisFSEL->SaveFileInfos[slot]->StarCoins % 10;
 
        int START_X = 54 + XOffset, add_x = 0;
 
        if (DigitHundreds != 0)
            {
                copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, START_X + add_x, 132, (16 *  DigitHundreds), 238);
                add_x += CharWidths[DigitHundreds];
            }
 
        if (add_x > 0 || DigitTens != 0)
            {
                copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, START_X + add_x, 132, (16 * DigitTens), 238);
                add_x += CharWidths[DigitTens];
            }
 
        copyVRAMTile(ThisFSEL->ENPGPointers[3], (u16*)TopScreenTopLayer, 14, 18, START_X + add_x, 132, (16 * DigitOnes), 238);
    }
 
void ScrollOffLayer(int layer, int direction)
    {
        for (int i = 0; i <= 256; i += 64)
            {
                waitForVblankIrqless();
                bgSetScroll(layer, i * direction, 0);
                ScrollTopScreenBottomLayer();
            }
    }
 
void ScrollInLayer(int layer, int direction)
    {
        for (int i = 256; i >= 0; i -= 64)
            {
                waitForVblankIrqless();
                bgSetScroll(layer, i * -direction, 0);
                ScrollTopScreenBottomLayer();
            }
    }
 
void FSELPlusMinus(int mode)
    {
        PlaySNDEffect(SE_SYS_CURSOR, 0);
 
        ScrollOffLayer(6, mode);
        bgSetScroll(6, 256*mode,  0);
        ArrangeAFileScreen(ThisFSEL->currentSlot);
        ThisFSEL->copyslot = ThisFSEL->currentSlot;
        ScrollInLayer(6, mode);
    }
 
void FSELPlusMinusInCopyMode(int slot)
    {
        PlaySNDEffect(SE_SYS_CURSOR, 0);
        copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 112, 80, 146, 0, 0, 0);
        copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 112, 80, 146, 80, 0, 0);
        copyVRAMTile(ThisFSEL->ENPGPointers[1], (u16*)TopScreenTopLayer, 20, 20, 140, 110, 0, 0);
 
        ArrangeCopyScreen(slot, 144);
        waitForVblankIrqless();
    }
 
void DrawBottomScreenSlot(int slot, int layer, int style)
    {
        u16* LayerAddress = (u16*)SubScreenTopLayer;
        if (!layer) LayerAddress = (u16*)SubScreenBotLayer;
 
        int World = 0;
        if (ThisFSEL->SaveFileInfos[slot]->Inited) World = ThisFSEL->SaveFileInfos[slot]->World + 1;
 
        if (style == 2) World += 3;
 
        int MainENPG;
        switch (style)
            {
                case 0: MainENPG = 6; break;
                case 1: MainENPG = 9; break;
                default: MainENPG = 7; break;
            }
             
        int ENPGAddition = World / 6;
        int BGENPG = (style == 2) ? 11 : 5;
     
        int Completion = ThisFSEL->SaveFileInfos[slot]->CompletionBitmask;
 
        int ButtonBaseX = ((World / 3) % 2) * 128;
        int ButtonBaseY = (World % 3) * 80;
        int TopRowY = ThisFSEL->SaveFileInfos[slot]->Inited ? 248 : 240;
 
        int ballX, ballY; 
         
        ballX = 12 + (slot * 76);
        ballY = 14 + ((slot & 1) * 54);
 
        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[BGENPG], LayerAddress, 96, 96, ballX - 8, ((ballY - 14) * (slot & 1)) - 1, ((slot & 1) * 96), (((bool)(slot & 2)) * 96));   // SHADOW
 
        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 80, 72, ballX, ballY, ButtonBaseX, ButtonBaseY);                   //BIG BALL PART
        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG], LayerAddress, 48, 8, ballX + 16, ballY - 8, 208, TopRowY);                                       //TOP PIXEL ROW
         
        if (slot != 0)                                                                                                                                          // FILE #
            {
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 48, 24, ballX+8, ballY+20, ButtonBaseX + 80, ButtonBaseY - 24 + (24 * slot));                        
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 16, 24, ballX+56, ballY+20, ButtonBaseX + 80 + (16 * slot), ButtonBaseY + 48); 
            }
             
        if (Completion & 1)                                                                                                                                      //STARS
            {
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 4, 4, ballX + 20, ballY + 44, ButtonBaseX + 96, ButtonBaseY + 72);
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 24, 8, ballX + 8, ballY + 48, ButtonBaseX + 0, ButtonBaseY + 72);
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 24, 8, ballX + 8, ballY + 56, ButtonBaseX + 24, ButtonBaseY + 72);
            }
 
        if (Completion & 2)
            {
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 16, 24, ballX + 32, ballY + 52, ButtonBaseX + 80, ButtonBaseY + 48);
            }
 
        if (Completion & 4)
            {
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 4, 4, ballX + 56, ballY + 44, ButtonBaseX + 96, ButtonBaseY + 72);
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 24, 8, ballX + 48, ballY + 48, ButtonBaseX + 48, ButtonBaseY + 72);
                copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[MainENPG + ENPGAddition], LayerAddress, 24, 8, ballX + 48, ballY + 56, ButtonBaseX + 72, ButtonBaseY + 72);
            }
    }
 
void FooterDraw(bool hide, int color, bool waitafter)
    {
        if (hide)
            {
                bgSetScroll(2, 0, -48);
                bgUpdate();
            }
 
        switch(color)
            {
                case 0: copyVRAMTile(ThisFSEL->ENPGPointers[4], (u16*)SubScreenBotLayer, 256, 48, 0, 144, 0, 16); break;                                                // No footer
                case 1: for (int i=0; i<=256; i+=32) copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 32, 48, 0 + i, 144, 32, 24); break;      // Red Footer
                case 2: for (int i=0; i<=256; i+=32) copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 32, 48, 0 + i, 144, 128, 24); break;     // Blue Footer
                case 3: copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[4], (u16*)SubScreenTopLayer, 256, 48, 0, 144, 0, 192); break;                                      // Black Footer
                default: break;
            }
             
        if ((ThisFSEL->currentSlot == 1) || (ThisFSEL->copyslot == 1)) DrawBottomScreenSlot(1, 0, 2);
        if (waitafter) waitForVblankIrqless();
    }
 
void FooterMove(int mode)
    {
        switch(mode)
            {
                case 1:                         //Scroll in
                    {
                        for (int i=-48; i <= 0; i+=4)
                            {
                                waitForVblankIrqless();
                                bgSetScroll(2, 0, i);
                                ScrollTopScreenBottomLayer();
                            }
                    }
                    break;
                case 2:                         //Scroll out
                    {
                        for (int i=0; i >= -48; i-=4)
                            {
                                waitForVblankIrqless();
                                bgSetScroll(2, 0, i);
                                ScrollTopScreenBottomLayer();
                            }
                    }
                    break;
                default: bgSetScroll(2, 0, 0); break;            //Just appear
            }
 
        if (mode == 1) copyVRAMTileWithAlpha((u16*)SubScreenTopLayer, (u16*)SubScreenBotLayer, 256, 48, 0, 144, 0, 144);
        copyVRAMTile((u16*)SubScreenTopLayer, (u16*)SubScreenTopLayer, 256, 48, 0, 144, 0, 0);
        if (ThisFSEL->currentSlot == 1) DrawBottomScreenSlot(1, 0, 2);
        waitForVblankIrqless();
    }
     
void DrawOKButton (bool red, bool pressed)
    {
        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[9], (u16*)SubScreenTopLayer, 40, 32, 208, 160, pressed * 40, red * 32);
    }
 
void DisplayMessage(int messageid, bool pressed)
    {
        switch(messageid)
            {
                case 0:                                                                                                                     // Copy over which file?
                    {
                        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer,  64, 24,  40, 168, 192,   0);
                        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer,  64, 24, 104, 168, 192,  24);
                        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer,  48, 24, 168, 168, 192,  48);
                    }
                    break;
                case 1: copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 160, 24,   8, 168,  96,  96); break;      // Overwrite this file?
                case 2: copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 112, 24,  72, 168,  96,  144); break;      // Copying file...
                case 3:                                                                                                                     // The file has been copied.
                    {
                        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 200, 24,   8, 168,   8, 192); 
                        DrawOKButton(false, pressed);
                    }
                    break;     
                case 4: copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 120, 24,  12, 168,  96, 120); break;      // Erase this file?
                case 5: copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 112, 24,  72, 168,  96, 168); break;      // Erasing file...
                case 6:                                                                                                                     // The file has been erased.
                    {
                        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenTopLayer, 200, 24,  8,  168,   8, 216);
                        DrawOKButton(true, pressed); 
                    }
                    break;
                default: messageid = 0; break;
            }
             
        ThisFSEL->currentmessage = messageid;
    }
	
void FadeMusicOut(int speed)
	{
		int* volume = (int*)0x208FBF4;
		int newvolume = 0;
		if (*volume) newvolume = (*volume - speed);
		if (newvolume < 0) newvolume = 0;
		moveVolumeForPlayer(newvolume, 1);	
	}
 
void ChangeMessage(int newmessage, bool pressed, bool dontredrawslot2)
    {
        DisplayMessage(ThisFSEL->currentmessage, pressed);

        int srcX = (ThisFSEL->currentmessage < 4) ? 128 : 32;
        for (int i = 0; i <= 256; i += 32)
            copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[5], (u16*)SubScreenBotLayer, 32, 48, 0 + i, 144, srcX, 24);
         
        // Else the file flickers.  But, it still has to be done if we're in the copy screen and copying ends
        if ((ThisFSEL->copyslot == 1) || (!dontredrawslot2 && (ThisFSEL->currentSlot == 1))) 
            DrawBottomScreenSlot(1, 0, 2);
         
        ScrollOffLayer(2, 1);
        copyVRAMTile((u16*)SubScreenTopLayer, (u16*)SubScreenTopLayer, 256, 48, 0, 144, 0, 0);
        DisplayMessage(newmessage, pressed);
        ScrollInLayer(2, 1);
 
        copyVRAMTileWithAlpha((u16*)SubScreenTopLayer, (u16*)SubScreenBotLayer, 256, 48, 0, 144, 0, 144);
        copyVRAMTile((u16*)SubScreenTopLayer, (u16*)SubScreenTopLayer, 256, 48, 0, 144, 0, 0);
    }
 
void DrawYesNoButton(bool redBackground, bool saysNo, int style, int layer)
    {   // "style": 0 = normal, 1 = selected, 2 = pressed

        u16* LayerAddress = layer ? (u16*)SubScreenTopLayer : (u16*)SubScreenBotLayer;

        int enpg = 4 + (redBackground ? 3 : 0) + style;
        int srcX = (saysNo ? 80 : 0);
        int destX = (saysNo ? 208 : 168);

        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[enpg], (u16*)LayerAddress, 40, 16, destX, 160, srcX + 0, 240);
        copyVRAMTileWithAlpha(ThisFSEL->ENPGPointers[enpg], (u16*)LayerAddress, 40, 16, destX, 176, srcX + 40, 240);
    }
 
void ShowYesNoButtons(bool red) 
    {
        DrawYesNoButton(red, false, 0, 1);
        DrawYesNoButton(red, true, 1, 1);
        copyVRAMTileWithAlpha((u16*)SubScreenTopLayer, (u16*)SubScreenBotLayer, 88, 32, 168, 160, 168, 160);
        copyVRAMTile((u16*)SubScreenTopLayer, (u16*)SubScreenTopLayer, 88, 32, 168, 160, 0, 0);
    }
 
void SwitchYesNoSelection(bool red)  
    {
        ThisFSEL->yesnoselection ^= 1;
        DrawYesNoButton(red, false, (ThisFSEL->yesnoselection ? 1 : 0), 1);
        DrawYesNoButton(red, true, (ThisFSEL->yesnoselection ? 0 : 1), 1);
    }
 
void OperateOnYesNoButtons(bool red) 
    {
        if (((pressedKeys & LEFT && !ThisFSEL->FSELheld)) || ((pressedKeys & RIGHT && !ThisFSEL->FSELheld)))
            {
                PlaySNDEffect(SE_SYS_CURSOR, 0);
                SwitchYesNoSelection(red);
                ThisFSEL->FSELheld = true;
            }
 
        else if (AREA_TOUCHED(168,160,40,32) && !ThisFSEL->FSELheld)
            {
                ThisFSEL->yesnoselection = 0;
                SwitchYesNoSelection(red);
                pressedKeys |= A;
            }
 
        else if (AREA_TOUCHED(208,160,40,32) && !ThisFSEL->FSELheld)
            {
                ThisFSEL->yesnoselection = 1;
                SwitchYesNoSelection(red);
                pressedKeys |= A;
            }
    }
	
void ArrangeBottomScreen(int highlighted)
    {
		MI_DmaCopy32(2, (u16*)SubScreenBotLayer, (u16*)SubScreenTopLayer, 0x10000);
		MI_DmaCopy32(2, (u16*)ThisFSEL->ENPGPointers[4], (u16*)SubScreenBotLayer, 0x9200);
		
		bgHide(3);
		
        for (int i = 0; i < 3; i++)
            {
                if (i != highlighted) DrawBottomScreenSlot(i, false, 0);
            }
			
		DrawBottomScreenSlot(highlighted, 0, 2);
		waitForVblankIrqless();
		bgShow(3);
		
		MIi_CpuClear32(0, (u16*)SubScreenTopLayer, 0x10000);
    }
     
void ArrangeBottomCopyScreen(int copied, int copiedto)
    {
		MI_DmaCopy32(2, (u16*)SubScreenBotLayer, (u16*)SubScreenTopLayer, 0x10000);
		MI_DmaCopy32(2, (u16*)ThisFSEL->ENPGPointers[4], (u16*)SubScreenBotLayer, 0x9200);
		bgHide(3);
         
        for (int i = 0; i < 3; i++)
        {
			if ((copied == copiedto) && (copied == i)) DrawBottomScreenSlot(i, false, 0);
            if ((copied != i) && (copiedto != i)) DrawBottomScreenSlot(i, false, 0);
        }
         
        DrawBottomScreenSlot(copied, false, 1);
        if (copied != copiedto) DrawBottomScreenSlot(copiedto, false, 2);
		
		waitForVblankIrqless();
		bgShow(3);
		MIi_CpuClear32(0, (u16*)SubScreenTopLayer, 0x10000);     
    }   
 
void MainScreenCode()
    {
        if (ThisFSEL->FSELSelection == 1)      // GO TO FILE
            {
                if (ThisFSEL->Timer)
                    {
                        ThisFSEL->Timer--;
						if (ThisFSEL->Timer < 80) FadeMusicOut(2);
                        return;
                    }
 
                loadSave2(ThisFSEL->currentSlot, 0, &saveData);
 
                FSELCleanup();

                if ((u8)saveData.inited) causeSceneChange(9, (u8)saveData.world);
                else causeSceneChange(100, 140);
 
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 2)      // BACK TO TS
            {
				if (ThisFSEL->Timer)
					{
						ThisFSEL->Timer--;
						FadeMusicOut(3);
						return;
					}
						
                FSELCleanup();
                causeSceneChange(4,0);
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 3)      // GO TO ERASE
            {
                FooterDraw(0, 3, true);
                FooterDraw(0, 0, true);
                FooterMove(2);
 
                FooterDraw(1, 1, false);
                 
                DisplayMessage(4, false);
                FooterMove(1);
                ThisFSEL->yesnoselection = 0;
                ShowYesNoButtons(true);
                ThisFSEL->FSELSelection = 0;
                ThisFSEL->currentScreen = 1;
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 4)      // GO TO COPY
            {
                FooterDraw(0, 3, true);
                FooterDraw(0, 0, true);
                FooterMove(2);
 
                ScrollOffLayer(6, 1);
 
                FooterDraw(1, 2, false);
                DisplayMessage(0, false);
                FooterMove(1);
 
                MIi_CpuClear32(0, (u16*)TopScreenTopLayer, 0x10000);
                ArrangeCopyScreen(ThisFSEL->currentSlot, 0);
				
                ArrangeBottomCopyScreen(ThisFSEL->currentSlot, ThisFSEL->currentSlot);

                ScrollInLayer(6, 1);
 
                ThisFSEL->copyslot = ThisFSEL->currentSlot;
                ThisFSEL->FSELSelection = 0;
                ThisFSEL->currentScreen = 2;
                return;
            }
 
        else if ((pressedKeys & LEFT && !ThisFSEL->FSELheld))   // SHOW FILE -1
            {
                ThisFSEL->currentSlot -= 1;
                if (ThisFSEL->currentSlot < 0) ThisFSEL->currentSlot = 2;
                FSELPlusMinus(-1);
                 
                ArrangeBottomScreen(ThisFSEL->currentSlot);
                 
                 
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if ((pressedKeys & RIGHT && !ThisFSEL->FSELheld))  // SHOW FILE +1
            {
                ThisFSEL->currentSlot += 1;
                if (ThisFSEL->currentSlot > 2) ThisFSEL->currentSlot = 0;
                FSELPlusMinus(1);
                 
                ArrangeBottomScreen(ThisFSEL->currentSlot);
                 
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if ((pressedKeys & A) && !ThisFSEL->FSELheld)       // A is pressed, file is selected next frame
            {
                ThisFSEL->FSELSelection = 1;
                PlaySNDEffect(SE_SYS_FILE_SELECTED, 0);
                PlaySNDEffect(SE_SYS_DECIDE, 0);
                ThisFSEL->Timer = 125;
 
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if ((pressedKeys & B) && !ThisFSEL->FSELheld)       // B is pressed, back to TS is done next frame
            {
                ThisFSEL->FSELSelection = 2;
                PlaySNDEffect(SE_SYS_BACK, 0);
				ThisFSEL->Timer = 45;
 
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if (((pressedKeys & X) && !ThisFSEL->FSELheld) || (AREA_TOUCHED(0,168,80,24) && !ThisFSEL->FSELheld))        // X is pressed, erasing starts next frame
            {
                if (ThisFSEL->SaveFileInfos[ThisFSEL->currentSlot]->Inited)
                    {
                        PlaySNDEffect(SE_SYS_DECIDE, 0);
                        ThisFSEL->FSELSelection = 3;
                    }
                else PlaySNDEffect(SE_SYS_WARNING, 0);
 
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if (((pressedKeys & Y) && !ThisFSEL->FSELheld) || (AREA_TOUCHED(176,168,80,24) && !ThisFSEL->FSELheld))  // Y is pressed, copying starts next frame
            {
                if (ThisFSEL->SaveFileInfos[ThisFSEL->currentSlot]->Inited)
                    {
                        PlaySNDEffect(SE_SYS_DECIDE, 0);
                        ThisFSEL->FSELSelection = 4;
                    }
                else PlaySNDEffect(SE_SYS_WARNING, 0);
 
                ThisFSEL->FSELheld = true;
                return;
            }
        else if (!ThisFSEL->FSELheld) // Check for buttons touched on the touchscreen
            {
                int touchedFile = -1;
                if (AREA_TOUCHED(8,4,88,88)) touchedFile = 0;
                if (AREA_TOUCHED(84,54,88,88)) touchedFile = 1;
                if (AREA_TOUCHED(160,4,88,88)) touchedFile = 2;

                if (touchedFile != -1)
                    {
                        if (touchedFile > ThisFSEL->currentSlot)
                            {
                                ThisFSEL->currentSlot = touchedFile;
                                FSELPlusMinus(1);
                                ArrangeBottomScreen(ThisFSEL->currentSlot);
                            }
                        else if (touchedFile < ThisFSEL->currentSlot)
                            {
                                ThisFSEL->currentSlot = touchedFile;
                                FSELPlusMinus(-1);
                                ArrangeBottomScreen(ThisFSEL->currentSlot);
                            }
                        else
                            {
                                ThisFSEL->FSELSelection = 1;
                                PlaySNDEffect(SE_SYS_FILE_SELECTED, 0);
                                PlaySNDEffect(SE_SYS_DECIDE, 0);
                                ThisFSEL->Timer = 60;
                            }

                        ThisFSEL->FSELheld = true;
                    }
                return;
            }
    }
 
void EraseScreenCode()
    {
        if (ThisFSEL->Timer)                 // Timer when erasing, finishes the bomb sfx
            {
                ThisFSEL->Timer--;
 
                if (ThisFSEL->Timer == 1)
                    {
						Rumble(5);
                        PlaySNDEffect(SE_SYS_FILE_DELETE_END, 0);
                        SetSaveInfo(ThisFSEL->currentSlot, ThisFSEL->SaveFileInfos[ThisFSEL->currentSlot]);
                        ThisFSEL->Timer = 0;
                        ThisFSEL->FSELSelection = 3;
                 
                        ScrollOffLayer(6, -1);
                        ArrangeAFileScreen(ThisFSEL->currentSlot);
                        ArrangeBottomScreen(ThisFSEL->currentSlot);
                        ScrollInLayer(6, -1);
                         
                        ChangeMessage(6, false, false);
                        return;
                    }
 
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 1)      // Actual erasing operation. Also reassigns the last slot to the first available inited one.
            {
                bool IsCurrentlyLatest = false;
                if (ThisFSEL->SaveFileInfos[ThisFSEL->currentSlot]->Header == 0x31303037) IsCurrentlyLatest = true;
 
                EraseSave(ThisFSEL->currentSlot, &saveData);
 
                if (IsCurrentlyLatest)
                    {
                        lastslot = 0;
 
                        for (int i = 0; i<3; i++)
                            {
                                loadSave2(i, 0, &saveData);
                                if (saveData.inited)
                                    {
                                        lastslot=i;
                                        saveData.header=0x31303037;
                                        saveSave(i, &saveData);
                                        break;
                                    }
                            }
                    }
 
                    ThisFSEL->Timer = 10;
                    return;
            }
 
        else if (ThisFSEL->FSELSelection == 2)      // BACK TO MAIN
            {
                FooterDraw(0, 1, false);
                DisplayMessage(ThisFSEL->currentmessage, true);
                waitForVblankIrqless();
                 
                FooterDraw(0, 0, false);
                FooterMove(2);
 
                FooterDraw(0, 3, false);
                FooterMove(1);
 
                ThisFSEL->FSELheld = true;
                ThisFSEL->currentScreen = 0;
                ThisFSEL->FSELSelection = 0;
                ThisFSEL->yesnoselection = -1;
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 3)      // Wait for OK after erasing is done
            {
                if (((pressedKeys & A) && !ThisFSEL->FSELheld) || (TouchDown && !ThisFSEL->FSELheld))
                {
                    DrawOKButton (1, 1);
                    waitForVblankIrqless();
                    PlaySNDEffect(SE_SYS_DECIDE, 0);
                    ThisFSEL->FSELSelection = 2;
                    ThisFSEL->FSELheld = true;
                }
            }
 
        else                                        // Confirm you want to erase
        {
            OperateOnYesNoButtons(true);
 
            if ((pressedKeys & A) && ThisFSEL->yesnoselection && !ThisFSEL->FSELheld)     // A pressed on Yes
                {
                    DrawYesNoButton(true, false, 2, 1);
                    waitForVblankIrqless();
                    ChangeMessage(5, false, false);
                    PlaySNDEffect(SE_SYS_FILE_DELETING, 0);
                    ThisFSEL->FSELSelection = 1;
                    ThisFSEL->FSELheld = true;
                    return;
                }
                 
            else if (((pressedKeys & B) && !ThisFSEL->FSELheld ) || ((pressedKeys & A) && !ThisFSEL->yesnoselection && !ThisFSEL->FSELheld))       //B pressed or A pressed on "no"
                {
                    DrawYesNoButton(true, true, 2, 1);
                    waitForVblankIrqless();
                    PlaySNDEffect(SE_SYS_BACK, 0);
                    ThisFSEL->FSELSelection = 2;
                    ThisFSEL->FSELheld = true;
                    return;
                }
        }
    }
     
void ConfirmInCopy()                            
    {
        if (ThisFSEL->copyslot == ThisFSEL->currentSlot)
            {
                PlaySNDEffect(SE_SYS_WARNING, 0);
                ThisFSEL->FSELheld = true;
                return;
            }
 
        ThisFSEL->FSELSelection = 3;
        PlaySNDEffect(SE_SYS_DECIDE, 0);
        ChangeMessage(1, false, true);
        ShowYesNoButtons(false);
        ThisFSEL->yesnoselection = 0;
        ThisFSEL->FSELheld = true;
    }
     
void TouchedSlotInCopy(int slot)
    {
        if (ThisFSEL->copyslot == slot)
            {
                ConfirmInCopy();
                return;
            }
                         
        if (ThisFSEL->currentSlot != slot)
            {
                ThisFSEL->copyslot = slot;
                ArrangeBottomCopyScreen(ThisFSEL->currentSlot, ThisFSEL->copyslot);
                FSELPlusMinusInCopyMode(ThisFSEL->copyslot);
            }
        else PlaySNDEffect(SE_SYS_WARNING, 0);
 
        ThisFSEL->FSELheld = true;
        return;
    }
 
void CopyScreenCode()
    {
        if (ThisFSEL->Timer)                     // Copying timer. Ends sfx.
            {
                ThisFSEL->Timer--;
 
                if (ThisFSEL->Timer == 1)
                    {
                        PlaySNDEffect(SE_SYS_FILE_COPY_END, 0);
                        SetSaveInfo(ThisFSEL->currentSlot, ThisFSEL->SaveFileInfos[ThisFSEL->currentSlot]);
                        SetSaveInfo(ThisFSEL->copyslot, ThisFSEL->SaveFileInfos[ThisFSEL->copyslot]);
                        ThisFSEL->Timer = 0;
                        ThisFSEL->currentSlot = ThisFSEL->copyslot;
                        lastslot = ThisFSEL->copyslot;
 
                        ScrollOffLayer(6, -1);
                        ArrangeAFileScreen(ThisFSEL->currentSlot);
                        ArrangeBottomScreen(ThisFSEL->currentSlot);
                        ScrollInLayer(6, -1);
 
                        ChangeMessage(3, false, false);
                        ThisFSEL->FSELSelection = 4;
                        return;
                    }
 
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 1)      // Actual copying operation. Also sets last slot to the file copied to.
            {
                for (int i = 0; i<3; i++)
                    {
                        loadSave2(i, 0, &saveData);
                        if (saveData.header == 0x31303037)
                            {
                                saveData.header=0x30303037;
                                saveSave(i, &saveData);
                                break;
                            }
                    }
 
                loadSave2(ThisFSEL->currentSlot, 0, &saveData);
                saveData.header=0x31303037;
                saveSave(ThisFSEL->copyslot, &saveData);
 
                loadSave2(ThisFSEL->copyslot, 0, &saveData);
 
                ThisFSEL->Timer = 10;
 
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 2)      // BACK TO MAIN
            {
                FooterDraw(0, 2, false);
                DisplayMessage(ThisFSEL->currentmessage, true);
                waitForVblankIrqless();
                 
                FooterDraw(0, 0, false);
                 
                FooterMove(2);
                FooterDraw(0, 3, false);
                FooterMove(1);
 
                ScrollOffLayer(6, -1);
                ArrangeAFileScreen(ThisFSEL->currentSlot);
                ArrangeBottomScreen(ThisFSEL->currentSlot);
                ScrollInLayer(6, -1);
 
                ThisFSEL->FSELheld = true;
                ThisFSEL->currentScreen = 0;
                ThisFSEL->FSELSelection = 0;
                ThisFSEL->yesnoselection = -1;
                ThisFSEL->copyslot = ThisFSEL->currentSlot;
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 3)      // Confirm whether you want to copy
            {
                OperateOnYesNoButtons(false);
 
                if ((pressedKeys & A) && ThisFSEL->yesnoselection && !ThisFSEL->FSELheld)     // Confirm
                    {
                        DrawYesNoButton(false, false, 2, 1);
                        waitForVblankIrqless();
                        ChangeMessage(2, false, true);
                        PlaySNDEffect(SE_SYS_FILE_COPYING, 0);
                        ThisFSEL->FSELSelection = 1;
                        ThisFSEL->FSELheld = true;
                        ThisFSEL->yesnoselection = -1;
                    }
 
 
                if (((pressedKeys & A) && !ThisFSEL->yesnoselection && !ThisFSEL->FSELheld) || ((pressedKeys & B) && !ThisFSEL->FSELheld))     // Deny (A on "no" or B)
                    {
                        DrawYesNoButton(false, true, 2, 1);
                        waitForVblankIrqless();
                        ChangeMessage(0, false, true);
                        PlaySNDEffect(SE_SYS_BACK, 0);
                        ThisFSEL->FSELSelection = 0;
                        ThisFSEL->FSELheld = true;
                        ThisFSEL->yesnoselection = -1;
                    }
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 4)      // Wait for OK after copying
            {
                if (((pressedKeys & A) && !ThisFSEL->FSELheld) || (TouchDown && !ThisFSEL->FSELheld))
                    {
                        DrawOKButton (0, 1);
                        waitForVblankIrqless();
                        PlaySNDEffect(SE_SYS_DECIDE, 0);
                        ThisFSEL->FSELSelection = 5;
                        ThisFSEL->FSELheld = true;
                    }
                return;
            }
 
        else if (ThisFSEL->FSELSelection == 5)      // Back to main without swapping topscreen display (for after copying)
            {
                FooterDraw(0, 2, false);
                DisplayMessage(ThisFSEL->currentmessage, true);
                waitForVblankIrqless();
                 
                FooterDraw(0, 0, false);
                FooterMove(2);
 
                FooterDraw(0, 3, false);
                FooterMove(1);
 
                ThisFSEL->FSELheld = true;
                ThisFSEL->currentScreen = 0;
                ThisFSEL->FSELSelection = 0;
                ThisFSEL->yesnoselection = -1;
                return;
            }
 
        else if ((pressedKeys & B) && !ThisFSEL->FSELheld )      // B pressed. Going back to main next frame
            {
                PlaySNDEffect(SE_SYS_BACK, 0);
                ThisFSEL->currentmessage = 0;
                ThisFSEL->FSELSelection = 2;
                ThisFSEL->FSELheld = true;
                return;
            }
 
        else if ((pressedKeys & A) && !ThisFSEL->FSELheld)       // A is pressed, try to select that file for copying
            {
                ConfirmInCopy();
                return;
            }
 
        else if ((pressedKeys & LEFT && !ThisFSEL->FSELheld))   // Highlight file -1 out of the available
            {
                ThisFSEL->copyslot -= 1;
                if (ThisFSEL->copyslot < 0) ThisFSEL->copyslot = 2;
                if (ThisFSEL->copyslot == ThisFSEL->currentSlot) ThisFSEL->copyslot -= 1;
                if (ThisFSEL->copyslot < 0) ThisFSEL->copyslot = 2;
                 
                ArrangeBottomCopyScreen(ThisFSEL->currentSlot, ThisFSEL->copyslot);
                FSELPlusMinusInCopyMode(ThisFSEL->copyslot);
 
                ThisFSEL->FSELheld = true;
                return; 
            }
 
        else if ((pressedKeys & RIGHT && !ThisFSEL->FSELheld))  // Highlight file +1 out of the available
            {
                ThisFSEL->copyslot += 1;
                if (ThisFSEL->copyslot > 2) ThisFSEL->copyslot = 0;
                if (ThisFSEL->copyslot == ThisFSEL->currentSlot) ThisFSEL->copyslot += 1;
                if (ThisFSEL->copyslot > 2) ThisFSEL->copyslot = 0;
                 
                ArrangeBottomCopyScreen(ThisFSEL->currentSlot, ThisFSEL->copyslot);
                FSELPlusMinusInCopyMode(ThisFSEL->copyslot);
 
                ThisFSEL->FSELheld = true;
                return; 
            }
 
        else if (AREA_TOUCHED(8,4,88,88) && !ThisFSEL->FSELheld) // File 1 touched when choosing a file to overwrite
            {
                TouchedSlotInCopy(0);
            }
 
        else if (AREA_TOUCHED(84,58,88,88) && !ThisFSEL->FSELheld)       // File 2 touched when choosing a file to overwrite
            {
                TouchedSlotInCopy(1);           
            }
 
        else if (AREA_TOUCHED(160,4,88,88) && !ThisFSEL->FSELheld)   // File 3 touched when choosing a file to overwrite
            {
                TouchedSlotInCopy(2);
            }
    }
 
void FSELCleanup()
    {
        // This has to be done, because for whatever reason, a little garbled graphic
        // appears on the titlescreen if the screens aren't cleared before scene change
        FadeOut(3, 0, 1);
 
        MIi_CpuClear32(0, BG_GFX, 0x10000);
        MIi_CpuClear32(0, BG_GFX + 0x8000, 0x10000);
        MIi_CpuClear32(0, BG_GFX_SUB, 0x10000);
 
        setBrightness(3, 0);
        vramDefault();
 
        for (int i=0; i<12; i++)
            {
                freeToCurrentHeapIfNotNull(ThisFSEL->ENPGPointers[i]);
            }
 
        for (int i=0; i<3; i++)
            {
                freeToCurrentHeapIfNotNull(ThisFSEL->SaveFileInfos[i]);
            }
 
        freeToCurrentHeapIfNotNull(ThisFSEL);
        ThisFSEL = 0;
    }
 
void nsub_020D2F2C_ov_09()  //Hook in original FSEL code
    {
        FadeOut(3,0,2);
        causeSceneChange(8, 30);
    }