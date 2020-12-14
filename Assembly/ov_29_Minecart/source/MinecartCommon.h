#include "Enemy.h"


#ifndef MINECART_COMMON_H
#define MINECART_COMMON_H

#define MIN(a,b) ((a)<(b)?(a):(b))

//#define MINECART_PATH_MAGIC 17229
#define MINECART_PATH_MAGIC 64 // rip meaningful magic
// note: the above constant must have the least-significant bit clear,
// as that's used to mark looped paths

#define MAX_BRAKING_POWER 100
#define BRAKING_POWER_DIVISOR 50
// larger numbers for the divisor = less braking power

extern u16 startNodeAdjustment;
extern PathEntry **pathsBlockPtr;
extern PathNode **nodesBlockPtr;
extern u32 *pathsCount;
extern u32 *nodesCount;


typedef struct
{
    TexInfo cartTex;
    TexInfo wheelTex;
    
    PathEntry* currentPath;
    PathNode* currentNode;
    PathNode* nextNode;

    s16 intendedAngle;
    int intendedX;
    int intendedY;

    int targetRotation;
    int brakingPower;
    int landingBounceCounter;

    bool isAirborne;
    bool isLanding;
    bool isDead;
    bool doneFirstBounce;
    bool doneSecondBounce;
    bool goingBackwards;
    bool spinClockwiseDuringWallOrCeilingJump;
	bool isSilent;

    s16 wheelRot;
    s16 secondWheelRotAddition;
    s8 jumpMode;
    u8 speedNybble;

    // All of the following are fixed-point: 20.12
    int gravity;
    int nodeDistTraveled;
    int speed;
    
    // things to get rid of
    int Slam;
    int WheelSnd;
    
    int HP;
    int blinking;
    int smoke;
	

} GenericMinecartInfo;


int AdjustAngleToward(int currentAngle, int targetAngle, int delta, int direction);
bool CheckIfSolidRelativeToActor(EnemyClassObject* Actor, int TileXOffs, int TileYOffs);
bool pathLooped(PathEntry* path);
PathNode* getNextNode(PathNode* currentNode, PathEntry* currentPath, bool goingBackwards);

void MinecartCommon_Setup(GenericMinecartInfo* info, u32 spriteData);

PathNode* MinecartCommon_MoveOnGround(GenericMinecartInfo* info, Vec3* position, Vec3* velocity, S16Vec3* rotation);
bool MinecartCommon_InitiateJump(GenericMinecartInfo* info,
        Vec3* position, Vec3* velocity, bool enemyJump, bool fallingOffEdge);

int whereDoLineSegmentsIntersect(int p1x, int p1y, int p2x, int p2y, int q1x, int q1y, int q2x, int q2y);
s64 crossProduct(s64 x1, s64 y1, s64 x2, s64 y2);
bool allEqual(int a1, int a2, int a3, int a4);

bool MinecartCommon_CheckForLanding(GenericMinecartInfo* info, Vec3* position, int oldX, int oldY);
bool MinecartCommon_MoveDuringJump(GenericMinecartInfo* info,
        Vec3* position, Vec3* velocity, S16Vec3* rotation, bool jumpJustLetGo);

void G3_LoadMtx43(u32 unk);
void DrawGenericMinecartPoly(TexInfo Texture,
        Vec3 position, int SizeX, int SizeY,
        Vec3 Scale, Vec3 Translate1, Vec3 Translate2,
        Vec3 Offset, int RotaAngle1, int RotaAngle2,
        bool flippedX, bool flippedY);

#endif // MINECART_COMMON_H