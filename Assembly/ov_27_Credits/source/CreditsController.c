#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "CreditsController.h"
#include "VariousFunctions.h"
#include "sfx.h"

extern bool IsSpecialLevel;
extern bool ControlLock;
extern bool TouchscrControlLock;
extern bool TitleScrNumber;
extern bool DisableMarioYRotationForce;
bool* MenuLocked = (bool*)0x020CA880;
u8* ActivePlayer = (u8*)0x2085A50;
u8* Coins = (u8*)0x208B37C;
u8* CoinsFrom10CoinBlock = (u8*)0x20CA338;
extern u8 lastslot;

u8  CreditsCharactersWidths[] = {
	3, 5, 7, 11, 11, 10, 10, 5, 7, 7, 7, 8, 4, 7, 5, 
	9, 9, 7, 9, 9, 10, 9, 10, 9, 9, 10, 5, 4, 10, 9, 10, 
	8, 7, 12, 11, 10, 12, 10, 9, 11, 11, 5, 11, 10, 10, 12, 12, 
	10, 10, 10, 10, 11, 12, 11, 10, 14, 13, 11, 10, 6, 10, 5, 9, 
	9, 4, 9, 10, 9, 10, 10, 8, 11, 10, 6, 9, 10, 5, 13, 11, 
	9, 11, 12, 9, 10, 8, 11, 10, 12, 10, 10, 10, 6, 4, 7, 7, 12,
	3, 5, 7, 10, 9, 12, 11, 5, 6, 6, 7, 8, 5, 7, 5, 7,
    9, 7, 9, 9, 9, 9, 9, 9, 9, 9, 5, 5, 8, 8, 8, 8,
    12, 11, 10, 10, 11, 9, 9, 11, 10, 5, 9, 10, 9, 13, 11, 11,
    10, 12, 9, 9, 9, 11, 12, 13, 11, 10, 9, 6, 6, 7, 5, 9,
    6, 9, 9, 8, 9, 9, 8, 9, 8, 5, 6, 8, 5, 11, 7, 9,
    8, 9, 7, 8, 7, 8, 9, 11, 9, 9, 7, 6, 4, 6, 7, 12};
	
u8  CoinFontWidths[] = { 17, 16, 19, 19, 20, 19, 21, 19, 20, 19, 21 };


int CreditsController_loadFiles()
	{
		return 1; 
	}
	
void CreditsController_setup(CreditsController* credits)
	{	
		if (!IsSpecialLevel)
			{
				Base__deleteIt(credits);
				return;
			}
	
		// Setting draw distance
		credits->DrawDist.x = credits->DrawDist.y = 0x80000;
		credits->position.z = 0x3A0000;

		// Player setup
		credits->ActivePlayer = (EnemyClassObject*)getPtrToMarioOrLuigi(0);
		credits->InactivePlayer = (EnemyClassObject*)getPtrToMarioOrLuigi(1);
		
		// Don't want the other player getting to any funny business
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;			
		Inactive->unk980 = (u32)&Player_InactiveState;

		// Set brightness to dark
		credits->ScreenBrightness = -16;
		setBrightness(3, credits->ScreenBrightness);
		
		// Set up bottom screen; clearing, copying NCG, copying NCL, copying NSCS, setting the position to offscreen.
		MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);		
		loadFileByExtId_LZ_Dest(2855-0x83, G2S_GetBG0CharPtr());  		//NCG
		u32* nclptr = (u32*)loadFileByExtId_LZ(2856-0x83);				//NCL	
		GXS_LoadBGPltt(nclptr, 0, 0x120);
		freeToCurrentHeapIfNotNull(nclptr);
		
		credits->BottomNSCPtr = (u32*) allocFromCurrentHeap(2048, 0);
		loadFileByExtId_LZ_Dest(2857-0x83, credits->BottomNSCPtr);		//NSC TOP
		loadFileByExtId_LZ_Dest(2858-0x83, G2S_GetBG3ScrPtr()); 		//NSC BTM
		REG_BG2VOFS_SUB = 128;											//BOTTOM LOGO POS
		
		SetTopBgTo(2859-0x83, 2860-0x83, 2861-0x83, 5);
		
		// Loading in the credits command file, and setup
		credits->CommandFilePtr = (u32*)loadFileByExtId(2848-0x83);
		credits->CurrentCommandPtr = (Command*)credits->CommandFilePtr;
		
		// Saving the game
		u8* saveslot = (u8*)(0x02088F28);
		
		u8 slot = *saveslot;
		saveData.header = 0x31303037;
		saveData.completion |= 0x20;
		saveSave(slot, &saveData);
		for (int i = 0; i<3; i++)
			{
				loadSave(i, &saveData);
				if (slot == i) continue;
				
				if (slot != i && saveData.inited) 
					{
						saveData.header=0x30303037;
						saveSave(*saveslot, &saveData); 
					}
			}
		loadSave(slot, &saveData);	
		lastslot = *saveslot;

		// Locking all control
		ControlLock = true; 
		TouchscrControlLock = true;
		*MenuLocked = true;

		// Other variables
		credits->LogoFade = 0;
		credits->LogoFall = 0;
		credits->HeaderFade = 0;
		credits->BodyFade = 0;
		credits->CoinFade = 0;
		credits->DarknessFade = 0;
		credits->TheEndBounceIn = 0;
		credits->BottomFallTimer = 64;
		credits->BottomFallTimer2 = 200;
		credits->MakingFireworks = false;
		credits->CollectedCoins = 0;		
		pressedKeys = 0;									
		*Coins = 0;
	}
	
void CreditsController_ExecuteCommand(CreditsController* credits)			// State 0
	{	
		// Not letting the player get too high up.
		s32 *camY = (s32*)(0x020CA2E4);		
		s32 MaxPosY = 0xFFFFFFFF - ((*camY)<<12) + 0x10000;
		
		if (credits->ActivePlayer->position.y > MaxPosY)
			credits->ActivePlayer->position.y = MaxPosY;	
	
		credits->CollectedCoins += *Coins;
		*Coins = 0;
		*CoinsFrom10CoinBlock = 0;
	
		// Moving the credits to player position just so that it always is drawn
		credits->position.x = credits->ActivePlayer->position.x;
		credits->position.y = credits->ActivePlayer->position.y;

		DoFireworks(credits);
		
		// Command engine
		switch (credits->CurrentCommandPtr->CommandID)					
			{
				case 1: Command_Delay(credits); break;
				case 2: Command_SwitchScene(credits); break;
				case 3: Command_FadeInLogo(credits); break;
				case 4: Command_DropLogo(credits); break;
				case 5: Command_FadeToBlack(credits); break;
				case 6: Command_FadeFromBlack(credits); break;
				case 7: Command_FadeToWhite(credits); break;
				case 8: Command_FadeFromWhite(credits); break;
				case 9: Command_ShowText(credits); break;
				case 10: Command_HideText(credits); break;
				case 11: Command_SetHeaderText(credits); break;
				case 12: Command_ShowHeaderText(credits); break;
				case 13: Command_HideHeaderText(credits); break;
				case 14: Command_SetBodyText(credits); break;
				case 15: Command_ShowBodyText(credits); break;
				case 16: Command_HideBodyText(credits); break;	
				case 17: Command_DisablePlayerControl(credits); break;	
				case 18: Command_EnablePlayerControl(credits); break;	
				case 19: Command_EnableLowGravityPhysics(credits); break;
				case 20: Command_DisableLowGravityPhysics(credits); break;
				case 21: Command_UnlockInactiveCharacter(credits); break;
				case 22: Command_SetPlayersFacingScreen(credits); break;
				case 23: Command_PlacePeach(credits); break;
				case 24: Command_PlayCharacterWinAnimations(credits); break;
				case 25: Command_BeginFireworks(credits); break;
				case 26: Command_EndFireworks(credits); break;
				case 27: Command_ShowDarknessOverlay(credits); break;
				case 28: Command_ShowTheEnd(credits); break;
				case 29: Command_HideTheEnd(credits); break;
				case 30: Command_ShowCoinCounter(credits); break;
				case 31: Command_HideCoinCounter(credits); break;				
				case 32: Command_LoadFile(credits); break;
				case 33: Command_UnloadFile(credits); break;
				case 34: Command_ExitStage(credits); break;
				default: break;
			}
	}

void Command_NextCommand(CreditsController* credits)
	{
		u8* CommandPtr = (u8*)credits->CurrentCommandPtr;
		CommandPtr += credits->CurrentCommandPtr->Length;		// Getting next command
		credits->CurrentCommandPtr = (Command*)CommandPtr;
	}
	
void Command_Delay(CreditsController* credits)
	{
		DelayCommand* Command = (DelayCommand*)credits->CurrentCommandPtr;
		if (!Command->Frames) 
			{
				Command_NextCommand(credits);
				return;
			}
		Command->Frames--;
	}
	
void Command_SwitchScene(CreditsController* credits)
	{
		SwitchSceneCommand* Command = (SwitchSceneCommand*)credits->CurrentCommandPtr;
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		int* EntranceDestArea = (int*)0x208B09C;
		int* CurrentAreaNum = (int*)0x2085A94;
		
		if (!Command->EndingScene)
			{
				// Swapping the sets based on the information in the command.
				SetTilesetToID(Command->TilesetID);
				SetTopBgToID(Command->BgIDTop);
				SetBottomBgToID(Command->BgIDBottom);
			}
		else
			{
				SetTilesetToID(Command->TilesetID);
				SetTopBgTo(2862 - 0x83, 2863 - 0x83, 2864 - 0x83, 0);
				SetBottomBgTo(2865 - 0x83, 2866 - 0x83, 2867 - 0x83, 0);
				loadFileByExtId_LZ_Dest(2868 - 0x83, *((u32**)0x20C7258)+0x2800);				// This reloads subnohara NCG.
			}
		
		// Going to entrance and area specified by the command; if area is blank, goes to current area.
		if (Command->Area) *EntranceDestArea = Command->Area;
		else *EntranceDestArea = *CurrentAreaNum;
		setEntranceIdForPlayer(Command->Entrance, 0);
		setEntranceIdForPlayer(Command->Entrance, 1);
		enterEntrance(credits->ActivePlayer, 5);

		// Destroying held item, if there is such.
		if (Active->HeldItem) 
			{
				Base__deleteIt(Active->HeldItem);
				Active->HeldItem = 0;
				Active->IsHoldingSomething = 0;
			}	
		
		Command_NextCommand(credits);	
	}
	
void Command_FadeInLogo(CreditsController* credits)
	{
		if (credits->LogoFade!=31) credits->LogoFade++;
		else Command_NextCommand(credits);
	}
	
void Command_DropLogo(CreditsController* credits)
	{
		credits->LogoFall += 0x3000;
		
		// Horrible
		if (credits->LogoFall > 0x12000)
			{
				if (credits->BottomFallTimer != 0) credits->BottomFallTimer--;
				if (credits->BottomFallTimer2 > 0) credits->BottomFallTimer2-=2;
			
				if ((credits->BottomFallTimer != 0) && (credits->BottomFallTimer % 2))
					{
						for(int i = 0; i < 32; i++) 
							{
								((u16*)G2S_GetBG2ScrPtr())[credits->BottomFallTimer/2 * 32 + i] = ((u16*)credits->BottomNSCPtr)[credits->BottomFallTimer/2 * 32 + i];	
							}
						
					}
				
				if (credits->BottomFallTimer2 > 0) REG_BG2VOFS_SUB = credits->BottomFallTimer2;
				else REG_BG2VOFS_SUB = 0;
			}
		
		if (credits->BottomFallTimer2 == 0) 
			{
				credits->LogoFade = 0;
				Command_NextCommand(credits);
			}
	}	
	
void Command_FadeToBlack(CreditsController* credits)
	{
		if (credits->ScreenBrightness > -17) setBrightness(3, credits->ScreenBrightness--);
		else Command_NextCommand(credits);
	}

void Command_FadeFromBlack(CreditsController* credits)
	{
		if (credits->ScreenBrightness <= 0) setBrightness(3, credits->ScreenBrightness++);
		else Command_NextCommand(credits);
	}	

void Command_FadeToWhite(CreditsController* credits)
	{
		if (credits->ScreenBrightness < 17) setBrightness(3, credits->ScreenBrightness++);
		else Command_NextCommand(credits);
	}	

void Command_FadeFromWhite(CreditsController* credits)
	{
		if (credits->ScreenBrightness >= 0) setBrightness(3, credits->ScreenBrightness--);
		else Command_NextCommand(credits);
	}	
	
void Command_ShowText(CreditsController* credits)
	{
		if (credits->HeaderFade != 31) credits->HeaderFade++;
		if (credits->BodyFade != 31) credits->BodyFade++;
		if ((credits->HeaderFade == 31) && (credits->BodyFade == 31)) Command_NextCommand(credits);
	}	
	
void Command_HideText(CreditsController* credits)
	{
		if (credits->HeaderFade != 0) credits->HeaderFade--;
		if (credits->BodyFade != 0) credits->BodyFade--;
		if ((credits->HeaderFade == 0) && (credits->BodyFade == 0)) Command_NextCommand(credits);
	}	
	
void Command_SetHeaderText(CreditsController* credits)
	{
		SetTextCommand* Command = (SetTextCommand*)credits->CurrentCommandPtr;
						
		MIi_CpuClear32(0, &credits->HeaderText, 256);									// Clear current message
		MI_CpuCopy8(&Command->Text, &credits->HeaderText, Command->textlen);			// Copy new message
		Command_NextCommand(credits);
	}
	
void Command_ShowHeaderText(CreditsController* credits)
	{
		if (credits->HeaderFade != 31) credits->HeaderFade++;
		else Command_NextCommand(credits);
	}	
	
void Command_HideHeaderText(CreditsController* credits)
	{
		if (credits->HeaderFade != 0) credits->HeaderFade--;
		else Command_NextCommand(credits);
	}	
	
void Command_SetBodyText(CreditsController* credits)
	{
		SetTextCommand* Command = (SetTextCommand*)credits->CurrentCommandPtr;
					
		MIi_CpuClear32(0, &credits->BodyText, 256);									// Clear current message
		MI_CpuCopy8(&Command->Text, &credits->BodyText, Command->textlen);			// Copy new message
		
		Command_NextCommand(credits);
	}
	
void Command_ShowBodyText(CreditsController* credits)
	{
		if (credits->BodyFade != 31) credits->BodyFade++;
		else Command_NextCommand(credits);
	}	
	
void Command_HideBodyText(CreditsController* credits)
	{
		if (credits->BodyFade != 0) credits->BodyFade--;
		else Command_NextCommand(credits);
	}
	
void Command_DisablePlayerControl(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;
		
		// Clearing keys and Player Status
		pressedKeys = 0;									
		Active->xVelocity = 0;
		Active->yVelocity = 0;
		Active->xAccel = 0;
		Active->yAccel = 0;
		Inactive->xVelocity = 0;
		Inactive->yVelocity = 0;
		Inactive->xAccel = 0;
		Inactive->yAccel = 0;

		ControlLock = true;
		Command_NextCommand(credits);
	}	
	
void Command_EnablePlayerControl(CreditsController* credits)
	{
		// Making sure nothing is pressed as we do this.
		pressedKeys = 0;															
		ControlLock = false;
		Command_NextCommand(credits);
	}
	
void Command_EnableLowGravityPhysics(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;
		
		Active->MoonPhysics = 1;
		Inactive->MoonPhysics = 1;
		
		Command_NextCommand(credits);
	}
	
void Command_DisableLowGravityPhysics(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;
		
		Active->MoonPhysics = 0;
		Inactive->MoonPhysics = 0;
		
		Command_NextCommand(credits);
	}
	
void Command_UnlockInactiveCharacter(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;
		
		Inactive->unk980 = Active->unk980;													// Setting Inactive's state back to what active's is currently.
		
		credits->InactivePlayer->position.x = credits->ActivePlayer->position.x - 0x40000;	// Placing him just a few tiles left of active.
		credits->InactivePlayer->position.y = credits->ActivePlayer->position.y;
		
		Command_NextCommand(credits);
	}
	
void Command_SetPlayersFacingScreen(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		MarioActor* Inactive = (MarioActor*)credits->InactivePlayer;
		
		DisableMarioYRotationForce = true;												// Game tries to update the Y Rot every frame. Prevent that.
		
		Active->RotY = 0;																// 0 = squarely at screen.
		Inactive->RotY = 0;
		
		Command_NextCommand(credits);
	}
	
void Command_PlacePeach(CreditsController* credits)
	{
		PlacePeachCommand* Command = (PlacePeachCommand*)credits->CurrentCommandPtr;

		loadFileByExtId_3dModel(1594 - 0x83, 0);										// Loading the model here, since we ALWAYS need it anyway.		
		loadFileByExtId_3dModel(1595 - 0x83, 0);										// Loading the animation.	
		
		Vec3 Position;
		if (!Command->PosX) Position.x = credits->ActivePlayer->position.x + 0x40000;	// If Peach X is blank, put her next to active player	
		if (!Command->PosY) Position.y = credits->ActivePlayer->position.y + 0x10000;	// Same for Y.
		Position.z = credits->ActivePlayer->position.z;
		
		createActor(0x136, 1, &Position, 0, 0, 0);										// Spawning the Peach actor.

		Command_NextCommand(credits);
	}
	
void Command_PlayCharacterWinAnimations(CreditsController* credits)
	{
		MarioActor* Active = (MarioActor*)credits->ActivePlayer;
		
		if (credits->CollectedCoins < 300)
			{
				Command_NextCommand(credits);
				return;
			}
		
		if ((!credits->AnimationTimer))				
			{
				PlaySNDEffect((*ActivePlayer) ? SE_VOC_LU_WIN : SE_VOC_MA_WIN, 0);		// If player is Luigi, play Luigi's sound. Else, Mario's.
				SetPlayerAnimation(credits->ActivePlayer, 0x63, 0, 0, 0x1000);			// Set Active Player's animation to the win one,
				Active->unk980 = (u32)&Player_WinningState;								// and set his state to one that'll update it.
				credits->AnimationTimer = 100;											// For 100 frames, anyway.
			}
		
		if (credits->AnimationTimer) credits->AnimationTimer--;							// Decrease the frame counter.					
			

		if (credits->AnimationTimer == 1)
			{
				Active->unk980 = (u32)0x2115AAC;										// Set state back to normal once we're done.
				credits->AnimationTimer = 0;
				Command_NextCommand(credits);
			}
	}
	
void Command_BeginFireworks(CreditsController* credits)
	{
		credits->MakingFireworks = true;
		Command_NextCommand(credits);
	}
	
void Command_EndFireworks(CreditsController* credits)
	{
		credits->MakingFireworks = false;
		Command_NextCommand(credits);
	}
	
	
void Command_ShowDarknessOverlay(CreditsController* credits)
	{
		if (credits->DarknessFade != 31) 
		{
			credits->DarknessFade++;										
			setBrightness(2, (-(credits->DarknessFade/2)));								// Setting bottom brightness alongside this.
		}
		
		else 
		{
			setBrightness(2, -16);
			MIi_CpuClear16(0, BG_GFX, 0x200000);										// Clearing BG, because otherwise the pixels seep through.
			MIi_CpuClear16(0, BG_GFX_SUB, 0x200000);									// Clearing SUBBG, because fade.
			
			Command_NextCommand(credits);	
		}
	}

void Command_ShowTheEnd(CreditsController* credits)
	{
		if (credits->TheEndBounceIn != 16) credits->TheEndBounceIn++;
		else Command_NextCommand(credits);
	}		
	
void Command_HideTheEnd(CreditsController* credits)
	{
		if (credits->TheEndBounceIn != 0) credits->TheEndBounceIn--;
		else Command_NextCommand(credits);
	}		
	
void Command_ShowCoinCounter(CreditsController* credits)
	{
		if (credits->CoinFade == 0) PlaySNDEffect(SE_OBJ_GET_COIN, 0);
		
		if (credits->CoinFade < 31) credits->CoinFade += 2;
		
		if (credits->CoinFade > 31)
		{
			credits->CoinFade = 31;
			Command_NextCommand(credits);
		}
	}		
	
void Command_HideCoinCounter(CreditsController* credits)
	{
		if (credits->CoinFade != 0) credits->CoinFade -= 2;
		
		if (credits->CoinFade <= 0)
		{
			credits->CoinFade = 0;
			Command_NextCommand(credits);
		}		
	}			
	
void Command_LoadFile(CreditsController* credits)
	{
		FileLoadOperationCommand* Command = (FileLoadOperationCommand*)credits->CurrentCommandPtr;
		if (Command->Slot > 6) Command->Slot = 6;
		loadFileByExtId_3dModel(Command->FileID - 0x83, 0);
		
		credits->ExtraFileIDs[Command->Slot] = Command->FileID - 0x83;

		u8* texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(Command->FileID - 0x83));
		
		switch (Command->Slot)
		{
			case 0:			// Logo. 
				{
					getTextureParams(texture, 0, &credits->TexLogo.texparam);
					getTexturePalBase(texture, 0, &credits->TexLogo.palbase);
					break;
				}
			case 1:			// Font header. 
				{
					getTextureParams(texture, 0, &credits->TexHeader.texparam);
					getTexturePalBase(texture, 0, &credits->TexHeader.palbase);
					break;
				}
			case 2:			// Font body. 
				{
					getTextureParams(texture, 0, &credits->TexBody.texparam);
					getTexturePalBase(texture, 0, &credits->TexBody.palbase);
					break;
				}
			case 3:			// The end. 
				{
					getTextureParams(texture, 0, &credits->TheEndText.texparam);
					getTexturePalBase(texture, 0, &credits->TheEndText.palbase);
					break;
				}
			case 4:			// Coin font
				{	
					getTextureParams(texture, 0, &credits->CoinTex.texparam);					
					getTexturePalBase(texture, 0, &credits->CoinTex.palbase);	
					break;
				}
			case 5:			// Darkness
				{	
					getTextureParams(texture, 0, &credits->DarkTex.texparam);
					getTexturePalBase(texture, 0, &credits->DarkTex.palbase);		
					break;
				}
			default: break;	
		}
		
		Command_NextCommand(credits);
	}	
	
void Command_UnloadFile(CreditsController* credits)
	{
		FileUnloadOperationCommand* Command = (FileUnloadOperationCommand*)credits->CurrentCommandPtr;
		if (Command->Slot > 3) Command->Slot = 3;
		
		unloadFileByExtId_cached(credits->ExtraFileIDs[Command->Slot]);
		
		credits->ExtraFileIDs[Command->Slot] = 0;
		Command_NextCommand(credits);
	}	
	
void Command_ExitStage(CreditsController* credits)
	{
		credits->CurrentCommandPtr->CommandID = -1;		
		causeSceneChange(4, 0);
	}		
	
void DoFireworks(CreditsController* credits)
    {
        if ((credits->MakingFireworks) && (!credits->FireworksTimer))
            {
                int fireworkstyle = RNG(6);
                
                switch (fireworkstyle)
                    {
                        case 0: fireworkstyle = 62; break;
                        case 1: fireworkstyle = 63; break;
                        case 2: fireworkstyle = 67; break;
                        case 3: fireworkstyle = 68; break;
                        case 4: fireworkstyle = 73; break;
                        case 5: fireworkstyle = 75; break;
                        default: fireworkstyle = 62; break;
                    }
                    
                Vec3 Position = credits->ActivePlayer->position;
                Position.x -= 0x68000;
                Position.y += 0x38000;

                int addX, addY;

                do {
                    addX = RNG(0xD0000);
                    addY = RNG(0x58000);
                } while ((0x50000 <= addX && addX <= 0x80000)                         // castle tower
                         || (addY <= 0x18000 && 0x10000 <= addX && addX <= 0xC0000)); // main part of castle

                Position.x += addX;
                Position.y += addY;
                
                SpawnParticle(fireworkstyle, &Position);
                PlaySNDEffect(SE_OBJ_GOAL_HANABI, &Position);
                credits->FireworksTimer = RNG(20) + 15;
				Rumble(1);
            }

        if (credits->FireworksTimer) credits->FireworksTimer--;
        
        return;
    }

void Player_InactiveState(EnemyClassObject* Player)
	{
		EnemyClassObject* Active = (EnemyClassObject*)getPtrToPlayerActor();
		Player->position.x = Active->position.x;
		Player->position.y = Active->position.y + 0x100000;
	}
	
void Player_WinningState(EnemyClassObject* Player)
	{
		UpdatePlayerAnimation(Player);
		return;
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

void CreditsController_draw(CreditsController* credits)
	{
        u32* CameraObject = (u32*)*((u32*)(0x20CAA38));
        s32 ScreenX = -(*(CameraObject + (0xC0 / 4))); 
        s32 ScreenY = -(*(CameraObject + (0xC4 / 4))) + 0xB0000;  
    
		if (credits->ExtraFileIDs[0] != 0) Logo_Draw(credits, ScreenX, ScreenY - 0x80000 - credits->LogoFall);
		
		if (credits->ExtraFileIDs[1] != 0)
			{
			// Draw the Header
			s32 DrawX = ScreenX + 0x8000;
			s32 DrawY = ScreenY - 0x8000;
			for (int i = 0; i < 256; i++)
				{
					// End of the text, cease printing
					if (credits->HeaderText[i] == 0x0) 			
						{
							break;
						}
						
					// We've reached a linebreak, so, return X to base and decrease Y position.
					if (credits->HeaderText[i] == 0xA) 			
						{
							DrawY -= 0x10000; 
							DrawX = ScreenX + 0x8000; 
							continue;
						}
					
					char ASCII = credits->HeaderText[i] - 32;
					Letter_Draw(credits, DrawX, DrawY, true, ASCII);
					DrawX += 0x1000 * CreditsCharactersWidths[(u8)ASCII];			// Increase draw position by the character width.
				}
			}
			
		if (credits->ExtraFileIDs[2] != 0)
			{
			// Draw the Body
			s32 DrawX = ScreenX + 0x8000;
			s32 DrawY = ScreenY - 0x8000;
			int LongestStringSize = 0;
			int CurrentStringSize = 0; 
			
			// Get each line's length, compare them, choose the longest one.
			for (int i = 0; i < 255; i++)											
				{
					char ASCII = credits->BodyText[i] - 32 + 96;

					CurrentStringSize += CreditsCharactersWidths[(u8)ASCII];
					if ((credits->BodyText[i] == 0xA) || (credits->BodyText[i] == 0x0))
						{
							if (CurrentStringSize > LongestStringSize)
								{
									LongestStringSize = CurrentStringSize;
								}
							CurrentStringSize = 0;
							if (credits->BodyText[i] == 0x0) break;
						}
				}
				
			// Start printing characters at right edge of screen - longest string lenght, so they're aligned.	
			DrawX = ScreenX + 0x100000 - (LongestStringSize * 0x1000) - 0x8000;				
			DrawY = ScreenY - 0x8000;		
			
			for (int i = 0; i < 255; i++)
				{
					// End of the text, cease printing
					if (credits->BodyText[i] == 0x0) 
						{
							break;
						}
					
					// We've reached a linebreak, so, return X to base and decrease Y position.
					if (credits->BodyText[i] == 0xA) 
						{
							DrawY -= 0x10000; 
							DrawX = ScreenX + 0x100000 - (LongestStringSize * 0x1000) - 0x8000;
							continue;
						}
					
					char ASCII = credits->BodyText[i] - 32;
					Letter_Draw(credits, DrawX, DrawY, false, ASCII);
					DrawX += 0x1000 * CreditsCharactersWidths[(u8)(ASCII + 96)];
				}
			}
			
			// Draw the end.
			if (credits->ExtraFileIDs[3] != 0) TheEnd_Draw(credits, ScreenX + 0x80000, ScreenY - 0x40000);
			
			// Draw the coin score.
			if (credits->ExtraFileIDs[4] != 0)
			{
				int CoinCounterLength = 0;
				
				int DigitHundreds = credits->CollectedCoins / 100;
				if (DigitHundreds) CoinCounterLength += CoinFontWidths[DigitHundreds];

				int DigitTens = (credits->CollectedCoins / 10) % 10;
				if ((DigitTens) || (DigitHundreds)) CoinCounterLength += CoinFontWidths[DigitHundreds];
		
				int DigitOnes = credits->CollectedCoins % 10;
				CoinCounterLength += CoinFontWidths[DigitOnes];
				
				// Centering.
				s32 DrawBeginX = ScreenX + 0x70000 - 0x800 * CoinCounterLength; 
				s32 DrawBeginY = ScreenY - 0x40000;
				
				CoinScore_Draw(credits, DrawBeginX, DrawBeginY, 10);
				DrawBeginX += (0x1000 * CoinFontWidths[10]);
				
				if (DigitHundreds)
					{
						CoinScore_Draw(credits, DrawBeginX, DrawBeginY, DigitHundreds);
						DrawBeginX += (0x1000 * CoinFontWidths[DigitHundreds]);
					}

				if ((DigitTens) || (DigitHundreds))
					{
						CoinScore_Draw(credits, DrawBeginX, DrawBeginY, DigitTens);
						DrawBeginX += (0x1000 * CoinFontWidths[DigitTens]);
					}
					
				CoinScore_Draw(credits, DrawBeginX, DrawBeginY, DigitOnes);
			}
			
			// Draw the darkness.
			if (credits->ExtraFileIDs[5] != 0) DarknessFade_Draw(credits, ScreenX, ScreenY - 0xB0000);

	}
	
void Letter_Draw(CreditsController* credits, s32 DrawX, s32 DrawY, bool IsHeader, char ASCII)
	{
		if ((IsHeader) && (!credits->HeaderFade)) return;
		if ((!IsHeader) && (!credits->BodyFade)) return;
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		REG_MTX_TRANSLATE = DrawX;
		REG_MTX_TRANSLATE = DrawY;
		REG_MTX_TRANSLATE = credits->position.z;	

		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0x1;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;
		
		if (IsHeader) 
		{
			REG_POLY_FORMAT = (0x3880 | (credits->HeaderFade << 16));
			REG_TEXT_FORMAT = credits->TexHeader.texparam;
			REG_PAL_BASE = credits->TexHeader.palbase;		
		}
		else
		{
			REG_POLY_FORMAT = (0x3880 | (credits->BodyFade << 16));
			REG_TEXT_FORMAT = credits->TexBody.texparam;
			REG_PAL_BASE = credits->TexBody.palbase;
		}

		REG_COLOR = 0x7FFF;
		
		int XAdd = 0x100 * (ASCII % 0x10);
		int YAdd = 0x100 * (ASCII / 0x10);
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x100 + XAdd, 0 + YAdd);
		vec3(0x100, 	0x100, 	0x0);
			
		texcoord2(0 + XAdd, 0 + YAdd);
		vec3(0x0000,	0x100, 	0x0);
			
		texcoord2(0 + XAdd, 0x100 + YAdd);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x100 + XAdd, 0x100 + YAdd);
		vec3(0x100,	0x0000,		0x0);

		REG_GFX_END = 0;	
	}
	
void Logo_Draw(CreditsController* credits, s32 DrawX, s32 DrawY)
	{
		if (!credits->LogoFade) return;
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		REG_MTX_TRANSLATE = DrawX;
		REG_MTX_TRANSLATE = DrawY;
		REG_MTX_TRANSLATE = credits->position.z;	
	
		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0x80000;
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (credits->LogoFade << 16));
		REG_TEXT_FORMAT = credits->TexLogo.texparam;
		REG_PAL_BASE = credits->TexLogo.palbase;
		
		REG_COLOR = 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x1000, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0x800);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x1000, 0x800);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;	
	}
	
void TheEnd_Draw(CreditsController* credits, s32 DrawX, s32 DrawY)
	{
		if (!credits->TheEndBounceIn) return;

		int scale = (832 - 36 * credits->TheEndBounceIn) * credits->TheEndBounceIn; // fixed-point 4.12
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		DrawX -= scale * 0x40; // for scale == 1.0, this == 0x40000
		DrawY -= scale * 0x10; // for scale == 1.0, this == 0x40000
		REG_MTX_TRANSLATE = DrawX;
		REG_MTX_TRANSLATE = DrawY;
		REG_MTX_TRANSLATE = credits->position.z;

		REG_MTX_SCALE = scale * 0x80; // for scale == 1.0, this = 0x80000
		REG_MTX_SCALE = scale * 0x20; // same as above, but 0x20000
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x1F3880;
		REG_TEXT_FORMAT = credits->TheEndText.texparam;
		REG_PAL_BASE = credits->TheEndText.palbase;
		
		REG_COLOR = 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x800, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0x200);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x800, 0x200);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;	
	}
	
void CoinScore_Draw(CreditsController* credits, s32 DrawX, s32 DrawY, int num)
	{
		if (!credits->CoinFade) return;
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		REG_MTX_TRANSLATE = DrawX;
		REG_MTX_TRANSLATE = DrawY;
		REG_MTX_TRANSLATE = credits->position.z;	

		REG_MTX_SCALE = 0x180000;
		REG_MTX_SCALE = 0x180000;
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (credits->CoinFade << 16));
		REG_TEXT_FORMAT = credits->CoinTex.texparam;
		REG_PAL_BASE = credits->CoinTex.palbase;		

		REG_COLOR = 0x7FFF;
		
		// Finding the coordinates of the character in the texture
		int XAdd = 0x180 * (num % 0x10);
		int YAdd = 0;
		
		if (num == 10)
		{
			XAdd = 0;
			YAdd = 0x180;
		}				
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x180 + XAdd, 0 + YAdd);
		vec3(0x100, 	0x100, 	0x0);
			
		texcoord2(0 + XAdd, 0 + YAdd);
		vec3(0x0000,	0x100, 	0x0);
			
		texcoord2(0 + XAdd, 0x180 + YAdd);
		vec3(0x0000, 	0x0000,	0x0);
			
		texcoord2(0x180 + XAdd, 0x180 + YAdd);
		vec3(0x100,		0x0000,	0x0);

		REG_GFX_END = 0;	
	}

void DarknessFade_Draw(CreditsController* credits, s32 DrawX, s32 DrawY)
	{
		if (!credits->DarknessFade) return;
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);
		
		REG_MTX_TRANSLATE = DrawX;
		REG_MTX_TRANSLATE = DrawY;
		REG_MTX_TRANSLATE = credits->position.z - 0x20000;	

		REG_MTX_SCALE = 0x1000000;
		REG_MTX_SCALE = 0x1000000;
		REG_MTX_SCALE = 0;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (credits->DarknessFade << 16));
		REG_TEXT_FORMAT = credits->DarkTex.texparam;
		REG_PAL_BASE = credits->DarkTex.palbase;		

		REG_COLOR = 0x7FFF;

		REG_GFX_BEGIN = 1;
		texcoord2(0x100, 0);
		vec3(0x100, 	0x100, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x100, 	0x0);
			
		texcoord2(0, 0x100);
		vec3(0x0000, 	0x0000,	0x0);
			
		texcoord2(0x100, 0x100);
		vec3(0x100,		0x0000,	0x0);

		REG_GFX_END = 0;	
	}
	
void* CreditsController_dtor(CreditsController* credits)
    {
        credits->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        credits->Unk4Vec2 = dtor2;
        credits->Unk3Vec2 = dtor2;
        credits->DeleteDist = dtor2;
        credits->DrawDist = dtor2;
        credits->ExistDist = dtor2;
        credits->unkv13 = dtor;
        credits->unkv12 = dtor;
        credits->unkv11 = dtor;
		
		freeToCurrentHeapIfNotNull(credits->BottomNSCPtr);
		freeToCurrentHeapIfNotNull(credits->CommandFilePtr);

        base_onDelete(credits);
	
        MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);	
		
		TitleScrNumber = 0;
		IsSpecialLevel = false;
		ControlLock = false;	
		TouchscrControlLock = false;
		*MenuLocked = false;
		DisableMarioYRotationForce = false;
		
        return credits;
    }

void* CreditsController_dtorFree(CreditsController* credits)
    {
        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        credits->vtable = (u32*)0x20C4F04;
        credits->Unk4Vec2 = dtor2;
        credits->Unk3Vec2 = dtor2;
        credits->DeleteDist = dtor2;
        credits->DrawDist = dtor2;
        credits->ExistDist = dtor2;
        credits->unkv13 = dtor;
        credits->unkv12 = dtor;
        credits->unkv11 = dtor;
		
		freeToCurrentHeapIfNotNull(credits->BottomNSCPtr);
		freeToCurrentHeapIfNotNull(credits->CommandFilePtr);

		base_onDelete(credits);
		freeToGameHeap(credits);
		
        MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);	
		
		TitleScrNumber = 0;
		IsSpecialLevel = false;
		ControlLock = false;	
		TouchscrControlLock = false;
		*MenuLocked = false;
		DisableMarioYRotationForce = false;
		
        return credits;
    }