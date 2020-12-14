#include <nds.h>
#include <math.h>
#include "hammer.h"
#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "VariousFunctions.h"
#include "Enemy.h"

extern bool HammerThrowingEnabled;
bool HammerThrowingDisabledByEndOfControlLock = false;

u32 hammers = 0;
u32 livinghammers = 0;

u32 Hammer_Actor_colls[] =
{
	0x00000000,
	0x15554000,
	0x15A5A440,
	0x00055000,
	0x00181500,
	0x50550544,
	0x41109561,
	0x50419409,
	0x40010A9A,
	0x02441555,
	0x00000000,
	0x00080000,
	0x00000000,
	0x00004000,
	0xA0000000,
	0x40000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};

u32 doCollision(ActivePhysics* a, ActivePhysics* c, bool killActor);

// I should just do this entire function in asm, really
int Hammer_collisionCB(ActivePhysics* hammerPhysics, ActivePhysics* b)
	{
		void* otherActor = b->owner;
		EnemyClassObject* awful = b->owner;
		Hammer* hammer = hammerPhysics->owner;
		u16* actorID = (u16*)(otherActor + 0xC); 
		
		u8 collType = Hammer_Actor_colls[*actorID / 16] >> (*actorID % 16 * 2) & 3;
		
		if (collType != 1 && collType != 2)
			return 1;
		
		bool killActor = (collType == 1);
		
		u32* spriteData = (u32*)(otherActor + 0x8);
		
		if (*actorID == 54 && (*spriteData >> 0xC) & 1)
			killActor = false;
		if (*actorID == 136 && (*spriteData >> 0x4) & 1)
			killActor = false;
		
		doCollision(hammerPhysics, b, killActor);
		
		if ((collType == 1) && (killActor == true)) 
		{

			UnregisterActivePhysics(b);
			
			Vec3 ScorePos = awful->position;
			ScorePos.y += 0x18000;
			
			AddPointsToScore(0, 200);
			DrawScore(1, &ScorePos);
		}
		
		if (livinghammers && collType != 0) livinghammers--;
		if (collType != 0) hammer->alive = false;
		
		return 1;
	}

u32 Hammer_physics[] = 
	{
		0x8000,	// X offset
		0x0000,	// Y offset
		0x6000,	// Width
		0x6000,	// Height
		0x09300001, // behavior
		0x0000,		// flags
		(u32)&Hammer_collisionCB
	};

// behavior:
// bit2 = react to non-player collision (enemies, block hit from below)
// bit16 = react to player collision
// bit17 = ???
// bit20 = die when hit from below
// bit21 = 
// bit22 = ???
// bit23 = react to fireballs

// flags:
// bit1 = die when receiving fireball
// bit10 = die when receiving koopa shell


void Hammer_loadFiles()
	{
		loadFileByExtId_3dModel(2629-0x83, 0);
	}

void Hammer_setup(Hammer* h)
	{
		u8* texture = (u8*) NNS_G3dGetTex(getPtrToLoadedFile(2629-0x83));
		
		getTextureParams(texture, 0, &h->tex.texparam);
		getTexturePalBase(texture, 0, &h->tex.palbase);
		
		initActivePhysics(&h->activephysics, h, Hammer_physics, 0);
		registerCollider(&h->activephysics);
		
		h->direction = h->spriteData & 1;
		
		h->position.z = 0x200000;
		h->rotation.z = 0;

		if (h->direction)
			h->initX = h->position.x - 0x10000;
		else
			h->initX = h->position.x + 0x10000; 
		
		h->initY = h->position.y + 0x10000;
		
		h->alive = true;

		PlaySNDEffect(SE_EMY_HAMMER, &h->position);
	}

void Hammer_beforedelete(Hammer* h)
	{
		if (hammers) hammers--;
		if ((h->alive) && (livinghammers)) livinghammers--;
	}

void setupCurve(Hammer* h, s32 curveWidth, s32 curveHeight)
	{
		float x1, x2, x3, y1, y2, y3;
		
		x1 = 0;
		x2 = (float) (curveWidth / 2);
		x3 = (float) curveWidth;
		
		y1 = 0;
		y2 = (float) curveHeight;
		y3 = 0;
		
		float denominator = (x1 - x2) * (x1 - x3) * (x2 - x3);
		h->curveA      	  = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denominator;
		h->curveB         = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denominator;
		h->curveC         = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denominator;
		
		h->curveWidth = curveWidth;
	}

void move(Hammer* h)
	{
		float xs;
		if (!h->direction)
			xs = h->position.x - h->initX;
		else
			xs = -(h->position.x - h->initX);
		float speedModifier = abs((2*h->curveA*xs + h->curveB)*100);
		
		s32 speed = h->velocity.x - (s32)(speedModifier*0xF);
		if (speed < 10)
			speed = 10;
		
		float x;
		if (!h->direction)
			x = h->position.x - h->initX  + speed;
		else
			x = -(h->position.x - h->initX) + speed;
		
		s32 y = (s32) (h->curveA * x * x + h->curveB * x + h->curveC);
		
		if (!h->direction)
			h->position.x += speed;
		else
			h->position.x -= speed;
		
		h->position.y = h->initY + y;
		
		s32 *camX = (s32*)(0x02085BFC);
		s32 *camY = (s32*)(0x020CA2E4);		
		s32 ScreenX = *camX;
		s32 ScreenY = 0xFFFFFFFF - ((*camY)<<12) - 0x10000;
		
		s32 xoffs = h->position.x - ScreenX;
		s32 yoffs = h->position.y - ScreenY;
		
		u16 *StageZoom = (u16*)(0x020CADB4);
		
		if ((*(*(int**)(0x208B168)) >> 20) & 0xF)
		{
			if (yoffs > 0x100 * (*StageZoom) || yoffs < -0x100 * (*StageZoom))
			{
				if (hammers) hammers--;
				Base__deleteIt(h);
			}
		}
		else
		{
			if (xoffs > 0x100 * (*StageZoom) || xoffs < -0x100 * (*StageZoom) || yoffs > 0x100 * (*StageZoom) || yoffs < -0x100 * (*StageZoom))
				{
					if (hammers) hammers--;
					Base__deleteIt(h);
				}
		}
	}

void Hammer_update_state0(Hammer* h)
	{
		move(h);
        
        if (!h->direction)
			h->rotation.z += 0x300;
		else
            h->rotation.z -= 0x300;
        
		if (!(h->rotation.z % 0x6000)) PlaySNDEffect(SE_EMY_HAMMER, &h->position);
	}

bool Hammer_beforeDraw()
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

void NNS_G3dGeFlushBuffer();
void G3_LoadMtx43(u32 unk);

void Hammer_draw(Hammer* h)
{
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);
	
	if ((*(*(int**)(0x208B168)) >> 20) & 0xF) REG_MTX_TRANSLATE = (h->position.x & 0x000FFFFF);
	else REG_MTX_TRANSLATE = h->position.x;

	REG_MTX_TRANSLATE = h->position.y - 0x8000;
	REG_MTX_TRANSLATE = h->position.z;
	
	glTranslatef32(0x8000, 0x8000, 0);
	glRotateZi(h->rotation.z);
	glTranslatef32(-0x8000, -0x8000, 0);
	
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
	REG_MTX_SCALE = 0x10000;
		
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;
		
	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = h->tex.texparam;
	REG_PAL_BASE = h->tex.palbase;
	
	REG_COLOR = 0x7FFF;
		
	REG_GFX_BEGIN = 1;
	
	if (!h->direction)
		{
			texcoord2(0x100, 0);
			vec3(0x1000, 0x1000, 0);
				
			texcoord2(0, 0);
			vec3(0x0000, 0x1000, 0);
				
			texcoord2(0, 0x100);
			vec3(0x0000, 0x0000, 0);
				
			texcoord2(0x100, 0x100);
			vec3(0x1000, 0x0000, 0);
		}
	
	else
		{
			texcoord2(0, 0);
			vec3(0x1000, 0x1000, 0);
				
			texcoord2(0x100, 0);
			vec3(0x0000, 0x1000, 0);
				
			texcoord2(0x100, 0x100);
			vec3(0x0000, 0x0000, 0);
				
			texcoord2(0, 0x100);
			vec3(0x1000, 0x0000, 0);
		}
	
	REG_GFX_END	 = 0;
}

bool cantshoot = true;
bool isshooting = true;

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
	cantshoot = isshooting;

	if (isshooting && !RunHeld())
		isshooting = false;
	else
		isshooting = RunPressed();
}

bool RunKeyPressed()
{
	return isshooting && !cantshoot && HammerThrowingEnabled && !HammerThrowingDisabledByEndOfControlLock;
}

void hook_020FD1D4(MarioActor* mA)
{	
	UpdateKeys();

	if ((mA->unk7B0 & 0x1) ||                // game is frozen
		(mA->HeldItem) ||              		 // holding something
		(mA->currentMarioAnim == 0x12) ||    // already throwing
		(mA->currentMarioAnim == 0xC) ||     // sliding
		(mA->currentMarioAnim == 0x2F) ||    // spinning
		(mA->currentMarioAnim == 0x6E) ||    // in shell
		(pressedKeys & DOWN) ||              // groundpounding, crouching, crouch-jumping, etc
		(hammers >= 5) ||                    // there's too many hammers
		(livinghammers >= 2) ||   
		(mA->IsMarioGroundPounding77E&2) ||  // groundpouding (again)
		(!(mA->marioPowerupState == 0x5)) || // Mario is not Hammer Mario
		(*((u8*)0x20CA8C0))) return;         // not in flagpole state
	
	bool direction = mA->direction & 0x1;
	
	if (RunKeyPressed())
		{			
			Vec3 vec;
			vec.x = mA->xPos;

			if (!direction)
				{
					vec.x += 0x9000;
				}
			else
				{
					vec.x -= 0x9000;
				}
					
			vec.y = mA->yPos + 0x13000;
			if (pressedKeys & UP) vec.y += 0x8000;		
			
			vec.z = -230;
				
			Hammer* h = (Hammer*) createActor(253, direction, &vec, 0, 0, 0);
				
			s32 curveHeight = 0x10000;
			if (pressedKeys & UP) curveHeight += 0x8000;
			
			setupCurve(h, 0x38000 + abs(mA->xAccel0)*0xF, curveHeight);
				
			h->velocity.x = 10000 + abs(mA->xAccel0/2);
			
			hammers++;
			livinghammers++;
				
			mA->unk980 = (u32)0x210E85C;
				
			UpdatePlayerAnimation(mA);
			SetPlayerAnimation(mA, 0x12, 1, 0, 0x1300);	
		}
}

void hook_020FFE24_ov_0A()
	{
		cantshoot = true;
		isshooting = true;	
	}
