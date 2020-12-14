#include "game.h"
#include "registers.h"
#include "MovementController.h"
#include "VariousFunctions.h"
#include <math.h>

// Macros, because these show up in many places
#define actorAlive(mcon,i) (!((mcon)->Dead>>(i)&1))
#define GetClassID(mcon) (mcon->settings->classID & 0x1FF)
#define GetDrawDistance(mcon)  (mcon->settings->classID >> 12)


void MovementController_setup(MovementController* mcon)
    {
		u32* pointerToBlock14 = (u32*)0x0208B19C;

        startNodeAdjustment = getNodeIndexAdjustmentValue();
	
		u8 SettingsID = (mcon->spriteData & 0xF);					// Nybble 12
        
		mcon->settings = (MovementControllerSettings*)(*pointerToBlock14 + 16 + SettingsID * 16);
		mcon->spritecount = (mcon->settings->amount);
		
		if (mcon->spritecount == 0) 					
			{
				Base__deleteIt(mcon);
				return;
			}
		
		mcon->width = (mcon->spriteData >> 28 & 0xF); 				// Nybble 5			
		mcon->height = (mcon->spriteData >> 24 & 0xF); 				// Nybble 6		
		mcon->delay = (mcon->spriteData >> 20 & 0xF);				// Nybble 7
		mcon->startangle = (mcon->spriteData >> 16 & 0xF);			// Nybble 8
		
		mcon->movementdistance = (mcon->spriteData >> 12 & 0xF);	// Nybble 9. If Movement Type is constant, 0 = Continous Loop, 1 = From start angle to start angle + this,
																	//			 If Movement Type is swaying, this is half of the angle to rotate through.
																	
		mcon->speed = (mcon->spriteData >> 8 & 0xF);				// Nybble 10, signed.
		
		mcon->movementshape = (mcon->spriteData >> 4 & 0x3);		// Nybble 11, bits 1-2. 0 = Ellipse, 1 = Parabola, 2 = Path (not looped), 3 = Path (looped)
        mcon->movementtype = (mcon->spriteData >> 7 & 1);			// Nybble 11, bit 4. 0 = constant, 1 = swaying

        mcon->pathID = mcon->width << 4 | mcon->height;				// Nybble 5+6. Path ID.
		
        if (mcon->movementshape == 2 || mcon->movementshape == 3)
            {
                mcon->pathTime = CalculatePathTime(mcon->pathID, (mcon->movementshape == 3));
                u64 pathsize = PathSize(mcon->pathID);
				mcon->PathWidth = pathsize>>32;
				mcon->PathHeight = ((pathsize << 32) >> 32);
            }

		mcon->ticker = 0;
		mcon->Dead = 0;
		
		if (GetClassID(mcon) == 0x12B)
			{
				for (int i = 0; i < mcon->spritecount; i++)
					{
						mcon->DeadMovconDeads[i] = 0;
					}
			}
		
		mcon->actorsdeleted = true;
        mcon->swaystate = 0;
        mcon->swayticker = 0;
		
		mcon->position.y -= 0x10000;								// So it's centered on the spritebox in NSMBe
		mcon->position.z = mcon->settings->ZPosition * 10000;		

		if (GetDrawDistance(mcon) == 0xF) mcon->ExistDistance = mcon->DrawDistance = 0xFFFF;
		else if (GetDrawDistance(mcon) == 0xE) 
			{
				mcon->ExistDistance = 0xFFFF; 
				mcon->DrawDistance = 0x100;
			}
		else 
			{
				mcon->ExistDistance = 0x7D0 + (GetDrawDistance(mcon) * 0x64);
				mcon->DrawDistance = mcon->ExistDistance / 11;
			}	
		
		MovementController_PositionStuff(mcon);
		
		if (MovementController_CheckIfWithinRange(mcon, true)) MovementController_CreateStuff(mcon);
    }
	
void MovementController_update_state0(MovementController* mcon)
    {
        if (!MovementController_CheckIfAnythingAlive(mcon)) Base__deleteIt(mcon);
		 
        if (!mcon->actorsdeleted)
			{
				for (int i = 0; i < mcon->spritecount; i++)
					{
						if (actorAlive(mcon, i)) 
						{
							if (!(MovementController_CheckIfSpriteAlive(mcon,i))) mcon->Dead |= 1<<i;
						}

					}
				if (MovementController_CheckIfWithinRange(mcon, false)) MovementController_DeleteStuff(mcon,false);
				else MovementController_MoveStuff(mcon);
			}
        else if (MovementController_CheckIfWithinRange(mcon, true)) MovementController_CreateStuff(mcon);
    }

bool MovementController_CheckIfAnythingAlive(MovementController* mcon)
    {
        for (int i = 0; i < mcon->spritecount; i++)
            {
                if (actorAlive(mcon, i)) return true;
            }
        return false;
    }
	
bool MovementController_CheckIfSpriteAlive(MovementController* mcon, int i)
    {
        // Koopa Paratroopa had to be special-cased to work properly.
		if (GetClassID(mcon) == 0x5F)		
			{
				if (((u32)mcon->ArrayOfPointers[i]->SpriteSpecificData2 == 0)) return false;
				else return true;
			}
		// Paragoombas don't really work well due to animation.
		else if (GetClassID(mcon) == 0x54)		
			{
				if (((u32)mcon->ArrayOfPointers[i]->SpriteSpecificData1 != (u32)mcon->ArrayOfActivePhysics[i])) return false;
				else return true;
			}
		
        if (((u32)mcon->ArrayOfPointers[i]->activephysics.DeadMaybe == (u32)mcon->ArrayOfActivePhysics[i]) && ((u32)mcon->ArrayOfPointers[i]->vtable == (u32)mcon->ArrayOfVtables[i])) return true; 
		else return false;
    }
	
bool MovementController_CheckIfWithinRange(MovementController* mcon, bool mode)
// Checks for whether the player is within range or outside of range of the sprite.
// Mode 0 = checks whether the player isn't within range. Used for deleting.
// Mode 1 = checks whether the player is within range. Used for creating.
	{
		MarioActor* mA = (MarioActor*) getPtrToPlayerActor();
		u16 *StageZoom = (u16*)(0x020CADB4);
		const float TILE_SIZE = 0x10000;
		
		bool skip = false;
		if (GetDrawDistance(mcon) >= 0xE) skip = true;

        int xoffs = mA->xPos - mcon->position.x;
		int yoffs = mA->yPos - mcon->position.y;
		int WidthMultiplier = 0;
		int HeightMultiplier = 0;
		
		if (mcon->movementshape == 2 || mcon->movementshape == 3)
			{
				WidthMultiplier = mcon->PathWidth + 1;
				HeightMultiplier = mcon->PathHeight + 1;
			}
		else 
			{
				WidthMultiplier = mcon->width + 1;
				HeightMultiplier = mcon->height + 1;
			}
		
		int ExistDistanceX = (0x2 * TILE_SIZE) + (*StageZoom * 0x80) + (WidthMultiplier * TILE_SIZE) + (GetDrawDistance(mcon) * TILE_SIZE);
		int ExistDistanceY = (0x25 * TILE_SIZE) + (HeightMultiplier * TILE_SIZE) + (GetDrawDistance(mcon) * TILE_SIZE);
		
		if (GetClassID(mcon) == 0x12B) 
			{
				ExistDistanceX += 0x100;
				ExistDistanceY += 0x100;
			}
		
		if (!mode)
			{
				if (skip) return false;
				if ((xoffs > ExistDistanceX || xoffs < -ExistDistanceX) || (yoffs > ExistDistanceY || yoffs < -ExistDistanceY)) return true; 
				else return false; 
			}
		else 
			{
				if (skip) return true;
				if ((xoffs < ExistDistanceX && xoffs > -ExistDistanceX) && (yoffs < ExistDistanceY && yoffs > -ExistDistanceY)) return true; 
				else return false;
			}
	}
	
void UpdatePositions(MovementController* mcon, int i, bool forcepos)
	{
		if (!(mcon->settings->tinyoffset >> 3 & 1) || forcepos) mcon->ArrayOfPointers[i]->position = mcon->NextPosition[i].position;

		if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 6) & 3)) mcon->ArrayOfPointers[i]->rotation.x = mcon->NextPosition[i].rotation.x;
		if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 4) & 3)) mcon->ArrayOfPointers[i]->rotation.y = mcon->NextPosition[i].rotation.y;
		if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 2) & 3)) mcon->ArrayOfPointers[i]->rotation.z = mcon->NextPosition[i].rotation.z;
	}

void MovementController_CreateStuff(MovementController* mcon)
    {
		mcon->actorsdeleted = false; 
		
        for (int i = 0; i < mcon->spritecount; i++)
            {
                if (actorAlive(mcon, i))
                    {
                        mcon->ArrayOfPointers[i] = (EnemyClassObject*)createActor(GetClassID(mcon), mcon->settings->spriteData, &mcon->position, 0, 0, 0);
						
						if ((u32)mcon->ArrayOfPointers[i] == 0)				// Prevents crashing if createActor fails (which it can)
							{
								mcon->Dead |= 1<<i;
								continue;
							}

                        // Dumb fish try to spawn facing Mario
                        if (GetClassID(mcon) == 28)
                            {
                                mcon->ArrayOfPointers[i]->direction = 0;
                                mcon->ArrayOfPointers[i]->rotation.y = -0x4000;
                            }
						
						if (GetClassID(mcon) == 0x12B)		// If spawning another mcon, restore its dead members.
							{
								MovementController* AnotherMcon = (MovementController*)mcon->ArrayOfPointers[i];
								AnotherMcon->Dead = mcon->DeadMovconDeads[i];
								MovementController_DeleteStuff(AnotherMcon, true);
							}

						mcon->ArrayOfPointers[i]->EventFieldID1 = mcon->settings->event1;	
						mcon->ArrayOfPointers[i]->EventFieldID2 = mcon->settings->event2;

						mcon->ArrayOfPointers[i]->ExistDist.x = mcon->ExistDistance;		//
                        mcon->ArrayOfPointers[i]->ExistDist.y = mcon->ExistDistance;		// Ensure the spawned sprites will always be visible and won't ever despawn on their own as long as they're attached to the controller.
						mcon->ArrayOfPointers[i]->DrawDist.x = mcon->DrawDistance;			// Doesn't matter for performance, since we delete them if player gets out of range anyway.
                        mcon->ArrayOfPointers[i]->DrawDist.y = mcon->DrawDistance;			//
											
						UpdatePositions(mcon, i, true); 						// Has to run this instead of using the creating position, because sprites sometimes move themselves in the OnCreate. Forces position even if movement updating is off.
						
						if (GetClassID(mcon) == 0x54) mcon->ArrayOfActivePhysics[i] = mcon->ArrayOfPointers[i]->SpriteSpecificData1;
                        else mcon->ArrayOfActivePhysics[i] = (u32)mcon->ArrayOfPointers[i]->activephysics.DeadMaybe;
						mcon->ArrayOfVtables[i] = (u32*)mcon->ArrayOfPointers[i]->vtable;
                    }
            }
    }

void MovementController_DeleteStuff(MovementController* mcon, bool mode)
    {
	    // Mode 0 = deletes all things
	    // Mode 1 = deletes things that are supposed to be dead
		if (!mode) mcon->actorsdeleted = true;

        for (int i = 0; i < mcon->spritecount; i++)
            {
				if ((actorAlive(mcon, i)) && (!mode)) MovementController_DoDelete(mcon, i);
				else if (!(actorAlive(mcon, i)) && (mode)) MovementController_DoDelete(mcon, i);
            }
    }
	
void MovementController_DoDelete(MovementController* mcon, int i)
    {
		if (GetClassID(mcon) == 0x12B)			// If this mcon is controlling a mcon, delete the sprites of said mcon first.
			{
				MovementController* AnotherMcon = (MovementController*)mcon->ArrayOfPointers[i];
				mcon->DeadMovconDeads[i] = AnotherMcon->Dead;
				MovementController_DeleteStuff(AnotherMcon, false);
			}
		Base__deleteIt(mcon->ArrayOfPointers[i]);
    }

void MovementController_MoveStuff(MovementController* mcon)
    {
        if ((!mcon->EventFieldID1) || (eventIsActivated(mcon->EventFieldID1))) 
			{
				MovementController_PositionStuff(mcon);
				mcon->ticker++;
			}
		
        for (int i = 0; i < mcon->spritecount; i++)
            {
                if (actorAlive(mcon, i)) UpdatePositions(mcon, i, false);
            }
    }
	
void MovementController_PositionStuff(MovementController* mcon)
    {
        // Some constants you can adjust!
        const float SPEED_MULTIPLIER_CONSTANT = 1/800.0;
        const float SPEED_MULTIPLIER_SWAYING = 1/160.0;
        const float SPEED_MULTIPLIER_PATHS = 100;
        const float DELAY_MULTIPLIER = 20; // frames per unit
        const float TILE_SIZE = 0x10000;
        const float TAU = 6.283185307179586;

        bool isPath = mcon->movementshape == 2 || mcon->movementshape == 3;

        // The size of one unit is either one tile or half a tile,
        // depending on a setting
        float unitSize = TILE_SIZE;
        if (mcon->settings->tinyoffset & 4) unitSize /= 2;
		
        // This is essentially the logic used in NSMB Wii for Circling Boo Buddies.
        float missingMemberWeight;
        if ((mcon->settings->rotationAxes_noAccelFlag_gapFlag & 1) == 1)
			missingMemberWeight = 0.75 - (1.0 / mcon->spritecount);
        else
			missingMemberWeight = 0;
		
        // Get the speed nybble as a signed value, and
        // factor in the speed multiplier setting, too
        int speedSign = (mcon->speed >> 3) ? -1 : 1;
        double signedSpeed;
        if (isPath)
                signedSpeed = speedSign * SPEED_MULTIPLIER_PATHS / mcon->pathTime;
        else
            {
                signedSpeed = mcon->speed & 7;
                if (speedSign == -1) signedSpeed -= 8;
                signedSpeed *= mcon->settings->speedmultiplier + 1;
            }

        // Update the position of every sprite
        for (int i = 0; i < mcon->spritecount; i++)
            {	
				// Find our place in the loop at time 0.
				float loopPosition = mcon->startangle / 16.0;
				loopPosition += i / (mcon->spritecount + missingMemberWeight); // from 0.0 to 1.0		

                // Update our place in the loop based on the ticker
                if (mcon->movementtype == 0) // Constant speed
                    {
                        loopPosition += mcon->ticker * signedSpeed * SPEED_MULTIPLIER_CONSTANT;
                    }
                else // Swaying
                    {
                        // Old attempts; resulted in working but unreadable code:
                        // https://i.imgur.com/BnT5PQI.png  https://i.imgur.com/K9gXdPc.png

                        bool transition = !(mcon->swaystate & 1);
                        bool returning = mcon->swaystate >> 1 & 1;
                        int goal = ((speedSign == -1) ^ returning) ? -1 : 1;

                        float swayPos; // -1 to 1

                        if (transition)
                            {
                                swayPos = mcon->swayticker;
                                if (goal == -1) swayPos = -swayPos;

                                mcon->swayticker += abs(signedSpeed) * SPEED_MULTIPLIER_SWAYING;

                                if (fabs(mcon->swayticker) >= 1)
                                    {   // Switch states
                                        mcon->swaystate += 1;
                                        mcon->swayticker = 0;
                                    }
                            }
                        else // Delay
                            {
                                swayPos = goal;

                                mcon->swayticker += 1;

                                if (mcon->swayticker > (float)(mcon->delay) * DELAY_MULTIPLIER)
                                    {   // Switch states
                                        mcon->swaystate += 1;
                                        mcon->swayticker = -1;
                                    }
                            }

                        // Now adjust it for nice acceleration, if that's not disabled
                        if (!(mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 1) & 1)
                            swayPos = sin(swayPos * TAU/4);

                        // Finally, add to loopPosition.
                        float movementdistance = isPath ? 0.5 : (mcon->movementdistance / 15.0);
                        loopPosition += swayPos * movementdistance;
                        if (isPath) loopPosition += 0.5;
                    }

                // Ensure it's still between 0 and 1, without using fmod()
                // because compiling it adds an extra KB of code
                loopPosition -= (int)loopPosition;
                if (loopPosition < 0) loopPosition += 1;
                if (loopPosition > 0.999) loopPosition = 0.999;

                if (!(0 <= loopPosition && loopPosition < 1)) for(;;); // poor man's assert

                // Get the movement controller's effective position, which can be
                // offset by some nybbles. This will be adjusted to find our new
                // position.
                s32 newX = mcon->position.x + (mcon->settings->posoffsetx * TILE_SIZE);
                s32 newY = mcon->position.y + (mcon->settings->posoffsety * TILE_SIZE);
                if (mcon->settings->tinyoffset & 1) newX += TILE_SIZE / 2;     
                if (mcon->settings->tinyoffset & 2) newY += TILE_SIZE / 2; 

                // Adjust that to find our position in the shape we're tracing out
                switch (mcon->movementshape)
                    {
                        case 0:
							{
								float ringRadians = loopPosition * TAU;

								// Use sin for x and cos for y so that 0 is straight up and +angle is clockwise
								newX += mcon->width  * sin(ringRadians) * unitSize;
								newY += mcon->height * cos(ringRadians) * unitSize;
							}
                            break;
                        case 1:
							{
								newX += mcon->width * (loopPosition * 2 - 1) * unitSize;
								newY -= mcon->height * (loopPosition * 2 - 1) * (loopPosition * 2 - 1) * unitSize;
							}
                            break;
							
                        default:
							{
								u64 xy = TravelPercentageOfPath(mcon->pathID, (mcon->movementshape == 3),
									mcon->pathTime, loopPosition);
								newX = (xy >> 32) << 12;
								newY = (-(xy & 0xFFFFFFFF)) << 12;
							}
                            break;
                    }

                // Update the rotation so that we are facing in the direction
                // we're about to move in
                float actorRotOff = (mcon->settings->tinyoffset >> 4) * TAU / 16.0; // clockwise
                s16 rotation = (atan2(newY - mcon->NextPosition[i].position.y,
                                      newX - mcon->NextPosition[i].position.x)
                                      - actorRotOff + TAU/4) / TAU * 65536;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 6) & 3) == 2)
                    mcon->NextPosition[i].rotation.x = rotation;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 6) & 3) == 3)
                    mcon->NextPosition[i].rotation.x = -rotation;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 4) & 3) == 2)
                    mcon->NextPosition[i].rotation.y = rotation;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 4) & 3) == 3)
                    mcon->NextPosition[i].rotation.y = -rotation;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 2) & 3) == 2)
                    mcon->NextPosition[i].rotation.z = rotation;
                if (((mcon->settings->rotationAxes_noAccelFlag_gapFlag >> 2) & 3) == 3)
                    mcon->NextPosition[i].rotation.z = -rotation;

                // Truly update the position
                mcon->NextPosition[i].position.x = newX;
                mcon->NextPosition[i].position.y = newY;
                mcon->NextPosition[i].position.z = mcon->position.z;
            }
    }


TraverseCompletedPathSegments_ReturnValue* TraverseCompletedPathSegments(
            int pathID, bool looped, float totalTime, float percentage, bool quitEarly)
    {   // Sorry this has to exist, it saves a lot of compiled code

        PathEntry* path = getPath(pathID);

        // Find the initial velocity:
        // this is 0 if the path is not looped, or
        // the speed of the last node if it is.
        float incomingSpeed;
        if (looped)
            incomingSpeed = ((*nodesBlockPtr) + path->startNodeIdx \
                - startNodeAdjustment + path->nodeCount - 1)->speed;
        else
            incomingSpeed = 0;

        float targetTime = totalTime * percentage;

        // Quickly iterate over all the path segments already traversed
        float totalTimeSoFar = 0;
        float segmentLength = 1;
        float accelDistance = 0;
        int signedAccel = 0;
        bool withinAccelPart = true;
        PathNode *node1 = *nodesBlockPtr + path->startNodeIdx - startNodeAdjustment;
        PathNode *node2 = *nodesBlockPtr + path->startNodeIdx - startNodeAdjustment;
        for (int idx1 = 0; idx1 < path->nodeCount + (looped ? 0 : -1); idx1++)
            {
                // Calculate node indices, taking looping into account;
                // then get the nodes themselves
                int idx2;
                if (idx1 < path->nodeCount - 1)
                    idx2 = idx1 + 1;
                else
                    idx2 = 0;
                node1 = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment + idx1);
                node2 = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment + idx2);

                // Find the length of the segment
                int dx = node2->xPos - node1->xPos;
                int dy = node2->yPos - node1->yPos;
                segmentLength = sqrt(dx * dx + dy * dy);

                // ################ Accel/decel part

                signedAccel = node1->accel;
                if (signedAccel == 0) signedAccel = 1;
                if (node1->speed < incomingSpeed) signedAccel = -signedAccel;

                // Find the distance traveled during (ac|de)celeration
                accelDistance = (node1->speed * node1->speed - incomingSpeed * incomingSpeed) \
                    / (2 * signedAccel);

                // Crop that distance to the segment length if it exceeds that;
                // at the same time, find the final velocity of this part
                float accelFinalSpeed;
                if (accelDistance > segmentLength)
                    {
                        accelDistance = segmentLength;
                        accelFinalSpeed = sqrt(2 * signedAccel * accelDistance + incomingSpeed * incomingSpeed);
                    }
                else
                    {
                        accelFinalSpeed = node1->speed;
                    }

                // Finally, find the time it takes to travel that distance.
                float accelTime = 2 * accelDistance / (incomingSpeed + accelFinalSpeed);

                // ################ Constant-speed part

                // Find the distance traveled during this part
                float constantDistance = segmentLength - accelDistance;

                // Find the time taken during this part
                float constantTime = constantDistance / node1->speed;

                // ################ Putting those together

                withinAccelPart = true;
                if (quitEarly && totalTimeSoFar + accelTime > targetTime)
                    break;
                totalTimeSoFar += accelTime;
                withinAccelPart = false;
                if (quitEarly && totalTimeSoFar + constantTime > targetTime)
                    break;
                totalTimeSoFar += constantTime;
                incomingSpeed = node1->speed;
            }

        TraverseCompletedPathSegments_ReturnValue *returnValue = \
            allocFromCurrentHeap(sizeof (TraverseCompletedPathSegments_ReturnValue), 0);
        returnValue->targetTime = targetTime;
        returnValue->totalTimeSoFar = totalTimeSoFar;
        returnValue->incomingSpeed = incomingSpeed;
        returnValue->currentSegmentLength = segmentLength;
        returnValue->currentAccelDistance = accelDistance;
        returnValue->currentSignedAccel = signedAccel;
        returnValue->endedWithinAccelPart = withinAccelPart;
        returnValue->node1 = node1;
        returnValue->node2 = node2;
        return returnValue;
    }

float CalculatePathTime(int pathID, bool looped)
    {
        TraverseCompletedPathSegments_ReturnValue* stuff = \
            TraverseCompletedPathSegments(pathID, looped, 0, 0, false);

        float totalTime = stuff->totalTimeSoFar;

        freeToCurrentHeapIfNotNull(stuff);

        return totalTime;
    }

u64 TravelPercentageOfPath(int pathID, bool looped, float totalTime, float percentage)
    {
        TraverseCompletedPathSegments_ReturnValue* stuff = \
            TraverseCompletedPathSegments(pathID, looped, totalTime, percentage, true);

        float timeToTravelOnThisSegment = stuff->targetTime - stuff->totalTimeSoFar;

        float distance;
        if (stuff->endedWithinAccelPart)
            distance = stuff->incomingSpeed * timeToTravelOnThisSegment + \
                0.5 * stuff->currentSignedAccel * timeToTravelOnThisSegment * timeToTravelOnThisSegment;
        else
            distance = stuff->currentAccelDistance + stuff->node1->speed * timeToTravelOnThisSegment;

        float xVec = (stuff->node2->xPos - stuff->node1->xPos) / stuff->currentSegmentLength; // These form a
        float yVec = (stuff->node2->yPos - stuff->node1->yPos) / stuff->currentSegmentLength; // unit vector
        float x = stuff->node1->xPos + xVec * distance, y = stuff->node1->yPos + yVec * distance;

        freeToCurrentHeapIfNotNull(stuff);

        u64 x64 = x, y64 = y;
        return (x64 << 32) | y64;
    }


u64 PathSize(int pathID)
    {
        PathEntry* path = getPath(pathID);

        s16 minX = 0x7FFF, minY = 0x7FFF, maxX = 0x8000, maxY = 0x8000;
        for (int i = 0; i < path->nodeCount; i++)
            {
                PathNode* node = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment + i);
                if (node->xPos < minX) minX = node->xPos;
                else if (node->xPos > maxX) maxX = node->xPos;
                if (node->yPos < minY) minY = node->yPos;
                else if (node->yPos > maxY) maxY = node->yPos;
            }

        u64 w64 = (maxX - minX + 15) / 16, h64 = (maxY - minY + 15) / 16;
        return (w64 << 32) | h64;
    }