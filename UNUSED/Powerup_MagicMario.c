#include <nds.h>
#include <math.h>
#include "PlayerSpell.h"
#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "VariousFunctions.h"
#include "Enemy.h"
#include "Mario.h"
#include "Files.h"

extern int SpellsInPlay;

bool CannotShoot = true;
bool CurrentlyShooting = true;
bool DrawCap = false;
extern bool DisableYRotationForce;

int Spinning = 0;
s16 YRot = 0;

void DoSpin();
void DoMagic();
void UpdateKeys();

void hook_020FD1D4(MarioActor* mA)
{	
	UpdateKeys();
	
	DoSpin(mA);
	DoMagic(mA);
}

void DoSpin(MarioActor* mA)
{
	if (((*ButtonsHeld & SLEFT) || 
		(*ButtonsHeld & SRIGHT))  
		&& (Spinning == 0) && 
		(!mA->collider.isOnGround) && 
		mA->Velocity.y < 0 && 
		(mA->State == 0x21135B8))
	{
		Spinning = 30;	
		YRot = mA->rotation.y;
		SetPlayerAnimation(mA, 0x6, 1, 0, 0x1300);
	}
	
	if (Spinning > 15)
	{
		if (Spinning == 25)
			PlaySNDEffect(SE_PLY_JUMP_3, &mA->position);
		
		DisableYRotationForce = true;
		YRot += 0x1100;
		mA->rotation.y = YRot;
		mA->Velocity.y = 0;

	}
	else
		DisableYRotationForce = false;
	
	if (Spinning != 0)
		Spinning--;
}

bool RunPressed()
{
	if (controlOptionsA)
		return (*ButtonsPressed & Y) != 0 || (*ButtonsPressed & X) != 0;
	else
		return (*ButtonsPressed & Y) != 0 || (*ButtonsPressed & B) != 0;
}

bool RunHeld()
{
	if (controlOptionsA)
		return (*ButtonsHeld & Y) != 0 || (*ButtonsHeld & X) != 0;
	else
		return (*ButtonsHeld & Y) != 0 || (*ButtonsHeld & B) != 0;
}

void UpdateKeys()
{
	CannotShoot = CurrentlyShooting;

	if (CurrentlyShooting && !RunHeld())
		CurrentlyShooting = false;
	else
		CurrentlyShooting = RunPressed();
}

bool RunKeyPressed()
{
	return CurrentlyShooting && !CannotShoot;
}

void hook_020FFE24_ov_0A()
{
	CannotShoot = true;
	CurrentlyShooting = true;	
}

bool CheckIfPlayerCanShoot(MarioActor* mA)
{
	if ((mA->Frozen & 0x1) ||                  // Game is frozen.
		(mA->HeldItem) ||              		   // Mario is holding something.
		(mA->CurrentAnimation == 0x12) ||      // Mario is currently shooting.
		(mA->CurrentAnimation == 0xC) ||       // Mario is sliding.
		(mA->CurrentAnimation == 0x2F) ||      // Mario is spinning.
		(mA->CurrentAnimation == 0x6E) ||      // Mario has ducked into shell.
		(pressedKeys & DOWN) ||                // Holding down (groundpouding, crouching, etc.)
		(SpellsInPlay >= 3) ||                 // There's too many Spells! 
		(mA->IsGroundpounding & 2) ||  		   // Groundpouding (again)
		(!(mA->CurrentPowerupState == 0x5)) || // Mario is not Magic Mario
		(*((u8*)0x20CA8C0))) return false;     // In Flagpole State
		
		return true;
}

void DoMagic(MarioActor* mA)
{
	bool Direction = mA->direction & 0x1;
	

		
	if (RunKeyPressed())
	{
		if (!CheckIfPlayerCanShoot(mA))
			return;

		Vec3 Speed;
		Speed.x = (0x3000 + mA->acceleration.x / 2);
		Speed.y = (mA->acceleration.y / 8);
		
		if (Direction)
			Speed.x *= -1;
		
		Vec3 Position;
		Position.x = mA->position.x;
		Position.y = mA->position.y + 0x6000;
		Position.z = 0x200000;

		if (!Direction)
		{
			Position.x += 0x10000;
			SpawnCoinParticle(Position.x + 0x8000, Position.y, 0);
		}
		else
		{
			Position.x -= 0x10000;
			SpawnCoinParticle(Position.x - 0x8000, Position.y, 0);
		}


		PlayerSpell* spell = (PlayerSpell*)createActor(0x147, 0, &Position, 0, 0, 0);
		PlaySNDEffect(SE_PLY_THROW_FIRE, &spell->position);
		spell->velocity = Speed;
	
	
		mA->State = (u32)0x210E85C;
		UpdatePlayerAnimation(mA);
		SetPlayerAnimation(mA, 0x12, 1, 0, 0x1300);
	}
}