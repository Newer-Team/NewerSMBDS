#include "game.h"
#include "Enemy.h"
#include "WorldSel.h"
#include "VariousFunctions.h"
#include "sfx.h"
#include "Scene8.h"

extern WSEL* ThisWSEL;

u8 Levels[8][12] = {
		{1,2,3,5,6,12,4,7,0,0,0,0},		// World 1
		{1,2,3,4,7,9,12,5,10,0,0,0},	// World 2
		{2,4,7,11,13,15,8,5,10,0,0,0},	// World 3
		{1,2,3,6,9,10,14,8,4,11,0,0},	// World 4
		{1,3,7,10,13,16,18,8,5,11,0,0}, // World 5
		{2,4,6,7,8,10,11,13,19,5,12,0},	// World 6
		{1,3,4,6,7,16,17,8,2,5,10,0},	// World 7
		{1,2,4,5,7,8,9,10,11,3,6,12},	// World 8
	};

const u8 FontWidths[10] = {18, 18, 20, 20, 22, 20, 22, 20, 22, 20};


void DrawCollectedStarCoinGraphicsAtXY(int x, int y, u8 bitmask)
	{
		if (bitmask & 1) copyVRAMTile(BG_GFX, BG_GFX, 16, 17, x, y, x, y - 128);
		if (bitmask & 2) copyVRAMTile(BG_GFX, BG_GFX, 18, 17, x + 16, y, x + 16, y - 128);
		if (bitmask & 4) copyVRAMTile(BG_GFX, BG_GFX, 16, 17, x + 34, y, x + 34, y - 128);
	}

void DrawCollectedStarCoins(int WorldID)
	{
		int DrawX = 61;
		int DrawY = 140;

		for (int i = 0; i < 12; i++)
			{
				if (Levels[WorldID][i] == 0) break;

				u8 StarCoinsGotten = ((*(saveData.starCoins + (WorldID * 0x19) + (Levels[WorldID][i]))) & 0x7);
				DrawCollectedStarCoinGraphicsAtXY(DrawX, DrawY, StarCoinsGotten);
				DrawY += 17;
				if (i == 5)
					{
						DrawX += 126;
						DrawY = 140;
					}
			}
	}

void DrawTotalStarCoinNumber()
	{
		int TotalCoins = saveData.gottenstarcoins;

		if (TotalCoins == 240)
			{
				copyVRAMTileWithAlpha(BG_GFX + 0x8000, BG_GFX + 0x8000, 122, 32, 42, 153, 80, 224);
				return;
			}

		int DigitHundreds = TotalCoins / 100;
		int DigitTens = (TotalCoins / 10) % 10;
		int DigitOnes = TotalCoins % 10;
		int START_X = 42, add_x = 0;

		if (DigitHundreds != 0)
		    {
		        copyVRAMTileWithAlpha(BG_GFX + 0x8000, BG_GFX + 0x8000, FontWidths[DigitHundreds], 32, START_X + add_x, 153,
		            DigitHundreds * 24, 192);
		        add_x += FontWidths[DigitHundreds];
		    }

		if (add_x > 0 || DigitTens != 0)
		    {
		        copyVRAMTileWithAlpha(BG_GFX + 0x8000, BG_GFX + 0x8000, FontWidths[DigitTens], 32, START_X + add_x, 153,
		            DigitTens * 24, 192);
		        add_x += FontWidths[DigitTens];
		    }

		// Last digit and "/240"
		copyVRAMTileWithAlpha(BG_GFX + 0x8000, BG_GFX + 0x8000, FontWidths[DigitOnes], 32, START_X + add_x, 153,
		    DigitOnes * 24, 192);
		copyVRAMTileWithAlpha(BG_GFX + 0x8000, BG_GFX + 0x8000, 72, 32,
		    START_X + add_x + FontWidths[DigitOnes] - 2, 153, 0, 224);
	}

void InitWSEL()
	{
		u8 *menuopen = (u8*)(0x020EE398);
		u8 *JrOnMap = (u8*)(0x02088B64);
		u8 *switching = (u8*)(0x020931FC);
		u8 *levelenter = (u8*)(0x020931D8);
		u8 *sceneparam = (u8*)(0x02085974);
		u8 *frozenmap = (u8*)(0x020EE384);
		
		*menuopen = 0;
		*JrOnMap = 0;
		*switching = 0;
		*levelenter = 0;
		*frozenmap = 0;
		*sceneparam = 0;
		
		vramDefault();
		SetupScreensForEnpgs();

		DrawEnpgToScreen(3058, 3, 1, false);
		DrawEnpgToScreen(3059+ThisWSEL->WorldID, 2, 0, false);
		DrawEnpgToScreen(3048+ThisWSEL->WorldID, 3, 0, true);
		
		bgSetScroll(2, 0, 128);
		bgUpdate();

		DrawCollectedStarCoins(ThisWSEL->WorldID);
		DrawTotalStarCoinNumber();

		PlayMusic(0x6E, 0);
		
		setBrightness(3,0);

		ThisWSEL->InitDone = true;
	}
	
void Cleanup()
	{
		// This has to be done, because for whatever reason, a little garbled graphic
		// appears on the titlescreen if the screens aren't cleared before scene change
		FadeOut(3, 0, 1);
					
		MIi_CpuClear16(0, BG_GFX, 0x10000);
		MIi_CpuClear16(0, BG_GFX + 0x8000, 0x10000);
		MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);
					
		setBrightness(3, 0);
	}

void ExecuteWSEL()
	{
		if (!ThisWSEL->InitDone) InitWSEL();
		
		if (ThisWSEL->opentimer < 35) 
			{
				ThisWSEL->opentimer++;
				return;
			}
			
		if (ThisWSEL->SelectionMade == 1) 
			{
				GoToWorld();
				return;
			}
		if (ThisWSEL->SelectionMade == 2) 
			{
				GoBack();
				return;
			}

		if (!(GBA_BUTTON_PRESSED(GBA_BUTTON_RIGHT)) && !(GBA_BUTTON_PRESSED(GBA_BUTTON_LEFT))
				&& !(GBA_BUTTON_PRESSED(GBA_BUTTON_A)) && !(GBA_BUTTON_PRESSED(GBA_BUTTON_B)))
			{
				ThisWSEL->held = false;
				copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 216, 160, 216, 192);
				copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 0, 160, 0, 192);
				
			}

		if (((GBA_BUTTON_PRESSED(GBA_BUTTON_A)) && !ThisWSEL->held))
			{
				ThisWSEL->SelectionMade = 1;
				PlaySNDEffect(SE_SYS_WORLD_CHG, 0);
				PlaySNDEffect(SE_SYS_DECIDE, 0);
			}

		if (((GBA_BUTTON_PRESSED(GBA_BUTTON_B)) && !ThisWSEL->held))
			{
				ThisWSEL->SelectionMade = 2;
				PlaySNDEffect(SE_SYS_BACK, 0);
			}

		if ((GBA_BUTTON_PRESSED(GBA_BUTTON_RIGHT)) || (GBA_BUTTON_PRESSED(GBA_BUTTON_R)))
			{
				if ((!(GBA_BUTTON_PRESSED(GBA_BUTTON_LEFT))) && (!(GBA_BUTTON_PRESSED(GBA_BUTTON_L))))
					{
						copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 216, 160, 216, 224);
						PlusMinus(1);
					}
			}

		if (GBA_BUTTON_PRESSED(GBA_BUTTON_LEFT) || (GBA_BUTTON_PRESSED(GBA_BUTTON_L)))
			{
				if ((!(GBA_BUTTON_PRESSED(GBA_BUTTON_RIGHT))) && (!(GBA_BUTTON_PRESSED(GBA_BUTTON_R))))
					{
						copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 0, 160, 0, 224);
						PlusMinus(-1);
					}
			}
	}

void GoToWorld()
	{
		Cleanup();
		causeSceneChange(9, ThisWSEL->WorldID);
	}
	
void GoBack()
	{
		Cleanup();
		*((u8*)(0x02088C04)) = ThisWSEL->node;
		causeSceneChange(9,*((int*)(0x02088BFC)));
	}

void PlusMinus(int mode)
	{
		ThisWSEL->held = true;

		// Switch to the next unlocked world in the
		// direction indicated by mode.
		// If the current world is the only available
		// one, don't bother doing the transition.
		int Originalnode = ThisWSEL->WorldID;
		do
			{
				ThisWSEL->WorldID = (ThisWSEL->WorldID + mode) & 7;
			}
		while (!(saveData.worlds[ThisWSEL->WorldID] & 0x40));
		if (Originalnode == ThisWSEL->WorldID) return;
		
		PlaySNDEffect(SE_SYS_CURSOR, 0);

		for (int i = 0; i <= 256; i += 64)
		    {
		        bgSetScroll(2, i * mode, 128);
		        setBrightness(2, -i / 16);
		        bgUpdate();
		        waitForVblankIrqless();
		    }

		CopyEnpgToScreen(3048+ThisWSEL->WorldID, 3, 0, true);
		CopyEnpgToScreen(3059+ThisWSEL->WorldID, 2, 0, false);
		DrawCollectedStarCoins(ThisWSEL->WorldID);
		
		if ((GBA_BUTTON_PRESSED(GBA_BUTTON_RIGHT)) || (GBA_BUTTON_PRESSED(GBA_BUTTON_R)))
			copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 216, 160, 216, 224);
		if (GBA_BUTTON_PRESSED(GBA_BUTTON_LEFT) || (GBA_BUTTON_PRESSED(GBA_BUTTON_L)))
			copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 40, 32, 0, 160, 0, 224);

		if (mode == 1) bgSetScroll(2, 256,  128);
		else           bgSetScroll(2, -256, 128);

		for (int i = 256; i >= 0; i -= 64)
		    {
		        bgSetScroll(2, i * -mode, 128);
		        setBrightness(2, -i / 16);
		        bgUpdate();
		        waitForVblankIrqless();
		    }
	}

