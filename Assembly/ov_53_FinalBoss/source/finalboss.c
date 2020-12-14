// Note: finalboss.h is in append
#include "finalboss.h"
#include "bowserjr.h"
#include "sfx.h"
#include "VariousFunctions.h"

#include <math.h>


// File IDs
#define KOOPA_MODEL_FILE_ID     2704-0x83
#define KOOPA_ANIM_FILE_ID      2705-0x83

#define COPTER_MODEL_FILE_ID    2702-0x83
#define COPTER_ANIM_FILE_ID     2703-0x83

#define FB_SAR_COURSE_FILE_ID               2832-0x83
#define FB_BANK_COURSE_SE_FILE_ID           2831-0x83
#define FB_WAVE_COURSE_SE_FILE_ID           2830-0x83
#define FB_SAR_PLAYER_BASE_FILE_ID          2829-0x83
#define FB_BANK_PLAYER_BASE_SE_FILE_ID      2828-0x83
#define FB_WAVE_PLAYER_BASE_SE_FILE_ID      2827-0x83
#define FB_SAR_OLD_KOOPA_FILE_ID            2826-0x83
#define FB_BANK_OLD_KOOPA_SE_FILE_ID        2825-0x83
#define FB_WAVE_OLD_KOOPA_SE_FILE_ID        2824-0x83
#define FB_SAR_LAST_KOOPA_DEMO_FILE_ID      2823-0x83
#define FB_BANK_LAST_KOOPA_DEMO_SE_FILE_ID  2822-0x83
#define FB_WAVE_LAST_KOOPA_DEMO_SE_FILE_ID  2821-0x83

#define TRACKS_FADE_IN_1 (1 << 6 | 1 << 7)
#define TRACKS_FADE_IN_2 (1 << 8)

//#define DEBUG_SKIP_P1

#define fb_stub(msg)   nocashPrint("FinalBoss Stub: " msg "\r\n")


extern u32 bmgFlagSomething;
extern u32 isTimeStopped;
extern bool ControlLock;
extern u32 freezeFlag;
extern bool DisableMarioYRotationForce;
extern bool Frozen;
extern EnemyClassObject* CameraControllerPtr;

extern u32 ViewStartAndEndForBoundsLimit[2];

enum KoopaAnimations
{
    KoopaAnimation_Idle = 0,
    KoopaAnimation_Roar = 1,
    KoopaAnimation_ThrowEnemy = 2
};

enum CopterAnimations
{
    CopterAnimation_Spin = 0
};

enum CopterSoundStates
{
    CopterSilent = 0,
    CopterHover = 1,
    CopterDive = 2
};                
   

u32 Propeller_physics[] = 
{
	0x00000,	// X offset
	-0x20000,	// Y offset
	0x10000,	// Width (from center to edge)
	0x4000,		// Height (from center to edge)
	0x00330004,	// behavior
	0x0000,		// flags
	(u32)&FinalBoss_collisionCallback // player/sprite collision callback
};

u32 BowsersFace_physics[] = 
{
	0x00000,	// X offset
	0x2A000,	// Y offset
	0x12000,	// Width (from center to edge)
	0xC000,		// Height (from center to edge)
	0x00330004,	// behavior
	0x0000,		// flags
	(u32)&FinalBoss_collisionCallback // player/sprite collision callback
};

void FinalBoss_loadFiles()
{
    loadFileByExtId_3dModel(KOOPA_MODEL_FILE_ID, 0);
    loadFileByExtId_3dModel(KOOPA_ANIM_FILE_ID, 0);

    loadFileByExtId_3dModel(COPTER_MODEL_FILE_ID, 0);
    loadFileByExtId_3dModel(COPTER_ANIM_FILE_ID, 0);
	
	loadFileByExtId_3dModel(2706 - 0x83, 0);		// BowserFire NSBTX
}

bool FinalBoss_heapCreated(FinalBoss* fb)
{
    return true;	
}

int FinalBoss_onCreate(FinalBoss* fb)
{
    // Spawn Bowser Jr.
    fb->jrState = 0;

	fb->Mario = (MarioActor*)getPtrToPlayerActor();

    Vec3 jrPos = fb->position;
    jrPos.y -= 0x8000;
#ifndef DEBUG_SKIP_P1
    BowserJr* bowserJr = (BowserJr*) createActor(113, 0x01020001, &jrPos, 0, 0, 0);
    bowserJr->finalBoss = fb;
    fb->bowserJr = bowserJr;
#endif

    // Set initial positions
    fb->position.x -= 0x4B000;
    fb->baseline = fb->position.y + 0x60000;
    fb->position.y += 0xE0000;

    // Init physics
    initActivePhysics(&fb->activephysics, fb, Propeller_physics, 0);
    registerCollider(&fb->activephysics);
	initActivePhysics(&fb->activephysics2, fb, BowsersFace_physics, 0);
    registerCollider(&fb->activephysics2);

    // Set up models
    modelAnim3d_setup(&fb->koopaModel, getPtrToLoadedFile(KOOPA_MODEL_FILE_ID), getPtrToLoadedFile(KOOPA_ANIM_FILE_ID), 0, 0, 0);
    modelAnim3d_setup(&fb->copterModel, getPtrToLoadedFile(COPTER_MODEL_FILE_ID), getPtrToLoadedFile(COPTER_ANIM_FILE_ID), 0, 0, 0);

    modelAnim3d_init(&fb->copterModel, CopterAnimation_Spin, 0, 0x2000, 0);

    // Set up sounds
    SwapASsar( 0, FB_SAR_COURSE_FILE_ID);
    SwapASbnk(34, FB_BANK_COURSE_SE_FILE_ID);
    SwapASwar(35, FB_WAVE_COURSE_SE_FILE_ID);
    SwapASsar(29, FB_SAR_PLAYER_BASE_FILE_ID);
    SwapASbnk(39, FB_BANK_PLAYER_BASE_SE_FILE_ID);
    SwapASwar(40, FB_WAVE_PLAYER_BASE_SE_FILE_ID);
    SwapASsar(23, FB_SAR_OLD_KOOPA_FILE_ID);
    SwapASbnk(59, FB_BANK_OLD_KOOPA_SE_FILE_ID);
    SwapASwar(65, FB_WAVE_OLD_KOOPA_SE_FILE_ID);
    SwapASsar(37, FB_SAR_LAST_KOOPA_DEMO_FILE_ID);
    SwapASbnk(71, FB_BANK_LAST_KOOPA_DEMO_SE_FILE_ID);
    SwapASwar(79, FB_WAVE_LAST_KOOPA_DEMO_SE_FILE_ID);
	
	u32* ViewStart = (u32*)0x20CAE60;
	u32* ViewEnd = (u32*)0x20CAE64;
	ViewStartAndEndForBoundsLimit[0] = *ViewStart;
	ViewStartAndEndForBoundsLimit[1] = *ViewEnd;
    
    u32* pointerToViewBlock = (u32*)0x0208B184;
    View* view = (View*)(*pointerToViewBlock + sizeof(View)*fb->ViewID);

    fb->killLine = (0xFFFFFFFF - ((view->y + view->height) << 12)) - (24 << 16);
    fb->camMaxX = ((view->x + view->width) << 12) - (4 << 16);
    fb->copterSoundState = CopterSilent;

    FinalBoss_begin_Intro(fb);
#ifdef DEBUG_SKIP_P1
    FinalBoss_begin_ChaseIntro(fb);
    freezeWorld(false);
#endif

    return 1;
}

int FinalBoss_onDraw(FinalBoss* fb)
{
    DrawAModel(&fb->koopaModel, &fb->position, &fb->rotation, &fb->scale);
    DrawAModel(&fb->copterModel, &fb->position, &fb->rotation, &fb->scale);
    return 1;
}

int FinalBoss_beforeDraw(FinalBoss* fb)
{
    // Always render
    return 1;
}

void FinalBoss_marioCollision(FinalBoss* fb, MarioActor* ma)
{
    if (ma->unk980 == 0x021103B8)	
		{
			return;
		}

	if ((ma->marioPowerupState == 5) && (ma->currentMarioAnim == 0x6E))
		{
			ma->marioPowerupState = 1;
			return;
		}

    PlaySNDEffect(SE_PLY_CLASH_2, &ma->unk05C);

    if (ma->xPos < fb->position.x)
    {
        ma->direction = 1;
        ma->xVelocity = -0x3000;
    }
    else
    {
       ma->direction = 0;
       ma->xVelocity = 0x3000;
    }
}

void FinalBoss_collisionCallback(ActivePhysics* a, ActivePhysics* b)
{
    FinalBoss* fb = (FinalBoss*)a->owner;
    EnemyClassObject* Actor = (EnemyClassObject*) b->owner;

    if (Actor->ClassID == 0x15)
        {
            MarioActor* mario = (MarioActor*)Actor;
            if (((u16)mario->unk79C == 0) // We don't care if the hurtbox touches Mario while he's already hitstunned
                && !((mario->marioPowerupState == 5) && (mario->currentMarioAnim == 0x6E))) // Ignore if in shell
                fb->hitMarioTimer = 4;
        }
	
	if ((Actor->ClassID == 35) && (fb->action == 1))
		{
			return;
		}
		
	if ((Actor->ClassID != 0x15) && (fb->state != FinalBoss_Stomp))
		{
			return;
		}
		
    EnemyKillingCollisionCallback(a, b);
}

void MarioActor_CutsceneState(MarioActor* Mario)
    {
        UpdatePlayerAnimation(Mario);
        return;
    }

////////////
// States
////////////

bool FinalBoss_executeState0(FinalBoss* fb)
{
    int *player = (int*)(0x02085A50);

    switch (fb->state)
    {
    case FinalBoss_Intro:
        FinalBoss_update_Intro(fb); break;
    case FinalBoss_Swoop:
        FinalBoss_update_Swoop(fb); break;
    case FinalBoss_SpawnBulletBills:
        FinalBoss_update_SpawnBulletBills(fb); break;
    case FinalBoss_Follow:
        FinalBoss_update_Follow(fb); break;
    case FinalBoss_SpawnFire:
        FinalBoss_update_SpawnFire(fb); break;
    case FinalBoss_ChaseIntro:
        FinalBoss_update_ChaseIntro(fb); break;
    case FinalBoss_Stomp:
        FinalBoss_update_Stomp(fb); break;
    }

    if (fb->updateAnimation)
    {
        anim3d_update(&fb->koopaModel.anim);
        anim3d_update(&fb->copterModel.anim);
    }

    if (fb->playerLookAtBowser)
    {
        setPlayerLooksToTarget(true);
        setPlayerLookTarget(&fb->position);
    }

    // Copter noises
    if (fb->copterSoundState == CopterHover)
        PlaySNDEffect2(SE_PLY_CANNON_BACK_SHOT, &fb->position);
    else if (fb->copterSoundState == CopterDive)
        PlaySNDEffect2(SE_PLY_MONKEY_BARS, &fb->position);

    // Bowser-hit-Mario laugh
    if (fb->copterSoundState != CopterDive)
    {
        if ((fb->state != FinalBoss_ChaseIntro) && (fb->hitMarioTimer == 1))
            PlaySNDEffect(SE_PLY_TARZAN_ROPE, &fb->position);

        if (fb->hitMarioTimer)
            fb->hitMarioTimer--;
    }


    if (fb->phase == 2)
    {
        if (CameraControllerPtr->velocity.x < 0x800)
            CameraControllerPtr->velocity.x += 100;

        const int BEGIN_DRUM_TRACK_X = 0x120000;
        const int DRUM_TRACK_FADE_X = 0x100000;
        if (CameraControllerPtr->position.x >= fb->camMaxX - BEGIN_DRUM_TRACK_X)
        {
            u32 volume = CameraControllerPtr->position.x - fb->camMaxX + BEGIN_DRUM_TRACK_X;
            volume = (volume * 127) / DRUM_TRACK_FADE_X;
            if (volume > 127) volume = 127;
            NNS_SndPlayerSetTrackVolume(0x208FC08, TRACKS_FADE_IN_2, volume);
        }

        if (CameraControllerPtr->position.x >= fb->camMaxX)
        {
            fb->phase = 3;
            CameraControllerPtr->velocity.x = 0;
            
            u32* pointerToViewBlock = (u32*)0x0208B184;
            View* view = (View*)(*pointerToViewBlock + sizeof(View)*fb->ViewID);

            fb->battleMaxX = (view->x + view->width) << 12;
            fb->battleMinX = fb->battleMaxX - (16 << 16);
            fb->battleMaxX -= (1 << 16);
        }
    }

    if (fb->phase == 3)
    {
		if ((fb->position.y < fb->killLine) && (!fb->dead))
		{
			fb->dead = true;
            fb->playerLookAtBowser = false;
			PlaySNDEffect(SE_EMY_OLKP_CRY, 0);
            PlaySNDEffect(SE_EMY_OLKP_FALL_DOWN, 0);
		}
		
        if ((fb->position.y < fb->killLine) && (fb->Mario->isOnGround))
        {
            fb->endtimer++;
            
            if (fb->endtimer == 1)
            {
                bmgFlagSomething |= 0x1;
				isTimeStopped |= 0x10;
			
                StopMusicWithFade(0x1E);
                freezeWorld(true);
                fb->Mario->unk980 = (u32)&MarioActor_CutsceneState;
				SetPlayerAnimation(fb->Mario, 0, 1, 0, 0x1000);
                DisableMarioYRotationForce= true;           
                fb->Mario->xAccel = 0;
                fb->Mario->xVelocity = 0;
                fb->Mario->RotY = 0;                   
            }
            
            if (fb->endtimer == 20)
            {
                PlayMusicFile(0x13, 0xFF);
                SetPlayerAnimation(fb->Mario, 0x63, 1, 0, 0x1000);         
                
                if (!*player)
                    PlaySNDEffect(SE_VOC_MA_WIN, &fb->Mario->unk05C);
                else
                    PlaySNDEffect(SE_VOC_LU_WIN, &fb->Mario->unk05C);
            }

            if (fb->endtimer == 220)
            {
                SetPlayerAnimation(fb->Mario, 0, 1, 0, 0x1000);         
            }			

            if (fb->endtimer == 420)
            {
                DisableMarioYRotationForce = false;  
                fb->Mario->unk980 = 0x2115AAC; 
                PlaySNDEffect(SE_OBJ_SWITCH_APP, 0);
                activateEvent(fb->spriteData & 0xFF);
                freezeWorld(false);
            }
        }
    }


    // Replicate solid collision that only interacts with Mario 
    u32 marioWidth = fb->Mario->unk13C / 2 + (2 << 12);
    u32 marioHeight = fb->Mario->unk140 + (2 << 12);

    u32 left = fb->position.x - (26 << 12)    - marioWidth;
    u32 right = fb->position.x + (26 << 12)   + marioWidth;
    u32 top = fb->position.y + (28 << 12)     + (2 << 12);
    u32 bottom = fb->position.y - (22 << 12)  - marioHeight;

    if (fb->Mario->yPos > bottom && fb->Mario->yPos < top)
    {
        if (fb->Mario->xPos < fb->position.x && fb->Mario->xPos > left)
        {
            fb->Mario->xPos = left;
            FinalBoss_marioCollision(fb, fb->Mario);
        }
        else if (fb->Mario->xPos > fb->position.x && fb->Mario->xPos < right)
        {
            fb->Mario->xPos = right;
            FinalBoss_marioCollision(fb, fb->Mario);
        }
    }


    u32* cameraX = (u32*)(0x020CA2EC);
    u32* zoom = (u32*)(0x212AE8C);

    u32 leftBorder = *cameraX << 12;
    u32 rightBorder = leftBorder + (*zoom << 8);

    if (fb->Mario->xPos < leftBorder)
    {
        fb->Mario->xPos = leftBorder;
        if (fb->Mario->xVelocity < 0)
            fb->Mario->xVelocity = 0;
    }
    else if (fb->Mario->xPos > rightBorder)
    {
        fb->Mario->xPos = rightBorder;
        if (fb->Mario->xVelocity > 0)
            fb->Mario->xVelocity = 0;
    }
    
    // ma->blockGlitchRelated == 3 maybe check both? this one can fire in other situations as well. maybe 24F as well...
    if (fb->Mario->unk24F == 2 && fb->Mario->xPos < leftBorder + (8 << 12))
    {
		PlaySNDEffect(SE_PLY_DOWN, 0);
        fb->Mario->DeathState = 0x02119B24;

        fb->Mario->unk980 = (u32)0x210E85C;
        SetPlayerAnimation(fb->Mario, 0x5F, 0, 0, 0x1000);

        fb->Mario->RotX = 0;
        fb->Mario->RotY = 0;
        fb->Mario->RotZ = 0;

        bmgFlagSomething |= 0x1;
		isTimeStopped |= 0x10;
        freezeFlag = 1;
    }

    if (CameraControllerPtr)
	{
        CameraControllerPtr->position.x += CameraControllerPtr->velocity.x;
	}
	
	if (fb->limitbounds)
	{
		fb->boundsmiddle += CameraControllerPtr->velocity.x;
		
		u32* ViewStart = (u32*)0x20CAE60;
		u32* ViewEnd = (u32*)0x20CAE64;
		
		*ViewStart = fb->boundsmiddle - 0x90000;
		*ViewEnd = fb->boundsmiddle + 0x90000;
	}

    return true;
}


// Intro

void FinalBoss_begin_Intro(FinalBoss* fb)
{
    fb->state = FinalBoss_Intro;
    fb->phase = 0;
    fb->action = 0;

    fb->timer = 0;
    fb->timer2 = 0;

    fb->updateAnimation = true;
    fb->playerLookAtBowser = false;

    fb->rotation.y = -0x1000;
    fb->yPos = fb->baseline - 0x10000;

    u32* zoneBlockSize = (u32*)(0x0208B150);
    u32* zoneBlock = (u32*)0x0208B188;
    Zone* zone0 = NULL;

    for (u32 i = 0; i < *zoneBlockSize; i++)
    {
        Zone* zone = (Zone*)(*zoneBlock + sizeof(Zone)*i);
        if (zone->id == 0)
        {
            zone0 = zone;
            break;
        }
    }

    if (zone0 == NULL)
        OS_Panic();

    fb->battleMinX = (zone0->x << 12) + (3 << 16);
    fb->battleMaxX = (zone0->x << 12) + (18 << 16) - (3 << 16);

    modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Idle, 0, 0x1000, 0);
}

void MarioState_Cutscenes(EnemyClassObject* Mario)
	{
		UpdatePlayerAnimation(Mario);
		return;
	}

void FinalBoss_update_Intro(FinalBoss* fb)
{
    // FUCK ME AND THIS CODE
    if (fb->action == 0)
    {
		EnemyClassObject* Mario = (EnemyClassObject*)fb->Mario;
		u32* cameraX = (u32*)(0x020CA2EC);
        u32 minCamX = 0;

        u32* zoneBlockSize = (u32*)(0x0208B150);
        u32* zoneBlock = (u32*)0x0208B188;
                
        for (u32 i = 0; i < *zoneBlockSize; i++)
        {
            Zone* zone = (Zone*)(*zoneBlock + sizeof(Zone)*i);
            if (zone->id == 0)
            {
                minCamX = zone->x << 12;
                break;
            }
        }
		
		if (Mario->position.x >= minCamX)
		{
			if (!fb->started)
			{
				StopMusicWithFade(0x1E);
                PlayMusicFile(0x15, 0xFF);
				fb->Mario->xAccel = 0;
				fb->Mario->xVelocity = 0x1000;
				freezeWorld(true);
				createActor(0x146, 0, &Mario->position, 0, 0, 0);
				fb->started = true;
			}
			
			if (fb->started)
			{
				fb->Mario->xVelocity = 0x1000;
				fb->Mario->xAccel = 0x1000;
				
				if (*cameraX < minCamX)
					CameraControllerPtr->velocity.x = 0x1200;
			}

            if ((*cameraX << 12) >= minCamX - 0x20000)
                fb->copterSoundState = CopterHover;
			
			if ((*cameraX << 12) >= minCamX && (Mario->position.x >= (minCamX + 0x18000)))
			{
				fb->limitbounds = true;
				fb->boundsmiddle = minCamX + 0x90000;
				CameraControllerPtr->velocity.x = 0;
				activateEvent((fb->spriteData >> 8) & 0xFF);
				fb->Mario->xAccel = 0;
				fb->Mario->xVelocity = 0;
				fb->action = 1;
				fb->timer = 0;
			}
		}
    }

    // Wait
    else if (fb->action == 1)
    {
        if (fb->timer >= 5)
        {
            fb->action = 2;
            fb->playerLookAtBowser = true;
            NNS_SndPlayerSetTrackVolume(0x208FC08, TRACKS_FADE_IN_1 | TRACKS_FADE_IN_2, 0);
        }
    }

    // Fly in
    else if (fb->action == 2)
    {
        if (fb->position.y > fb->yPos)
            fb->position.y -= (fb->position.y - fb->yPos + 0x4000) / 40;
        else
        {
            fb->position.y = fb->baseline - 0x10000;
            fb->action = 3;
        }
    }

    else
    {
        int bobbingFactor = 120;

        // Start Roar
        if (fb->action == 3)
        {
            if (fb->timer % 60 == 0)
            {
                fb->action = 4;
                fb->timer = 0;
                modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Roar, 0, 0x1000, 0);
            }
        }

        // Roar
        else if (fb->action == 4)
        {
            if (fb->timer == 50)
            {
				Rumble(1);
                shakeScreen(6, fb->ViewID);
                PlaySNDEffect(SE_EMY_OLKP_BARK, &fb->position);
            }

            if (fb->timer == 160)
            {
                modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Idle, 0, 0x1000, 0);
            }

            if (fb->timer == 170)
            {
                fb->playerLookAtBowser = false;
                fb->jrState = 1;
            }

            if (fb->timer >= 230)
            {
                fb->action = 5;
                fb->timer = 0;
            }
        }

        // End Bobbing
        else
        {
            if (fb->timer == 60)
                freezeWorld(false);

            if (fb->timer >= 120)
            {
                FinalBoss_begin_Swoop(fb, 1);
                return;
            }

            bobbingFactor -= fb->timer;
        }

        fb->position.y = ((((-(s32)(cosLerp(fb->timer2 * 260)) * 0x6000) / 4096) * bobbingFactor) / 120) + 0x6000 + fb->yPos;
        fb->timer2++;
    }

    fb->timer++;
}


// Swoop

void FinalBoss_begin_Swoop(FinalBoss* fb, u32 mode)
{
    fb->state = FinalBoss_Swoop;
    fb->timer = 0;
    fb->swoopOverBLine = false;
    fb->lastVelocityX = 0;
    fb->mode = mode;

    u32 mX = fb->Mario->xPos;
    u32 mY = fb->Mario->yPos;
    
    u32 sX = fb->position.x;
    u32 sY = fb->position.y;
    
    float x1, x2, x3, y1, y2, y3;

    if (mode == 1 || mode == 3)
    {
        fb->swoopX = sX + (32 << 12);
        fb->swoopY = sY - (16 << 12);

        x1 = (float)sX - fb->swoopX;
        x2 = 0;
        x3 = (float)fb->battleMaxX - fb->swoopX;
        if (mode == 1) x3 -= (float)(1 << 16);

        y1 = (float)sY - fb->swoopY;
        y2 = 0;
        y3 = (float)fb->baseline - fb->swoopY;
        
        fb->swoopDirection = x1 >= 0;

        fb->swoopMaxSpeed = 10000;
        if (mode == 1)
            fb->velocity.x = -fb->swoopMaxSpeed / 2;
        else
            fb->velocity.x = 0;
    }
    else if (mode == 2)
    {
        fb->swoopX = fb->battleMinX + (fb->battleMaxX - fb->battleMinX) / 2;
        fb->swoopY = fb->baseline - (3 << 16);

        x1 = (float)sX - fb->swoopX;
        x2 = 0;
        x3 = -x1;
        
        y1 = (float)sY - fb->swoopY;
        y2 = 0;
        y3 = y1;

        fb->swoopMaxSpeed = 12000;
    }
    else
    {
        fb->swoopX = mX-(96<<12);
        fb->swoopY = mY-(32<<12);

        x1 = (float)sX - fb->swoopX;
        x2 = 0;
        x3 = -x1;
        
        y1 = (float)sY - fb->swoopY;
        y2 = 0;
        y3 = y1;

        fb->swoopMaxSpeed = 12000;
    }
    
    float denominator = (x1 - x2) * (x1 - x3) * (x2 - x3);
    float swoopA      = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denominator;
    float swoopB      = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denominator;
    float swoopC      = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denominator;

    fb->swoopA = swoopA * 268435456.0f; // 1 << 28
    fb->swoopB = swoopB * 268435456.0f; // 1 << 28
    fb->swoopC = swoopC;
}

void FinalBoss_update_Swoop(FinalBoss* fb)
{
    fb->velocity.x += fb->swoopMaxSpeed / 30;
    if (fb->velocity.x > fb->swoopMaxSpeed)
        fb->velocity.x = fb->swoopMaxSpeed;

    if (!fb->swoopDirection)
        fb->position.x += fb->velocity.x;
    else
        fb->position.x -= fb->velocity.x;
    
    s32 sX = fb->position.x - fb->swoopX;
    fb->position.y = (u32)(((fb->swoopA * (sX >> 10) * (sX >> 10)) >> 8) + (((fb->swoopB >> 8) * (sX >> 8)) >> 12) + fb->swoopC + fb->swoopY);
    
    if (fb->jrState == 3)
    {
        if (fb->position.y > fb->baseline + (8 << 16))
        {
            fb->copterSoundState = CopterSilent;
            if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x30)
                FinalBoss_begin_SpawnBulletBills(fb);
            else if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x60)
                FinalBoss_begin_SpawnFire(fb);
            else
                FinalBoss_begin_ChaseIntro(fb);
            return;
        }
        goto FinalBoss_update_Swoop_end;
    }
    else if (fb->jrState > 3) {
        if (fb->position.y > fb->baseline + (8 << 16))
        {
            fb->copterSoundState = CopterSilent;
            FinalBoss_begin_ChaseIntro(fb);
            return;
        }
        goto FinalBoss_update_Swoop_end;
    }

    if (fb->position.y < fb->baseline && fb->swoopOverBLine) fb->swoopOverBLine = false;
    if (fb->position.y > fb->baseline)
    {
        if (fb->phase == 2)
        {
            FinalBoss_begin_Follow(fb, 2);
            return;
        }

        if (fb->mode == 3)
        {
            FinalBoss_begin_Swoop(fb, 2);
            return;
        }

        if (!fb->swoopOverBLine)
        {
            if (fb->phase == 0)
            {
                fb->phase = 1;
                FinalBoss_begin_Follow(fb, 0);
                return;
            }

            fb->swoopDirection = !fb->swoopDirection;
            fb->velocity.x *= -1;

            fb->swoopOverBLine = true;
        }

        if (fb->velocity.x < 0)
            fb->velocity.x = (fb->velocity.x * 17) / 20;
    }

    fb->lastVelocityX = fb->velocity.x;

    FinalBoss_update_Swoop_end:
    {
        s32 newRot = (fb->position.x - fb->lastPosition.x) * 2;
        if (newRot > 0x1000) newRot = 0x1000;
        else if (newRot < -0x1000) newRot = -0x1000;
        fb->rotation.y = (s16)newRot;
    }
}


// Spawn Bullet Bills

void FinalBoss_begin_SpawnBulletBills(FinalBoss* fb)
{
    fb->state = FinalBoss_SpawnBulletBills;

    fb->timer = 0;
    fb->timer2 = 0;

    fb->rotation.y = 0;
    fb->yPos = fb->baseline - (12 << 12);
}

void FinalBoss_update_SpawnBulletBills(FinalBoss* fb)
{
    fb->timer++;

    if (fb->timer == 60)
        PlaySNDEffect(SE_OBJ_HONE_IN_TSUBO, 0); // copy of SE_EMY_KILLER_SHOT

    if (fb->timer == 90)
    {
        u32* pointerToViewBlock = (u32*)0x0208B184;
        View* view = (View*)(*pointerToViewBlock + sizeof(View)*fb->ViewID);

        Vec3 position;
        position.y = (0xFFFFFFFF - (view->y << 12)) - (40 << 12);
        position.z = 0;

        for (int i = 0; i < 5; i++)
        {
            if (i & 0x1)
                position.x = fb->battleMaxX + (4 << 16);
            else
                position.x = fb->battleMinX - (4 << 16);

            position.y -= (2 << 16);

            createActor(27, 0x00000000, &position, 0, 0, 0);
        }
    }

    else if (fb->timer == 360)
    {
        if (fb->jrState <= 3)
            fb->jrState = 2;

        fb->position.x = fb->battleMinX + (fb->battleMaxX-fb->battleMinX) / 2;
        fb->position.y = fb->baseline + (8 << 16);
    
        fb->copterSoundState = CopterHover;
    }

    else if (fb->timer > 400)
        FinalBoss_transitionToNextSegment(fb);
}


// Follow

void FinalBoss_begin_Follow(FinalBoss* fb, u32 mode)
{
    fb->state = FinalBoss_Follow;
    fb->action = 0;
    fb->actionChangeTime = 0;
    fb->mode = mode;
    if (mode != 0) fb->velocity.x = 0;
    fb->lastVelocityX = fb->velocity.x;

    fb->timer = 0;
    fb->timer2 = 0;
    
    fb->yPos = fb->position.y;

    fb->followLine = fb->baseline - (1 << 16);
}

void FinalBoss_update_Follow(FinalBoss* fb)
{
    s32 deltaX = fb->Mario->xPos - fb->position.x;

    s32 direction = 1;
    if (deltaX < 0) direction = -1;

    s32 speedFactor;
    s32 maxSpeed;
    s32 edgeSpeed;
    s32 bobbingDist = 19;
    s32 bobbingSpeed = 16;

    if (fb->mode == 0)
    {
        speedFactor = 140;
        maxSpeed = 10000;
        edgeSpeed = 100;
    }
    else if (fb->mode == 1)
    {
        speedFactor = 200;
        maxSpeed = 14000;
        edgeSpeed = 200;
    }

    // aka chase
    else if (fb->mode >= 2)
    {
        bobbingDist = 12;
        bobbingSpeed = 15;

        if (fb->mode == 3)
        {
            speedFactor = 170;
            maxSpeed = 12000;
            edgeSpeed = 150;
        }
        else
        {
            speedFactor = 140;
            maxSpeed = 10000;
            edgeSpeed = 100;
        }

        if (fb->yPos < fb->followLine)
            fb->yPos += 0x140;
        else
        {
            // Idle
            if (fb->action == 0)
            {
                if (fb->actionChangeTime == 0) fb->actionChangeTime = 30;
                fb->timer2++;
                
                if (fb->timer2 > fb->actionChangeTime)
                {
                    fb->timer2 = 0;
                    
                    u32 rand = RNG(2);

                    switch (rand)
                    {
                    case 0:     // Stomp
                        fb->action = 1;
                        break;
                    default:    // Bomb Throw
                        fb->action = 2;
                        fb->timer2 = 20 + RNG(80);
                        fb->timer3 = 0;
                        break;
                    }                                    
                }
            }

            // Throw enemy
            else if (fb->action == 1)
            {
                if (fb->timer2 == 0)
                {
                    // Make sure only to throw at the top of bobbing cycle.
                    // This decreases the chance of bob-ombs getting stuck in the solid collision a lot
                    if ((deltaX > 0 && fb->velocity.x > 0) || (deltaX < 0 && fb->velocity.x < 0))
                    {
                        modelAnim3d_init(&fb->koopaModel, KoopaAnimation_ThrowEnemy, 0, 0x1000, 0);
                        fb->timer2 = 1;
                    }
                }
                else
                {
                    fb->timer2++;

                    if (fb->timer2 == 35)
                    {
						PlaySNDEffect(SE_EMY_OLKP_FALL_DOWN, &fb->position);
                        PlaySNDEffect(SE_PLY_WALK_ON_ROPE, &fb->position);
                        Vec3 pos = fb->position;
                        pos.y += (20 << 12);

                        EnemyClassObject* bomb = createActor(35, 0x31000200, &pos, 0, 0, 0);
                        bomb->position.z = 0x100000;

                        s32 xVel = fb->velocity.x;
                        if (xVel >= 0)
                        {
                            xVel += 0x200;
                            if (xVel < 0x2000) xVel = 0x2000;
                            bomb->rotation.y = 0x2000;
                        }
                        else
                        {
                            xVel -= 0x200;
                            bomb->direction = 1;
                            if (xVel > -0x2000) xVel = -0x2000;
                            bomb->rotation.y = -0x2000;
                        }

                        bomb->velocity.x = xVel;
                        bomb->velocity.y = 0x2380;
                    }

                    if (fb->timer2 >= 60)
                    {
                        fb->timer2 = 0;
                        modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Idle, 0, 0x1000, 0);
                        fb->action = 0;
                    }
                }
            }

            // Prepare Stomp
            else if (fb->action == 2)
            {
                if (fb->timer3 > fb->timer2)
                {
                    if (fb->position.x > (fb->Mario->xPos - (2 << 16)) && fb->position.x < (fb->Mario->xPos + (2 << 16)))
                    {
                        FinalBoss_begin_Stomp(fb);
                        return;
                    }
                    else
                        fb->timer3 = fb->timer2 - RNG(20);
                }
                else
                    fb->timer3++;
            }
        }
    }


    if (fb->action <= 2)
        fb->timer++;


    s32 speedDelta = speedFactor * direction;

    fb->velocity.x += speedDelta;
    
    if (fb->velocity.x > maxSpeed)
        fb->velocity.x = maxSpeed;
    else if (fb->velocity.x < -maxSpeed)
        fb->velocity.x = -maxSpeed;

    s32 newPos = fb->position.x + fb->velocity.x;
    s32 minX = CameraControllerPtr->position.x - 0x90000;
    s32 maxX = CameraControllerPtr->position.x + 0x90000;
    
    if (newPos > maxX) fb->velocity.x -= edgeSpeed;
    else if (newPos < minX) fb->velocity.x += edgeSpeed;

    if (fb->action <= 2)
    {
        s32 newRot = fb->velocity.x * 2;
        if (newRot > 0x1000) newRot = 0x1000;
        else if (newRot < -0x1000) newRot = -0x1000;
        fb->rotation.y = (s16)newRot;
    }

    fb->lastVelocityX = fb->velocity.x;

    fb->position.x += fb->velocity.x;

    if (fb->phase < 2 && fb->jrState >= 3)
    {
        fb->timer2++;

        if (fb->timer2 > 120)
        {
            fb->yPos += 0x1000;
            if (fb->position.y > fb->baseline + (8 << 16))
            {
                fb->copterSoundState = CopterSilent;
                if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x30)
                    FinalBoss_begin_SpawnBulletBills(fb);
                else if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x60)
                    FinalBoss_begin_SpawnFire(fb);
                else
                    FinalBoss_begin_ChaseIntro(fb);
                return;
            }
        }
    }

    fb->lastPosition.y = fb->position.y;
    fb->position.y = fb->yPos + ((s32)(sinLerp((fb->timer * 5215) / bobbingSpeed)) * bobbingDist);

    fb->velocity.y = fb->position.y - fb->lastPosition.y;
}


// Spawn Fire

void FinalBoss_begin_SpawnFire(FinalBoss* fb)
{
    fb->state = FinalBoss_SpawnFire;

    fb->timer = 0;
    fb->timer2 = 0;

    fb->rotation.y = 0;
    fb->yPos = fb->baseline - (12 << 12);
}

void FinalBoss_update_SpawnFire(FinalBoss* fb)
{
    fb->timer++;

    if (fb->timer >= 90 && fb->timer <= 500 && (fb->timer % 40) == 0)
    {
        Vec3 position;
        position.y = fb->baseline + (6 << 16);
        position.z = 0;

        u32 index = ((fb->timer - 90) / 40 + 4) % 8;
        position.x = fb->battleMinX - (24 << 12) + (index * (2 << 16));
        
        BowserJr* jr = (BowserJr*)fb->bowserJr;
        s32 jrDist = jr->position.x - (8 << 12) - position.x;
        if (jrDist < 0) jrDist *= -1;

        if (jrDist > (16 << 12))  // Don't hit Jr.
		{
			PlaySNDEffect(SE_OBJ_FIREBALL_DISAPP, &position);
            createActor(0x14C, 0x00000000, &position, 0, 0, 0);
		}
    }

    else if (fb->timer == 760)
    {
        if (fb->jrState <= 3)
            fb->jrState = 2;

        fb->position.x = fb->battleMinX + (fb->battleMaxX - fb->battleMinX) / 2;
        fb->position.y = fb->baseline + (8 << 16);

        fb->copterSoundState = CopterHover;
    }

    else if (fb->timer > 790)
        FinalBoss_transitionToNextSegment(fb);
}


// Fire + Bills + Chase Intro

void FinalBoss_begin_ChaseIntro(FinalBoss* fb)
{
    fb->jrState = 0xFF;     // RIP :(

    fb->state = FinalBoss_ChaseIntro;
    fb->action = 0;

    fb->timer = 0;
    fb->timer2 = 0;
    fb->musictimer = 0;

    fb->rotation.y = 0;

    fb->position.x = fb->battleMinX + (fb->battleMaxX - fb->battleMinX) / 2;
    fb->position.y = fb->baseline + (8 << 16);

    fb->yPos = fb->baseline - (28 << 12);

#ifdef DEBUG_SKIP_P1
    fb->timer = 1160;
#endif
}

void FinalBoss_update_ChaseIntro(FinalBoss* fb)
{
    fb->timer++;

    if (fb->action == 0)
    {
        if (fb->timer == 50 || fb->timer == 170 || fb->timer == 290)
            PlaySNDEffect(SE_OBJ_HONE_IN_TSUBO, 0); // copy of SE_EMY_KILLER_SHOT

        if (fb->timer == 80 || fb->timer == 200 || fb->timer == 320)
        {
            u32* pointerToViewBlock = (u32*)0x0208B184;
            View* view = (View*)(*pointerToViewBlock + sizeof(View)*fb->ViewID);

            Vec3 position;
            position.y = (0xFFFFFFFF - (view->y << 12)) - (10 << 16);
            position.z = 0;
            position.x = fb->battleMinX - (5 << 16);

            // Banzai Bill
            createActor(238, 0x00000000, &position, 0, 0, 0);

            position.x = fb->battleMaxX + (5 << 16);
            position.y += (3 << 16);

            // Bullet Bills
            createActor(27, 0x00000000, &position, 0, 0, 0);
            position.y -= (6 << 16);
            createActor(27, 0x00000000, &position, 0, 0, 0);
        }

        if (fb->timer >= 500 && fb->timer <= 965 && (fb->timer % 35) == 0)
        {
            Vec3 position;
            position.y = fb->baseline + (6 << 16);
            position.z = 0;

            u32 number = (fb->timer - 500) / 35;
            u32 column = ((number + 2) % 9) * 2;
            if (number >= 7) column -= 1;
            position.x = fb->battleMinX - (2 << 16) + (column << 16);

            PlaySNDEffect(SE_OBJ_FIREBALL_DISAPP, &position);
            createActor(0x14C, 0x00000000, &position, 0, 0, 0);
        }

        if (fb->timer == 1200)
            fb->copterSoundState = CopterHover;

        if (fb->timer > 1235)
        {
            if (fb->position.y > fb->yPos)
                fb->position.y -= (fb->position.y - fb->yPos + 0x4000) / 40;
            else
            {
                fb->action = 1;
                fb->timer = 0;
                fb->position.y = fb->yPos;
            }
        }

        if (fb->timer == 1255)
            fb->playerLookAtBowser = true;
    }

    else
    {
        int bobbingFactor = 120;

        // Wait + Start Roar
        if (fb->action == 1)
        {
            if (fb->timer == 20)
            {
                fb->action = 2;
                fb->timer = 0;
                modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Roar, 0, 0xAAA, 0);
            }
        }

        // Roar
        else if (fb->action == 2)
        {
            if (fb->timer == 75)
            {
				Rumble(1);
                shakeScreen(7, fb->ViewID);
                PlaySNDEffect(SE_EMY_OLKP_BARK, &fb->position);
            }

            if (fb->timer == 180)
			{
                shakeScreen(0, fb->ViewID);
				Rumble(1);
			}

            if (fb->timer >= 180)
                fb->musictimer++;

            if (fb->timer >= 240)
            {
                fb->action = 3;
                fb->timer = 0;
                modelAnim3d_init(&fb->koopaModel, KoopaAnimation_Idle, 0, 0x1000, 0);
            }
        }

        // End Bobbing
        else
        {
            if (fb->timer == 30)
            {
                Vec3 position;
                position.x = fb->battleMinX - (8 << 12);
                position.y = fb->baseline - (104 << 12);
                position.z = 0;
                
                // Spawn Signboard
				PlaySNDEffect(SE_OBJ_SWITCH_APP, &position);
                EnemyClassObject* Sign = (EnemyClassObject*)createActor(77, 0x00010001, &position, 0, 0, 0);
                SpawnParticle(52, &position);
				Sign->DrawDist.x = 0x80;
				Sign->DrawDist.y = 0x80;
            }

            if (fb->timer >= 120)
            {
                u32* pointerToViewBlock = (u32*)0x0208B184;
                View* view = (View*)(*pointerToViewBlock + sizeof(View)*fb->ViewID);

                // Start Chase Phase
                fb->phase = 2;
                fb->baseline += (1 << 16);
                fb->battleMaxX = ((view->x + view->width) << 12) - (3 << 16);

                FinalBoss_begin_Follow(fb, 2);
                return;
            }

            fb->musictimer++;
            bobbingFactor -= fb->timer;
        }

        if (fb->musictimer > 240) fb->musictimer = 240;
        if (fb->musictimer)
            NNS_SndPlayerSetTrackVolume(0x208FC08, TRACKS_FADE_IN_1, (fb->musictimer * 127) / 240);

        fb->position.y = ((((-(s32)(cosLerp(fb->timer2 * 260)) * 0x6000) / 4096) * bobbingFactor) / 120) + 0x6000 + fb->yPos;
        fb->timer2++;
    }

}


// Stomp Player

void FinalBoss_begin_Stomp(FinalBoss* fb)
{
    fb->state = FinalBoss_Stomp;
    fb->action = 0;

    fb->timer = 0;
    fb->timer2 = 0;
    fb->velocity.x = 0;
    fb->velocity.y = 0;
    fb->wigglePosX = fb->position.x;

    fb->liftUp = false;

    modelAnim3d_init(&fb->copterModel, CopterAnimation_Spin, 0, 0x5000, 0);
}

void FinalBoss_update_Stomp(FinalBoss* fb)
{
    if (fb->rotation.y > 0)
    {
        fb->rotation.y -= 0x180;
        if (fb->rotation.y < 0)
            fb->rotation.y = 0;
    }
    else if (fb->rotation.y < 0)
    {
        fb->rotation.y += 0x180;
        if (fb->rotation.y > 0)
            fb->rotation.y = 0;
    }

    fb->timer++;

    if (fb->action == 0)
    {
        if (fb->velocity.y > (-0x5000))
            fb->velocity.y -= 0x550;

        fb->position.x = fb->wigglePosX + (RandomNumberGenerator() % (4 << 12)) - (2 << 12);

        if (fb->timer == 20)
            fb->copterSoundState = CopterDive;
        
        if (fb->timer > 40)
        {
            bool stompDown = false;
            bool hitNonbreakable = false;
            
            if (!fb->liftUp)
            {
                u32 xPos = (fb->position.x >> 12) - 24;
                u32 yPos = (-fb->position.y >> 12) + 24;

                Vec3 pos = fb->position;
                pos.y -= (3 << 16);
                SpawnParticle(159, &pos);

                for (int i = 0; i < 4; i++)
                {
                    u32 tb = getTileBehaviorAtPos2(xPos + 16 * i, yPos);
                    u32 tbType = (tb >> 16) & 0xFF;

                    int particle = ParticlesForTile(tb);

                    if (tbType == 0x01 || tb == 0x80000000)
                    {
                        stompDown = true;
                        hitNonbreakable = true;
                    }
                    else if (particle != -1)
                    {
                        setTileTable[0] = 0;
                        changeTile(ptrToLevelActor, xPos + 16 * i, yPos, 0);
                        setTileTable[0] = 0;
                        
                        Vec3 pos = fb->position;
                        pos.x = ((xPos / 16 + i) << 16);
                        pos.y = 0xFFFFFFFF - ((yPos / 16) << 16);

                        u32 spriteData = 0x00000030 | (particle << 20);
                        createActor(0xF1, spriteData, &pos, 0, 0, 0);
                        PlaySNDEffect(SE_OBJ_BLOCK_BREAK, &pos);
						Rumble(1);

                        stompDown = true;
                    }
                }

                if (stompDown)
                {
					PlaySNDEffect(SE_EMY_OLKP_LAND, &fb->position);

                    if (!hitNonbreakable)
					{
                        stompDown = false;
                        for (int i = 0; i < 4; i++)
                        {
                            u32 tb = getTileBehaviorAtPos2(xPos + 16 * i, yPos + 16);
                            u32 tbType = (tb >> 16) & 0xFF;
                            if (tbType == 0x01  || tb == 0x80000000 || tbType == 0x08 || tbType == 0x10 || tbType == 0x05)
                            {
                                stompDown = true;
                                fb->liftUp = true;
                                break;
                            }
                        }
                    }
                }
            }
            else
                stompDown = true;

            if (!stompDown)
                fb->position.y += fb->velocity.y;
            else
            {
                Vec3 pos = fb->position;
                pos.y -= (2 << 16);
                SpawnParticle(52, &pos);

                fb->copterSoundState = CopterHover;
				
				Rumble(1);
                shakeScreen(8, fb->ViewID);
                modelAnim3d_init(&fb->copterModel, CopterAnimation_Spin, 0, 0x2000, 0);
                fb->action = 1;
                fb->timer = 0;
            }
        }
    }

    else
    {
        if (fb->timer == 20)
        {
			Rumble(1);
            shakeScreen(0, fb->ViewID);
            if (fb->hitMarioTimer)
                PlaySNDEffect(SE_PLY_TARZAN_ROPE, &fb->position); // pride
            else
                PlaySNDEffect(SE_PLY_CLIMB_ROPE, &fb->position); // annoyance
            fb->hitMarioTimer = 0;
        }
        
        if (fb->timer > 20)
        {
            fb->position.y += 0x2000;
            if (fb->position.y > fb->followLine)
            {
                u32* zoneBlockSize = (u32*)(0x0208B150);
                u32* zoneBlock = (u32*)0x0208B188;
                
                u32 mode = 0;
                for (u32 i = 0; i < *zoneBlockSize; i++)
                {
                    Zone* zone = (Zone*)(*zoneBlock + sizeof(Zone)*i);
                    if (zone->id < 10)
                    {
                        if (zone->id > mode && fb->position.x > (zone->x << 12))
                            mode = zone->id;
                    }
                }

                FinalBoss_begin_Follow(fb, mode+2);
                return;
            }
        }
    }
}

void FinalBoss_transitionToNextSegment(FinalBoss* fb)
{
    // This replaces the direct calls to begin_Swoop and begin_Follow,
    // so the correct segment will always be started if the player does
    // a super-quick kill on Bowser Jr

    // Step 1: decide exactly which phase we're supposed to be starting
    int phaseToStart; // 2, 3, or 4
    if (fb->jrState <= 3)
    {
        if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x30)
            phaseToStart = 2;
        else if (((BowserJr*)fb->bowserJr)->bowserJrUnk3[0x10] <= 0x60)
            phaseToStart = 3;
        else
            phaseToStart = 4;
    }
    else
        phaseToStart = 4;

    // Step 2: figure out where Bowser is supposed to descend to
    int desiredYPos;
    if (phaseToStart == 4)
        desiredYPos = fb->baseline - (28 << 12);
    else
        desiredYPos = fb->baseline - (12 << 12);

    // Step 3: make him slowly descend there
    if (fb->position.y > desiredYPos)
        fb->position.y -= (fb->position.y - desiredYPos + 0x4000) / 40;
    else
    {
        // Step 4: finally we can launch the proper state for the next phase

        if (phaseToStart == 2)
            FinalBoss_begin_Swoop(fb, 3);
        else if (phaseToStart == 3)
            FinalBoss_begin_Follow(fb, 1);
        else
        {
            FinalBoss_begin_ChaseIntro(fb);

            // Normally, begin_ChaseIntro is entered from update_Swoop.
            // If it's being entered from here, then the player *just*
            // got done with the either the bullets or podoboos challenge.
            // So we need to do a bit of extra work to skip the bullets-
            // -and-podoboos combined challenge in this phase and just
            // cut to the roar and the chase sequence.
            fb->copterSoundState = CopterHover;
            fb->action = 1;
            fb->position.y = fb->yPos;
            fb->playerLookAtBowser = true;
        }
    }
}

void* FinalBoss_dtor(FinalBoss* fb)
    {
        fb->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        fb->Unk4Vec2 = dtor2;
        fb->Unk3Vec2 = dtor2;
        fb->DeleteDist = dtor2;
        fb->DrawDist = dtor2;
        fb->ExistDist = dtor2;
        fb->unkv13 = dtor;
        fb->unkv12 = dtor;
        fb->unkv11 = dtor;
		
        UnregisterActivePhysics(&fb->activephysics);
		UnregisterActivePhysics(&fb->activephysics2);
		 
		ControlLock = false;
		DisableMarioYRotationForce = false;
		
		ViewStartAndEndForBoundsLimit[0] = 0;
		ViewStartAndEndForBoundsLimit[1] = 0;
		setBrightness(3, 0);

        return fb;
    }

void* FinalBoss_dtorFree(FinalBoss* fb)
    {
        FinalBoss_dtor(fb);
		freeToGameHeap(fb);
        return fb;
    }


// Misc Functions
void freezeWorld(bool freeze)
{
	if (freeze)
	{
        ControlLock = true;
        pressedKeys = 0;
		bmgFlagSomething |= 0x1;
		isTimeStopped |= 0x10;
	}
	else
	{
		ControlLock = false;
		bmgFlagSomething &= ~0x1;
		isTimeStopped &= ~0x10;
	}
}
