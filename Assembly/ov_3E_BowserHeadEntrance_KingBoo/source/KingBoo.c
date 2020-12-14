#include "game.h"
#include "registers.h"
#include "VariousFunctions.h"
#include "sfx.h"
#include "KingBoo.h"

extern bool ControlLock;
extern bool MusicOffMode;
extern bool DisableMarioYRotationForce;
extern u32 bmgFlagSomething;
extern u32 isTimeStopped;

const int BOOB_MODEL = 2699 - 0x83;
const int BOOB_ANIMATIONS = 2700 - 0x83;
const int ICEBALL = 2645 - 0x83;

const int BOOB_INTRO_MOVE_DISTANCE = 0x50000;
const int BOOB_INTRO_MOVE_SPEED = 0xA00;
const int BOOB_MOVE_SPEED = 0xA00;

const Vec3 BOOB_DEFAULT_SCALE = {0, 0x50000, 0x50000, 0x50000};
const S16Vec3 BOOB_DEFAULT_ROTATION = {0, 0, 0x2800, 0};

u32* ViewStart = (u32*)0x20CAE60;
u32* ViewEnd = (u32*)0x20CAE64;

extern u32 ViewStartAndEndForBoundsLimit[2];


u32 KingBoo_physics[] = 										
	{
		0x0,
		0x0,		
		0x50000,	
		0x50000,	
		0x00D30004,	
		0, 
		(u32)&KingBoo_Collision,
	};		
	
u32 KingBoo_physics2[] = 										
	{
		-0x20000,	
		0x10000,		
		0x10000,	
		0x60000,	
		0x00330004,	
		0x0, 
		(u32)&KingBoo_EnemyKillingCollision,
	};		

int KingBoo_loadFiles()
	{
		loadFileByExtId_3dModel(BOOB_MODEL, 0);
		loadFileByExtId_3dModel(BOOB_ANIMATIONS, 0);
		loadFileByExtId_3dModel(ICEBALL, 0);
		return 1;
	}

void KingBoo_setup(KingBoo* boob)
	{	
		// Init the model
		modelAnim3d_ctor(&boob->Model);
		modelAnim3d_setup(&boob->Model, getPtrToLoadedFile(BOOB_MODEL), getPtrToLoadedFile(BOOB_ANIMATIONS), 0, 0, 0);
		modelAnim3d_init(&boob->Model, 0, 0, 0x800, 0);
		
		// Init active physics; round.
		initActivePhysics(&boob->activephysics, boob, KingBoo_physics, 0);
		boob->activephysics.Shape = 1;
		registerCollider(&boob->activephysics);
		
		// Init enemy-active physics; square.
		initActivePhysics(&boob->activephysics2, boob, KingBoo_physics2, 0);
		registerCollider(&boob->activephysics2);
	
		// Set basic info; goes above player, starts in the intro state, set the timers to 0, get spritedata.
		boob->position.z = 0x220000;
		boob->scale = BOOB_DEFAULT_SCALE;
		boob->rotation = BOOB_DEFAULT_ROTATION;
		boob->state = 0;
		boob->statetimer = 0;
		boob->lastattack = 0;
		boob->deathevent = ((boob->spriteData) & 0xFF);			// Event the Boo will die on.
		boob->location = ((boob->spriteData >> 8) & 0xFF);		// Location in which the Boo will move. It stops once it's out of this location. 
																// Used to define the level bounds, as the sprite moves the level ones.
		int zoom = ((boob->spriteData >> 16) & 0xFF);			// Level zoom to scroll out to during the intro.
		
		// Get Mario's pointer, set him to the correct position.
		boob->Mario = (EnemyClassObject*)getPtrToPlayerActor();
		boob->Mario->position.x = boob->position.x + 0x120000;
		boob->Mario->direction = 1;
		boob->Mario->rotation.y = -0x3000;

		// Get the level brightness out of the Unk2 view settings. Set the brightness to that.
		boob->currentbrightness = -getUnk2ForView(boob->ViewID);
		setBrightness(1, boob->currentbrightness);

		// Set the beginning level bounds and background scroll bounds.
		ViewStartAndEndForBoundsLimit[0] = *ViewStart;
		ViewStartAndEndForBoundsLimit[1] = *ViewEnd;
		*ViewStart = boob->position.x + BOOB_INTRO_MOVE_DISTANCE;;
		*ViewEnd = *ViewStart + (zoom * 0x10000);
		
		MusicOffMode = true;				// We don't want level music to start during the intro.
		ControlLock = true;					// Locking controls...
		pressedKeys = 0;					// Should also stop moving if the player is still holding some keys.		
		
		SwapASwar(57, 2833 - 0x83);
		SwapASbnk(51, 2835 - 0x83);
		SwapASsar(17, 2834 - 0x83);
		
		SwapASwar(35, 2836 - 0x83);
		SwapASbnk(34, 2838 - 0x83);
		SwapASsar(0, 2837 - 0x83);
	}

void KingBoo_IntroState(KingBoo* boob)							// State 0
	{
		boob->statetimer++;
		int MovementFrames = BOOB_INTRO_MOVE_DISTANCE / BOOB_INTRO_MOVE_SPEED;
		int *player = (int*)(0x02085A50);
		u16 *StageZoom = (u16*)(0x020CADB4);
		
		// Play a scared sound effect. 
		if (boob->statetimer == 1) 
			{
				bmgFlagSomething |= 0x1;
				isTimeStopped |= 0x10;
		
				MarioActor* mA = (MarioActor*)boob->Mario;
				mA->unk980 = (u32)&MarioState_Cutscenes;
				SetPlayerAnimation(boob->Mario, 0x85, 1, 0, 0x500);	
				
				PlaySNDEffect(SE_EMY_FO_GLOW_UP, &boob->position);
				
				if (!*player)
					PlaySNDEffect(SE_VOC_MA_OTOTO, &boob->Mario->position);
				else
					PlaySNDEffect(SE_VOC_LU_OTOTO, &boob->Mario->position);
			}
			
		// Move the Boo in.
		if ((boob->statetimer > 19) && (boob->statetimer < 21 + MovementFrames))
			{
				int zoom = ((boob->spriteData >> 16) & 0xFF);
				if (*StageZoom != (zoom << 8)) *StageZoom+=0x10;
				
				boob->position.x += BOOB_INTRO_MOVE_SPEED;
			}
			
		// Alright, we're all on-screen now, so play a laughing animation and sound. Mario does his 
		// heroic little pose.
		if (boob->statetimer == 30 + MovementFrames) 
			{
				PlaySNDEffect(SE_EMY_TERESA, &boob->position);
				SetPlayerAnimation(boob->Mario, 0x84, 1, 0, 0x1200);
				modelAnim3d_init(&boob->Model, 0, 0, 0x8000, 0);
			}		
			
		// This happens when Mario throws his arm in the pose. Nice sound effect to go along with it.
		if (boob->statetimer == 64 + MovementFrames) 
			{
				if (!*player)
					PlaySNDEffect(SE_VOC_MA_THROW, &boob->Mario->position);
				else
					PlaySNDEffect(SE_VOC_LU_THROW, &boob->Mario->position);
			}
				
		// Restore animations to normal now that's done...
		if (boob->statetimer == 100 + MovementFrames) 	
			{
				modelAnim3d_init(&boob->Model, 0, 0, 0x800, 0);	
				SetPlayerAnimation(boob->Mario, 0x86, 1, 0, 0x1200);
			}
			
		// Switch Mario's state back to normal, then turn on music and unlock the controls. Go to move selection state.
		if (boob->statetimer == 117 + MovementFrames) 
			{
				bmgFlagSomething &= ~0x1;
				isTimeStopped &= ~0x10;
				MarioActor* mA = (MarioActor*)boob->Mario;
				mA->unk980 = 0x2115AAC;		
				
				MusicOffMode = false;
				moveVolumeForPlayer(127, 1);
				StartMusicNumber(0x63);
				ControlLock = false;
				
				boob->state = 1;
			}
	}
	
void KingBoo_ChooseAttack(KingBoo* boob)		// State 1
	{
		// Attack pattern always is [SLOW CHASE] -> [ATTACK] -> [SLOW CHASE] -> [ATTACK] etc...
		if (boob->lastattack == 2)
			boob->state = 3 + RNG(2);
		else
			boob->state = 2;
		
		// Remember what move we chose for next time and reset the state timer.
		boob->lastattack = boob->state;
		boob->statetimer = 0;
	}
	
void KingBoo_ChaseSlow(KingBoo* boob)			// State 2
	{
		// Slowly move. After 5 seconds, go back to attack selection state. 
		boob->statetimer++;
		boob->position.x += BOOB_MOVE_SPEED;

		if (boob->statetimer == 300)
			boob->state = 1;
	}
	
void KingBoo_ChaseFast(KingBoo* boob)			// State 3
	{
		boob->statetimer++;
		
		// Play animation 4 and laugh to indicate something's about to happen...
		if (boob->statetimer == 1)
			{
				PlaySNDEffect(SE_EMY_TERESA, &boob->position);
				modelAnim3d_init(&boob->Model, 4, 0, 0x800, 0);
			}
		
		// Chase Mario with increased speed while playing sound effects. 
		if ((boob->statetimer > 60) && (boob->statetimer < 350))
			{
				if (!(boob->statetimer % 20)) PlaySNDEffect(SE_EMY_FO_FADE_AWAY, &boob->position);		
				modelAnim3d_init(&boob->Model, 3, 0, 0x1000 - (boob->statetimer * 2), 0);
				
				int MoveSpeed = BOOB_MOVE_SPEED + 0x1000;
				
				boob->position.x += MoveSpeed;
			}
		
		// Back to regular animation and the attack select state...
		if (boob->statetimer == 350)
			{
				modelAnim3d_init(&boob->Model, 0, 0, 0x800, 0);
				boob->state = 1;
			}
	}
	
void KingBoo_ShootFireball(KingBoo* boob)			// State 4
	{	
		boob->statetimer++;
		
		// Laugh to indicate something's going to happen,
		// and also begin the fireball-shoot sound...
		if (boob->statetimer == 1)
			{
				PlaySNDEffect(SE_EMY_TERESA, &boob->position);
				PlaySNDEffect(SE_PLY_CLIMB_ROPE, &boob->position);
			}

		// In addition to that, do a full spin around, so it's different from State 3.
		if ((boob->statetimer > 1) && (boob->statetimer < 85))
			{		
				boob->rotation.y += 2385;
			}
		
		// Once we're done spinning, quickly grow 
		if ((boob->statetimer > 85) && (boob->statetimer < 90))	
			boob->scale.z = boob->scale.y = boob->scale.x += 0x2000;

		// ...then shrink immediately. This is supposed to indicate something's about to be launched.
		if ((boob->statetimer > 90) && (boob->statetimer < 95))	
			boob->scale.y = boob->scale.z = boob->scale.x -= 0x2000;
		
		// Now that the telegraphs are done, restore the Y rotation just to be safe and shoot some iceballs at Mario.
		if (boob->statetimer == 85)
			{
				boob->rotation.y = BOOB_DEFAULT_ROTATION.y;		
				
				Vec3 FireballCreatePos = boob->position;
				FireballCreatePos.x += 0x20000;
				FireballCreatePos.y -= 0x18000;
				FireballCreatePos.z -= 0xF0000;

				if (!RNG(2)) 
					{
						createActor(251, 0x01102218 + (RNG(10) << 8), &FireballCreatePos, 0, 0, 0);					// Angle going down and right.
						createActor(251, 0x01108218 - (RNG(10) << 8), &FireballCreatePos, 0, 0, 0);					// Angle going up and right.
					}
				else
					{
						if (!RNG(2)) createActor(251, 0x11102520 + (RNG(10) << 8), &FireballCreatePos, 0, 0, 0);	// Angle going down and right.
						else createActor(251, 0x11108520 - (RNG(10) << 8), &FireballCreatePos, 0, 0, 0);			// Angle going up and right.	
					}
			}
		
		// We're done. Go back to the attack selection.
		if (boob->statetimer == 95)
			{
				boob->state = 1;
			}
	}
	
void KingBoo_Bumped(KingBoo* boob)				// State 5
	{
		// This happens if Mario gets hit by the Boo.
		// Uses a different timer so we can preserve the normal one for State 3.
		boob->bumpedstatetimer++;
		
		if (boob->bumpedstatetimer == 1)
			{
				boob->rotation.y = BOOB_DEFAULT_ROTATION.y;			// Restore rotation in case Mario bumped the Boo while it was spinning in State 4.
				UnregisterActivePhysics(&boob->activephysics);		// Remove Boo's Active Physics.
				PlaySNDEffect(SE_EMY_TERESA, &boob->position);		// Laugh...
				modelAnim3d_init(&boob->Model, 0, 0, 0x8000, 0);	// ...and animate laughter.
			}
		
		// Okay, the laughing is done. Still giving the player 10 frames of indication that the Boo
		// will be going back to attacking.
		if (boob->bumpedstatetimer == 110)
			modelAnim3d_init(&boob->Model, 0, 0, 0x800, 0);
		
		// Let's go back to attacking. But we have to consider what the Boo was doing before it was interrupted.
		if (boob->bumpedstatetimer == 120)
			{
				// Can't attack without a collider, so here it is.
				registerCollider(&boob->activephysics);
				
				switch (boob->statewhenbumped)
					{
						case 0: boob->state = 0; break;							// If the Boo was doing the intro while it got bumped... 
																				// I guess we go back to the intro. Shouldn't happen.
						case 2:													// If the Boo was doing the slow or quick chase, 
						case 3: boob->state = boob->statewhenbumped; break;		// go back to it, for as long as it was still meant to happen.
						default:												// In other cases (i.e State 4), go back to Attack Selection, while resetting the timer.
							{
								boob->lastattack = 0;
								boob->statetimer = 0;
								boob->state = 1;
								break;
							}
					}
			}
	}
	
void KingBoo_Dying(KingBoo* boob)			// State 6
	{
		boob->statetimer++;
		MarioActor* mA = (MarioActor*)boob->Mario;	
		int *player = (int*)(0x02085A50);
		
		ControlLock = true;					// Can't move during the dying cutscene, obviously.
		pressedKeys = 0;					// Should also stop moving if the player is still holding some keys.
		mA->xAccel0 = 0;
		SetPlayerAnimation(boob->Mario, 0, 1, 0, 0x1000);

		// All the way until the celebration begins, have the Boo play a distress animation
		if (boob->statetimer < 110)
			{
				modelAnim3d_init(&boob->Model, 2, 0, 0x1000, 0);
			}
		
		// Play a electrical sound to indicate the lights were turned on.
		if (boob->statetimer == 1)
			{	
				bmgFlagSomething |= 0x1;
				isTimeStopped |= 0x10;
				PlaySNDEffect(SE_EMY_FO_CHASE, 0); // (a copy of SE_PLY_TOUCH_BIRI just for this level)
				// Unset Unk2 so that it doesn't undo the light being turned on.
				u8* ViewPtr = (u8*)getPtrToViewById(boob->ViewID, 0);
				*(ViewPtr + 0xC) = 0;
			}
			
		// Flicker the brightness briefly...
		if (boob->statetimer < 30)
			{
				if (boob->statetimer % 2) setBrightness(1, -RNG(9));
				modelAnim3d_init(&boob->Model, 2, 0, 0x1000, 0);
			}

		// Play distress sound...
		if (boob->statetimer == 20)
			{
				PlaySNDEffect(SE_EMY_BASABASA, &boob->position);
			}
			
		// Set brightness to full blast.
		if (boob->statetimer == 35)
			{
				setBrightness(1, -boob->currentbrightness);
			}
			
		// Make the Boo die.
		if (boob->statetimer > 35)
			{
				if (boob->scale.x > 0)		// If scale isn't yet 0...
					{
						boob->scale.z = boob->scale.y = boob->scale.x -= 0x1000;	// Scale down.
						boob->position.x += 0x800;									// Adjust position so the Boo stays on screen all the way.
						
						if (boob->currentbrightness != 0)							// Fade in the light.
							setBrightness(1, boob->currentbrightness+=1);
						
						if (boob->scale.x <= 0)										// Spawn a bunch of effects and a satisfying POP sfx.
							{
								PlaySNDEffect(SE_PLY_STAR_ATTACK, &boob->position);
								SpawnParticle(65, &boob->position);
								SpawnParticle(122, &boob->position);	
								SpawnParticle(73,  &boob->position);	
								SpawnParticle(122, &boob->position);	
							}
					}
			}

		// It's time to celebrate.
		if (boob->statetimer == 110)
			{
				LoadMusicFile(0x16, 0xFF);
				if (!*player)
					NNSi_SndArcLoadGroup(51, *((u32**)(0x208FBB8))); // Mario
				else
					NNSi_SndArcLoadGroup(52, *((u32**)(0x208FBB8))); // Luigi
				NNSi_SndArcLoadGroup(2, *((u32**)(0x208FBB8)));
				
				DisableMarioYRotationForce= true;								// Make Mario face the screen...
				mA->RotY = 0;
								
				mA->unk980 = (u32)&MarioState_Cutscenes;						// And do an animation. So, he gets set to the state that allows this.
				SetPlayerAnimation(boob->Mario, 0x13, 1, 0, 0x1000);			
				
				if (!*player)
					PlaySNDEffect(SE_VOC_MA_WIN, &boob->Mario->position);
				else
					PlaySNDEffect(SE_VOC_LU_WIN, &boob->Mario->position);

				
				PlayMusicFile(0x16, 0xFF);
			}
			
		// Done celebrating. Restore animation to normal.
		if (boob->statetimer == 200)					
			{	
				SetPlayerAnimation(boob->Mario, 0, 1, 0, 0x1000);
			}
			
		// Use the game's fancy way of setting Goal Exit type if the settings indicate it should and exit the level.
		if (boob->statetimer == 290)
			{
				SetSecretExitFromSpriteData(boob->spriteData);
				exitLevel(1);
			}
	}
	
void KingBoo_AllStates(KingBoo* boob)			// Happens during every state.
	{
		// Animate the Boo. 
		anim3d_update(&boob->Model.anim);
		
		// If the Boo is not in the intro state or the dying state...
		if ((boob->state) && (boob->state != 6))
			{
				// If the Boo is still in the location indicated by the settings...
				if (positionInLocation(boob->location, &boob->position))
					{
						// Move the level bounds according the Boo position and stage zoom.
						*ViewStart = boob->position.x;
						u16 *StageZoom = (u16*)(0x020CADB4);
						*ViewEnd = *ViewStart + (*StageZoom * 0x100);
					}
					
				// If the death event is activated, and the Boo isn't already dying... make it die.
				if (eventIsActivated(boob->deathevent))
					{
						StopMusicWithFade(0x1E);
						UnregisterActivePhysics(&boob->activephysics);
						boob->activephysics2.sizex += 0x140000;

						boob->statetimer = 0;
						boob->state = 6;
					}
			}
	
	}

void KingBoo_Collision(ActivePhysics* a, ActivePhysics* b)
	{
		KingBoo* boob = (KingBoo*)a->owner;
		MarioActor* Mario = (MarioActor*)b->owner;
		EnemyClassObject* Enemy = (EnemyClassObject*)b->owner;
	
		// If colliding with something that's not Mario, destroy it with prejudice.
		if (Enemy->ClassID != 0x15)
			{
				if (Enemy->ClassID == 251) return;	// Iceballs.
				if (Enemy->ClassID == 325) return;	// Iceballs.
				
				Vec3 ParticlePos = Enemy->position;
				ParticlePos.y+=0x8000;
				
				SpawnParticle(122, &ParticlePos);
				Base__deleteIt(Enemy);
				return;
			}
		
		// Make it hurt.
		EnemyCollisionCallback(a,b);
		
		PlaySNDEffect(SE_PLY_CLASH_2, &Mario->unk05C);
		Mario->xVelocity = 0x4000;
		
		// Make the Boo go into its bumped state.
		boob->bumpedstatetimer = 0;	
		boob->statewhenbumped = boob->state;
		boob->state = 5;
	}
	
void KingBoo_EnemyKillingCollision(ActivePhysics* a, ActivePhysics* b)
	{
		EnemyClassObject* Enemy = (EnemyClassObject*)b->owner;
	
		// If colliding with Mario, we don't care...
		if (Enemy->ClassID == 0x15) return;
		
		SpawnParticle(122, &Enemy->position);
		Base__deleteIt(Enemy);
	}
	
void MarioState_Cutscenes(EnemyClassObject* Mario)
	{
		UpdatePlayerAnimation(Mario);
		return;
	}
	
void KingBoo_onDelete(KingBoo* boob)
	{
		base_onDelete(boob);
	}
	
void KingBoo_draw(KingBoo* boob)
	{
		if (boob->scale.x <= 0) return;
		DrawAModel(&boob->Model, &boob->position, &boob->rotation, &boob->scale);
	}

void* KingBoo_dtor(KingBoo* boob)
    {
        boob->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        boob->Unk4Vec2 = dtor2;
        boob->Unk3Vec2 = dtor2;
        boob->DeleteDist = dtor2;
        boob->DrawDist = dtor2;
        boob->ExistDist = dtor2;
        boob->unkv13 = dtor;
        boob->unkv12 = dtor;
        boob->unkv11 = dtor;
		
        UnregisterActivePhysics(&boob->activephysics);
		UnregisterActivePhysics(&boob->activephysics2);
		
		ViewStartAndEndForBoundsLimit[0] = 1;
		ViewStartAndEndForBoundsLimit[1] = 0;
		
		bmgFlagSomething &= ~0x1;
		isTimeStopped &= ~0x10;
		
		ControlLock = false;
		DisableMarioYRotationForce = false;
		MusicOffMode = false;
        return boob;
    }

void* KingBoo_dtorFree(KingBoo* boob)
    {
        KingBoo_dtor(boob);
		freeToGameHeap(boob);
        return boob;
    }
	