#ifndef FINALBOSS_H
#define FINALBOSS_H

#include "game.h"
#include "Enemy.h"

typedef struct
{
   u32* vtable;
    
    u32 unk0;           // 4
    
    u32 spriteData;     // 8
    
    u16 ClassID;        // C
    u8 unk1[6];         // D
    
    Base base;          // 14
    
    u32 unk2[2];        // 54
    
    Vec3 position;      // 5C -- inited from createSprite_pos
    Vec3 lastPosition;  // 6C -- inited from createSprite_pos
    Vec3 unkv2;         // 7C -- inited to 0,0,0
    Vec3 unkv3;         // 8C -- seems to be used as a velocity vector?
    S16Vec3 rotation;   // 9C -- inited from createSprite_unk
    S16Vec3 unkv5;      // A8 -- inited from createSprite_unk
    
    u32 unk3[6];        // B4
    
    Vec3 velocity;      // CC -- velocity
    Vec3 unkv7;         // DC
    Vec3 scale;         // EC -- inited from createSprite_arg0
    Vec3 unkv9;         // FC -- inited to 0,0,0
    Vec3 unkv10;        // 10C -- inited to 0x8000,0x8000,0x8000
    
    u8 unk4;            // 11C -- inited to 0
    u8 unk5;            // 11D -- inited to 1
    u8 unk6;            // 11E -- inited from createSprite_arg4
    u8 unk7;            // 11F -- inited to 0x80
    
    // 120 -- ActivePhysics
    ActivePhysics activephysics;
    
    // 1D0 -- Collider
    Collider collider;
    
    u32 unk8[0x5];                      // 2A8
    u8  unk9;                           // 2BC
    u8  direction;                      // 2BD
    u16 ViewID;                         // 2BE
    
    u32 unk11[0x13];                    // 2C0
    
    Vec3 unkv11;                        // 30C
    Vec3 unkv12;                        // 31C

    u32 unkm12[0x2];                    // 32C
    
    u8 EventFieldID2;                   // 334
    u8 EventFieldID1;                   // 335
    u16 unk12;                          // 336
    u32 unk13;                          // 338
    u8 EventField2ID2;                  // 33C
    u8 EventField2ID1;                  // 33D
    u16 unk14;                          // 33E
    u32 unused;                         // 340
    
    u32 unk15[0x6];                     // 344
    
    Vec3 unkv13;                        // 35C

    Vec2 ExistDist;                     // 36C
    Vec2 DrawDist;                      // 378
    Vec2 DeleteDist;                    // 384
    Vec2 Unk3Vec2;                      // 390 vector copied here from ActivePhysics when a collision occurs
    Vec2 Unk4Vec2;                      // 39C
    
    u32  unk16[0x11];                   // 3A8
    u8   unk17;                         // 3EC
    u8   unk18;                         // 3ED
    u8   unk19;                         // 3EE
    u8   unk20;                         // 3EF
    u8   unk21;                         // 3F0
    
    // Models
    ModelAnim3d koopaModel;             // 3F4
    ModelAnim3d copterModel;            // 49C

    u32 state;                          // 544
    u32 action;                         // 548  "sub state"

    u32 timer;                          // 54C  general-purpose timer
    u32 timer2;                         // 550  2nd general-purpose timer
    
    u32 baseline;                       // 554  default y position
    u32 yPos;                           // 558  used for storing the vertical position while bobbing up and down

    // Flags
    u8 updateAnimation;                 // 55C
    u8 playerLookAtBowser;              // 55D  (actually used as a bool)
    u8 jrState;                         // 55E
        // The values of jrState are not obvious at all, yet are crucial to
        // understand when debugging certain types of bugs. So here's exactly
        // what every possible value of jrState means, and which sprite (either
        // the final boss sprite or Bowser Jr) updates jrState to each value.
        // Note: these are just my findings based on analyzing the existing code,
        // which wasn't written by me. Maybe take with a grain of salt.
        // 0: idle
        //     This value is set by the FB sprite upon spawning BJR
        // 1: playing laugh animation
        //     This value is set by the FB sprite to trigger the anim
        // 2: attacking
        //     This value is set by the FB sprite when it's time for BJR to wake up
        // 3: hiding in shell
        //     This value is set by Bowser Jr when he takes damage
        // 4: dead
        //     This value is set by Bowser Jr when he dies
        // FF: dead
        //     This value is set by the FB sprite when the bullets-and-fireballs segment begins
        // (yes, I realize 4 and FF are redundant. Nobody said this code was good.)
    u8 phase;                           // 55F

    u32 battleMinX;                     // 560
    u32 battleMaxX;                     // 564

    s32 swoopA;                         // 568 (fixed-point 4.28)
    s32 swoopB;                         // 56C (fixed-point 4.28)
    s32 swoopC;                         // 570 (integer -- NOT fixed-point)
    u32 swoopX;                         // 574
    u32 swoopY;                         // 578
    s32 swoopMaxSpeed;                  // 57C
    u32 swoopDirection;                 // 580
    u32 lastVelocityX;
    u32 mode;
    u8 swoopOverBLine;
    u8 liftUp;
    u8  pad[2];
    void* bowserJr;
    
    u32 actionChangeTime;
    u32 wigglePosX;
    u32 timer3;                         // yup, we need another one :P

    u32 camMaxX;
    u32 killLine;
    u32 followLine;
    
    bool started;
    bool dead;
    u8 copterSoundState;
    int hitMarioTimer;
    int endtimer;
    int musictimer;
    
    u32 BGPos1;
    u32 BGPos2;
    
    bool limitbounds;
    u32 boundsmiddle;
    
    MarioActor* Mario;
    ActivePhysics activephysics2;
} FinalBoss;

typedef struct
{
    u16 x;
    u16 y;
    u16 width;
    u16 height;
    u8 id;
    u8 camId;
    u8 music;
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 lighting;
    u8 progressPathId;
} View;

typedef struct
{
    u16 x;
    u16 y;
    u16 w;
    u16 h;
    u8 id;
    u8 pad[3];
} Zone;

enum FinalBoss_States
{
    FinalBoss_Intro = 0,
    FinalBoss_Swoop = 1,
    FinalBoss_SpawnBulletBills = 2,
    FinalBoss_Follow = 3,
    FinalBoss_SpawnFire = 4,
    FinalBoss_ChaseIntro = 5,
    FinalBoss_Stomp = 6
};

void FinalBoss_sideCallback(FinalBoss* fb, MarioActor* ma);
void FinalBoss_marioCollision(FinalBoss* fb, MarioActor* ma);
void FinalBoss_collisionCallback(ActivePhysics* a, ActivePhysics* b);

void FinalBoss_begin_Intro(FinalBoss* fb);
void FinalBoss_update_Intro(FinalBoss* fb);

void FinalBoss_begin_Swoop(FinalBoss* fb, u32 mode);
void FinalBoss_update_Swoop(FinalBoss* fb);

void FinalBoss_begin_SpawnBulletBills(FinalBoss* fb);
void FinalBoss_update_SpawnBulletBills(FinalBoss* fb);

void FinalBoss_begin_Follow(FinalBoss* fb, u32 mode);
void FinalBoss_update_Follow(FinalBoss* fb);

void FinalBoss_begin_SpawnFire(FinalBoss* fb);
void FinalBoss_update_SpawnFire(FinalBoss* fb);

void FinalBoss_begin_ChaseIntro(FinalBoss* fb);
void FinalBoss_update_ChaseIntro(FinalBoss* fb);

void FinalBoss_begin_Stomp(FinalBoss* fb);
void FinalBoss_update_Stomp(FinalBoss* fb);

void FinalBoss_transitionToNextSegment(FinalBoss* fb);

void freezeWorld(bool freeze);

#endif  // FINALBOSS_H
