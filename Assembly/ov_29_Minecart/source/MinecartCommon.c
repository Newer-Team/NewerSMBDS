#include "game.h"
#include "registers.h"
#include "VariousFunctions.h"
#include "MinecartCommon.h"

extern int MinecartHP;


/*
s16 *sinCosLUT = (s16*)0x02080304; // 4192 pairs of s16's (one for cos, then one for sin)
                                   // that go all the way around the circle
static inline s16 NSMB_Sin(int angle)
    {   // Angle goes from 0 to 0x8000
        // Output is fx4.12
        angle = angle % 0x8000;
        int idx = (angle / 16) * 2;
        return *(sinCosLUT + idx);
    }
*/



int AdjustAngleToward(int currentAngle, int targetAngle, int delta, int direction)
    {   // This method nudges currentAngle toward targetAngle by delta.
        // This is harder than it sounds, because we're dealing with angles
        // where one full circle is 0x8000. So, for example, if the current angle
        // is 0x7FFE and the target angle is 0x0001, we should move it upwards
        // two units and let it wrap. In other words, we're operating in modulo
        // arithmetic, and need to account for that. (Otherwise loop-de-loops break.)
        // If direction == 0, the angle will be adjusted using whichever direction
        //     is shortest, as described above.
        // If direction == 1, the angle will be adjusted clockwise no matter what.
        // If direction == 2, the angle will be adjusted counterclockwise no matter what.

        // No point doing any math if current == target, right?
        if (currentAngle == targetAngle)
            return currentAngle;

        // First, find the minimum distance between current and target angles.
        // This would be 2 for the example above.
        // Code based on http://stackoverflow.com/a/6193318/4718769
        int rotationDifference = abs(targetAngle - currentAngle);
        int modRotationDifference = MIN(rotationDifference, 0x8000 - rotationDifference);

        int rotationAdjustmentSign;
        if (direction == 0) // Shortest rotation
            {
                // Now we determine if we should add or subtract modRotationDifference to/from
                // rotation.z to get it to targetRotation. We do this by... performing the
                // addition in modulo arithmetic and seeing if it matches the target, and if not,
                // assuming that the corresponding subtraction would work. There's probably a
                // better way to do this, but this works.
                if ((currentAngle + modRotationDifference) % 0x8000 == targetAngle)
                    rotationAdjustmentSign = 1;
                else
                    rotationAdjustmentSign = -1;
            }
        else if (direction == 1) // Force clockwise rotation
            {
                rotationAdjustmentSign = -1;
            }
        else                     // Force counterclockwise rotation
            {
                rotationAdjustmentSign = 1;
            }

        // Now we calculate how much to *actually* spin the cart by on this frame.
        // This is capped at modRotationDifference (the total distance we'd need to
        // change it by to reach the target). We don't want to overshoot, after all.
        int rotationAmount = delta;
        if (rotationAmount > modRotationDifference)
            rotationAmount = modRotationDifference;

        // And now we actually adjust the rotation.
        currentAngle += rotationAdjustmentSign * rotationAmount;

        // We have to make sure it remains in range, or else the modulo
        // math can have hard-to-debug issues.
        if (currentAngle < 0) currentAngle += 0x8000;
        if (currentAngle > 0x8000) currentAngle -= 0x8000;

        return currentAngle;
    }


bool CheckIfSolidRelativeToActor(EnemyClassObject* Actor, int TileXOffs, int TileYOffs)       // Returns true if solid
    {   // x/y units: 0x10 = a tile

        u32 xPos =  ( Actor->position.x  >> 12) + TileXOffs;
        u32 yPos = ((-Actor->position.y) >> 12) - TileYOffs;
        u32 colls = getTileBehaviorAtPos2(xPos, yPos);

        // Values are endian-reversed from the tile data displayed in NSMBe.

        if (colls & 0x00020000) // Coin tile
            return false;

        if (colls & 0x80000000) // Solid-on-top (semisolid)
            return false;
        
        if ((colls & 0x00600000) && ((colls & 0xF) != 0xA)) // Slope or upside-down slope, except slope edge.
            return false;

        // Other than those special cases, we'll say that any nonzero collision value is "solid."
        return (colls != 0);
    }


bool pathLooped(PathEntry* path)
    {
        PathNode* firstNode = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment);
        return firstNode->pad0E & 1;
    }


PathNode* getNextNode(PathNode* currentNode, PathEntry* path, bool goingBackwards)
    {   // Get the next node entry that comes after the one given.
        // Returns NULL if that goes beyond the end of the path.
        // (Looped paths will loop back to the beginning.)

        PathNode* firstNode = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment);
        PathNode* lastNode = firstNode + path->nodeCount - 1;
        bool looped = pathLooped(path);

        if (goingBackwards)
        {
            if (currentNode == firstNode)
                return looped ? lastNode : NULL;
            else
                return currentNode - 1;
        }
        else
        {
            if (currentNode == lastNode)
                return looped ? firstNode : NULL;
            else
                return currentNode + 1;
        }
            
    }


void MinecartCommon_Setup(GenericMinecartInfo* info, u32 spriteData)
    {
        info->isSilent = 1;
        info->WheelSnd = 0;
        info->Slam = 0;
        info->brakingPower = 0;
        info->jumpMode = -1;
        info->landingBounceCounter = -1;
        info->intendedAngle = 0;
        info->doneFirstBounce = 0;
        info->doneSecondBounce = 0;
        if ((spriteData >> 20) & 4) info->HP = MinecartHP;  // If you're immediately put into the cart, use the stored HP.
        else                                                // else, get full HP.
            {
                info->HP = 1;
                MinecartHP = 1;
            }
        info->smoke = 0;
        info->blinking = 0;
        info->targetRotation = 0xFFFFFFFF;
        info->wheelRot = RNG(0xFFFF);
        info->secondWheelRotAddition = RNG(0xFFFF);

        int initialNode = (spriteData & 0xFF);           // Nybbles 11-12
        int initialPathID = ((spriteData >> 8) & 0xFF);  // Nybbles 9-10
        info->speedNybble = ((spriteData >> 16) & 0xF);  // Nybble 8
        info->goingBackwards = ((spriteData >> 20) & 1); // (Nybble 7) & 1

        // Prepare the path
        startNodeAdjustment = getNodeIndexAdjustmentValue();
        info->currentPath = getPath(initialPathID);
        info->currentNode = (*nodesBlockPtr) + info->currentPath->startNodeIdx - startNodeAdjustment + initialNode;
        info->nextNode = getNextNode(info->currentNode, info->currentPath, info->goingBackwards);
        info->nodeDistTraveled = 0;
    }


PathNode* MinecartCommon_MoveOnGround(GenericMinecartInfo* info, Vec3* position, Vec3* velocity, S16Vec3* rotation)
    {   // Return value:
        //     If the cart remains on the path, the PathNode* that it's currently on
        //     If it's fallen off the end, NULL

        PathNode* returnNode = info->currentNode;

        // Update speed
        int rawSpeed = (int)info->speedNybble * 0x1000 + (s16)info->currentNode->delay;
        info->speed = (rawSpeed * BRAKING_POWER_DIVISOR) / (info->brakingPower + BRAKING_POWER_DIVISOR);

       if (!info->isSilent)
            {
                PlaySNDEffect2(4, 0);
            }

        // Move by speed, and spin the wheels relative to speed
        info->nodeDistTraveled += info->speed;
        if (info->goingBackwards)
            info->wheelRot += info->speed / 0x10;
        else
            info->wheelRot -= info->speed / 0x10;

        // Find the length of the current path segment
        PathNode *node1 = info->currentNode, *node2 = info->nextNode;
        int dx = node2->xPos - node1->xPos;
        int dy = node2->yPos - node1->yPos;
        int segmentLength = sqrt32(dx * dx + dy * dy);

        // If we finished this segment, start the next one
        if (info->nodeDistTraveled > (segmentLength * 0x1000))
            {
                PathNode *node3 = getNextNode(node2, info->currentPath, info->goingBackwards);

                if (node3 == NULL)
                    {
                        returnNode = NULL;
                    }
                else
                    {
                        node1 = node2;
                        node2 = node3;

                        returnNode = node1;

                        info->nodeDistTraveled -= segmentLength * 0x1000;
                        info->currentNode = node1;
                        info->nextNode = node2;
                        info->targetRotation = 0xFFFFFFFF;

                        // And now we need to recalc segment length.
                        dx = node2->xPos - node1->xPos;
                        dy = node2->yPos - node1->yPos;
                        segmentLength = sqrt32(dx * dx + dy * dy);
                    }
            }

        // Account for small post-jump bouncing if necessary.
        // Units of additionalHeight: 0x10000 is a tile.
        // Units of additionalAngle: 0x8000 is a circle.
        int additionalHeight = 0, additionalAngle = 0;
        if (info->landingBounceCounter >= 0)
            {
                const int FIRST_BOUNCE_LEN = 20;
                const int SECOND_BOUNCE_LEN = 8;
                const int FIRST_BOUNCE_HEIGHT = 8;
                const int SECOND_BOUNCE_HEIGHT = 4;

                info->landingBounceCounter++;
                if (info->landingBounceCounter < FIRST_BOUNCE_LEN)
                    {
                        if (!info->doneFirstBounce)
                            {
                                PlaySNDEffect(13, position);
                                info->doneFirstBounce = true;
                            }

                        int animPos = info->landingBounceCounter * 16384 / FIRST_BOUNCE_LEN;
                        additionalHeight = sinLerp(animPos) * FIRST_BOUNCE_HEIGHT;
                        additionalAngle = -(animPos - 8192) / 8;
                    }
                else if (info->landingBounceCounter - FIRST_BOUNCE_LEN < SECOND_BOUNCE_LEN)
                    {
                        if (!info->doneSecondBounce)
                            {
                                PlaySNDEffect(13, position);
                                info->doneSecondBounce = true;
                            }
                       int animPos = (info->landingBounceCounter - FIRST_BOUNCE_LEN) * 16384 / SECOND_BOUNCE_LEN;
                        additionalHeight = sinLerp(animPos) * SECOND_BOUNCE_HEIGHT;
                        additionalAngle = -(animPos - 8192) / 8;
                    }
                else
                    info->landingBounceCounter = -1;
            }

        // Cause the cart to rumble a little, and also
        // shake a bit if it's broken
        int additionalX = 0;
        if (!info->HP)
            {
                additionalX = RNG(0x4000) - 0x2000;
            }
        additionalHeight += RNG(0x1000) - 0x800;

        // Actually find the new position
        int xVec = ((node2->xPos - node1->xPos) * 0x1000) / segmentLength; // These form a
        int yVec = ((node2->yPos - node1->yPos) * 0x1000) / segmentLength; // unit vector (unit = 0x1000)
        int x = ((node1->xPos * 0x1000) + ((xVec * (info->nodeDistTraveled / 0x100)) / 0x10)),
            y = -((node1->yPos * 0x1000) + ((yVec * (info->nodeDistTraveled / 0x100)) / 0x10));
        info->intendedX = x;
        info->intendedY = y;
        position->x = x + additionalX;
        position->y = y + additionalHeight;
        velocity->x = (xVec * info->speed) / 0x1000;
        velocity->y = (yVec * info->speed) / 0x1000;

        // And the new rotation: will always be between 0 and 0x8000
        if (info->targetRotation == 0xFFFFFFFF)
            {
                if (info->goingBackwards)
                    info->targetRotation = FX_Atan2Idx(yVec * 0x10, -xVec * 0x10) / 2;
                else
                    info->targetRotation = FX_Atan2Idx(-yVec * 0x10, xVec * 0x10) / 2;
            }

        // The mini-bounces after a jump use absolute angles, so we
        // can't just do "rotation.z += whatever" every frame. So instead, we
        // do "info->intendedAngle += whatever" every frame, followed by
        // "rotation->z = info->intendedAngle + additionalAngle".
        // This lets us nudge the angle little by little per frame, and also lets
        // us adjust the actual angle by arbitrary amounts each frame, too.

        // However, if we just landed after a jump, we should instantaneously
        // snap to the angle of the track (or else we'll mess up the bounce animation,
        // which would be bad). We can check for this by seeing if info.landingBounceCounter
        // == 1.

        // Spin speed is directly proportional to the cart's speed (for aesthetic reasons)
        int angleDelta;
        if (info->currentNode->pad0E & 2) // Fast rotation mode
            angleDelta = info->speed / 0x10;
        else
            angleDelta = info->speed / 0x20;

        if (info->landingBounceCounter == 1)
            info->intendedAngle = info->targetRotation;
        else
            info->intendedAngle = AdjustAngleToward(info->intendedAngle, info->targetRotation, angleDelta, 0);

        // And now tack on the angle for the bounce animation and we're done!
        rotation->z = info->intendedAngle + additionalAngle;

        return returnNode;
    }


bool MinecartCommon_InitiateJump(GenericMinecartInfo* info,
            Vec3* position, Vec3* velocity, bool enemyJump, bool fallingOffEdge)
    {   // Returns true if the jump happened, or false if it was cancelled
        const int JUMP_Y_SPEED = 0x7000;

        info->doneFirstBounce = 0;
        info->doneSecondBounce = 0;

        PathNode *currentNode = info->goingBackwards ? info->currentNode - 1 : info->currentNode;
        if (fallingOffEdge || enemyJump)
            info->jumpMode = 0;
        else
            info->jumpMode = currentNode->speed;
        int xSpeed = 0, ySpeed = 0;

        // Find the current speed.
        if (fallingOffEdge)
            {
                xSpeed = velocity->x;
                ySpeed = velocity->y;
            }
        else
            {
                int groundSpeed = info->goingBackwards ? -info->speed : info->speed;

                switch (info->jumpMode)
                    {
                        case 0: // Normal jump
                            {
                                xSpeed = groundSpeed; // An older version of this code took only the
                                                      // x-component of this; however, it felt
                                                      // unexpectedly odd when jumping on steep slopes
                                                      // (because your x-velocity was small, even though
                                                      // you were going fast, you would have a very slow
                                                      // jump). Taking the entire speed and putting it
                                                      // into x-speed feels more natural in-game.
                                ySpeed = JUMP_Y_SPEED; // Similarly, this used to be the y-component of
                                                       // speed plus a constant. However, you would pop
                                                       // up super-high when going uphill, and barely
                                                       // jump at all when going downhill. This works
                                                       // better in all cases.
                                position->y += 0x1000; // This is so you don't immediately land on
                                                       // the piece of track you just jumped off of.
                                break;
                            }

                        case 1: // Jump disabled
                            {
                                // TODO: Make a sound effect or something.
                                return false; // Return early, i.e. don't perform jump
                            }

                        case 2: // Jump off wall
                        case 4: // Jump off wall and flip upright
                        case 5: // Jump off wall and flip left
                        case 6: // Jump off wall and flip right
                            {
                                // Find the vector representing the direction of the current path segment
                                int dx = info->nextNode->xPos - info->currentNode->xPos;
                                int dy = (-info->nextNode->yPos) - (-info->currentNode->yPos); // positive = upwards
                                int segmentLength = sqrt32(dx * dx + dy * dy);

                                // Use that to set our current x/y speed
                                s64 temp = dx; temp *= info->speed; temp /= segmentLength;
                                xSpeed = temp;
                                temp = dy; temp *= info->speed; temp /= segmentLength;
                                ySpeed = temp;

                                // Rotate it 90 deg counterclockwise, and make it into a unit vector
                                // (fixed-point 20.12). This new vector is normal to the track.
                                int dx_rot, dy_rot;
                                if (info->goingBackwards)
                                    {
                                        dx_rot = dy; dy_rot = -dx;
                                    }
                                else
                                    {
                                        dx_rot = -dy; dy_rot = dx;
                                    }
                                int xVec = (dx_rot * 0x1000) / segmentLength;
                                int yVec = (dy_rot * 0x1000) / segmentLength;

                                // Add this, times a constant, to the speed
                                xSpeed += xVec * 5;
                                ySpeed += yVec * 5;

                                info->spinClockwiseDuringWallOrCeilingJump = (ySpeed > 0) ^ (!info->goingBackwards);

                                info->goingBackwards = !info->goingBackwards;
                                break;
                            }

                        case 3: // Jump off ceiling and flip upright
                            {
                                xSpeed = -groundSpeed;
                                ySpeed = -JUMP_Y_SPEED;
                                info->spinClockwiseDuringWallOrCeilingJump = (xSpeed > 0);
                                info->goingBackwards = !info->goingBackwards;
                                break;
                            }
                    }
            }
        velocity->x = xSpeed;
        velocity->y = ySpeed;
        info->isAirborne = true;
        info->currentPath = NULL;
        info->currentNode = NULL;
        info->nextNode = NULL;
        info->targetRotation = 0xFFFFFFFF;
        return true;
    }


// The following three functions (doLineSegmentsIntersect, crossProduct, allEqual) have
// been translated from JavaScript to C by RoadrunnerWMC. Original can be found at
// https://github.com/pgkelley4/line-segments-intersect

// Header from original:
// * @author Peter Kelley
// * @author pgkelley4@gmail.com

int whereDoLineSegmentsIntersect(int p1x, int p1y, int p2x, int p2y, int q1x, int q1y, int q2x, int q2y)
    {   /*  See if two line segments intersect. This uses the
            vector cross product approach described below:
            http://stackoverflow.com/a/565282/786339

            Returns -1 if the segments do not intersect; if they do,
            returns the distance from p1 to the intersection point.

            (p1x, p1y) and (p2x, p2y) form the first line segment;
            (q1x, q1y) and (q2x, q2y) form the second.
        */

        s64 rx = p2x - p1x, ry = p2y - p1y;
        s64 sx = q2x - q1x, sy = q2y - q1y;

        s64 uNumerator = crossProduct(q1x - p1x, q1y - p1y, rx, ry);
        s64 denominator = crossProduct(rx, ry, sx, sy);

        if (uNumerator == 0 && denominator == 0)
            {
                // They are colinear

                // Do they touch? (Are any of the points equal?)
                if (p1x == q1x && p1y == q1y) return 0;
                if (p1x == q2x && p1y == q2y) return 0;
                if (p2x == q1x && p2y == q1y) return sqrt64(rx * rx + ry * ry);
                if (p2x == q2x && p2y == q2y) return sqrt64(rx * rx + ry * ry);

                // Do they overlap? (Are all the point differences in either direction the same sign)
                // I don't know what non-(-1) value to return in this case.
                // So let's return 0! :P
                if (!allEqual(
                        (q1x - p1x < 0),
                        (q1x - p2x < 0),
                        (q2x - p1x < 0),
                        (q2x - p2x < 0))
                    || !allEqual(
                        (q1y - p1y < 0),
                        (q1y - p2y < 0),
                        (q2y - p1y < 0),
                        (q2y - p2y < 0)))
                    return 0;
                return -1;
            }

        if (denominator == 0)
            // lines are parallel
            return -1;

        // Fixed-point math, 52.12
        s64 u = (uNumerator * 0x1000) / denominator;
        s64 t = (crossProduct(q1x - p1x, q1y - p1y, sx, sy) * 0x1000) / denominator;
        if ((t >= 0) && (t <= 0x1000) && (u >= 0) && (u <= 0x1000))
            return (t * sqrt64(rx * rx + ry * ry)) / 0x1000;

        return -1;
    }


s64 crossProduct(s64 x1, s64 y1, s64 x2, s64 y2)
    {   // Calculate the cross product of the two points.
        return x1 * y2 - x2 * y1;
    }


bool allEqual(int a1, int a2, int a3, int a4)
    {   // Returns true if all arguments are equal.
        if (a2 != a1) return false;
        if (a3 != a1) return false;
        if (a4 != a1) return false;
        return true;
    }


bool MinecartCommon_CheckForLanding(GenericMinecartInfo* info, Vec3* position, int oldX, int oldY)
    {
        int x1 = oldX, y1 = oldY;
        int x2 = position->x, y2 = position->y;

        for (int pathI = 0; pathI < *pathsCount; pathI++)
            {
                PathEntry *path = (*pathsBlockPtr) + pathI;
                PathNode* firstNode = (*nodesBlockPtr) + (path->startNodeIdx - startNodeAdjustment);

                if ((firstNode->pad0E & MINECART_PATH_MAGIC) != MINECART_PATH_MAGIC) continue;

                // Initialize node1 and node2 like this because they're incremented
                // at the beginning of the loop
                PathNode* node1 = firstNode - 1;
                PathNode* node2 = firstNode;

                for (int nodeI = 0; nodeI < path->nodeCount; nodeI++)
                    {
                        // Increment node1 and node2
                        node1 = node2;
                        node2 = getNextNode(node2, path, false);

                        // Don't go past the end of an unlooped path
                        if (node2 == NULL) break;

                        int n1x = node1->xPos * 0x1000;
                        int n1y = -node1->yPos * 0x1000;
                        int n2x = node2->xPos * 0x1000;
                        int n2y = -node2->yPos * 0x1000;

                        // Check for obvious out-of-range's
                        if (x1 > n1x && x1 > n2x && x2 > n1x && x2 > n2x) continue;
                        if (x1 < n1x && x1 < n2x && x2 < n1x && x2 < n2x) continue;
                        if (y1 > n1y && y1 > n2y && y2 > n1y && y2 > n2y) continue;
                        if (y1 < n1y && y1 < n2y && y2 < n1y && y2 < n2y) continue;

                        // Prevent jumping onto upside-down track segments
                        if (n2x < n1x) continue;

                        // OK, the line segments might cross. Let's see.
                        // http://stackoverflow.com/a/565282/4718769
                        // https://github.com/pgkelley4/line-segments-intersect
                        int intersectDistance = whereDoLineSegmentsIntersect(
                            n1x, n1y, n2x, n2y,
                            x1, y1, x2, y2);

                        if (intersectDistance != -1)
                            {
                                // We crossed paths with a path segment. Before we declare a
                                // landing, though, we need to check for the direction in
                                // which the lines cross. This prevents us from "landing" when
                                // jumping through the bottom of a track segment.

                                s64 rx = x2 - x1, ry = y2 - y1;
                                s64 sx = n2x - n1x, sy = n2y - n1y;
                                s64 cp = crossProduct(rx, ry, sx, sy);

                                if (cp > 0)
                                    {   // We actually landed! :D
                                        info->currentPath = path;
                                        info->targetRotation = 0xFFFFFFFF;
                                        info->isLanding = true;
                                        info->isAirborne = false;
                                        info->jumpMode = -1;
                                        info->landingBounceCounter = 0;

                                        if (info->goingBackwards)
                                            {   // Measure distance from the next node instead.
                                                s64 dx = n2x - n1x, dy = n2y - n1y;
                                                int segmentLength = sqrt64(dx * dx + dy * dy);

                                                // Swap currentNode and nextNode
                                                info->currentNode = node2;
                                                info->nextNode = node1;
                                                info->nodeDistTraveled = segmentLength - intersectDistance;
                                            }
                                        else
                                            {
                                                info->currentNode = node1;
                                                info->nextNode = node2;
                                                info->nodeDistTraveled = intersectDistance;
                                            }
                                        return true;
                                    }
                            }
                    }
            }
        return false;
    }


bool MinecartCommon_MoveDuringJump(GenericMinecartInfo* info,
        Vec3* position, Vec3* velocity, S16Vec3* rotation, bool jumpJustLetGo)
    {   // Returns true if the cart has landed, false otherwise

        const int FALL_SPEED = 0x400;
        const int UNPRESS_SPEED_LOSS = 0x2000;
        const int TERMINAL_FALL_SPEED = 0x6000;
        const int SPIN_INTENSITY_DIVISOR = 10; // larger values = spins less

        int oldX = position->x, oldY = position->y;

        velocity->y -= FALL_SPEED;
        if (velocity->y < -TERMINAL_FALL_SPEED)
            velocity->y = -TERMINAL_FALL_SPEED;

        position->x += velocity->x;
        position->y += velocity->y;
        info->intendedX = position->x;
        info->intendedY = position->y;

        if (info->jumpMode == 0) // Regular jump
            {
                // Variable-height jump code based on the following:
                // http://gamedev.stackexchange.com/a/29618
                if (jumpJustLetGo && velocity->y > 0)
                    velocity->y -= UNPRESS_SPEED_LOSS;

                rotation->z = velocity->y / SPIN_INTENSITY_DIVISOR;
                info->intendedAngle = rotation->z;
                info->intendedY = position->y;

                if (MinecartCommon_CheckForLanding(info, position, oldX, oldY))
                    return true;
            }
        else if (info->jumpMode == 2) // Jump off wall
            {
                rotation->z = FX_Atan2Idx(-velocity->x, velocity->y) / 2;
            }
        else if (   info->jumpMode == 3  // Jump off ceiling and flip upright
                 || info->jumpMode == 4  // Jump off wall and flip upright
                 || info->jumpMode == 5  // Jump off wall and flip left
                 || info->jumpMode == 6) // Jump off wall and flip right
            {
                const int SPIN_SPEED = 0x300;

                int targetRotation = 0;
                if (info->jumpMode == 5)
                    targetRotation = 0x2000;
                else if (info->jumpMode == 6)
                    targetRotation = 0x6000;

                rotation->z = AdjustAngleToward(rotation->z, targetRotation, SPIN_SPEED,
                    info->spinClockwiseDuringWallOrCeilingJump ? 1 : 2);

                info->intendedAngle = rotation->z;
                info->intendedY = position->y;

                if (MinecartCommon_CheckForLanding(info, position, oldX, oldY))
                    return true;
            }
        return false;
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


void DrawGenericMinecartPoly(TexInfo Texture,
        Vec3 position, int SizeX, int SizeY,
        Vec3 Scale, Vec3 Translate1, Vec3 Translate2,
        Vec3 Offset, int RotaAngle1, int RotaAngle2,
        bool flippedX, bool flippedY)
    {
            NNS_G3dGeFlushBuffer();
            G3_LoadMtx43(0x02085B20);

            REG_MTX_TRANSLATE = position.x;
            REG_MTX_TRANSLATE = position.y;

            if (!RotaAngle2) REG_MTX_TRANSLATE = position.z; else REG_MTX_TRANSLATE = position.z + 0x1000;

            glTranslatef32(Translate1.x, Translate1.y, Translate1.z);
            glRotateZi(RotaAngle1);
            glTranslatef32(-Translate1.x + Offset.x, -Translate1.y + Offset.y, -Translate1.z + Offset.z);

            if (RotaAngle2)
                {
                    glTranslatef32(Translate2.x, Translate2.y, Translate2.z);
                    glRotateZi(RotaAngle2);
                    glTranslatef32(-Translate2.x, -Translate2.y, -Translate2.z);
                }

            REG_MTX_SCALE = Scale.x;
            REG_MTX_SCALE = Scale.y;
            REG_MTX_SCALE = Scale.z;

            REG_MTX_CONTROL = 3;
            REG_MTX_IDENTITY = 0;
            REG_MTX_CONTROL = 2;

            REG_POLY_FORMAT = 0x001F3880;
            REG_TEXT_FORMAT = Texture.texparam;
            REG_PAL_BASE = Texture.palbase;

            REG_COLOR = 0x7FFF;

            int X1 = flippedX ? SizeX : 0;
            int X2 = flippedX ? 0 : SizeX;
            int Y1 = flippedY ? SizeY : 0;
            int Y2 = flippedY ? 0 : SizeY;

            REG_GFX_BEGIN = 1;
            texcoord2(X2, Y1);
            vec3(0x1000,    0x1000,     0x0);

            texcoord2(X1, Y1);
            vec3(0x0000,    0x1000,     0x0);

            texcoord2(X1, Y2);
            vec3(0x0000,    0x0000,     0x0);

            texcoord2(X2, Y2);
            vec3(0x1000,    0x0000,     0x0);

            REG_GFX_END = 0;
    }