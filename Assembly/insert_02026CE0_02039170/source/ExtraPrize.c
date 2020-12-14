#include "game.h"
#include "registers.h"
#include "ExtraPrize.h"
#include "sfx.h"
#include "VariousFunctions.h"

int DragonCoinCount = 0;
int DragonCoinBitmask = 0;
extern bool IsSpecialLevel;

int ExtraPrize_loadFiles()
	{
		if (!IsSpecialLevel)
			{
				loadFileByExtId_3dModel(2648-0x83, 0);
				loadFileByExtId_3dModel(2649-0x83, 0);
				loadFileByExtId_3dModel(2650-0x83, 0);
				loadFileByExtId_3dModel(2651-0x83, 0);
			}
		loadFileByExtId_3dModel(2652-0x83, 0);
		return 1;
	}
	
void hook_020CF7D0_ov_08()
	{
		DragonCoinCount = 0;
		DragonCoinBitmask = 0;
	}
	
void ExtraPrize_setup(ExtraPrize* prz)
	{	
		prz->DrawDist.x = 0x64;
		prz->DrawDist.y = 0x64;
		
		prz->type = prz->spriteData&0xF;
		
		if (prz->type > 4) prz->type = 0;
		
		if ((DragonCoinBitmask >> ((prz->spriteData >> 12) & 0xF)) & 0x1)
			{
				prz->gotten = true;
				return;
			}
		
		u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile((2648-0x83 + prz->type)));	
		getTextureParams(texture, 0, &prz->tex.texparam);
		getTexturePalBase(texture, 0, &prz->tex.palbase);
		
		prz->position.x -= 0x4000;
		prz->position.y -= 0x10000;
		prz->position.z = 0;
	
		if (prz->spriteData>>8& 1) prz->position.x += 0x8000;     
        if (prz->spriteData>>8& 2) prz->position.y += 0x8000; 
		if (prz->spriteData>>8& 4) prz->position.z = 0x200000;
		
		prz->animtimer = 0;
		prz->gotten = false;
		prz->outofblock = false;
		prz->initialypos = prz->position.y;
		
		float scalex = 1.2;
		float scaley = 1.2;
		float scalez = 1.2;
		
		if ((prz->type == 1) || (prz->type == 2) || (prz->type == 4)) scaley = 1.4;
		if ((prz->type == 3) || (prz->type == 4)) scalex = 1.4;
		if (prz->type == 3) scaley = 2;
		
		prz->scale.x = 0x10000 * scalex;
		prz->scale.y = 0x10000 * scaley;
		prz->scale.z = 0x10000 * scalez;
			
		u32 ExtraPrize_physics[] = 
			{
				0x9000 * scalex,	
				0x8000 * scaley,	
				0x7800 * scalex,	
				0x7000 * scaley,	
				0x930006, 
				0x80000400,
				(u32)&ExtraPrizeCollision,
			};			
			
		initActivePhysics(&prz->activephysics, prz, ExtraPrize_physics, 0);
		registerCollider(&prz->activephysics);
	}

void ExtraPrize_IdleState(ExtraPrize* prz)
	{	
		return;
	}
	
void ExtraPrize_StateCollected(ExtraPrize* prz)
	{	
		switch (prz->type)
			{
				case 0: ExtraPrize_Get3UP(prz);	 
					break;
				case 1: ExtraPrize_GetPlusClock(prz,false);		
					break;
				case 2: ExtraPrize_GetPlusClock(prz,true);	
					break;
				case 3: ExtraPrize_GetDragonCoin(prz);	
					break;
				case 4: ExtraPrize_GetBigCoin(prz);
					break;
				default: ExtraPrize_Get3UP(prz);
					break;
			}
		prz->ticker++;

	}	
	
void ExtraPrize_Get3UP(ExtraPrize* prz)
	{
		if (prz->ticker == 0) 
		{
			GivePointsOfSomeKind(prz, 8, -0x18000, 0x18000, 0);
			GivePointsOfSomeKind(prz, 8, 0x8000, 0x28000, 0);
			GivePointsOfSomeKind(prz, 8, 0x28000, 0x18000, 0);
		}
		
		if (prz->ticker == 30) PlaySNDEffect(SE_SYS_ONE_UP, &prz->position);
		if (prz->ticker == 60) PlaySNDEffect(SE_SYS_ONE_UP, &prz->position);
		if (prz->ticker == 90) Base__deleteIt(prz);
	}	
	
void ExtraPrize_GetPlusClock(ExtraPrize* prz, bool type)
	{	
		if (prz->ticker == 0)
			{
				Vec3 ScorePos;
				ScorePos.x = prz->position.x + 0x8000;
				ScorePos.y = prz->position.y + 0x10000;
				ScorePos.z = 0;
				
				PlaySNDEffect(SE_SYS_SWITCH_CT_LAST, &ScorePos);
				if (!type) 
				{
					GivePointsOfSomeKind(prz, 9, 0xB000, 0x18000, 0);
					AddSecondsToTimer(10);
				}		
				else 
				{
					GivePointsOfSomeKind(prz, 0, 0xB000, 0x18000, 0);
					AddSecondsToTimer(100);
				}
			}
						
		if (prz->ticker == 200) Base__deleteIt(prz);	
	}
	
void ExtraPrize_GetDragonCoin(ExtraPrize* prz)
	{
		if (prz->ticker == 0) 
			{
				PlaySNDEffect(SE_OBJ_COIN_BOUND, &prz->position);
				GivePointsOfSomeKind(prz, DragonCoinCount+4, 0xB000, 0x28000, 0);

				SpawnCoinParticle(prz->position.x + 0x8000,prz->position.y + 0x10000,0);
	
				DragonCoinCount++;
				DragonCoinBitmask |= 1<<((prz->spriteData>>12) & 0xF);
				
				if (DragonCoinCount == 5) DragonCoinCount = 0;
			}
			
		if (DragonCoinCount) 
			{
				if (prz->ticker == 3) PlaySNDEffect(SE_OBJ_COIN_BOUND, &prz->position);
				if (prz->ticker == 6) PlaySNDEffect(SE_OBJ_COIN_BOUND, &prz->position);
				if (prz->ticker == 9) PlaySNDEffect(SE_OBJ_COIN_BOUND, &prz->position);
			}
		if (prz->ticker == 20) Base__deleteIt(prz);
	}	
	
void ExtraPrize_GetBigCoin(ExtraPrize* prz)
	{
		if (prz->ticker == 0) 
			{
				SpawnCoinParticle(prz->position.x + 0x10000, prz->position.y + 0x10000, 0);
				
				Vec3 ScorePos;
				
				ScorePos.x = prz->position.x + 0xB000;
				ScorePos.y = prz->position.y + 0x20000;
				ScorePos.z = 0;
				if (!IsSpecialLevel) DrawScore(9, &ScorePos);
			}

		if (prz->ticker < 10)
			{
				AddPointsToScore(0, 100);

				getCoin(0);
				PlaySNDEffect(SE_OBJ_GET_COIN, &prz->position);
			}

		if (prz->ticker == 11) Base__deleteIt(prz);
	}

bool ExtraPrize_beforeDraw(ExtraPrize* prz)
	{
		return 1;
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

void ExtraPrize_draw(ExtraPrize* prz)
	{
		
		if (prz->gotten) return;
	
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = prz->position.x;
		REG_MTX_TRANSLATE = prz->position.y;
		REG_MTX_TRANSLATE = prz->position.z;	
	
		REG_MTX_SCALE = prz->scale.x;
		REG_MTX_SCALE = prz->scale.y;
		REG_MTX_SCALE = prz->scale.z;
		
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = prz->tex.texparam;
		REG_PAL_BASE = prz->tex.palbase;
		
		REG_COLOR= 0x7FFF;
		
		int framePos = 0;

		if (prz->type) framePos = (prz->animtimer/8) * 0x200;
		
		REG_GFX_BEGIN = 1;
		texcoord2(0x200, framePos);
		vec3(0x1000, 	0x1000, 	0x0);
			
		texcoord2(0, framePos);
		vec3(0x0000,	0x1000, 	0x0);
			
		texcoord2(0, framePos+0x200);
		vec3(0x0000, 	0x0000,		0x0);
			
		texcoord2(0x200, framePos+0x200);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;	
		
		bool *Frozen = (bool*)(0x020CA850);
		bool *MenuOpen = (bool*)(0x020CA870);
		
		if ((*Frozen == 0) && (*MenuOpen == 0))
			{
				prz->animtimer++;
				if (prz->animtimer > 30)
					prz->animtimer = 0;
			}
	}
	
void ExtraPrizeCollision(ActivePhysics* a, ActivePhysics* b)
	{
		ExtraPrize* prz = (ExtraPrize*) a->owner;
		MarioActor* mA = (MarioActor*) b->owner;
		
		if (prz->gotten) return;
		if ((u32)mA != (u32)getPtrToPlayerActor()) return; 

		prz->gotten = true;
		prz->state = 1;
	}
	
void ExtraPrizeKoopaCollision(ExtraPrize* prz, EnemyClassObject* Shell)
	{
		if (prz->gotten) return;

		prz->activephysics.behaviour = 0;
		prz->activephysics.flags = 0;
		
		prz->gotten = true;
		prz->state = 1;
	}