#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "TripleBlock.h"

u32 Tripleblock_physics[] =                                         
{
    0x18000,        
    0x8000, 
    0x18000,    
    0x8000, 
    0x2D00004, 
    0,
    0,
};          

s32 Tripleblock_solid_physics[] =                                               
{
    0,                                  // Top Right
    0x10000,                            // Bottom Left
    0x30000,                            // Top Right
    0,                                  // Bottom Right
    (u32)&CheckIfMarioGP,               // Top Callback
    (u32)&CheckIfMario,                 // Bottom Callback
    (u32)&CheckIfShell,                 // Side Callback
    0,  
    0,
    0,
};          

int Tripleblock_loadFiles()
{
    return 1;
}
    
void Tripleblock_setup(Tripleblock* trpl)
{   
    int fileoffset = getUnk3ForView(trpl->ViewID);
    
    if (fileoffset > 3) 
        fileoffset = 0;
    
    trpl->ModelFile = 2667-0x83 + fileoffset;

    loadFileByExtId_3dModel(trpl->ModelFile, 0);
    
    if ((trpl->spriteData >> 28) & 1) 
        Base__deleteIt(trpl);
    
    u8* Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(trpl->ModelFile));

    getTextureParams(Texture, 0, &trpl->Tex.texparam);
    getTexturePalBase(Texture, 0, &trpl->Tex.palbase);

    trpl->movetimer = trpl->hit = trpl->hitgp = 0;
    trpl->colsdead = 0;
    
    trpl->DrawDist.x = 0x80;
    trpl->DrawDist.y = 0x80;

    trpl->scale.x = 0x30000;
    trpl->scale.y = trpl->scale.z = 0x10000;
    trpl->timeshit = 0;
    
    if ((trpl->spriteData >> 16) & 1) trpl->position.x += 0x8000;       //Nybble 8
    if ((trpl->spriteData >> 16) & 2) trpl->position.y += 0x8000;       //Nybble 8

    trpl->position.y -= 0x10000;                                        // Box in NSMBe = left side of block
     
    Tripleblock_MakeCollisions(trpl);
}
    
void Tripleblock_MakeCollisions(Tripleblock* trpl)
{
    if (!trpl->hit)
    {
        initActivePhysics(&trpl->activephysics, trpl, Tripleblock_physics, 0);
        registerCollider(&trpl->activephysics);
    }
    
    InitSolidCollision(&trpl->solidcollision, trpl, Tripleblock_solid_physics, 0, 0, 0);
    registerSolidCollider(&trpl->solidcollision);       
}
    

void Tripleblock_DestroyCollisions(Tripleblock* trpl)
{
    UnregisterSolidCollision(&trpl->solidcollision);
    UnregisterActivePhysics(&trpl->activephysics);
}

    
bool CheckIfMarioIsInRange(Tripleblock* trpl)
{
    u16 *StageZoom = (u16*)(0x020CADB4);
    
    MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
    
    int xoffs = Player->xPos - trpl->position.x;
    int yoffs = Player->yPos - trpl->position.y;
    int Distance = (*StageZoom * 0x100);
    
    if (xoffs > Distance || xoffs < -Distance || yoffs < -0x100000 || yoffs > 0x300000)  
        return false;
    else 
        return true;
}

void Tripleblock_IdleState(Tripleblock* trpl)
{
    bool range = CheckIfMarioIsInRange(trpl);
    
    if (!range && (!trpl->colsdead)) 
    {
        Tripleblock_DestroyCollisions(trpl);
        trpl->colsdead = true;
    }
        
    if (range && (trpl->colsdead)) 
    {
        Tripleblock_MakeCollisions(trpl);
        trpl->colsdead = false;
    }
        
    if (!trpl->hit)
    {
        trpl->animtimer++;
        
        if (!(trpl->animtimer % 32)) 
        {
            if (trpl->timeshit) 
                trpl->timeshit++;
        }

        if (trpl->animtimer == 64) 
            trpl->animtimer = 0;
    }
}

void CheckIfMarioGP(Tripleblock* trpl, MarioActor* Player)
{
    if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->marioPowerupState == 3)) 
    {
        Tripleblock_Megashroom(trpl);
        return;
    }

    if (trpl->hit) 
        return;
    
    if (((u32)Player == (u32)getPtrToPlayerActor()) && (Player->IsMarioGroundPounding77E & 2))
    {
        trpl->hitgp = true;
        Tripleblock_SetBlockStateHit(trpl);
    }
    else 
        return;
}

void CheckIfMario(Tripleblock* trpl, MarioActor* Player)
{
    if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->marioPowerupState == 3)) 
    {
        Tripleblock_Megashroom(trpl);
        return;
    }   

    if (trpl->hit) 
        return;
    
    if ((u32)Player == (u32)getPtrToPlayerActor()) 
        Tripleblock_SetBlockStateHit(trpl);
    else 
        return;
}
    
void CheckIfShell(Tripleblock* trpl, EnemyClassObject* Enemy)
{
    MarioActor* Player = (MarioActor*)Enemy;
    
    if ((u32)Player == (u32)getPtrToPlayerActor() && (Player->marioPowerupState == 3)) 
    {
        Tripleblock_Megashroom(trpl);
        return;
    }
        
    if (trpl->hit) 
        return;
    
    if ((Enemy->ClassID != 94) && 
        (Enemy->ClassID != 95) && 
        (Enemy->ClassID != 54) && 
        (Enemy->ClassID != 40) && 
        (Enemy->activephysics.callback != (u32*)0x02099B6C)) 
        return; 
    else 
        Tripleblock_SetBlockStateHit(trpl);
}   
    
void Tripleblock_SetBlockStateHit(Tripleblock* trpl)
{       
    if ((!((trpl->spriteData >> 20) & 1)) || (trpl->timeshit > 8))
    {
        trpl->animtimer = 0;

        u8* Texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(trpl->ModelFile));
        getTextureParams(Texture, 1, &trpl->Tex.texparam);
        
        if (trpl->ModelFile == 2537) 
            getTexturePalBase(Texture, 0, &trpl->Tex.palbase);
        else 
            getTexturePalBase(Texture, 1, &trpl->Tex.palbase);
    }
    
    trpl->hit = true;
    trpl->state = 1;
    trpl->timeshit++;
    trpl->position.z = 0x2000;
    trpl->activephysics.callback = (u32*)0x020D0984;

    Vec3 Position;
    Position.x = trpl->position.x + 0x28000;
    Position.y = trpl->position.y;
    Position.z = 0;
    
    u32 mask = 0;
    if (trpl->hitgp) mask = 0x000C0000;
    
    for (int i = 0; i < 3; i++)
    {
        createActor(31, (((trpl->spriteData >> (4*i)) & 0xF) | mask), &Position, 0, 0, 0);

        if (((trpl->spriteData >> (4*i)) & 0xF) == 2) 
            PlaySNDEffect(SE_OBJ_GET_COIN, &Position);
        else 
            PlaySNDEffect(SE_OBJ_ITEM_APPEAR, &Position);
        
        Position.x -= 0x10000;
    }
}
    
void Tripleblock_StateBeingHit(Tripleblock* trpl)
{
    trpl->movetimer++;
    
    s8 gpa = 1;
    if (trpl->hitgp) gpa = -1;
    
    if (trpl->movetimer < 6) 
        trpl->position.y += (0x1000 * gpa); 
    else 
        trpl->position.y -= (0x1000 * gpa);
    
    if (trpl->movetimer == 10) 
    {
        trpl->state = 0;    
        trpl->movetimer = 0;
        
        if (((trpl->spriteData >> 20) & 1) && (trpl->timeshit < 10)) 
            trpl->hit = false;
        
        if ((!((trpl->spriteData >> 20) & 1)) || (trpl->timeshit > 9)) 
            UnregisterActivePhysics(&trpl->activephysics);
    }
}
    
void Tripleblock_Megashroom(Tripleblock* trpl)
{
    Vec3 Pos = trpl->position;
    
    int spritedata = 0x300000;
    if (trpl->hit) 
        spritedata = 0x600000;
    
    Pos.y += 0x10000;
    
    for (int i = 0; i < 3; i++)
    {
        createActor(0xF1, spritedata, &Pos, 0, 0, 0);
        Pos.x += 0x10000;
    }
        
    PlaySNDEffect(SE_OBJ_BLOCK_BREAK, &trpl->position); 
    PlaySNDEffect(SE_PLY_STAR_ATTACK, &trpl->position);
    IncreaseMegaMushroomDestructionCounter(0, 30);      
    Base__deleteIt(trpl);
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

void Tripleblock_draw(Tripleblock* trpl)
{
    NNS_G3dGeFlushBuffer();
    G3_LoadMtx43(0x02085B20);

    REG_MTX_TRANSLATE = trpl->position.x;
    REG_MTX_TRANSLATE = trpl->position.y;
    REG_MTX_TRANSLATE = trpl->position.z;   

    REG_MTX_SCALE = trpl->scale.x;
    REG_MTX_SCALE = trpl->scale.y;
    REG_MTX_SCALE = trpl->scale.z;
    
    REG_MTX_CONTROL = 3;
    REG_MTX_IDENTITY = 0;
    REG_MTX_CONTROL = 2;

    REG_POLY_FORMAT = 0x001F3880;
    REG_TEXT_FORMAT = trpl->Tex.texparam;
    REG_PAL_BASE = trpl->Tex.palbase;
    
    REG_COLOR= 0x7FFF;
    
    int framePos = 0;
    framePos = (trpl->animtimer / 8) * 0x100;
    
    REG_GFX_BEGIN = 1;
    texcoord2(0x200, framePos);
    vec3(0x1000,    0x1000,     0x0);
        
    texcoord2(0, framePos);
    vec3(0x0000,    0x1000,     0x0);
        
    texcoord2(0, framePos+0x100);
    vec3(0x0000,    0x0000,     0x0);
        
    texcoord2(0x200, framePos+0x100);
    vec3(0x1000,    0x0000,     0x0);

    REG_GFX_END = 0;    
}