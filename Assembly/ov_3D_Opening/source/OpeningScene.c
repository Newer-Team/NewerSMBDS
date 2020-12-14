#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "OpeningScene.h"
#include "VariousFunctions.h"

extern bool IsSpecialLevel;
extern bool ControlLock;
extern bool TouchscrControlLock;
extern s32 ForcePlayerPowerupTo;
extern u8 lastslot;

int OpeningScene_loadFiles()
	{
		if (!IsSpecialLevel) return 1;
		
		loadFileByExtId_3dModel(1537-0x83, 0);  			  // KoopaJr_Demo.nsbmd
		loadFileByExtId_3dModel(1536-0x83, 0);		  		  // KoopaJr_Demo.nsbca
		loadFileByExtId_3dModel(1594-0x83, 0); 		      	  // Peach.nsbmd
		loadFileByExtId_3dModel(1595-0x83, 0);				  // Peach_ttl.nsbca
		loadFileByExtId_3dModel(2695-0x83, 0);				  // Textbox
		return 1;
	}
	
void OpeningScene_setup(OpeningScene* scene)
	{	
		if (!IsSpecialLevel)
			{
				Base__deleteIt(scene);
				return;
			}
			
		u8* Tex = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(2695 - 0x83));
		getTextureParams(Tex, 0, &scene->TextBox.texparam);
		getTexturePalBase(Tex, 0, &scene->TextBox.palbase);
		scene->TextBoxOpacity = 0;
			
		MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);		
		loadFileByExtId_LZ_Dest(2855-0x83, G2S_GetBG0CharPtr());  		//NCG
		u32* nclptr = (u32*)loadFileByExtId_LZ(2856-0x83);				//NCL	
		GXS_LoadBGPltt(nclptr, 0, 0x120);
		freeToCurrentHeapIfNotNull(nclptr);
		
		loadFileByExtId_LZ_Dest(2857-0x83, G2S_GetBG2ScrPtr());			//NSC TOP
		loadFileByExtId_LZ_Dest(2858-0x83, G2S_GetBG3ScrPtr()); 		//NSC BTM
		
		scene->MarioPtr = (EnemyClassObject*)getPtrToMarioOrLuigi(0);
		scene->LuigiPtr = (EnemyClassObject*)getPtrToMarioOrLuigi(1);
		scene->LuigiPtr->position.x -= 0x10000;
		scene->MarioPtr->direction = scene->LuigiPtr->direction = 0;
		scene->MarioPtr->rotation.y = scene->LuigiPtr->rotation.y = 0x3000;
		scene->CameraPtr = (EnemyClassObject*)createActor(0x146, 0, &scene->position, 0, 0, 0);

		modelAnim3d_setup(&scene->PeachModel, getPtrToLoadedFile(1594-0x83), getPtrToLoadedFile(1595-0x83), 0, 0, 0);
		modelAnim3d_setup(&scene->BowserJrModel, getPtrToLoadedFile(1537-0x83), getPtrToLoadedFile(1536-0x83), 0, 0, 0);
		
		modelAnim3d_init(&scene->PeachModel, 6, 0, 0x1000, 0);
		modelAnim3d_init(&scene->BowserJrModel, 10, 0, 0x1000, 0);

		scene->rotation.y = 12000;
		
		Vec3 InitialPositions = scene->position;
		InitialPositions.x -= 0x90000;

		scene->PeachPosition = scene->BowserJrPosition = InitialPositions;
		scene->PeachRotation = scene->BowserJrRotation = scene->rotation;
		scene->PeachScale = scene->BowserJrScale = scene->scale;

		SwapASwar(41, 2842 - 0x83);
		SwapASbnk(40, 2843 - 0x83);
		SwapASsar(4, 2844 - 0x83);

		pressedKeys = 0;
	}

bool OpeningScene_Start(OpeningScene* scene)	// State 0
	{
		MarioActor* Mario = (MarioActor*)scene->MarioPtr;
		MarioActor* Luigi = (MarioActor*)scene->LuigiPtr;	


		if (scene->timer == 5)
			PlaySNDEffect(SE_EMY_KPJR_LAUGH, &scene->BowserJrPosition);
		
		if (scene->timer < 200)
			{
				scene->BowserJrPosition.x += 0x3000;
				scene->PeachPosition.x += 0x3000;
			}
		
		if (scene->timer == 120) PlaySNDEffect(SE_VOC_MA_OTOTO, &scene->MarioPtr->position);
		if (scene->timer == 140) PlaySNDEffect(SE_VOC_MA_WOOO, &scene->LuigiPtr->position);
		
		if (scene->timer > 120)
			{
				Mario->xVelocity = Luigi->xVelocity = 0x3000;
			}
			
		if (scene->timer == 250)
			scene->state = 1;
					
		return 1;
	}
	
bool OpeningScene_TextBox(OpeningScene* scene)
	{
		if (scene->timer == 262) 
			{
				Base__deleteIt(scene->LuigiPtr);
				PlaySNDEffect(SE_OBJ_COIN_SUBSTANT, 0);
			}
		
		if ((scene->timer >= 260) && (scene->timer < 300))
			{
				if (scene->TextBoxOpacity < 31)
					scene->TextBoxOpacity++;
			}			
		
			if (scene->timer >= 760)
			{
				if (scene->TextBoxOpacity != 0)
					scene->TextBoxOpacity--;
			}			
		
		if (scene->timer == 790)
				GoToWorldMap(scene);
			
		return 1;
	}

bool OpeningScene_AllStates(OpeningScene* scene)
	{
		anim3d_update(&scene->PeachModel.anim);
		anim3d_update(&scene->BowserJrModel.anim);
		scene->timer++;
		return 1;
	}
	
void GoToWorldMap(OpeningScene* scene)
	{
		ForcePlayerPowerupTo = 0;
		causeSceneChangeWrapper(9,0);
	}


bool OpeningScene_Draw(OpeningScene* scene)
	{
		DrawAModel(&scene->PeachModel, &scene->PeachPosition, &scene->PeachRotation, &scene->PeachScale);
		DrawAModel(&scene->BowserJrModel, &scene->BowserJrPosition, &scene->BowserJrRotation, &scene->BowserJrScale);
		OpeningScene_DrawTextBox(scene);
		return true;
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
	
void OpeningScene_DrawTextBox(OpeningScene* scene)
	{
		if (scene->TextBoxOpacity == 0) return;
			
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = scene->CameraPtr->position.x - 0x60000;
		REG_MTX_TRANSLATE = scene->CameraPtr->position.y + 0x10000;
		REG_MTX_TRANSLATE = scene->CameraPtr->position.z;	
		
		REG_MTX_SCALE = 0x100000;
		REG_MTX_SCALE = 0x80000;
		REG_MTX_SCALE = 0x100000;
			
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = (0x3880 | (scene->TextBoxOpacity << 16));
		REG_TEXT_FORMAT = scene->TextBox.texparam;
		REG_PAL_BASE = scene->TextBox.palbase;
			
		REG_COLOR= 0x7FFF;
			
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
	
void* OpeningScene_dtor(OpeningScene* scene)
    {
        scene->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        scene->Unk4Vec2 = dtor2;
        scene->Unk3Vec2 = dtor2;
        scene->DeleteDist = dtor2;
        scene->DrawDist = dtor2;
        scene->ExistDist = dtor2;
        scene->unkv13 = dtor;
        scene->unkv12 = dtor;
        scene->unkv11 = dtor;
		
        base_onDelete(scene);
	
        MIi_CpuClear16(0, BG_GFX_SUB + 0x8000, 0x10000);
        MIi_CpuClear16(0, BG_GFX_SUB, 0x10000);	

		IsSpecialLevel = false;
		ControlLock = false;	
		TouchscrControlLock = false;
	
		
        return scene;
    }

void* OpeningScene_dtorFree(OpeningScene* scene)
    {
        OpeningScene_dtor(scene);
		freeToGameHeap(scene);
        return scene;
    }
	