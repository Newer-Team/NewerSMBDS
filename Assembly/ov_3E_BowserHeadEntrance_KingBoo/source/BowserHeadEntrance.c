#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "BowserHeadEntrance.h"
#include "VariousFunctions.h"

extern u32 bmgFlagSomething;
extern u32 isTimeStopped;
extern bool ControlLock;

int* volume = (int*)0x208FBF4;
const int MODEL_1 = 2696 - 0x83;
const int MODEL_2 =  2697 - 0x83;
const int MODEL_3 =  2698 - 0x83;
const int PWANGSWAR = 2841 - 0x83;
const int PWANGSSAR = 2840 - 0x83;
const int PWANGSBNK = 2839 - 0x83;


u32 BowserHeadEntrance_physics[] = 										
	{
		0x78000,		
		0x70000,	
		0x8000,	
		0x40000,	
		0x00D30004, 
		0x0400,
		(u32)&BowserHeadEntrance_Collision,
	};		

int BowserHeadEntrance_loadFiles()
	{
		loadFileByExtId_3dModel(MODEL_1, 0);
		loadFileByExtId_3dModel(MODEL_2, 0);
		loadFileByExtId_3dModel(MODEL_3, 0);
		return 1;
	}
	
void BowserHeadEntrance_setup(BowserHeadEntrance* bows)
	{	
		u8* Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(MODEL_1));
		getTextureParams(Texture, 0, &bows->Tex1.texparam);
		getTexturePalBase(Texture, 0, &bows->Tex1.palbase);
		
		Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(MODEL_2));
		getTextureParams(Texture, 0, &bows->Tex2.texparam);
		getTexturePalBase(Texture, 0, &bows->Tex2.palbase);

		Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(MODEL_3));
		getTextureParams(Texture, 0, &bows->Tex3.texparam);
		getTexturePalBase(Texture, 0, &bows->Tex3.palbase);		
		
		bows->DestEntrance = ((bows->spriteData >> 0) & 0xF);
		bows->DestArea = ((bows->spriteData >> 4) & 0xF);
		bows->AnimationTimer = 0;
		
		bows->position.x -= 0x30000;
		bows->position.y -= 0x40000;
		bows->position.z = 0x220000;

		initActivePhysics(&bows->activephysics, bows, BowserHeadEntrance_physics, 0);
		registerCollider(&bows->activephysics);
		
		SwapASwar(54, PWANGSWAR);
		SwapASbnk(48, PWANGSBNK);
		SwapASsar(14, PWANGSSAR);
		
		*volume = 127;
		bows->ToDraw = 0;
		bows->EyeOpacity = 0;
		bows->CrossOpacity = 22;
		bows->CrossScale = 0x100000;
		
		Vec2 Distances;
		Distances.x = 0x800;
		Distances.y = 0x800;
				
		bows->DrawDist = Distances;

	}

void BowserHeadEntrance_IdleState(BowserHeadEntrance* bows)
	{
		return;
	}
	
void BowserHeadEntrance_StateEntered(BowserHeadEntrance* bows)
	{
		bows->AnimationTimer++;
		
		// Lock menu and timer.
		if (bows->AnimationTimer == 1)
			{
				bmgFlagSomething |= 0x1;
				isTimeStopped |= 0x10;			
			}
		
		// Play sound effect to start off
		if (bows->AnimationTimer == 20) PlaySNDEffect(139, 0);
		
		// Twitch a bit as if it has trouble moving...
		if (bows->AnimationTimer > 20 && bows->AnimationTimer < 25)
			bows->rotation.z -= 0x40;
			
		if (bows->AnimationTimer > 25 && bows->AnimationTimer < 30)
			bows->rotation.z += 0x40;
		
		// Slowly move the jaw while playing sound effects and muting music
		if (bows->AnimationTimer > 65 && bows->AnimationTimer < 280)
		{
			int newvolume = 0;
			if (*volume) newvolume = (*volume - 1);
			moveVolumeForPlayer(newvolume, 1);
		
			PlaySNDEffect2(137, 0);
			bows->rotation.z -= 0x8;
		}
		
		// Play a sound effect to end off.
		if (bows->AnimationTimer == 280) 	
			PlaySNDEffect(138, 0);
		
		// Make the eye start glowing
		if (bows->AnimationTimer == 290)
		{
			bows->ToDraw = 1;
		}

		// Begin PWANG sound
		if (bows->AnimationTimer == 315)
		{
			PlaySNDEffect(130, 0);
		}
		
		// Increase glow level...
		if (bows->AnimationTimer > 320 && bows->AnimationTimer < 360)
		{	
			bows->EyeOpacity = ((bows->AnimationTimer - 320) * 8) / 10;
		}
		
		// Flash the eye
		if (bows->AnimationTimer == 360)
		{
			bows->ToDraw = 14;
			bows->rotation.x = -0x400; // the shine should start at a slight clockwise angle
		}
		
		// Rotate the effects and dim the brightest eye texture
		if (bows->AnimationTimer > 360 && bows->AnimationTimer <= 440)
		{	
			bows->EyeOpacity = 31;
			bows->rotation.x += 0x60;

			int thisEffectTime = bows->AnimationTimer - 360;
			bows->CrossOpacity = 22 - (thisEffectTime * 22) / 80;
			bows->CrossScale += 0x2000;
			if (bows->CrossOpacity < 0) bows->CrossOpacity = 0;
		}

		if (bows->AnimationTimer > 440)
		{
			bows->CrossOpacity = 0;
			bows->EyeOpacity = 31 - (bows->AnimationTimer - 440) / 2;
			if (bows->EyeOpacity < 0) bows->EyeOpacity = 0;
		}

		// Go to level specified by settings.
		if (bows->AnimationTimer == 578) 	
		{
			int* EntranceDestArea = (int*)0x208B09C;
			int* CurrentAreaNum = (int*)0x2085A94;
			
			*EntranceDestArea = bows->DestArea + *CurrentAreaNum;
			setEntranceIdForPlayer(bows->DestEntrance, 0);
			enterEntrance(getPtrToPlayerActor(), 5);
		}			
	}
	
void BowserHeadEntrance_Collision(ActivePhysics* a, ActivePhysics* b)
	{
		BowserHeadEntrance* bows = (BowserHeadEntrance*)a->owner;
		MarioActor* Mario = (MarioActor*)b->owner;
		
		if (Mario->ClassID != 0x15) return;
		
		Mario->xAccel0 = 0;
		Mario->yAccel0 = 0;
		ControlLock = true;
		pressedKeys = 0;
		
		bows->state = 1;
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

void BowserHeadEntrance_draw(BowserHeadEntrance* bows)
	{
		// Main head
		Vec3 Position = bows->position;
		Vec3 Scale = {0, 0x100000, 0x100000, 1};
		Vec2 TR = {0, 0xC00, 0};
		Vec2 TL = {0, 0, 0};
		Vec2 BL = {0, 0, 0xC00};
		Vec2 BR = {0, 0xC00, 0xC00};
		Vec3 RotTrans = {0, 0, 0, 0};
		
		BowserHeadEntrance_DrawAPolygon(bows->Tex1, Position, Scale, TR, TL, BL, BR, RotTrans, 0, 31);					
		
		// Jaw
		Position.x += 0x2A000;
		Position.z += 0x2000;
		
		Scale.x = 0x80000;
		Scale.y = 0x970A3;
		
		TR.x = BR.y = BL.y = BR.x = 0x1000;
		TR.y = TL.x = TL.y = BL.x = 0xA00;
		
		RotTrans.x = 0x5EAD7;
		RotTrans.y = 0x89CA6;
		
		BowserHeadEntrance_DrawAPolygon(bows->Tex1, Position, Scale, TR, TL, BL, BR, RotTrans, bows->rotation.z, 31);
		
		
		// Eye frame 1
		if (bows->ToDraw & 1)
		{
			Position = bows->position;
			Position.y += 0x80000;
			Position.z += 0x2000;
				
			Scale.x = 0xD5558;
			Scale.y = 0x547AE;
			
			TR.x = BR.x = 0xA00;
			TL.x = BL.x = 0;
			TR.y = TL.y = 0xC00;
			BR.y = BL.y = 0x1000;
				
			RotTrans.x = 0;
			RotTrans.y = 0;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex1, Position, Scale, TR, TL, BL, BR, RotTrans, 0, bows->EyeOpacity);
		}
		
		// Eye frame 2
		if (bows->ToDraw & 2)
		{
			Position = bows->position;
			Position.y += 0x80000;
			Position.z += 0x2000;
			
			Scale.x = 0xAAAAC;
			Scale.y = 0x80000;
				
			TR.x = BR.x = 0x800;
			TL.x = BL.x = 0;
			TR.y = TL.y = 0;
			BR.y = BL.y = 0x600;
				
			RotTrans.x = 0;
			RotTrans.y = 0;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex2, Position, Scale, TR, TL, BL, BR, RotTrans, 0, bows->EyeOpacity);

			Position = bows->position;
			Position.x += 0xAAAAA;
			Position.y += 0x80000;
			Position.z += 0x2000;
			
			Scale.x = 0x2AAAC;
			Scale.y = 0x80000;
				
			TR.x = BR.x = 0xC00;
			TL.x = BL.x = 0xA00;
			TR.y = TL.y = 0;
			BR.y = BL.y = 0x600;
				
			RotTrans.x = 0;
			RotTrans.y = 0;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex1, Position, Scale, TR, TL, BL, BR, RotTrans, 0, bows->EyeOpacity);
		}
		
		// Eye frame 3
		if (bows->ToDraw & 4)
		{
			Position = bows->position;
			Position.y += 0x80000;
			Position.z += 0x1000;
			
			Scale.x = 0xAAAAC;
			Scale.y = 0x80000;
				
			TR.x = BR.x = 0x800;
			TL.x = BL.x = 0;
			TR.y = TL.y = 0x600;
			BR.y = BL.y = 0xC00;
				
			RotTrans.x = 0;
			RotTrans.y = 0;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex2, Position, Scale, TR, TL, BL, BR, RotTrans, 0, 31);

			Position = bows->position;
			Position.x += 0xAAAAA;
			Position.y += 0x80000;
			Position.z += 0x1000;
			
			Scale.x = 0x2AAAC;
			Scale.y = 0x80000;
				
			TR.x = BR.x = 0xE00;
			TL.x = BL.x = 0xC00;
			TR.y = TL.y = 0;
			BR.y = BL.y = 0x600;
				
			RotTrans.x = 0;
			RotTrans.y = 0;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex1, Position, Scale, TR, TL, BL, BR, RotTrans, 0, 31);
		}
		
		// Cross
		if (bows->ToDraw & 8)
		{
			Position = bows->position;
			Position.x -= bows->CrossScale / 2;
			Position.y -= bows->CrossScale / 2;
			Position.x += 0x58000;
			Position.y += 0xB4000;
			Position.z += 0x3000;

			Scale.y = bows->CrossScale / 2;
			Scale.x = bows->CrossScale / 2;
			
			// Top-left corner
			TR.x = BR.x = 0x400;
			TL.x = BL.x = 0;
			TR.y = TL.y = 0x400;
			BR.y = BL.y = 0;
			
			RotTrans.x = Scale.x;
			RotTrans.y = Scale.x;	
				
			BowserHeadEntrance_DrawAPolygon(bows->Tex3, Position, Scale, TR, TL, BL, BR, RotTrans, bows->rotation.x, bows->CrossOpacity);

			// Top-right corner
			Position.x += Scale.x;
			
			TR.x = BR.x = 0;
			TL.x = BL.x = 0x400;
			
			RotTrans.x = 0;

			BowserHeadEntrance_DrawAPolygon(bows->Tex3, Position, Scale, TR, TL, BL, BR, RotTrans, bows->rotation.x, bows->CrossOpacity);

			// Bottom-right corner
			Position.y += Scale.y;
			
			TR.y = TL.y = 0;
			BR.y = BL.y = 0x400;
			
			RotTrans.y = 0;

			BowserHeadEntrance_DrawAPolygon(bows->Tex3, Position, Scale, TR, TL, BL, BR, RotTrans, bows->rotation.x, bows->CrossOpacity);

			// Bottom-left corner
			Position.x -= Scale.x;
			
			TR.x = BR.x = 0x400;
			TL.x = BL.x = 0;
			
			RotTrans.x = Scale.x;

			BowserHeadEntrance_DrawAPolygon(bows->Tex3, Position, Scale, TR, TL, BL, BR, RotTrans, bows->rotation.x, bows->CrossOpacity);
		}

	}

void BowserHeadEntrance_DrawAPolygon(TexInfo Texture, Vec3 Position, Vec3 Scale, Vec2 TopRight, Vec2 TopLeft, Vec2 BottomLeft, Vec2 BottomRight, Vec3 RotationTranslation, s16 Rotation, int Opacity)
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
			
			REG_COLOR= 0x7FFF;

			REG_GFX_BEGIN = 1;
			texcoord2(TopRight.x, TopRight.y);
			vec3(0x1000, 	0x1000, 	0x0);
			
			texcoord2(TopLeft.x, TopLeft.y);
			vec3(0x0000,	0x1000, 	0x0);
			
			texcoord2(BottomLeft.x, BottomLeft.y);
			vec3(0x0000, 	0x0000,		0x0);
			
			texcoord2(BottomRight.x, BottomRight.y);
			vec3(0x1000,	0x0000,		0x0);

			REG_GFX_END = 0;
	}
	
void* BowserHeadEntrance_dtor(BowserHeadEntrance* bows)
    {
        bows->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        bows->Unk4Vec2 = dtor2;
        bows->Unk3Vec2 = dtor2;
        bows->DeleteDist = dtor2;
        bows->DrawDist = dtor2;
        bows->ExistDist = dtor2;
        bows->unkv13 = dtor;
        bows->unkv12 = dtor;
        bows->unkv11 = dtor;
		
		UnregisterActivePhysics(&bows->activephysics);
		
		bmgFlagSomething &= ~0x1;
		isTimeStopped &= ~0x10;
		
		ControlLock = false;
		*volume = 127;

        return bows;
    }

void* BowserHeadEntrance_dtorFree(BowserHeadEntrance* bows)
    {
        BowserHeadEntrance_dtor(bows);
		freeToGameHeap(bows);
        return bows;
    }
	