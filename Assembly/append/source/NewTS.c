#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "NewTS.h"
#include "VariousFunctions.h"

extern u8 TitleScrNumber;
extern bool IsSpecialLevel;
extern bool ControlLock;
extern bool TouchscrControlLock;

int Titlescreen_loadFiles()
	{
		if (!IsSpecialLevel) return 1;
		
		int World = TitleScrNumber;
		if (TitleScrNumber > 8) World = TitleScrNumber - 10; 			// 10 is added during the save check when the secret music is supposed to trigger.
		
		int ModelFile = 3079 + World;					
		loadFileByExtId_3dModel(ModelFile - 0x83, 1);
				
		loadFileByExtId_3dModel(3088 - 0x83, 0);
		return 1; 
	}
	
void Titlescreen_setup(Titlescreen* tscr)
	{	
		if (!IsSpecialLevel)
			{
				Base__deleteIt(tscr);
				return;
			}
	
		int World = TitleScrNumber;
		bool secret = false;
		
		if (TitleScrNumber > 8)
			{	
				World = TitleScrNumber - 10; 
				secret = true;
			}

		// Graphics setup
		int ModelFile = 3079 + World;	
		u8* textureLogo = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(ModelFile - 0x83));
		u8* textureCopyright = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(3088 - 0x83));
		
		getTextureParams(textureLogo, 0, &tscr->TexLogo.texparam);
		getTexturePalBase(textureLogo, 0, &tscr->TexLogo.palbase);
		getTextureParams(textureCopyright, 0, &tscr->TexCopyright.texparam);
		getTexturePalBase(textureCopyright, 0, &tscr->TexCopyright.palbase);

		// Setting draw distance
		tscr->DrawDist.x = tscr->DrawDist.y = 0x80000;
		
		// Creating the Camera Controller and setting it ahead of the titlescreen
		tscr->CameraPtr = (EnemyClassObject*)createActor(0x146, 0, &tscr->position, 0, 0, 0);
		tscr->CameraPtr->DrawDist.x = 0x800;
		tscr->CameraPtr->DrawDist.y = 0x800;
		tscr->CameraPtr->ExistDist.x = 0x800;
		tscr->CameraPtr->ExistDist.y = 0x800;
		tscr->CameraPtr->position.x += 0x40000;

		// Setting the titlescreen's position relative to the camera		
		s32 *camX = (s32*)(0x02085BFC);	
		s32 ScreenX = *camX;
		
		tscr->position.x = ScreenX + 0xF0000;
		tscr->XScreenPos = *camX;
		
		tscr->CopyrightYPos = tscr->position.y -= 0x18000;
		tscr->position.y += 0x90000;
		if (World == 4) tscr->position.y += 0x10000;
		
		// Players setup; getting their pointers, setting them to the running animation, setting their direction
		// and rotation, then setting their position relative to camera (offscreen)
		tscr->MarioPtr = (EnemyClassObject*)getPtrToMarioOrLuigi(0);
		tscr->LuigiPtr = (EnemyClassObject*)getPtrToMarioOrLuigi(1);
		tscr->MarioPtr->direction = tscr->LuigiPtr->direction = 0;
		tscr->MarioPtr->rotation.y = tscr->LuigiPtr->rotation.y = 0x3000;
		tscr->MarioPtr->position.x = ScreenX + 0x9C000 + 0x20000;
		tscr->LuigiPtr->position.x = tscr->MarioPtr->position.x - 0x40000;

		// Misc setup
		tscr->DoneTimer = 0;	
		tscr->BoingTimer = -68;
		tscr->LandTimer = -21;
		tscr->CopyrightLogoFade = 0;
		setBrightness(3, -16);
		tscr->ScreenBrightness = -16;
		tscr->CurrentPressAFrame = 0;
		tscr->CurrentPressAForwards = true;
		pressedKeys = 0;
		tscr->state = 1;
		
		ControlLock = true;
		
		// Resetting volume
		int* volume = (int*)0x208FBF4;
		*volume = 127;
		
		// Replacing SFX
		SwapASsar(28, 2792 - 0x83);
		SwapASbnk(67, 2793 - 0x83);
		SwapASwar(74, 2794 - 0x83);
		

		// Secret music setup
		if (secret)
			{
				SwapASseq(0x1B, 2649);
				SwapASwar(0x15, 2650);
				SwapASbnk(0x14, 2651);				
				tscr->BoingTimer = -157;
				tscr->LandTimer = -110;
			}
			
		// Setup bottom screen; clearing, drawing base enpg, loading the ground enpg, drawing it.
        MIi_CpuClear32(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear32(0, BG_GFX_SUB, 0x10000);				
		bgInitNerDS(2, BgType_Bmp8, BgSize_B8_256x256, 4, 0, true);
		tscr->BottomENPGPtr = (u8*) allocFromCurrentHeap(66048, 0);
		loadFileByExtId_LZ_Dest(3089-0x83 + World, tscr->BottomENPGPtr);
		DrawEnpgToScreen(3089, 3, 0, true);	
		
		copyVRAMTile(tscr->BottomENPGPtr, (u16*)(BG_GFX_SUB + 0x8000), 16, 40, 0, 255, 240, 216);
		copyVRAMTile(tscr->BottomENPGPtr, (u16*)(BG_GFX_SUB + 0x8000), 240, 40, 16, 255, 0, 216);
	}

void Titlescreen_IdleState(Titlescreen* tscr)	// State 0
	{
		// This makes our characters move.
		MoveForwards(tscr);
		
		// This animates the "Press A!"
		PlayPressAAnimation(tscr);

		// This exits the titlescreen if A is pressed or the screen is touched.
		BottomScreenConfirmCode(tscr);

		// Move the titlescreen out if we're nearing the end of the level...
		if ((tscr->CameraPtr->position.x >= 0x01E20000) && (!tscr->DoneTimer)) tscr->state = 2;
	}
	
void Titlescreen_Start(Titlescreen* tscr)	// State 1
	{
		// We need the MarioActor struct for this
		MarioActor* Mario = (MarioActor*)tscr->MarioPtr;
		MarioActor* Luigi = (MarioActor*)tscr->LuigiPtr;
		
		// Start adding to the delay counters
		if (tscr->LandTimer < 0) tscr->LandTimer++; 
		tscr->BoingTimer++; 
		
		if (tscr->ScreenBrightness != 0) setBrightness(3, tscr->ScreenBrightness+=1);

		// If the jump isn't complete, but the delay has passed, set Mario and Luigi's velocities and max jumping velocities to 0x4000.
		if (tscr->LandTimer == 0) 
			{bgInitNerDS(2, BgType_Bmp8, BgSize_B8_256x256, 4, 0, true);
				if ((TitleScrNumber == 6) || (TitleScrNumber == 16))
					{
						pressedKeys |= A;
						Mario->xVelocity = Luigi->xVelocity = 0x1700;
						*((u32*)(((u32*)(Mario->unk6BC))+3)) = 0x1700;
						*((u32*)(((u32*)(Luigi->unk6BC))+3)) = 0x1700;
					}
				else
					{
						pressedKeys |= A;
						Mario->xVelocity = Luigi->xVelocity = 0x3800;
						*((u32*)(((u32*)(Mario->unk6BC))+3)) = 0x3800;
						*((u32*)(((u32*)(Luigi->unk6BC))+3)) = 0x3800;
					}
			}
			
		// If we've reached the running spot, set a delay before we start running so the landing animation plays properly
		if ((tscr->MarioPtr->position.x >= tscr->CameraPtr->position.x + 0x3A000) && (tscr->LandTimer == 0)) 
		{
			Rumble(1);
			tscr->LandTimer = 14;
		}

			
		// If the landing delay is set, null out velocity and keep decreasing the delay counter
		if (tscr->LandTimer > 1) 
			{
				Mario->xVelocity = Luigi->xVelocity = 0;
				tscr->LandTimer--;
			}
			
		// If the landing has ended, start moving.
		if (tscr->LandTimer == 1) 
			{
				MoveForwards(tscr);
			}
			
		// Do Boing if the Boing delay has been passed
		if (tscr->BoingTimer >= 0)
			{
				tscr->position.y = DoBoing(tscr, tscr->BoingTimer);
			}
			
		// Fade in the copyright logo. If we're done with that, move to Idle State.
		if (tscr->BoingTimer > 90)
			{
				if (tscr->CopyrightLogoFade!=31) tscr->CopyrightLogoFade++;
				else tscr->state = 0;
			}

		// Fixes a dumb bug when you go to TS from a Ghost House directly.
		copyVRAMTile((u16*)(u16*)BG_GFX_SUB, (u16*)BG_GFX_SUB, 128, 24, 0, 96, 0, 32);
	}
	
void Titlescreen_MoveOut(Titlescreen* tscr)		// State 2
	{
		MoveForwards(tscr);
		
		if (tscr->FrameCounter % 2) 			// Fade out
			{
				PlayFrameOfAnimation(tscr, tscr->CurrentPressAFrame);
				tscr->CurrentPressAFrame--;		
			}
		
		// Fade out the music
		int* volume = (int*)0x208FBF4;
		int newvolume = 0;
		if (*volume) newvolume = (*volume - 1);
		moveVolumeForPlayer(newvolume, 1);
		
		// Make the logo rise
		tscr->position.y += 0x3000;
		
		// Make the Copyright fade out
		if (tscr->CopyrightLogoFade!=0) tscr->CopyrightLogoFade--;

		// Reset the titlescreen.
		if ((tscr->CameraPtr->position.x >= 0x01F00000) && (!tscr->DoneTimer))
			{
				causeSceneChange(4,0);
				if (tscr->ScreenBrightness != -16) setBrightness(3, tscr->ScreenBrightness-=1);
			}
	}
	
void Titlescreen_AllStates(Titlescreen* tscr)
	{
		tscr->FrameCounter++;
	}
	
void MoveForwards(Titlescreen* tscr)
	{
		// Scroll Bottom Top BG. And no, this can't happen with bgSetScroll or anything; DS can't handle it.
		
		s32 *camX = (s32*)(0x02085BFC);
		s32 newpos = tscr->XScreenPos - *camX;

		if ((tscr->BoingTimer > 26) && newpos) 
			{
				tscr->XScreenPos = *camX;
				for (int j = 0; j < 40; j++)
					{
						u16 leftmost = ((u16*)BG_GFX_SUB)[j * 128];
						for (int i = 0; i < 127; i++)
							((u16*)BG_GFX_SUB)[j * 128 + i] = ((u16*)BG_GFX_SUB)[j * 128 + i + 1];
						((u16*)BG_GFX_SUB)[j * 128 + 127] = leftmost;
					}
			}		

		//Setting Mario and Luigi velocities, so they continue animating
		tscr->MarioPtr->unk3[0] = tscr->LuigiPtr->unk3[0] = 0x1100;
		
		// Moving the camera controller
		tscr->CameraPtr->position.x += 0x2000; 
		
		s32 ScreenX = *camX;
		// Setting positions of Mario/Luigi and the logo. To prevent glitches, the position is based on screen x, until it stops moving, where it
		// starts move based on CameraController position.
		if (!newpos)
		{
			tscr->MarioPtr->position.x = tscr->CameraPtr->position.x + 0x3A000;
			tscr->LuigiPtr->position.x = tscr->MarioPtr->position.x - 0x40000;
		}
		else
		{
			tscr->MarioPtr->position.x = ScreenX + 0x9C000;
			tscr->LuigiPtr->position.x = tscr->MarioPtr->position.x - 0x40000;
		}
		
		tscr->position.x = ScreenX;
		
	
	}

void PlayPressAAnimation(Titlescreen* tscr)
	{
		if (tscr->FrameCounter % 2)	// Every second frame
			{
				PlayFrameOfAnimation(tscr, tscr->CurrentPressAFrame);

				// Now determine the new frame number for next time.

				if (tscr->CurrentPressAFrame == 5 && tscr->CurrentPressAForwards)
					{
						// Just finished the fade in, so jump to the middle of the pulse animation
						// (because that's how the animation is supposed to work)
						tscr->CurrentPressAFrame = 11;
						return;
					}

				// Inc/decrement the frame number
				if (tscr->CurrentPressAForwards)
					tscr->CurrentPressAFrame++;
				else
					tscr->CurrentPressAFrame--;

				// Switch directions when we hit the beginning or end of the animation,
				// making sure to play the frames at the endpoints twice in a row
				if (tscr->CurrentPressAFrame == 18)
					{
						tscr->CurrentPressAFrame = 17;
						tscr->CurrentPressAForwards = false;
					}
				else if (tscr->CurrentPressAFrame == 5)
					{
						tscr->CurrentPressAFrame = 6;
						tscr->CurrentPressAForwards = true;
					}

				// If it's time to fade out and we're not yet doing so, do so
				if (tscr->DoneTimer && tscr->CurrentPressAFrame > 5)
					{
						tscr->CurrentPressAFrame = 5;
						tscr->CurrentPressAForwards = false;
					}
			}
	}

	
int titleLogoAnimY(int frame)
    {
        int t = 0;
		int endpos = 0x18000;
		
        // Initial fall-down
        int y = 0xB0000;
        int vel = -0x9000;
        while (y >= endpos)
            {
                if (t == frame) return y;
                t++;

                y += vel;
            }

        // Secondary bounce-up
        int g = -0x500;
        y = endpos;

        vel = 0x3800;
        while (y >= endpos)
            {
                if (t == frame) return y;
                t++;

                vel += g;
                y += vel;
            }

        // Tertiary bounce-up
        y = endpos;

        vel = 0x2000;
        while (y >= endpos)
            {
                if (t == frame) return y;
                t++;

                vel += g;
                y += vel;
            }

        return endpos;
    }
	
int DoBoing(Titlescreen* tscr, int frame)
    {
        return tscr->CopyrightYPos + titleLogoAnimY(frame);
    }
		
void BottomScreenConfirmCode(Titlescreen* tscr)
	{
		if ((IsTouched() || GBA_BUTTON_PRESSED(GBA_BUTTON_A)) && !tscr->DoneTimer)
			{
				PlaySNDEffect(SE_VOC_OPDM_YES, 0);
				tscr->DoneTimer = 1;
			}
		
		if (tscr->DoneTimer)
			{
				tscr->DoneTimer++;
			}
		
		if (tscr->DoneTimer == 30)
			{
				causeSceneChange(8,30);
			}
			
		if (tscr->DoneTimer > 30)
			{
				if (tscr->ScreenBrightness != -16) setBrightness(3, tscr->ScreenBrightness-=1);
			}
	}

void PlayFrameOfAnimation(Titlescreen* tscr, int frame)
	{
		// Fixes a dumb bug when you go to TS from a Ghost House directly.
		copyVRAMTile((u16*)(u16*)BG_GFX_SUB, (u16*)BG_GFX_SUB, 128, 24, 0, 96, 0, 32);
		
		int srcX = (frame < 9) ? 0 : 128;
		int srcY = (frame % 9) * 24;

		if (frame >= 0)
			copyVRAMTile(tscr->BottomENPGPtr, (u16*)BG_GFX_SUB, 128, 24, 64, 93, srcX, srcY);
		else
			copyVRAMTile((u16*)(u16*)BG_GFX_SUB, (u16*)BG_GFX_SUB, 128, 24, 64, 93, 64, 125);
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

void Titlescreen_draw(Titlescreen* tscr)
	{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = tscr->position.x;
		REG_MTX_TRANSLATE = tscr->position.y;
		REG_MTX_TRANSLATE = tscr->position.z;	
	
		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0xA0000;
		REG_MTX_SCALE = 0x10000;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = tscr->TexLogo.texparam;
		REG_PAL_BASE = tscr->TexLogo.palbase;
		
		REG_COLOR= 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x1000, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0xA00);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x1000, 0xA00);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;	
		
		if (tscr->CopyrightLogoFade == 0) return;
		
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = tscr->position.x;
		REG_MTX_TRANSLATE = tscr->CopyrightYPos;
		REG_MTX_TRANSLATE = tscr->position.z + 0x200000;	
	
		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0x10000;
		REG_MTX_SCALE = 0x10000;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (tscr->CopyrightLogoFade << 16));
		REG_TEXT_FORMAT = tscr->TexCopyright.texparam;
		REG_PAL_BASE = tscr->TexCopyright.palbase;
		
		REG_COLOR= 0x7FFF;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x1000, 0);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, 0);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, 0x100);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x1000, 0x100);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;
	}
	
void* Titlescreen_dtor(Titlescreen* tscr)
    {
        tscr->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        tscr->Unk4Vec2 = dtor2;
        tscr->Unk3Vec2 = dtor2;
        tscr->DeleteDist = dtor2;
        tscr->DrawDist = dtor2;
        tscr->ExistDist = dtor2;
        tscr->unkv13 = dtor;
        tscr->unkv12 = dtor;
        tscr->unkv11 = dtor;
		
		freeToCurrentHeapIfNotNull(tscr->BottomENPGPtr);
        base_onDelete(tscr);
	
        MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);	
		
		TitleScrNumber = 0;
		IsSpecialLevel = false;
		ControlLock = false;	
		TouchscrControlLock = false;

        return tscr;
    }

void* Titlescreen_dtorFree(Titlescreen* tscr)
    {
        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        tscr->vtable = (u32*)0x20C4F04;
        tscr->Unk4Vec2 = dtor2;
        tscr->Unk3Vec2 = dtor2;
        tscr->DeleteDist = dtor2;
        tscr->DrawDist = dtor2;
        tscr->ExistDist = dtor2;
        tscr->unkv13 = dtor;
        tscr->unkv12 = dtor;
        tscr->unkv11 = dtor;
		
		freeToCurrentHeapIfNotNull(tscr->BottomENPGPtr);
		base_onDelete(tscr);
		freeToGameHeap(tscr);
		
        MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);	
		
		TitleScrNumber = 0;
		IsSpecialLevel = false;
		ControlLock = false;	
		TouchscrControlLock = false;
		
        return tscr;
    }