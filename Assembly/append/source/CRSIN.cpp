#include "game.h"
#include "CRSIN.h"
#include "VariousFunctions.h"
#include "Enemy.h"
#include "sfx.h"
#include <math.h>

CRSIN* ThisCRSIN = 0;

const int LAYER_TOP_AUX = 2;
const int LAYER_TOP_MAIN = 3;
const int LAYER_BTM_AUX = 6;
const int LAYER_BTM_MAIN = 7;

const int FADE_IN_DURATION = 15;
const int SCREEN_SLIDE_IN_DURATION = 40;
const int PLAYER_SELECTION_DURATION = 200;
const int PAUSE_AFTER_SLIDE_IN_DURATION = 10;
const int BANNER_FALL_DURATION = 30;
const int SELECT_PROMPT_BLINK_DURATION = 30;

const int TEETH_WIDTH = 32;
const int BANNER_HEIGHT = 64;
const int BANNER_OVERSHOOT = 8;
const int CHAR_WIDTHS[10] = {12, 12, 14, 14, 14, 14, 14, 14, 14, 14};

const u32 SLIDE_IN_LERP_TABLE[33] = {0, 305473868, 596227857, 872714849, 1135373792, 1384630133, 1620896225, 1844571736, 2056044038, 2255688584, 2443869276, 2620938819, 2787239068, 2943101359, 3088846834, 3224786755, 3351222807, 3468447393, 3576743921, 3676387078, 3767643099, 3850770031, 3926017979, 3993629354, 4053839108, 4106874965, 4152957643, 4192301069, 4225112588, 4251593166, 4271937584, 4286334633, 4294967295};

extern int TitleScrNumber;
extern bool IsSpecialLevel;


bool nsub_02152840_ov_36(EnemyClassObject* NSMB_CRSIN)  // Replaces CRSIN Execute.
    {

        if ((TitleScrNumber) || (IsSpecialLevel))       // Don't show anything in special levels.
            {
                CRSIN_Exit(NSMB_CRSIN);
                return 1;
            }

        if (!ThisCRSIN) InitCRSIN();                    // Initial setup

        if ((!(pressedKeys & A)) &&
            (!(pressedKeys & B)) &&
            (!(pressedKeys & SELECT)) &&
            (!(pressedKeys & RIGHT)) &&
            (!(pressedKeys & LEFT)) &&
            (!TouchDown)) ThisCRSIN->CRSINHeld = false; // Clearing hold


        switch (ThisCRSIN->State)               // Function calls based on state
            {
                case 0: CRSIN_FadeInWait(); break;
                case 1: CRSIN_Scroll(); break;
                case 2: CRSIN_Wait(); break;
                case 3: CRSIN_DoPlayerSelect(); break;
                default: CRSIN_Exit(NSMB_CRSIN); break;
            }
        return 1;
    }


void InitCRSIN()
    {       
        // Make our "actor"
        ThisCRSIN = (CRSIN*)allocFromCurrentHeap(sizeof(CRSIN), 0);

        // Set graphics stuff up
        REG_DISPCNT &= ~DISPLAY_SPR_ACTIVE;
        videoSetModeSub(MODE_5_2D);
        vramSetBankC(VRAM_C_SUB_BG_0x06200000);

        CRSIN_LoadImages();
        CRSIN_SetupImages();
        CRSIN_SetupLives();

        // Set up the initial state
        ThisCRSIN->State = 0;
        ThisCRSIN->StateJustChanged = true;
        ThisCRSIN->DoingBannerDrop = true;
        ThisCRSIN->FirstTimeInWaitState = true;
        
        NNSi_SndArcLoadGroup(0x9, *((u32**)(0x208FBB8)));
        
        SwapASwar(40, 2845-0x83);
        SwapASbnk(39, 2846-0x83);
        SwapASsar(29, 2847-0x83);
    }


void CRSIN_LoadImages()
    {
        // Addresses
        int *world = (int*)(0x02088BFC);
        int *level = (int*)(0x02085A9C);

        // Top-screen image comes from file 2127;
        // if it's 0, default to 1209.
        // Bottom screen image is calculated by adding a theme
        // offset which is also found in file 2127.

        u16* enpgIDs = (u16*)loadFileByExtId_LZ(2127-0x83);
        ThisCRSIN->topScreenImg = *(enpgIDs + ((*world) * 48) + (*level * 2));
        ThisCRSIN->subScreenImg = *(enpgIDs + ((*world) * 48) + (*level * 2) + 1);
        freeToCurrentHeapIfNotNull(enpgIDs);

        // Fallback if none.
        if (ThisCRSIN->topScreenImg == 0)
            {
                ThisCRSIN->topScreenImg = 2128;
                ThisCRSIN->subScreenImg = 2128;
            }
    }


void CRSIN_SetupImages()
    {
        // Draw the images to layers...
        DrawEnpgToScreen(ThisCRSIN->topScreenImg + 1, LAYER_BTM_AUX - 4, 1, false);
        DrawEnpgToScreen(ThisCRSIN->topScreenImg, LAYER_BTM_MAIN - 4, 0, false);
        DrawEnpgToScreen(ThisCRSIN->subScreenImg + 1, LAYER_TOP_AUX, 1, true);
        DrawEnpgToScreen(ThisCRSIN->subScreenImg, LAYER_TOP_MAIN, 0, true);

        // Set the layer priorities so that the aux layers are behind
        // the main layers
        bgSetPriority(LAYER_TOP_AUX, 3);
        bgSetPriority(LAYER_BTM_AUX, 3);

        // Put images offscreen
        bgSetScroll(LAYER_TOP_MAIN, -256, 0);
        bgSetScroll(LAYER_BTM_MAIN, -256, 0);
        bgSetScroll(LAYER_TOP_AUX, 0, 256);
        bgSetScroll(LAYER_BTM_AUX, 0, 256);
        bgUpdate();

        // Turn the brightness to 0%
        setBrightness(3, -16);
    }


void CRSIN_SetupLives()
    {
        int *lives = (int*)(0x0208B364);
        int *player = (int*)(0x02085A50);

        // Copy Mario or Luigi head, and cross
        int playerHead = (*player == 0) ? 0 : 40;
        copyVRAMTile(BG_GFX, BG_GFX, 36, 32, 10, 96, playerHead, 216);

        // Copy life count
        int digitTens = *lives / 10;
        int digitOnes = *lives % 10;
        if (digitTens > 0)
            {
                copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 16, 20, 46,                          100, (16 *  digitTens), 196);
                copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 16, 20, 46 + CHAR_WIDTHS[digitTens], 100, (16 *  digitOnes), 196);
            }
        else
            {
                copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 16, 20, 46, 100, (16 *  digitOnes), 196);
            }
    }


void CRSIN_LoadLevelAndChangeScene(EnemyClassObject* NSMB_CRSIN)
    {
        LoadLevelClearAndSomeOtherGraphics();
        LoadLevelStuffs(NSMB_CRSIN->spriteData);
		causeSceneChange(3, NSMB_CRSIN->base.stuff[2]);
    }
    
void CRSIN_FadeInWait()
    {
        ThisCRSIN->FadeInTimer++;
        if (ThisCRSIN->FadeInTimer == FADE_IN_DURATION)
            {
                PlaySNDEffect(SE_PLY_LAND, 0);
                ThisCRSIN->State = 1;
                ThisCRSIN->ScrollingIn = true;
                GXx_SetMasterBrightness_ ((vu16*)(0x4000054), (char)(0));
                GXx_SetMasterBrightness_ ((vu16*)(0x4001054), (char)(0));
                setBrightness(3, 0);
            }
    }

void CRSIN_Scroll()
    {
        if (ThisCRSIN->StateJustChanged)
            {
                // Set up the timer
                ThisCRSIN->Timer = SCREEN_SLIDE_IN_DURATION;

                if (ThisCRSIN->ScrollingIn)
                    {
                        // Copy the big dot and "Select"
                        copyVRAMTile(BG_GFX, BG_GFX, 8, 8, 20, 129, 136, 217);
                        copyVRAMTile(BG_GFX, BG_GFX, 34, 9, 30, 128, 146, 216);
                    }
                else
                    {
                        // Copy a right edge-tooth tile to cover up a checkerboard tile
                        // used in the banner overshoot slide-down animation
                        copyVRAMTile(BG_GFX + 0x8000, BG_GFX + 0x8000, 32, 32, 224, 160, 224, 128);
                    }

                ThisCRSIN->StateJustChanged = false;
            }

        // Run the animation
        int i;
        if (ThisCRSIN->ScrollingIn)
            i = SCREEN_SLIDE_IN_DURATION - ThisCRSIN->Timer;
        else
            i = ThisCRSIN->Timer;
        int lerpIdx = (i * 32) / SCREEN_SLIDE_IN_DURATION;
        int lerpFracNumerator = (i * 32) % SCREEN_SLIDE_IN_DURATION;
        int lerpFracDenominator = SCREEN_SLIDE_IN_DURATION;
        u32 lerpValue1 = SLIDE_IN_LERP_TABLE[lerpIdx];
        u32 lerpValue2 = SLIDE_IN_LERP_TABLE[lerpIdx + 1];
        s64 lerpValue = (s64)lerpValue1 + ((s64)(lerpValue2 - lerpValue1) * lerpFracNumerator) / lerpFracDenominator;
        int animPosition = ((256 + TEETH_WIDTH) * (0xFFFFFFFF - lerpValue)) / 0xFFFFFFFF;

        // Old version based on a sine wave:
        //int animPosition = ((256 + TEETH_WIDTH) * (4096 - sinLerp(i * 0x2000 / SCREEN_SLIDE_IN_DURATION))) / 4096;

        int edgePos, mainOffsetFromEdge;
        if (ThisCRSIN->ScrollingIn)
            {
                edgePos = -animPosition + TEETH_WIDTH;
                mainOffsetFromEdge = -TEETH_WIDTH;  
            }
        else
            {
                edgePos = animPosition - TEETH_WIDTH;
                mainOffsetFromEdge = TEETH_WIDTH;
            }
            


        // Top screen
        bgSetScroll(LAYER_TOP_AUX, edgePos, 0);
        bgSetScroll(LAYER_TOP_MAIN, edgePos + mainOffsetFromEdge, 0);

        // Bottom screen
        bgSetScroll(LAYER_BTM_AUX, -edgePos, 0);
        bgSetScroll(LAYER_BTM_MAIN, -edgePos - mainOffsetFromEdge, 0);

        // Push these changes to the screen
        bgUpdate();

        // Update timer
        if (pressedKeys & SRIGHT)
            ThisCRSIN->Timer -= 2;
        else
            ThisCRSIN->Timer -= 1;

        // Check for the end of this state
        if (ThisCRSIN->Timer <= 0)
            {
                int desiredMainLayersPos;
                if (ThisCRSIN->ScrollingIn)
                    desiredMainLayersPos = 0;
                else
                    desiredMainLayersPos = 256;
                // Ensure that the layers end up where they need to be
                bgSetScroll(LAYER_TOP_MAIN, 0, desiredMainLayersPos);
                bgSetScroll(LAYER_BTM_MAIN, 0, desiredMainLayersPos);
                bgSetScroll(LAYER_TOP_AUX, 0, 256);
                bgSetScroll(LAYER_BTM_AUX, 0, 256);

                // Move the top aux layer on top of the top main layer
                // and update
                bgSetPriority(LAYER_TOP_AUX, 0);
                bgUpdate();

                // Now switch to the Wait state or exit
                if (ThisCRSIN->ScrollingIn)
                    ThisCRSIN->State = 2;
                else
                    ThisCRSIN->State = 4;
                ThisCRSIN->StateJustChanged = true;
            }
    }


void CRSIN_UpdateBannerDrop()
    {
        // If the drop is done, exit immediately
        if (!ThisCRSIN->DoingBannerDrop)
            return;

        PlaySNDEffect2(SE_PLY_SHUFFLE, 0);
        int i = BANNER_FALL_DURATION - ThisCRSIN->BannerDropTimer;
        int height;
        if (i < BANNER_FALL_DURATION / 2)
            height = 256 - (sinLerp(i * 0x4000 / BANNER_FALL_DURATION) * (BANNER_HEIGHT + BANNER_OVERSHOOT)) / 4096;
        else
            height = 256 - BANNER_HEIGHT - (BANNER_OVERSHOOT * (4096 - cosLerp(i * 0x8000 / BANNER_FALL_DURATION))) / 8192;

        bgSetScroll(2, 0, height);
        bgUpdate();

        if (ThisCRSIN->BannerDropTimer <= 0)
            {   // Clean up
                bgSetScroll(2, 0, 256);

                // Copy the alternative banner from layer 2 to 1 (two halves)
                copyVRAMTile(BG_GFX + 0x8000, BG_GFX, 128, 64, 0, 0, 32, 0);
                copyVRAMTile(BG_GFX + 0x8000, BG_GFX, 128, 64, 128, 0, 32, 64);
                bgUpdate();

                // Move the secondary layer below the main one again
                bgSetPriority(2, 3);
                bgSetScroll(2, 32, 0);

                // No more banner drops.
                ThisCRSIN->DoingBannerDrop = false;
                return;
            }

        ThisCRSIN->BannerDropTimer -= 1;
    }


void CRSIN_Wait()
    {
        if (ThisCRSIN->StateJustChanged)
            {
                // Set up the timers and other stuff
                ThisCRSIN->Timer = PLAYER_SELECTION_DURATION;
                ThisCRSIN->SelectBlinkTimer = SELECT_PROMPT_BLINK_DURATION;
                ThisCRSIN->SelectBlinkIsBig = false;

                if (ThisCRSIN->FirstTimeInWaitState)
                    {
                        // Copy another top checkerboard tile on top of the left light-colored teeth
                        // for the banner overshoot slide-down animation
                        copyVRAMTile(BG_GFX + 0x8000, BG_GFX + 0x8000, 32, 32, 0, 160, 32, 160);
                        ThisCRSIN->BannerDropTimer = BANNER_FALL_DURATION;
                    }

                ThisCRSIN->StateJustChanged = false;
                ThisCRSIN->FirstTimeInWaitState = false;
            }

        // Make the banner-drop animation happen
        CRSIN_UpdateBannerDrop();

        // If select is pressed, switch to Selection state
        if (!ThisCRSIN->CRSINHeld && pressedKeys & SELECT)
            {
                ThisCRSIN->CRSINHeld = true;
                ThisCRSIN->State = 3;
                ThisCRSIN->StateJustChanged = true;
                PlaySNDEffect(SE_OBJ_PUFF_MUSH_4, 0);
                return;
            }

        // Make the "Select" prompt blink
        ThisCRSIN->SelectBlinkTimer--;
        if (ThisCRSIN->SelectBlinkTimer <= 0)
            {
                if (ThisCRSIN->SelectBlinkIsBig)
                    copyVRAMTile(BG_GFX, BG_GFX, 8, 8, 20, 129, 136, 217);
                else
                    {
                        copyVRAMTile(BG_GFX, BG_GFX, 8, 8, 20, 129, 128, 217);
                        PlaySNDEffect(SE_PLY_CANNON_BACK_SHOT, 0);
                    }
                ThisCRSIN->SelectBlinkIsBig = !ThisCRSIN->SelectBlinkIsBig;
                ThisCRSIN->SelectBlinkTimer = SELECT_PROMPT_BLINK_DURATION;
            }

        // Decrease timer
        if (pressedKeys & SRIGHT)
            ThisCRSIN->Timer -= 5;
        else
            ThisCRSIN->Timer -= 1;

        // If timer is done, switch to Scroll state.
        if (ThisCRSIN->Timer <= 0)
            {
                PlaySNDEffect(SE_PLY_GLIDING, 0);
                PlaySNDEffect(SE_PLY_LAND, 0);
                ThisCRSIN->State = 1;
                ThisCRSIN->ScrollingIn = false;
                ThisCRSIN->StateJustChanged = true;
            }
    }


int CRSIN_getXForRightArrow()
    {
        // Get the X-coordinate where the right select arrow should be placed.

        int *lives = (int*)(0x0208B364);

        int digitTens = *lives / 10;
        int digitOnes = *lives % 10;

        int x = 46 + CHAR_WIDTHS[digitOnes];
        if (digitTens > 0)
            x += CHAR_WIDTHS[digitTens];

        return x;
    }


void CRSIN_DoPlayerSelect()
    {
        // The banner-drop animation should continue to happen in this state
        CRSIN_UpdateBannerDrop();

        if (ThisCRSIN->StateJustChanged)
            {
                // Update graphics
                copyVRAMTile(BG_GFX, BG_GFX, 64, 9, 8, 128, 8, 160);    // clear the "Ⓐ Confirm" area
                copyVRAMTile(BG_GFX, BG_GFX, 10, 8, 14, 129, 134, 233); // Ⓐ
                copyVRAMTile(BG_GFX, BG_GFX, 44, 9, 24, 128, 144, 232); // "Confirm"
                copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 8, 20,
                    CRSIN_getXForRightArrow(), 100, 80, 220);           // ▷
                copyVRAMTile(BG_GFX, BG_GFX, 10, 20, 0, 100, 88, 220);  // ◁

                ThisCRSIN->StateJustChanged = false;
            }

        // We'll set this if we find we should exit the state.
        bool exitState = false;

        if (!ThisCRSIN->CRSINHeld)
            {
                // Here, we can check for new button presses.

                if ((pressedKeys & LEFT) || (pressedKeys & RIGHT))
                    {
                        PlaySNDEffect(SE_SYS_CURSOR, 0);
                        // Left or right was just pressed.
                        ThisCRSIN->CRSINHeld = true;

                        // Replace the corresponding arrow with a "pressed" copy
                        if (pressedKeys & LEFT)
                            copyVRAMTile(BG_GFX, BG_GFX, 10, 20, 0, 100, 112, 220);
                        else
                            copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 8, 20,
                                CRSIN_getXForRightArrow(), 100, 104, 220);
                        ThisCRSIN->SelectionArrowPressed = true;

                        // Swap player IDs
                        u8 *playerid = (u8*)(0x0208FBC4);
                        u8 *playersnd = (u8*)(0x02085A50);
                        u8 *playermdl = (u8*)(0x0208B330);
                        *playerid = *playersnd = *playermdl = 1 - *playerid;

                        // Update the player icon
                        int playerHead = (*playerid == 0) ? 0 : 40;
                        copyVRAMTile(BG_GFX, BG_GFX, 36, 32, 10, 96, playerHead, 216);
                    }
                else
                    {
                        if (ThisCRSIN->SelectionArrowPressed)
                            {
                                // Put the normal arrows back
                                copyVRAMTileWithAlpha(BG_GFX, BG_GFX, 8, 20,
                                    CRSIN_getXForRightArrow(), 100, 80, 220);           // ▷
                                copyVRAMTile(BG_GFX, BG_GFX, 10, 20, 0, 100, 88, 220);  // ◁
                                ThisCRSIN->SelectionArrowPressed = false;
                            }
                    }

                if (pressedKeys & A)
                    {
                        u8 *playerid = (u8*)(0x0208FBC4);
                        
                        PlaySNDEffect(SE_SYS_DECIDE, 0);
                        
                        if (*playerid == 0)
                            PlaySNDEffect(SE_OBJ_PUFF_MUSH_LAST, 0);
                        else
                            PlaySNDEffect(SE_OBJ_GET_DRAGON_COIN, 0);
                        
                        exitState = true;
                    }
            }

        if (exitState)
            {   // Exit the state.

                ThisCRSIN->CRSINHeld = true;

                // Clear the area and copy the big dot and "Select" back in
                copyVRAMTile(BG_GFX, BG_GFX, 64, 9, 8, 128, 8, 160);
                copyVRAMTile(BG_GFX, BG_GFX, 8, 8, 20, 129, 128, 217);
                copyVRAMTile(BG_GFX, BG_GFX, 34, 9, 30, 128, 146, 216);

                // Clear the arrows away
                copyVRAMTile(BG_GFX, BG_GFX, 8, 20,
                    CRSIN_getXForRightArrow(), 100, CRSIN_getXForRightArrow(), 164); // ▷
                copyVRAMTile(BG_GFX, BG_GFX, 10, 20, 0, 100, 0, 164);                // ◁

                // Go back to the Wait state.
                ThisCRSIN->State = 2;
                ThisCRSIN->StateJustChanged = true;
            }
    }


void CRSIN_Exit(void* NSMB_CRSIN)
    {
        // If CRSIN actor exists, destroy it.
        if (ThisCRSIN) CRSIN_dtor();
        vramDefault();
        CRSIN_LoadLevelAndChangeScene((EnemyClassObject*)NSMB_CRSIN);
    }


void CRSIN_dtor()
    {
        freeToCurrentHeapIfNotNull(ThisCRSIN);
        ThisCRSIN = NULL;
    }


// Nuking original CRSIN functions:

void repl_0215F128_ov_36()                              // Broke Luigi Clear screen
    {
        return;
    }

void repl_0215F11C_ov_36()                              // Broke Luigi Clear screen
    {
        return;
    }

void nsub_02152884_ov_36(EnemyClassObject* NSMB_CRSIN)  // CRSIN Draw
    {
        return;
    }

void nsub_02152EC4_ov_36()                              // Level Load
    {
        return;
    }