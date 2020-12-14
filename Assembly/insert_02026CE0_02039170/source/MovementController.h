#include "Enemy.h"

extern u16 startNodeAdjustment;
extern PathEntry **pathsBlockPtr;
extern PathNode **nodesBlockPtr;
extern u32 *pathsCount;
extern u32 *nodesCount;

typedef struct
{
	// size: 0xF
	u16 classID;							// 0
	u8 amount;								// 2
	u8 ZPosition;							// 3
    u32 spriteData;							// 4
    u8 speedmultiplier;						// 8
	u8 unused;								// 9
	u8 rotationAxes_noAccelFlag_gapFlag;	// A
	u8 tinyoffset;							// B
    u8 posoffsetx;							// C
	u8 posoffsety;							// D
    u8 event2;								// E
	u8 event1;								// F
} MovementControllerSettings;

typedef struct
{
	// EnemyActor class stuff
	u32* vtable;
	
	u32 unk0;			// 4
	
	u32 spriteData;		// 8
	
	u32 ClassID;		// C
	u8 unk1[4];			// D
	
	Base base;			// 14
	
	u32 unk2[2];		// 54
	
	Vec3 position;		// 5C -- inited from createSprite_pos
	Vec3 lastPosition;	// 6C -- inited from createSprite_pos
	Vec3 unkv2;			// 7C -- inited to 0,0,0
	Vec3 unkv3;			// 8C -- seems to be used as a velocity vector?
	S16Vec3 rotation;	// 9C -- inited from createSprite_unk
	S16Vec3 unkv5;		// A8 -- inited from createSprite_unk
	
	u32 unk3[6];		// B4
	
	Vec3 velocity;		// CC -- velocity
	Vec3 unkv7;			// DC
	Vec3 scale;			// EC -- inited from createSprite_arg0
	Vec3 unkv9;			// FC -- inited to 0,0,0
	Vec3 unkv10;		// 10C -- inited to 0x8000,0x8000,0x8000
	
	u8 unk4;			// 11C -- inited to 0
	u8 unk5;			// 11D -- inited to 1
	u8 unk6;			// 11E -- inited from createSprite_arg4
	u8 unk7;			// 11F -- inited to 0x80
	
	// 120 -- ActivePhysics
	ActivePhysics activephysics;
	
	// 1D0 -- Collider
	Collider collider;
	
	u32 unk8[0x5];							// 2A8
	u8  unk9;								// 2BC
	u8  direction;  						// 2BD
	u16 unk10;								// 2BE
	
	u32 unk11[0x1D];						// 2C0

	u8 EventFieldID1;						// 334
	u8 EventFieldID2; 						// 335
	u16 unk12;								// 336
	u32 unk13;								// 338
	u8 EventField2ID2;						// 33C
	u8 EventField2ID1; 						// 33D
	u16 unk14;								// 33E
	u32 state;								// 340
	
	u32 unk15[0xA];							// 344
	Vec2 ExistDist;							// 36C
	Vec2 DrawDist;							// 378
	Vec2 Unk2Vec2;							// 384
	Vec2 Unk3Vec2;							// 390 vector copied here from ActivePhysics when a collision occurs
	u32 unk16[0x16];						// 39C

	MovementControllerSettings* settings;	// 3F4

	u8 delay;								// 3F8
	u8 width;
	u8 height;
	u8 pathID;
	u8 spritecount;						
	u8 movementtype;						// 3FC
	u8 movementshape;
	u8 Dead;								
	u8 DeadMovconDeads[15];					
	u8 swaystate;
	s8 speed;
	bool eventtriggered;
	bool actorsdeleted;		
	bool withinrange;
	
	u16 ExistDistance;						
	u16 DrawDistance;							
	u32 PathWidth;
	u32 PathHeight;
	u32 ticker;	

	float movementdistance;					
	float startangle;	
	float swayticker;						
	float pathTime;

	EnemyClassObject* ArrayOfPointers[15];	
	u32* ArrayOfVtables[15];				
	u32  ArrayOfActivePhysics[15];			
	PositionInfo NextPosition[15];			
} MovementController;


typedef struct
{
	float targetTime;
	float totalTimeSoFar;
	float incomingSpeed;
    float currentSegmentLength;
    float currentAccelDistance;
    int currentSignedAccel;
    bool endedWithinAccelPart;
    PathNode *node1;
    PathNode *node2;
} TraverseCompletedPathSegments_ReturnValue;


void MovementController_PositionStuff(MovementController*);
void MovementController_MoveStuff(MovementController*);
void MovementController_CreateStuff(MovementController*);
void MovementController_DeleteStuff(MovementController*, bool mode);
void MovementController_DoDelete(MovementController*, int i);
bool MovementController_CheckIfAnythingAlive(MovementController*);
bool MovementController_CheckIfSpriteAlive(MovementController*, int spritenumber);
bool MovementController_CheckIfWithinRange(MovementController* par, bool mode);
void UpdatePositions(MovementController* par, int spritenumber, bool forcepos);
float CalculatePathTime(int pathID, bool looped);
u64 TravelPercentageOfPath(int pathID, bool looped, float totalTime, float percentage);
u64 PathSize(int pathID);
