#include "game.h"
#include "registers.h"
#include "VariousFunctions.h"
#include <math.h>
#include "sfx.h"
#include "keys.h"

#include "Goal.h"
#include "Mario.h"
#include "Files.h"
#include "TexDraw.h"

extern u32 isTimeStopped;
extern bool ControlLock;
extern bool DisableYRotationForce;
extern bool FlagpoleMode;
extern Vec3 WipeFadeVec3;

extern EnemyClassObject* CameraControllerPtr;

int Goal_loadFiles()
	{
		loadFileByExtId_3dModel(GOAL, 0);
		
		int *player = (int*)(0x02085A50);
		loadFileByExtId_3dModel(LEVELCLEAR + *player, 0);
		loadFileByExtId_3dModel(COINREWARD, 0);
		return 1;
	}

void Goal_setup(Goal* goal)
	{	
		u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(GOAL));
		
		getTextureParams(texture, 0, &goal->Posts.texparam);
		getTexturePalBase(texture, 0, &goal->Posts.palbase);
		getTextureParams(texture, 1, &goal->Tape.texparam);
		getTexturePalBase(texture, 1, &goal->Tape.palbase);
		
		int *player = (int*)(0x02085A50);
		texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(LEVELCLEAR+ *player));
		getTextureParams(texture, 0, &goal->Clear.texparam);
		getTexturePalBase(texture, 0, &goal->Clear.palbase);
		
		texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(COINREWARD));
		getTextureParams(texture, 0, &goal->Reward.texparam);
		getTexturePalBase(texture, 0, &goal->Reward.palbase);
		
		u32 Goal_physics[] = 
			{
				0x1C000,	
				0x4000,	
				0x14000,	
				0x4000,	
				0x00030004, 
				0x0400,
				(u32)&GoalCollision,
			};	

		u32 Goal_physics_general[] = 
			{
				0x20000,	
				0x200000,	
				0x100,	
				0x200000,	
				0x00030004, 
				0x0400,
				(u32)&GoalCollision2,
			};				
			
		goal->position.y += 0x60000;
		goal->scale.x = 0x10000;
		goal->scale.y = 0x10000;
		goal->scale.z = 0x10000;
		
		goal->DrawDist.x = 0x8000;
		goal->DrawDist.y = 0x8000;
		goal->ExistDist.x = 0x8000;
		goal->ExistDist.y = 0x8000;
	
		
		initActivePhysics(&goal->activephysics, goal, Goal_physics, 0);
		registerCollider(&goal->activephysics);
	
		initActivePhysics(&goal->activephysics2, goal, Goal_physics_general, 0);
		registerCollider(&goal->activephysics2);
		
		if (CheckIfDesmume()) goal->Desmume = 0x100;
	}

void Goal_update_state0(Goal* goal)
	{	
		if (goal->TapeDir)
		{
			goal->TapeHeight+=0x1800;
			goal->activephysics.offsety += 0x1800;
		}
		else
		{
			goal->TapeHeight-=0x1800;
			goal->activephysics.offsety -= 0x1800;
		}
		
	
		if (goal->TapeHeight <= 0)
			goal->TapeDir = true;
		
		if (goal->TapeHeight >= 0x90000)
			goal->TapeDir = false;

	
		return;
	}
	
void Goal_update_state1(Goal* goal)
	{	
		NNS_SndPlayerSetTempoRatio((u32*)0x208FC08, 0x100); 
		
		MarioActor* Actor = (MarioActor*)goal->PlayerCrossedTape;
		s32 *camX = (s32*)(0x02085BFC);
		s32 ScreenX = *camX;
		s32 *camY = (s32*)(0x020CA2E4);		
		s32 ScreenY = 0xFFFFFFFF - ((*camY)<<12) - 0x10000;		
		
		if (goal->Substate == 0)
		{
			Actor->xVelocity = 0;
			Actor->rotation.y = 0x3000;
			Actor->direction = 0;
			
			SetPlayerAnimation(Actor, 0x8D, 1, 0, 0x800);
			goal->Substate = 1;
			
			Vec3 ParticlePos = goal->position;
			ParticlePos.x += 0x18000;
			ParticlePos.y = goal->position.y - 0x6000 + goal->TapeHeight;
			ParticlePos.z += 0x1000;
				
			if (goal->TapeBumped)
				SpawnParticle(0x41, &ParticlePos);
			else
				SpawnParticle(52, &ParticlePos);
			
			if (goal->CoinsAwarded)
			{
				ParticlePos.x = ScreenX + 0x7F000 + goal->Desmume;
				ParticlePos.y = ScreenY - 0x50000;
				SpawnParticle(35, &ParticlePos);	
			}
			
			
			return;
		}
		
		if (goal->Substate == 1)
		{
			if (goal->CoinsAwarded)
				goal->DrawReward = 31;			
			
			if (Actor->collider.isOnGround)
			{	
				createActor(0x146, 0, &Actor->position, 0, 0, 0);
				SetPlayerAnimation(Actor, 0x1, 1, 0, 0x1000);
				goal->Substate = 2;
				
				PlayMusicFile(0x4, 0xFF);
			}
			else
				return;
		}
		
		if (goal->Substate == 2)
		{
			u16 *StageZoom = (u16*)(0x020CADB4);
			if (*StageZoom != 0x1000)
				*StageZoom -=0x20;
			
			if (Actor->position.x < (CameraControllerPtr->position.x + 0x20000))
				CameraControllerPtr->position.x += 0x580;
			else
				CameraControllerPtr->position.x += 0x800;
			
			
			Actor->xVelocity = 0x800;
			goal->StateTimer++;
			
			if (goal->CoinsAwarded && goal->StateTimer % 4 == 0)
			{
				PlaySNDEffect(SE_OBJ_GET_COIN, &Actor->position);
				goal->CoinsAwarded--;
				getCoin(0);
			}
			
			if (!goal->CoinsAwarded)
			{
				
				int *timer = (int*)(0x020CA8B4);
				
				if (*timer > 4096 * 3)
				{
					*timer -= (4096 * 3);
					AddPointsToScore(0, 200);
					PlaySNDEffect2(SE_SYS_SLOT, &Actor->position);
				}		
				else
				{
					*timer = 0;
					if (goal->StateTimer > 200)
					{
						goal->Substate = 3;
						goal->StateTimer = 0;
						return;
					}
					else
						return;
				}		
			}			
		}
		
		if (goal->Substate > 2)
		{
			if (goal->DrawReward)
				goal->DrawReward--;
		}
		
		if (goal->Substate == 3)
		{
			DisableYRotationForce = true;
			
			if (Actor->rotation.y > 0)
				Actor->rotation.y -= 0x500;	
			else
				goal->Substate = 4;
		}
		
		if (goal->Substate == 4)
		{
			SetPlayerAnimation(Actor, 0x63, 1, 0, 0x1000);
			
			int *player = (int*)(0x02085A50);
			
			if (*player)
				PlaySNDEffect(SE_VOC_LU_WIN, &Actor->position);
			else
				PlaySNDEffect(SE_VOC_MA_WIN, &Actor->position);
			
			goal->DrawClear = 1;
			goal->Substate = 5;			
		}
		
		if (goal->Substate == 5)
		{
			goal->StateTimer++;
			
			if (goal->DrawClear != 31)	
				goal->DrawClear+=2;
			
			if (goal->StateTimer > 200)
				goal->Substate = 6;
			
			int fireworkstyle = RNG(100);
                
			switch (fireworkstyle)
			{
				case 0: fireworkstyle = 62; break;
				case 1: fireworkstyle = 63; break;
				case 2: fireworkstyle = 67; break;
				case 3: fireworkstyle = 68; break;
				case 4: fireworkstyle = 73; break;
				case 5: fireworkstyle = 75; break;
				default: fireworkstyle = 500; break;
			}
			
			if (fireworkstyle != 500)
			{
				Vec3 Position;
				Position.x = ScreenX;
				Position.y = ScreenY;

				int addX, addY;
				addX = RNG(0x100000);
				addY = RNG(0x50000);
			
				Position.x += addX;
				Position.y -= addY;
					
				SpawnParticle(fireworkstyle, &Position);
				PlaySNDEffect(SE_OBJ_GOAL_HANABI, &Position);
				Rumble(1);
			}			
		}
		
		if (goal->Substate == 6)
		{
			WipeFadeVec3 = Actor->position;
			WipeFadeVec3.y += 0x10000;
			
			SetSecretExitFromSpriteData(goal->spriteData);
			exitLevel(1);
		}
		return;
	}

bool Goal_beforeDraw(Goal* goal)
	{
		return 1;
	}
	
bool Goal_beforeDelete(Goal* goal)
	{
		return 1;
	}


void Goal_draw(Goal* goal)
	{
		// Left stake...
		Vec3 Translation = {0,0,0,0};
		Vec3 Position = goal->position;
		s32 *camX = (s32*)(0x02085BFC);
		s32 ScreenX = *camX;
		s32 *camY = (s32*)(0x020CA2E4);		
		s32 ScreenY = 0xFFFFFFFF - ((*camY)<<12) - 0x10000;		
		
		DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x100, 0x200, 0, 0x300, 31);
		Position.y += 0x10000;
			
		for (int i = 0; i < 8; i++)
		{
			DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x100, 0x100, 0, 0x200, 31);
			Position.y += 0x10000;
		}
		
		DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x100, 0, 0, 0x100, 31);
		
		// Right stake
		Position = goal->position;
		Position.x += 0x2C000;
		
		if (goal->PlayerCrossedTape == 0)
			Position.z = 0x200000;
		else
			Position.z = goal->PlayerCrossedTape->position.z + 0x12000;
		
		DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x200, 0x200, 0x100, 0x300, 31);
		Position.y += 0x10000;
		
		for (int i = 0; i < 8; i++)
		{
			DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x200, 0x100, 0x100, 0x200, 31);
			Position.y += 0x10000;
		}
		
		DrawPolygon(goal->Posts, Position, goal->scale, goal->rotation, Translation, 0x200, 0, 0x100, 0x100, 31);
		
		// Tape
		
		Position = goal->position;
		Position.x += 0x8000;
		Position.y = goal->position.y - 0x6000 + goal->TapeHeight;
		Position.z += 0x1000;
		
		Vec3 TapeScale;
		TapeScale.x = 0x30000;
		TapeScale.y = 0x10000;
		
		if (!goal->TapeCrossedAt)
			DrawPolygon(goal->Tape, Position, TapeScale, goal->rotation, Translation, 0x200, 0, 0, 0x100, 31);
		
		// LEVEL CLEAR

		if (goal->DrawClear)
		{			
			Position.x = ScreenX + 0x8000;
			Position.y = ScreenY - 0x40000;
			Position.z = goal->PlayerCrossedTape->position.z + 0x13000;
			
			Vec3 Scale;
			Scale.x = 0xF0000;
			Scale.y = 0x20000;
			
			DrawPolygon(goal->Clear, Position, Scale, goal->rotation, Translation, 0x1000, 0, 0, 0x200, goal->DrawClear);
		}
		
		if (goal->DrawReward)
		{
			Position.x = ScreenX + 0x60000 + goal->Desmume;
			Position.y = ScreenY - 0x40000;
			Position.z = goal->PlayerCrossedTape->position.z + 0x13000;
			
			Vec3 Scale;
			Scale.x = 0x20000;
			Scale.y = 0x20000;
			
			int Digit1 = goal->CoinsAwarded_2 / 10;
			int Digit2 = goal->CoinsAwarded_2 % 10;
			
			if (Digit1 != 0)
			{
				DrawPolygon(goal->Reward, Position, Scale, goal->rotation, Translation, 
				0x200 + ((Digit1 % 5) * 0x200), 0 + ((Digit1 / 5) * 0x200), 0 + ((Digit1 % 5) * 0x200), 0x200 + ((Digit1 / 5) * 0x200), goal->DrawReward);
				Position.x += 0x15000;
			}
			else
				Position.x += 0x8000;
	
			DrawPolygon(goal->Reward, Position, Scale, goal->rotation, Translation, 
			0x200 + ((Digit2 % 5) * 0x200), 0 + ((Digit2 / 5) * 0x200), 0 + ((Digit2 % 5) * 0x200), 0x200 + ((Digit2 / 5) * 0x200), goal->DrawReward);				
		}
	}
	
void MarioGoalState(MarioActor* mA)
	{
		UpdatePlayerAnimation(mA);
	}
	
void AwardCoins(Goal* goal)
	{
		if (goal->TapeCrossedAt < 0x15000)
			goal->CoinsAwarded = 1;
		
		else if (goal->TapeCrossedAt < 0x25000)
			goal->CoinsAwarded = 5;
		
		else if (goal->TapeCrossedAt < 0x45000)
			goal->CoinsAwarded = 10;
		
		else if (goal->TapeCrossedAt < 0x65000)
			goal->CoinsAwarded = 15;
		
		else if (goal->TapeCrossedAt < 0x75000)
			goal->CoinsAwarded = 20;

		else if (goal->TapeCrossedAt < 0x85000)
			goal->CoinsAwarded = 30;	
		
		else goal->CoinsAwarded = 50;			
		
		goal->CoinsAwarded_2 = goal->CoinsAwarded;	
	}
		
void GoalCollision(ActivePhysics* a, ActivePhysics* b)
	{
		Goal* goal = (Goal*)a->owner;
		MarioActor* actor = (MarioActor*)b->owner;
		
		if (actor->ClassID != 21)
			return;
		
		UnregisterActivePhysics(&goal->activephysics);
		UnregisterActivePhysics(&goal->activephysics2);
		actor->State = (u32)&MarioGoalState;
		goal->state = 1;
		goal->TapeBumped = true;
		goal->TapeCrossedAt = goal->TapeHeight;
		goal->PlayerCrossedTape = actor;
		AwardCoins(goal);
		
		isTimeStopped = true;
		FlagpoleMode = true;
		ControlLock = true;
		pressedKeys = 0;	
		
		PlaySNDEffect(SE_SYS_GOAL_FLAG, &goal->position);
		StopMusicWithFade(0x1E);	
		
		return;
	}
	
void GoalCollision2(ActivePhysics* a, ActivePhysics* b)
	{
		Goal* goal = (Goal*)a->owner;
		MarioActor* actor = (MarioActor*)b->owner;
		
		if (actor->ClassID != 21)
			return;
		
		UnregisterActivePhysics(&goal->activephysics);
		UnregisterActivePhysics(&goal->activephysics2);
		actor->State = (u32)&MarioGoalState;
		goal->state = 1;
		goal->TapeCrossedAt = 1;	
		goal->PlayerCrossedTape = actor;
		
		isTimeStopped = true;
		FlagpoleMode = true;
		ControlLock = true;
		pressedKeys = 0;
		
		PlaySNDEffect(SE_SYS_GOAL_FLAG, &goal->position);	
		StopMusicWithFade(0x1E);		
		
		return;
	}
	
void* Goal_dtor(Goal* goal)
    {
        goal->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        goal->Unk4Vec2 = dtor2;
        goal->Unk3Vec2 = dtor2;
        goal->DeleteDist = dtor2;
        goal->DrawDist = dtor2;
        goal->ExistDist = dtor2;
        goal->unkv13 = dtor;
        goal->unkv12 = dtor;
        goal->unkv11 = dtor;
		
        base_onDelete(goal);
		
		ControlLock = false;
		FlagpoleMode = false;
		DisableYRotationForce = false;
		WipeFadeVec3.vtable = 0;

        return goal;
    }

void* Goal_dtorFree(Goal* goal)
    {
        Goal_dtor(goal);
		freeToGameHeap(goal);
		
        return goal;
    }