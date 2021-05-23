#include <nds.h>
#include <math.h>

#include "PrincessPeach.h"
#include "game.h"
#include "sfx.h"
#include "registers.h"
#include "VariousFunctions.h"

#define PEACH_MODEL_ID          1594 - 0x83
#define PEACH_ANIMATION_ID      1593 - 0x83
#define PEACH_ANIMATION_IDLE_ID 1595 - 0x83
#define PEACH_CAGE_ID           2707 - 0x83

extern u32 bmgFlagSomething;
extern u32 isTimeStopped;
extern bool ControlLock;
extern bool IsSpecialLevel;
extern bool TimerDisabled;
bool* MenuLocked = (bool*)0x020CA880;


bool PeachCage_loadFiles()
{
    loadFileByExtId_3dModel(PEACH_MODEL_ID, 0);
    loadFileByExtId_3dModel(PEACH_ANIMATION_ID, 0);
    loadFileByExtId_3dModel(PEACH_ANIMATION_IDLE_ID, 0);
    loadFileByExtId_3dModel(PEACH_CAGE_ID, 0);
    return true;
}

bool PeachCage_heapCreated(PeachCage* pc)
{
    modelAnim3d_ctor(&pc->model);
    return true;
}

void PeachCage_setup(PeachCage* pc)
{
    if (pc->spriteData & 0x1)
        modelAnim3d_setup(&pc->model, getPtrToLoadedFile(PEACH_MODEL_ID), getPtrToLoadedFile(PEACH_ANIMATION_IDLE_ID), 0, 0, 0);
    else
    {
        modelAnim3d_setup(&pc->model, getPtrToLoadedFile(PEACH_MODEL_ID), getPtrToLoadedFile(PEACH_ANIMATION_ID), 0, 0, 0);
        
        void* texture = NNS_G3dGetTex(getPtrToLoadedFile(PEACH_CAGE_ID));
        getTextureParams(texture, 0, &pc->cageFront.texparam);
        getTexturePalBase(texture, 0, &pc->cageFront.palbase);
        getTextureParams(texture, 1, &pc->cageBack.texparam);
        getTexturePalBase(texture, 1, &pc->cageBack.palbase);
        *MenuLocked = true;
        bmgFlagSomething |= 0x1;
        isTimeStopped |= 0x10;

        // Reload sound effects (to undo swapping for the Bowser battle)
        LoadMusicFile(0x6F, 0xFF);
        NNS_SndArcLoadSeq(0x56, *((u32**)(0x208FBB8)));
        NNSi_SndArcLoadGroup(0x21, *((u32**)(0x208FBB8)));
    }

    if (pc->spriteData & 0x1)
        pc->position.y -= 0x10000;
    
    pc->position.x += 0x8000;
    pc->CagePos = pc->position;
    pc->Mario = (MarioActor*)getPtrToPlayerActor();
    pc->state = 0;
    pc->fade = 0;

    if (pc->spriteData & 0x1)
    {
        modelAnim3d_init(&pc->model, 2, 0, 0x1000, 0);
        pc->rotation.y = 0xD800;
    }
    else
    {
        modelAnim3d_init(&pc->model, 0, 0, 0x1000, 0);
        pc->rotation.y = 0xA000;
    }
}

void PeachCage_IdleState(PeachCage* pc)
{
    if (eventIsActivated(1))
    {
        pc->timer++;
        
        if (pc->timer == 1)
        {
            pc->Mario->xVelocity = 0;
            pc->Mario->xAccel = 0;
            pc->Mario->RotY = 0x3000;
            pressedKeys = 0;
            SetPlayerAnimation(pc->Mario, 0, 1, 0, 0x1000); 
            pc->Mario->unk980 = (u32)&MarioActor_CutsceneState;
            ControlLock = true;
        }
        
        if (pc->timer == 60)
        {
            pc->timer = 0;
            pc->state = 1;
        }
    }
}

void PeachCage_DoEndingState(PeachCage* pc)
{
    pc->timer++;
    int *player = (int*)(0x02085A50);
    
    EnemyClassObject* mA = (EnemyClassObject*)pc->Mario;
    u16 *StageZoom = (u16*)(0x020CADB4);
    
    if (pc->timer == 1)
    {
        StopMusicWithFade(0x1E);
        StartMusicNumber(0x6F);
        
        PlaySNDEffect(SE_EMY_WANWAN_GACHI, &pc->position);
        
        pc->Camera = (EnemyClassObject*)createActor(0x146, 0, &mA->position, 0, 0, 0);
    }
    
    if (pc->timer == 5)
    {
        pc->rotation.y = -0x3000;
        modelAnim3d_init(&pc->model, 6, 0x40000000, 0x1000, 0);
    }
    
    if (pc->timer == 50)
    {
        SetPlayerAnimation(mA, 0x8A, 0x40000000, 0, 0x1000);
        modelAnim3d_init(&pc->model, 7, 0, 0x1000, 0);
    }
    
    if ((pc->timer > 50) && (pc->timer < 212))
        pc->position.y -= 0x800;
    
    if (pc->timer == 210)
        modelAnim3d_init(&pc->model, 8, 0x40000000, 0x1000, 0);
    
    if (pc->timer == 230)
    {
        modelAnim3d_init(&pc->model, 9, 0, 0x1000, 0);
        SetPlayerAnimation(mA, 1, 1, 0, 0x1000);    
    }
    
    if ((pc->timer > 230) && (pc->timer < 300))
    {
        *StageZoom-=0x1A;
        if (mA->position.x < pc->position.x - 0x12000)
        {
            mA->position.x+=0x1000;
            pc->Camera->position.x += 0x1400;
        }
    }
    
    if (pc->timer == 300)
    {
        mA->position.x = pc->position.x - 0x12000;
        SetPlayerAnimation(mA, 0, 1, 0, 0x1000);    
    }
    
    if (pc->timer == 330)
    {
        if (*player)
            PlaySNDEffect(SE_VOC_LU_WIN, &mA->position);
        else
            PlaySNDEffect(SE_VOC_MA_WIN, &mA->position);
        SetPlayerAnimation(mA, 0x8B, 0x40000000, 0, 0x1000);    
    }

    if (pc->timer == 490)
    {
        if ((pc->Mario->marioPowerupState == 0) || (pc->Mario->marioPowerupState == 4)) 
            modelAnim3d_init(&pc->model, 11, 0x40000000, 0x1000, 0);
        else
            modelAnim3d_init(&pc->model, 10, 0x40000000, 0x1000, 0);
    }   
    
    if (pc->timer == 565)
    {
        if (*player)
            PlaySNDEffect(SE_VOC_L_HOEE, &mA->position);
        else
            PlaySNDEffect(SE_VOC_M_HOEE, &mA->position);
        SetPlayerAnimation(pc->Mario, 0x87, 1, 0, 0x1000);
    }
    
    if (pc->timer > 720)
    {
        if (pc->fade > -16) setBrightness(3, pc->fade--);
        else
        {
            setBrightness(3, -16);
            exitLevel(1);
        }
    }
}

void PeachCage_AllStates(PeachCage* pc)
{
    anim3d_update(&pc->model.anim);
}

bool PeachCage_beforeDraw(PeachCage* pc)
{
    return true;
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

void PeachCage_onDraw(PeachCage* pc)
{
    DrawAModel(&pc->model, &pc->position, &pc->rotation, &pc->scale);

    if ((pc->spriteData & 0x1) == false)
    {
        NNS_G3dGeFlushBuffer();
        G3_LoadMtx43(0x02085B20);

        REG_MTX_TRANSLATE = pc->CagePos.x - 0x20000;
        REG_MTX_TRANSLATE = pc->CagePos.y - 0x6000;
        REG_MTX_TRANSLATE = pc->CagePos.z + 0x20000;

        REG_MTX_SCALE = 0x10000;
        REG_MTX_SCALE = 0x10000;
        REG_MTX_SCALE = 0x10000;

        REG_MTX_CONTROL = 3;
        REG_MTX_IDENTITY = 0;
        REG_MTX_CONTROL = 2;

        REG_POLY_FORMAT = 0x001F3880;
        REG_TEXT_FORMAT = pc->cageFront.texparam;
        (*(vu32*) 0x40004AC) = pc->cageFront.palbase;

        REG_COLOR = 0x7FFF;

        REG_GFX_BEGIN = 1;

        texcoord2(0x400, 0x100);
        vec3(0x4000,    0x7000,     0x0);

        texcoord2(0, 0x100);
        vec3(0x0000,    0x7000,     0x0);

        texcoord2(0, 0x800);
        vec3(0x0000,    0x0000,     0x0);

        texcoord2(0x400, 0x800);
        vec3(0x4000,    0x0000,     0x0);

        REG_GFX_END = 0;


        NNS_G3dGeFlushBuffer();
        G3_LoadMtx43(0x02085B20);

        REG_MTX_TRANSLATE = pc->CagePos.x - 0x4000;
        REG_MTX_TRANSLATE = pc->CagePos.y - 0x4B000;
        REG_MTX_TRANSLATE = pc->CagePos.z - 0x20000;

        REG_MTX_SCALE = 0x10000;
        REG_MTX_SCALE = 0x21000;
        REG_MTX_SCALE = 0x10000;

        REG_MTX_CONTROL = 3;
        REG_MTX_IDENTITY = 0;
        REG_MTX_CONTROL = 2;

        REG_POLY_FORMAT = 0x001F3880;
        REG_TEXT_FORMAT = pc->cageBack.texparam;
        (*(vu32*) 0x40004AC) = pc->cageBack.palbase;

        REG_COLOR = 0x7FFF;

        REG_GFX_BEGIN = 1;

        texcoord2(0x80, 0x0);
        vec3(0x0800,    0x4000,     0x0);

        texcoord2(0, 0x0);
        vec3(0x0000,    0x4000,     0x0);

        texcoord2(0, 0x800);
        vec3(0x0000,    0x0000,     0x0);

        texcoord2(0x080, 0x800);
        vec3(0x0800,    0x0000,     0x0);

        REG_GFX_END = 0;
    }
}

void MarioActor_CutsceneState(MarioActor* Mario)
{
    UpdatePlayerAnimation(Mario);
    return;
}

void* PeachCage_dtor(PeachCage* pc)
{
    pc->vtable = (u32*)0x20C4F04;

    Vec3 dtor = {0,0,0};
    Vec2 dtor2 = {0,0};

    pc->Unk4Vec2 = dtor2;
    pc->Unk3Vec2 = dtor2;
    pc->DeleteDist = dtor2;
    pc->DrawDist = dtor2;
    pc->ExistDist = dtor2;
    pc->unkv13 = dtor;
    pc->unkv12 = dtor;
    pc->unkv11 = dtor;
    
    bmgFlagSomething &= ~0x1;
    isTimeStopped &= ~0x10;
    
    freeToCurrentHeapIfNotNull(pc->BottomNSCPtr);
    *MenuLocked = false;
    ControlLock = false;
    
    return pc;
}

void* PeachCage_dtorFree(PeachCage* pc)
{
    PeachCage_dtor(pc);
    freeToGameHeap(pc);
    return pc;
}

