#include "game.h"
#include "registers.h"
#include "VariousFunctions.h"
#include "Minecart.h"

#define MINECART_MARIO_TURN_AROUND_DURATION 40

extern bool HammerThrowingEnabled; // VariousFunctions.cpp
extern bool MarioUsingCart;
extern int  MinecartHP;

bool* MenuLocked = (bool*)0x020CA880;

Minecart* Minecart_global;
MarioActor* Player_global;
extern EnemyClassObject* CameraControllerPtr;

EnemyClassObject* Dummy_global = 0;

int Minecart_loadFiles()
    {
        loadFileByExtId_3dModel(2692-0x83, 0);
        return 1;
    }

void Minecart_setup(Minecart* cart)
    {
        if ((!getStartingFromMidway()) && ((cart->spriteData >> 20) & 2)) // (Nybble 7) & 2, "appears only from midpoint"
            {
                Base__deleteIt(cart);
                return;
            }

		SwapASwar(35, 2795-0x83);

        u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(2692-0x83));

        getTextureParams(texture, 0, &cart->info.cartTex.texparam);
        getTexturePalBase(texture, 0, &cart->info.cartTex.palbase);

        getTextureParams(texture, 3, &cart->info.wheelTex.texparam);
        getTexturePalBase(texture, 3, &cart->info.wheelTex.palbase);

        cart->position.z = 0x20000;
        cart->info.intendedX = cart->position.x;
        cart->info.intendedY = cart->position.y;

        cart->MarioIn = 0;
        cart->EnsureMarioIsIn = false;
        cart->PathDirectionalityMarioSpinTimer = 0;

        MinecartCommon_Setup(&cart->info, cart->spriteData);

        Player_global = (MarioActor*)getPtrToPlayerActor();
        Minecart_global = cart;

		if ((cart->spriteData >> 20) & 4) // (Nybble 7) & 4, "start Mario in Minecart"
			{
				PutMarioInCart(cart);

				MinecartCommon_MoveOnGround(&cart->info, &cart->position, &cart->velocity, &cart->rotation);
				SetPlayerPosToMinecart(Player_global);
            }
		else
            {   // If Mario does not snap to the cart immediately, start at lowest speed
                cart->info.brakingPower = MAX_BRAKING_POWER;
            }

        u32 Minecart_physics[] =
            {
                0x18000,
                0xA000,
                0x13000,
                0xC000,
                0x03F70001,
                0x0400,
                (u32)&MinecartCollision,
            };

        initActivePhysics(&cart->activephysics, cart, Minecart_physics, 0);
        registerCollider(&cart->activephysics);

        u32 Minecart_StompPhysics[] =
            {
                0x16000,
                -0x8000,
                0x12000,
                0x1,
                0x00D30004,
                0x0400,
                (u32)&MinecartCollisionStomp,
            };

        initActivePhysics(&cart->activephysics2, cart, Minecart_StompPhysics, 0);

        u32 Minecart_CollectingPhysics[] =
            {
                0x15000,
                0x10000,
                0x11000,
                0x17000,
                0x03F70000,
                0x0400,
                (u32)&MinecartCollectingCollision,
            };

        initActivePhysics(&cart->activephysics3, cart, Minecart_CollectingPhysics, 0);
        registerCollider(&cart->activephysics3);

        // Set the cart's initial position
        MinecartCommon_MoveOnGround(&cart->info, &cart->position, &cart->velocity, &cart->rotation);

		// Make move play sounds from now on
		cart->info.isSilent = 0;

        cart->cameraDistance = cart->cameraAngle = 0;
    }

// MISC FUNCTIONS

bool Minecart_JumpPressed()
    {
        if (controlOptionsA)
            return (*ButtonsPressed & A) || (*ButtonsPressed & B);
        else
            return (*ButtonsPressed & A) || (*ButtonsPressed & X);
    }

bool Minecart_JumpHeld()
    {
        if (controlOptionsA)
            return (*ButtonsHeld & A) || (*ButtonsHeld & B);
        else
            return (*ButtonsHeld & A) || (*ButtonsHeld & X);
    }

void Minecart_InitiateJump(Minecart* cart, bool enemyJump, bool fallingOffEdge)
    {
        registerCollider(&cart->activephysics2);

        bool initialFPL = cart->info.goingBackwards;

        bool jumped = MinecartCommon_InitiateJump(
            &cart->info, &cart->position, &cart->velocity, enemyJump, fallingOffEdge);
        cart->jumpButtonHeld = true;  // whether we actually jumped or not

        if (jumped)
            {
                cart->state = 1;
                if (!fallingOffEdge && !enemyJump)
                    cart->marioSignal_jumpBegan = true;
            }

        if (cart->info.goingBackwards != initialFPL)
            {   // The cart switched directions as a result of the jump.
                cart->PathDirectionalityMarioSpinTimer = MINECART_MARIO_TURN_AROUND_DURATION;
            }
    }
	
void SetDummyCollisionPositionToMinecart(Minecart* cart)
	{
		Vec3 Dummy = cart->position;
        Dummy.x += 0x1A000;
		Dummy_global->position = Dummy;
	}

void PutMarioInCart(Minecart* cart)
    {   // Put Mario into the cart and set everything up.

        if (Minecart_JumpHeld()) cart->jumpButtonHeld = true;
        Player_global->unk980 = (u32)&MarioInMinecartState_Idle;

        cart->MarioIn = true;
        MarioUsingCart = true;
        HammerThrowingEnabled = false;

        // Set initial camera controller position
        Vec3 RelCamPos;
        RelCamPos.x = Player_global->xPos - cart->info.intendedX;
        RelCamPos.y = Player_global->yPos - cart->info.intendedY;
        RelCamPos.z = cart->position.z;
        cart->RelativeCamPos = RelCamPos;

        UnregisterActivePhysics(&Player_global->unk120);    // Fuck you, Mario, you don't get to touch anything

        // Resize Mario tile collider
		Dummy_global = allocFromCurrentHeap(0x6C,0);	// Sexy
		Player_global->unk1D4 = (u32)Dummy_global;		// Mario Collider owner.
        Player_global->unk7F8 = 0x20000;                // YPos1 Mario Side Collider
        Player_global->unk7FC = 0;                      // YPos2 Mario Side Collider
        Player_global->unk800 = 0x14000;                // X Size Mario Side Collider
		SetDummyCollisionPositionToMinecart(cart);	
		
		// Create a camera controller and position it.
        CameraControllerPtr = (EnemyClassObject*)createActor(0x146, 0, &cart->position, 0, 0, 0);
		SetCameraPositionToMinecart(cart);

        int *player = (int*)(0x02085A50);
        u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(2692-0x83));
        getTextureParams(texture, *player + 1, &cart->info.cartTex.texparam);
        getTexturePalBase(texture, *player + 1, &cart->info.cartTex.palbase);
    }

// MINECART STATES
void Minecart_StateIdle(Minecart* cart)                     // State 0
    {
        if (!Minecart_JumpHeld()) cart->jumpButtonHeld = false;

        if (cart->MarioIn)
            {
                if (!cart->EnsureMarioIsIn)
                    {
                        if (Player_global->unk980 != (u32)&MarioInMinecartState_Idle) Player_global->unk980 = (u32)&MarioInMinecartState_Idle;
                        cart->EnsureMarioIsIn = true;
                    }

                if (cart->info.brakingPower > 0)
                    cart->info.brakingPower -= 1;

                PathNode* nodeFollowing = MinecartCommon_MoveOnGround(&cart->info,
                                                                      &cart->position,
                                                                      &cart->velocity,
                                                                      &cart->rotation);
                if (nodeFollowing)
                    {
                        cart->cameraAngle = nodeFollowing->accel >> 16;
                        cart->cameraDistance = nodeFollowing->accel & 0xFFFF;
                    }
                else
                    {
                        Minecart_InitiateJump(cart, false, true);
                        return;
                    }

                if (Minecart_JumpPressed() && !cart->jumpButtonHeld && !cart->info.isAirborne)
                    {
                        Minecart_InitiateJump(cart, false, false);
                        return;
                    }
            }
    }

void Minecart_StateJumping(Minecart* cart)      // State 1
    {
        Minecart_MoveDuringJump(cart);
    }

void Minecart_StateDying(Minecart* cart)        // State 2
    {
        cart->info.Slam++;
        cart->rotation.z += 0x100;

        if (cart->info.Slam < 5) cart->position.y += 0x1000; else cart->position.y -= 0x1000;

        if ((cart->info.Slam < 8) && (cart->info.Slam & 1))
            {
                cart->rotation.z = cart->rotation.z + sinLerp(cart->info.Slam * 0x1000) / 8;
            }

        if (cart->info.Slam > 8)
            {
                Player_global->DeathState = (u32)0x21197FC;
                cart->info.isDead = true;
            }
    }

void Minecart_StateLanding(Minecart* cart)         // State 3
    {
        if (cart->info.landingBounceCounter == -1)
            {
                PlaySNDEffect(13, &cart->position);
                cart->info.doneFirstBounce = 0;
                cart->info.doneSecondBounce = 0;
                cart->info.isLanding = false;
                cart->state = 0;
            }

        Minecart_StateIdle(cart);
    }

void Minecart_AllStates(Minecart* cart)         // This code runs all the time. Use carefully.
    {
        if (cart->MarioIn)
            {
                SetCameraPositionToMinecart(cart);
                Minecart_CheckTileCollisions(cart);

                if (cart->info.blinking)
                    {
                        cart->info.blinking--;
                    }

                if (cart->info.blinking == 1)
                    {
                        registerCollider(&cart->activephysics);
                    }

                if (!cart->info.HP)
                    {
                        if ((cart->info.smoke) && (!(cart->info.smoke % 5)))
                        {
                            Vec3 Pos = cart->position;

                            Pos.x += RNG(0x20000);
                            Pos.y += RNG(0x20000);
                            Pos.z += 0x200000;
                            SpawnParticle(22, &Pos);
                        }
                        cart->info.smoke++;
                    }

            }
    }

// OTHER MINECART SPECIFIC CODE

void Minecart_SpawnDeathParticles(Minecart* cart)
    {
        Vec3 Pos = cart->position;
        Pos.x += 0x10000;

        SpawnParticle(23, &Pos);
        SpawnParticle(30, &Pos);
        PlaySNDEffect(0x8A, &Pos);
    }

void Minecart_Flagpole(Minecart* cart)
    {
        Minecart_SpawnDeathParticles(cart);
        Player_global->zPos = 0x100000;
        Player_global->RotZ = 0;

        Base__deleteIt(cart);
        Base__deleteIt(CameraControllerPtr);
    }

void Minecart_Die(Minecart* cart)
    {
        cart->state = 2;
        *MenuLocked = true;

        UnregisterActivePhysics(&cart->activephysics);
        UnregisterActivePhysics(&cart->activephysics2);
        UnregisterActivePhysics(&cart->activephysics3);
        Minecart_SpawnDeathParticles(cart);

        cart->rotation.z += 0x100;
    }

// MOVEMENT RELATED


void Minecart_MoveDuringJump(Minecart* cart)
    {
        bool jumpJustLetGo = false;
        if (cart->jumpButtonHeld && !Minecart_JumpHeld())
            {
                cart->jumpButtonHeld = false;
                jumpJustLetGo = true;
            }

        if (MinecartCommon_MoveDuringJump(&cart->info, &cart->position, &cart->velocity, &cart->rotation, jumpJustLetGo))
			{
				UnregisterActivePhysics(&cart->activephysics2);
				cart->state = 3;
			}
    }


// COLLISION RELATED

void MinecartCollision(ActivePhysics* a, ActivePhysics* b)
    {
        Minecart* cart = (Minecart*) a->owner;
        MarioActor* Player = (MarioActor*) b->owner;
        EnemyClassObject* Enemy = (EnemyClassObject*) b->owner;

        if ((cart->MarioIn) && (cart->state != 2))
            {
                if (Player != Player_global 	// Ignore player and...
					&& Enemy->ClassID != 0x43 	// Star Coins
					&& Enemy->ClassID != 0x145 	// Fireballs
					&& Enemy->ClassID != 0xFB	// Hammers
					&& Enemy->ClassID != 0x1F 	// Items
					&& Enemy->ClassID != 0x57 	// Warps
					&& Enemy->ClassID != 0x9F	// Midpoints
					&& Enemy->ClassID != 0x24) 	// Goals
                    {
                        if (cart->info.HP)
                            {
								Rumble(1);
                                cart->info.HP = 0;
								MinecartHP = 0;
                                PlaySNDEffect(362, &cart->position);
                                PlaySNDEffect(334, &cart->position);
                                cart->info.blinking = 50;
                                UnregisterActivePhysics(&cart->activephysics);
                            }

                        else
                            {
								Rumble(2);
                                Minecart_Die(cart);
								MinecartHP = 1;
                                return;
                            }
                    }
            }

        else if ((!cart->MarioIn) && (cart->activephysics.YContactPos < 0) && (Player->yAccel0 <= 0))
            {
				Rumble(1);
                PutMarioInCart(cart);

                MinecartCommon_MoveOnGround(&cart->info, &cart->position, &cart->velocity, &cart->rotation);
                SetPlayerPosToMinecart(Player_global);

                int *player = (int*)(0x02085A50);

                PlaySNDEffect(12, &cart->position);
                Vec3 Pos = cart->position;
                Pos.x += 0x18000;
                Pos.y += 0xA000;

                SpawnParticle(0x41, &Pos);
                if (*player) SpawnParticle(68, &Pos); else SpawnParticle(73, &Pos);
            }
    }

void MinecartCollisionStomp(ActivePhysics* a, ActivePhysics* b)
    {
        Minecart* cart = (Minecart*) a->owner;
        EnemyClassObject* Enemy = (EnemyClassObject*) b->owner;

        if ((u32)Enemy != (u32)Player_global    // Ignore player and...
					&& Enemy->ClassID != 0x43 	// Star Coins
					&& Enemy->ClassID != 0x145 	// Fireballs
					&& Enemy->ClassID != 0xFB	// Hammers
					&& Enemy->ClassID != 0x1F 	// Items
					&& Enemy->ClassID != 0x57 	// Warps
					&& Enemy->ClassID != 0x9F	// Midpoints
					&& Enemy->ClassID != 0x24) 	// Goals
            {
                if (cart->state != 2)
                    {
                        ShellCollisionCallback(b, b);
                        cart->info.landingBounceCounter = -1;
                        cart->info.isLanding = false;
                        Minecart_InitiateJump(cart, true, false);
                    }
            }
    }

void MinecartCollectingCollision(ActivePhysics* a, ActivePhysics* b)
    {
        Minecart* cart = (Minecart*) a->owner;
        EnemyClassObject* Enemy = (EnemyClassObject*) b->owner;

		if (Enemy->ClassID == 0x43)		// Star Coin
			{
				CollectStarCoin(Enemy);
				return;
			}

        if (Enemy->ClassID == 0x1F)
            {
                if ((Enemy->spriteData & 0xF) == 3)    // Mushroom
                    {
                        if (!cart->info.HP) PlaySNDEffect(355, &cart->position); else PlaySNDEffect(381, &cart->position);
						cart->info.HP = 1;
						MinecartHP = 1;
						SpawnParticle(62, &Enemy->position);
                        Base__deleteIt(Enemy);
                        return;
                    }

                else return;
            }

		if (Enemy->ClassID == 0x9F)		// Midway Point
			{
				if (!cart->info.HP) PlaySNDEffect(355, &cart->position);
				cart->info.HP = 1;
				MinecartHP = 1;
			}

        if (Enemy->ClassID == 0x24) 	// Flagpole
            {
				MinecartHP = 1;
                MarioUsingCart = false;
                HammerThrowingEnabled = true;
            }

    }

void Minecart_CheckTileCollisions(Minecart* cart)
    {   // Remember that the origin of the minecart is its bottom-left corner.
        // Positive y-values go upward

        bool checkHead = (Player_global->marioPowerupState != 4) && (!(pressedKeys & DOWN));

        // Define points at which to detect collision
        // https://i.imgur.com/7alkDEM.png
        // (beach tileset there is a test level; not some unused/scrapped idea :P)
        // Units here: 4 = a tile
        int xPositions[] = {2,
                            6,
                            10,
                            0,
                            2,
                            4,
                            8,
                            10,
                            12,
                            8,  // | Collision points for Mario's head
                            8}; // |
        int yPositions[] = {7,
                            7,
                            7,
                            4,
                            2,
                            1,
                            1,
                            2,
                            4,
                            10,
                            13};
        int arrayLen = checkHead ? 11 : 9;

        // Get stuff for the rotation matrix
        int cosTheta = cosLerp(cart->info.intendedAngle);
        int sinTheta = sinLerp(cart->info.intendedAngle);

        // Iterate over the detection points
        for (int i = 0; i < arrayLen; i++)
            {
                int x = xPositions[i];
                int y = yPositions[i];

                // To rotate about the center of the cart, we need to
                // make these values be relative to the cart's center
                x -= 6;
                y -= 5; // Not -= 3, since the sprite's y-position is the middle of the wheels.

                // Apply the rotation to x and y
                // https://en.wikipedia.org/wiki/Rotation_matrix#In_two_dimensions

                // Remember, the trig values here are 4.12 fixed-point
                int x2 = cosTheta * x - sinTheta * y;
                int y2 = sinTheta * x + cosTheta * y;

                // Take the fixed-point stuff into account:
                // >> 12 for the trig functions
                // >> 2 for the x/y units in the hardcoded arrays above
                // << 4 for the CheckIfSolidRelativeToActor units (see below)
                // makes >> 10.
                x = x2 >> 10;
                y = y2 >> 10;

                // Make the position be relative to the sprite origin again
                x += 0x18;
                y += 0x0C;

                // // DEBUG
                // Vec3 Pos = ((EnemyClassObject*)cart)->position;
                // Pos.x += x * 0x1000;
                // Pos.y += y * 0x1000;
                // SpawnAndEndParticle(0, 0xC9, &Pos, 0, 0, 0, 0);

                // Units for this function: 0x10 = a tile
                if (CheckIfSolidRelativeToActor((EnemyClassObject*)cart, x, y))
                    {
                        Minecart_Die(cart);
                        return;
                    }
            }
    }

// DRAW RELATED

bool Minecart_beforeDraw(Minecart* cart)
    {
        return 1;
    }


void Minecart_draw(Minecart* cart)
    {
        if (cart->info.isDead)
            {
                cart->position.y -= 0x4000;
                cart->rotation.z += 0x200;
            }

        if ((!(cart->info.blinking % 5)) && (cart->info.blinking)) return;

        Vec3 Scale;
        Vec3 Translate1;
        Vec3 Translate2;
        Vec3 Offset;

        Scale.x = 0x30000;
        Scale.y = 0x18000;
        Scale.z = 0;

        Translate1.x = 0x18000;
        Translate1.y = 0xC000;

        Scale.z = Translate1.z = Translate2.x = Translate2.y = Translate2.z = 0;
        Offset = Translate2;

        DrawGenericMinecartPoly(cart->info.cartTex, cart->position, 0x800, 0x400, Scale,
                         Translate1, Translate2, Offset, cart->rotation.z, 0, false, false);

        Scale.x = Scale.y = 0xD000;
        Translate2.x = Translate2.y = 0x6800;
        Translate2.z = -0x1000;
        Offset.x = 0x7000;
        Offset.y = -0x7000;

        DrawGenericMinecartPoly(cart->info.wheelTex, cart->position, 0x200, 0x200, Scale,
                        Translate1, Translate2, Offset, cart->rotation.z,
                        cart->info.wheelRot, false, false);

        Offset.x = 0x1B000;

        DrawGenericMinecartPoly(cart->info.wheelTex, cart->position, 0x200, 0x200, Scale,
                        Translate1, Translate2, Offset, cart->rotation.z,
                        cart->info.wheelRot + cart->info.secondWheelRotAddition, false, false);
    }

// PLAYER RELATED

void SetPlayerPosToMinecart(MarioActor* Player)
    {
        Player->xVelocity = 0;
        Player->yVelocity = 0;

        Player->xPos0 = Player->xPos;
        Player->yPos0 = Player->yPos;

        int heightInCart = 0; // With a mushroom/flower/etc
        if (Player->marioPowerupState == 0) // Small
            heightInCart = 0x60;
        else if (Player->marioPowerupState == 4) // Mini
            heightInCart = 0x80;

        int xOffMult = Minecart_global->info.goingBackwards ? -1 : 1;
        int xRotationOffset = (-heightInCart * sinLerp(Minecart_global->rotation.z) \
                                      + xOffMult * 0x30 * cosLerp(Minecart_global->rotation.z)) / 0x10;
        int yRotationOffset =  (heightInCart * cosLerp(Minecart_global->rotation.z) \
                                      + xOffMult * 0x30 * sinLerp(Minecart_global->rotation.z)) / 0x10;

        Player->xPos = Minecart_global->position.x + 0x18000 + xRotationOffset;
        Player->yPos = Minecart_global->position.y + 0x11800 + yRotationOffset;
        Player->zPos = 0;

        int pdmst = Minecart_global->PathDirectionalityMarioSpinTimer;
        if (Minecart_global->PathDirectionalityMarioSpinTimer > 0)
            Minecart_global->PathDirectionalityMarioSpinTimer--;

        // RotX = pitch, RotY = yaw, RotZ = roll.
        // For RotY: 0 = facing camera, 0x4000 = facing right, -0x4000 = facing left.
        // Values outside 0-0xFFFF are truncated because the variables are s16.

        // X rotation (tilting forward/back) is applied first,
        // followed by Y (spinning around vert. axis),
        // then Z (spinning around axis into the screen)
        s64 facingScreenAngle;
		double m_TAD = (double)MINECART_MARIO_TURN_AROUND_DURATION;
		
		
        if (!Minecart_global->info.goingBackwards)
            {
                if (Minecart_global->info.isAirborne)
                    facingScreenAngle = 0x2800 - (pdmst * 0x5C00) / m_TAD;
                else
                    facingScreenAngle = 0x4000 - (pdmst * 0x8000) / m_TAD;
            }
        else
            {
                if (Minecart_global->info.isAirborne)
                    facingScreenAngle = -0x3400 + (pdmst * 0x5C00) / m_TAD;
                else
                    facingScreenAngle = -0x4000 + (pdmst * 0x8000) / m_TAD;
            }

        rotateMarioYZ(Player, facingScreenAngle, Minecart_global->rotation.z * 2);
        moveMariosModelToHisPosition(Player);
    }

void rotateMarioYZ(MarioActor* Player, int yRotation, int zRotation)
    {   // Rotate Mario about the Y axis and then the Z axis, rather than
        // in the order Z, -X, Y as the game does.

        // How this was derived: http://pastebin.com/raw/HGvTSFVf

        int PI = 0x8000; // :D
        int sinY = sinLerp(yRotation / 2), cosY = cosLerp(yRotation / 2);
        int sinZ = sinLerp(zRotation / 2), cosZ = cosLerp(zRotation / 2);
        int A23 = (sinY * sinZ) / 0x1000;
        if (A23 == 0x1000)
            {
                Player->RotX = PI/2;
                Player->RotY = 0;
                Player->RotZ = FX_Atan2Idx(sinY * 0x1000, cosY * cosZ);
            }
        else if (A23 == -0x1000)
            {
                Player->RotX = 3*PI/2;
                Player->RotY = 0;
                Player->RotZ = FX_Atan2Idx(-sinY * 0x1000, cosY * cosZ);
            }
        else
            {
                Player->RotX = asinLerp(A23) * 2;
                Player->RotY = FX_Atan2Idx(sinY * cosZ, cosY * 0x1000);
                Player->RotZ = FX_Atan2Idx(cosY * sinZ, cosZ * 0x1000);
            }
    }

void moveMariosModelToHisPosition(MarioActor* Player)
    {   // For some rotation directions, Mario's model can end up such that
        // his feet aren't centered on his position. This function adjusts
        // his position to fix that. Only call this after you're done setting
        // his true position and rotation.

        // How this was derived: http://pastebin.com/raw/7JDBXR0K

        int marioHeight;
        if (Player->marioPowerupState == 0) // No powerup
            marioHeight = 0x10000; // 1 tile tall
        else if (Player->marioPowerupState == 4) // Mini mushroom
            marioHeight = 0x8000; // 1/2 tile tall
        else // Other
            marioHeight = 0x20000; // 2 tiles tall

        Player->yPos -= (0x1000 - cosLerp(Player->RotX / 2)) * marioHeight / 0x2000;
        s64 xShift = sinLerp(Player->RotX / 2) * sinLerp(Player->RotY / 2);
        xShift *= marioHeight / 2;
        xShift /= 0x1000000;
        Player->xPos -= xShift;
    }

void SetCameraPositionToMinecart(Minecart* cart)
    {
        const int CAMERA_PAN_DIVISOR = 35; // smaller values = faster camera movement (1 = instant)
        const int CAMERA_DISTANCE = 100;
        const int CAMERA_UPWARD_ANGLE = 0x400; // 0 = forward, 0x2000 = up

        if (!CameraControllerPtr) return;

        Vec3 TargetPos;
        TargetPos.z = cart->position.z;
        if (Minecart_IsApproachingFlagpole(cart))
            {
                // Scroll back to Mario.
                TargetPos.x = Player_global->xPos - cart->position.x;
                TargetPos.y = Player_global->yPos - cart->position.y;
            }
        else
            {
                // Figure out the angle the camera should be at
                int camAngle;
                if (cart->cameraAngle == 0)
                    {
                        camAngle = cart->rotation.z;
                        camAngle += cart->info.goingBackwards ? (0x4000 - CAMERA_UPWARD_ANGLE) : CAMERA_UPWARD_ANGLE;
                    }
                else
                    {
                        camAngle = cart->cameraAngle;
                    }

                // Figure out the distance the camera should be
                int camDist = (cart->cameraDistance != 0) ? cart->cameraDistance : CAMERA_DISTANCE;

                // From that, figure out where the camera should ideally be, relative to the MC
                // DON'T put a large value for x -- this compensates for the camera's hardcoded rightward bias
                TargetPos.x = 0      + camDist * cosLerp(camAngle);
                TargetPos.y = 0x9800 + camDist * sinLerp(camAngle);
            }

        // Update the relative camera position smoothly
        cart->RelativeCamPos.x += (TargetPos.x - cart->RelativeCamPos.x) / CAMERA_PAN_DIVISOR;
        cart->RelativeCamPos.y += (TargetPos.y - cart->RelativeCamPos.y) / CAMERA_PAN_DIVISOR;

        // Update the absolute camera position based on that
        Vec3 CamPos = CameraControllerPtr->position;
        CamPos.x = cart->info.intendedX + cart->RelativeCamPos.x;
        CamPos.y = cart->info.intendedY + cart->RelativeCamPos.y;
        CameraControllerPtr->position = CamPos;

    }

bool Minecart_IsApproachingFlagpole(Minecart* cart)
    {
        const int LIST_LENGTH = 2;
        int locationIDs[] = {(cart->spriteData >> 28) & 0xF,  // Nybble 5
                             (cart->spriteData >> 24) & 0xF}; // Nybble 6

        for (int i = 0; i < LIST_LENGTH; i++)
            if (positionInLocation(locationIDs[i], &cart->position))
                return true;

        return false;
    }

void MarioInMinecartState_Idle(MarioActor* Player)
    {
        SetPlayerPosToMinecart(Player);

        // Check for jumping
        if (Minecart_global->marioSignal_jumpBegan)
            {
                PlaySNDEffect(327, &Player->unk05C);
                Player->unk980 = (u32)&MarioInMinecartState_Jumping;
                Minecart_global->marioSignal_jumpBegan = false;
                return;
            }

        // Check for ducking (but only if we're not airborne)
        if ((pressedKeys & DOWN) && !Minecart_global->info.isAirborne)
            {
                PlaySNDEffect(345, &Player->unk05C);
                Player->unk980 = (u32)&MarioInMinecartState_Ducking;
                return;
            }

        // Set the correct animation and update it
        SetPlayerAnimation(Player, 0, 0, 0, 0x1000);
        UpdatePlayerAnimation(Player);
    }


void MarioInMinecartState_Ducking(MarioActor* Player)
    {
        SetPlayerPosToMinecart(Player);

        // Check for jumping
        if (Minecart_global->marioSignal_jumpBegan)
            {
                PlaySNDEffect(327, &Player->unk05C);
                Player->unk980 = (u32)&MarioInMinecartState_DuckJumping;
                Minecart_global->marioSignal_jumpBegan = false;
                return;
            }

        // Check for unducking
        if (!(pressedKeys & DOWN))
            {
                Player->unk980 = (u32)&MarioInMinecartState_Idle;
                return;
            }

        // Set the correct animation
        SetPlayerAnimation(Player, 0x11, 0, 0, 0);
    }


void MarioInMinecartState_Jumping(MarioActor* Player)
    {
        SetPlayerPosToMinecart(Player);

        // Check for landing
        if (!Minecart_global->info.isAirborne)
            Player->unk980 = (u32)&MarioInMinecartState_Idle;

        // Set the correct animation and update it
        if ((Minecart_global->velocity.y > 0x200))
            SetPlayerAnimation(Player, 5, 0, 0, 0);
        else
            SetPlayerAnimation(Player, 6, 1, 0, 0);

        UpdatePlayerAnimation(Player);
    }


void MarioInMinecartState_DuckJumping(MarioActor* Player)
    {
        SetPlayerPosToMinecart(Player);

        // Check for landing
        if (!Minecart_global->info.isAirborne)
            Player->unk980 = (u32)&MarioInMinecartState_Ducking;

        // Check for unducking
        if (!(pressedKeys & DOWN))
            {
                Player->unk980 = (u32)&MarioInMinecartState_Jumping;
                return;
            }

        // Set the correct animation
        SetPlayerAnimation(Player, 0x11, 0, 0, 0);
    }


void* Minecart_dtor(Minecart* cart)
    {
        cart->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0, 0, 0};
        Vec2 dtor2 = {0, 0};

        cart->Unk4Vec2 = dtor2;
        cart->Unk3Vec2 = dtor2;
        cart->DeleteDist = dtor2;
        cart->DrawDist = dtor2;
        cart->ExistDist = dtor2;
        cart->unkv13 = dtor;
        cart->unkv12 = dtor;
        cart->unkv11 = dtor;

		UnregisterActivePhysics(&cart->activephysics);
		UnregisterActivePhysics(&cart->activephysics2);
		UnregisterActivePhysics(&cart->activephysics3);
		
		freeToCurrentHeapIfNotNull(Dummy_global);

        return cart;
    }

void* Minecart_dtorFree(Minecart* cart)
    {
        Minecart_dtor(cart);
        freeToGameHeap(cart);
        return cart;
    }