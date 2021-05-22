#include "game.h"
#include "BootDisplay.h"
#include "VariousFunctions.h"

bool displayedboot = false;
extern u8 RumblePakType;
extern int RumbleForce;

u8 BOOT_SCREEN_HASH[] = {0x3E, 0x1D, 0x02, 0xD4, 0x06, 0x9B, 0xEC, 0xD8, 0x90, 0x24, 0x6E, 0x80, 0x80, 0x3B, 0xD8, 0xFA, 0x38, 0x0D, 0x2A, 0xC1};

#pragma GCC optimize 0
s64 CrashHachiHachi()
    {
        int Whoops = 0, Nintendo = 40;
        u32 Didnt = 0;
        u32 Support = 305473868;
        s64 This = (s64)Didnt + ((s64)(Support - Didnt) * Whoops) / Nintendo;
        return This;
    }
#pragma GCC reset_options

void BootScreenCredit()
	{	
		//CrashHachiHachi();
		SetLastSaveSlot();

		for (s8 brightness = 0; brightness >= -32; brightness--) 
			{
				setBrightness(3, brightness/2);
				waitForVblankIrqless();
			}

		vramDefault();
		SetupScreensForEnpgs();
		
		if (!(!CheckSaveForBlankness() && ((GBA_BUTTON_PRESSED(GBA_BUTTON_A)) || IsTouched())))
			{
				DrawEnpgToScreen(2088, 3, 0, false);
				DrawEnpgToScreen(2089, 3, 0, true);
				
				FadeIn(3, -16, 1);
				if (!CheckSaveForBlankness())
					{
						for (int timer = 0; timer < 181; timer++) 
							{
								if (((GBA_BUTTON_PRESSED(GBA_BUTTON_A)) || IsTouched()) && timer > 5) 
									break;
								else
									waitForVblankIrqless();
							}
					}
				else
					{
						
						for (int timer = 0; timer < 121; timer++) 
							{
								waitForVblankIrqless();
							}
						int timer = 0;
						while (!(GBA_BUTTON_PRESSED(GBA_BUTTON_A)) && !IsTouched())	
							{
								if (timer == 0)
									{
										copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 32, 24, 213, 168, 0, 192);
									}
								if (timer == 40)
									{
										copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 32, 24, 213, 168, 32, 192);
									}
								if (timer == 80)
									{
										timer = -1;
									}
								timer++;
								waitForVblankIrqless();
							}
					}
				FadeOut(3, 0, 1);
			}

		RumblePakType = CheckForRumblePak();

		if (RumblePakType != 0) 
		{
			DrawEnpgToScreen(2091, 2, 0, false);
			DrawEnpgToScreen(2090, 2, 0, true);	

			if (RumblePakType == 1) 
				{
					copyVRAMTile(BG_GFX_SUB, BG_GFX_SUB, 256, 64, 0, 0, 0, 192);
				}

			FadeIn(3, -16, 1);
			
			for (int i = 0; i < 800; i++)
				{
					if (GBA_BUTTON_PRESSED(GBA_BUTTON_A))
						{
							if (CheckIfDesmume()) 
								{
									RumbleForce = 2;
									RUMBLE_PAK = 2;
								}
							else 
								{
									RumbleForce = 1;
									
									if (RumblePakType == 1) 
										{
											WARIOWARE_PAK = 8;
											WARIOWARE_PAK = 0;
										}
									else if (RumblePakType == 2) 
										{
											RUMBLE_PAK = 2;
											RUMBLE_PAK = 0;
										}
								}
							break;
						}

					else if (GBA_BUTTON_PRESSED(GBA_BUTTON_B))
						{
							RumbleForce = 0;
							break;
						}
					
					else waitForVblankIrqless();
				}
				
			FadeOut(3, 0, 1);
			
			if (CheckIfDesmume())
				RUMBLE_PAK = 0;
		}	
			
		displayedboot = true;
	}
