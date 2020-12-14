#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "FlipBlock.h"
#include "Files.h"

u32 FlipBlock_physics[] = 										
	{
		0,		
		0,	
		0x8000,	
		0x8000,	
		0x2D00004, 
		0,
		0,
	};			

s32 FlipBlock_solid_physics[] = 												
	{
		-0x8000,									// Top Right
		0x8000,										// Bottom Left
		0x8000,										// Top Right
		-0x8000,									// Bottom Right
		(u32)&FlipBlock_CheckIfMarioGP,				// Top Callback
		(u32)&FlipBlock_CheckIfMario, 				// Bottom Callback
		(u32)&FlipBlock_CheckIfShell,				// Side Callback
		0,	
		0,
		0,
	};			

int FlipBlock_loadFiles()
	{
		loadFileByExtId_3dModel(FLIPBLOCK, 0);	
		return 1;
	}
	
void FlipBlock_setup(FlipBlock* flip)
	{	
		model3d_setup(&flip->Model, getPtrToLoadedFile(FLIPBLOCK), 0);
		model3d_init(&flip->Model);
		
		flip->DrawDist.x = 0x80;
		flip->DrawDist.y = 0x80;

		flip->scale.x = 0x68000;
		flip->scale.y = flip->scale.z = 0x68000;
		
		flip->position.x += 0x8000;
		flip->position.y -= 0x8000;
		
		if ((flip->spriteData >> 16) & 1) flip->position.x += 0x8000;		//Nybble 8
		if ((flip->spriteData >> 16) & 2) flip->position.y += 0x8000;		//Nybble 8
	
		FlipBlock_MakeCollisions(flip);
	}
	
void FlipBlock_MakeCollisions(FlipBlock* flip)
	{
			if (!flip->hit)
				{
					initActivePhysics(&flip->activephysics, flip, FlipBlock_physics, 0);
					registerCollider(&flip->activephysics);
				}
			InitSolidCollision(&flip->solidcollision, flip, FlipBlock_solid_physics, 0, 0, 0);
			registerSolidCollider(&flip->solidcollision);		
	}
	

void FlipBlock_DestroyCollisions(FlipBlock* flip)
	{
		UnregisterSolidCollision(&flip->solidcollision);
		UnregisterActivePhysics(&flip->activephysics);
	}

	
bool FlipBlock_CheckIfMarioIsInRange(FlipBlock* flip)
	{
		u16 *StageZoom = (u16*)(0x020CADB4);
		
		MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
		
		int xoffs = Player->position.x - flip->position.x;
		int yoffs = Player->position.y - flip->position.y;
		int Distance = (*StageZoom * 0x100);
		
		if (xoffs > Distance || xoffs < -Distance || yoffs < -0x100000 || yoffs > 0x100000)  return false;
		
		else return true;
	}

void FlipBlock_IdleState(FlipBlock* flip)
	{
		bool range = FlipBlock_CheckIfMarioIsInRange(flip);
		
		if (!range && (!flip->colsdead)) 
			{
				FlipBlock_DestroyCollisions(flip);
				flip->colsdead = true;
			}
			
		if (range && (flip->colsdead)) 
			{
				FlipBlock_MakeCollisions(flip);
				flip->colsdead = false;
			}
	}

void FlipBlock_CheckIfMarioGP(FlipBlock* flip, MarioActor* Player)
	{
		if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->CurrentPowerupState == 3)) 
			{
				FlipBlock_Megashroom(flip);
				return;
			}

		if (flip->hit) return;
		
		if (((u32)Player == (u32)getPtrToPlayerActor()) && (Player->IsGroundpounding & 2))
			{
				flip->hitgp = true;
				FlipBlock_SetBlockStateHit(flip);
			}
		else return;
	}

void FlipBlock_CheckIfMario(FlipBlock* flip, MarioActor* Player)
	{
		if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->CurrentPowerupState == 3)) 
			{
				FlipBlock_Megashroom(flip);
				return;
			}	

		if (flip->hit) return;
		
		if ((u32)Player == (u32)getPtrToPlayerActor()) FlipBlock_SetBlockStateHit(flip);
		else return;
	}
	
void FlipBlock_CheckIfShell(FlipBlock* flip, EnemyClassObject* Enemy)
	{
		MarioActor* Player = (MarioActor*)Enemy;
		
		if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->CurrentPowerupState == 3)) 
			{
				FlipBlock_Megashroom(flip);
				return;
			}
			
		if (flip->hit) return;
		
		if ((Enemy->ClassID != 94) && (Enemy->ClassID != 95) && (Enemy->ClassID != 54) && (Enemy->ClassID != 40) && (Enemy->activephysics.callback != (u32*)0x02099B6C)) return;	
		else FlipBlock_SetBlockStateHit(flip);
	}	
	
void FlipBlock_SetBlockStateHit(FlipBlock* flip)
	{		
		flip->RotateTimer = 250;
		flip->state = 1;
		flip->activephysics.callback = (u32*)0x020D0984;
		UnregisterSolidCollision(&flip->solidcollision);
	}
	
void FlipBlock_StateBeingHit(FlipBlock* flip)
	{
		if (flip->RotateTimer)
			flip->RotateTimer--;
		
		if (flip->RotateTimer == 248)
			UnregisterActivePhysics(&flip->activephysics);
		
		MarioActor* Mario = (MarioActor*)getPtrToPlayerActor();
		
		if ((flip->RotateTimer || (flip->rotation.x != 0)) ||
		   (((flip->position.x - 0x10000) <= Mario->position.x) && 
			(flip->position.x >= Mario->position.x) &&
			((flip->position.y - 0x10000) <= Mario->position.y) &&
			(flip->position.y >= Mario->position.y)))
		{
			if (flip->hitgp)
				flip->rotation.x += 0x800;
			else
				flip->rotation.x -= 0x800;
		}
		else
		{
			flip->state = 0;
			flip->hitgp = false;
			flip->hit = false;
			FlipBlock_MakeCollisions(flip);
		}
	}
	
void FlipBlock_Megashroom(FlipBlock* flip)
	{
		Vec3 Pos = flip->position;
		
		int spritedata = 0x300000;
		if (flip->hit) spritedata = 0x600000;
		
		Pos.y += 0x10000;
		
		for (int i = 0; i < 3; i++)
			{
				createActor(0xF1, spritedata, &Pos, 0, 0, 0);
				Pos.x += 0x10000;
			}
			
		PlaySNDEffect(SE_OBJ_BLOCK_BREAK, &flip->position);	
		PlaySNDEffect(SE_PLY_STAR_ATTACK, &flip->position);
		IncreaseMegaMushroomDestructionCounter(0, 30);		
		Base__deleteIt(flip);
	}
	

void G3_LoadMtx43(u32 unk);

void FlipBlock_draw(FlipBlock* flip)
	{
		DrawAModel(&flip->Model, &flip->position, &flip->rotation, &flip->scale);
	}