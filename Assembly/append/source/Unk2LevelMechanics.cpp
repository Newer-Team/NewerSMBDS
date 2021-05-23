#include "game.h"
#include "sfx.h"
#include "VariousFunctions.h"

u8 *PlayerID = (u8*)(0x2085A50);
u8 *storedpowerup = (u8*)(0x0208B32C);

int **PtrToBlock1 = (int**)(0x208B168);
int *JumpHeight = (int*)(0x212E2D8);

bool *touchedflagpole = (bool*)(0x20CA2C1);
bool *disablestorage = (bool*)(0x0208B098);
extern bool Frozen;
bool *MenuOpen = (bool*)(0x020CA870);

int ticker = 0;
int lightningtimer = 0;
int storedpos = 0;
int currentW8TowerBrightness = 0;

bool cantjump = false;
bool isjumping = false;
bool doneinitialthunderstrike = false;

extern int MinecartHP;

void DoWindstorm(MarioActor* Player);
void DoSML2(MarioActor* Player);

bool JumpPressed()
{
    if(controlOptionsA)
        return (pressedKeys & A) != 0 || (pressedKeys & B) != 0;
    else
        return (pressedKeys & A) != 0 || (pressedKeys & X) != 0;
}

void UpdateKeys()
{
    cantjump = isjumping;
    isjumping = JumpPressed();
}

bool jumpKeyPressed()
{   
    return isjumping && !cantjump;
}

void hook_020A2CC8()                                                        // Hook in Stage Execute. 
{
    MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
    
    UpdateKeys();
    
    if (((*((*PtrToBlock1))>>16)&0xF) == 2)                             //Fixes Mini Mushroom jump height in space.
    {
        if (Player->marioPowerupState == 4)
        {
            if (*PlayerID == 0) 
                *JumpHeight = 0x4280;
            else 
                *JumpHeight = 0x4610;
        }
        else
        {
            if (*PlayerID == 0) 
                *JumpHeight = 0x3A80;
            else 
                *JumpHeight = 0x4010;
        }   
    }

    switch(getUnk2ForView(Player->CurrentViewID2BE))
    {
        case 1: DoSML2(Player); break;                              // Mario Land 2 Space Physics
        case 2: DoWindstorm(Player); break;                         // Windstorm Type 1
        case 3: DoWindstorm(Player); break;                         // Windstorm Type 2
        default: break;
    }
}
    
void DoSML2(MarioActor* Player)
{
    if(jumpKeyPressed() && !Player->isOnGround && !(Player->IsMarioGroundPounding77E&2) && (Frozen == 0)) 
    {
        if (Player->marioPowerupState == 4) 
        {
            PlaySNDEffect(SE_PLY_JUMP_3, 0);
            Player->yAccel=0x3100;
        }
        else 
        {
            PlaySNDEffect(SE_PLY_JUMP_2, 0);
            Player->yAccel=0x2900;
        }
    }
    else 
        return;
}
                    
void DoWindstorm(MarioActor* Player)
{
    if ((Frozen == 0) && (*touchedflagpole == 0))
    {
        ticker++;
        if (Player->NotInPipeCannonB9F == 1)
        {
            if (ticker < 240)
            {
                PlaySNDEffect2(SE_AMB_TORNADO, 0);
                
                if (RNG(20) < 2) 
                    Rumble(1);

                if (Player->xAccel > 0) 
                    storedpos = Player->xPos;

                if (Player->xAccel < 0 && (pressedKeys & DOWN) != 0)
                {
                    Player->xPos = storedpos-(-1*(Player->xAccel)/5);
                    storedpos = Player->xPos;
                }

                if (Player->xAccel < 0 && (pressedKeys & DOWN) != 0)
                {
                    Player->xPos = storedpos-(-1*(Player->xAccel)/5);
                    storedpos = Player->xPos;
                }

                if ((!(pressedKeys & DOWN)) || (((!Player->isOnGround)) && !(Player->IsMarioGroundPounding77E&2)) || (Player->marioPowerupState == 3))
                {
                    if (ticker<=32) 
                        Player->xPos = Player->xPos+(0xC60+(ticker*0x80));
                    else if (ticker>=208) 
                        Player->xPos = Player->xPos+(0x2560-(ticker*16));
                    else  
                        Player->xPos = Player->xPos+0x2560;
                    
                    storedpos = Player->xPos;
                }
                if (((Player->xAccel >= 0 && (pressedKeys & DOWN) != 0)) || Player->IsMarioGroundPounding77E&2)
                {
                    Player->xPos = Player->xPos;
                    storedpos = Player->xPos;
                }               
            }
            else 
                storedpos = Player->xPos;   
                
            if (ticker==480) ticker = 0;
                return;
        }
        if (ticker==480) 
            ticker = 0;
    }
}

void hook_020A2ED0()                                                // Hook in Stage Draw. Darkness setting.
{
    int CurrentView = ((*(getPtrToPlayerActor()+0xAF)>>16)&0xF);
    
    int Unk2 = getUnk2ForView(CurrentView);
    
    if (Unk2 < 4) 
        return;
    
    if ((Unk2 == 4) && (getUnk1ForView(CurrentView) == 0))
    {
        setBrightness(1,0);
        return;
    }
    
    if (((u8)REG_MASTER_BRIGHT < (getUnk1ForView((CurrentView)))) &&  !(*MenuOpen))
    {
        setBrightness(1,-(((u8)REG_MASTER_BRIGHT)+1));  
    }
    if (((u8)REG_MASTER_BRIGHT) != 0)
    {
        if (*MenuOpen)
        {
            setBrightness(1,-(((u8)REG_MASTER_BRIGHT)-1));  
        }
    }
        
    if ((Unk2 == 5) ||( Unk2 == 7))                     // Lightning
    {
        if (lightningtimer) 
            lightningtimer--;
        
        if (((RandomNumberGenerator() % 451 == 1) && !(*MenuOpen) && !lightningtimer) || (Frozen == 0 && !doneinitialthunderstrike))
        {
            setBrightness(1,13);
            if (Unk2 != 7)   
                PlaySNDEffect(SE_AMB_THUNDER, 0);   
            else
                PlaySNDEffect(SE_PLY_CANNON_READY, 0);                          //For final boss.
            
            lightningtimer = 10;
            doneinitialthunderstrike = true;
        }
        
        if ((lightningtimer == 0) && !(*MenuOpen))  
            setBrightness(1,-getUnk1ForView(CurrentView));
        
        if ((!*MenuOpen) && (Frozen == 0)) 
            PlaySNDEffect2(SE_PLY_KISS, 0);         // Rain.
    }
        
    if (Unk2 == 6)                      // Fading light
    {           
        if (!(*MenuOpen))
        {
            const float TOWER_BOTTOM = 0xFF120000;
            const float TOWER_TOP = 0xFF780000;
            const float TILE_SIZE = 0x10000;
            const float BACKTRACK_BUFFER = TILE_SIZE * 6;
            
            int BRIGHTNESS_BOTTOM = -getUnk1ForView(CurrentView);
            int BRIGHTNESS_TOP = BRIGHTNESS_BOTTOM-10;

            MarioActor* mA = (MarioActor*)getPtrToPlayerActor();
            float y = mA->yPos;
            float progress;
            int proposedBrightness;

          // Attempt to darken due to forward progress
            progress = (y - TOWER_BOTTOM) / (TOWER_TOP - TOWER_BOTTOM);
            proposedBrightness = (int)(BRIGHTNESS_BOTTOM + (BRIGHTNESS_TOP - BRIGHTNESS_BOTTOM) * progress);
            if (proposedBrightness < currentW8TowerBrightness)
            {
                setBrightness(1, proposedBrightness);
                currentW8TowerBrightness = proposedBrightness;
            }
                
            // Attempt to lighten due to backward progress
            progress = ((y + BACKTRACK_BUFFER) - TOWER_BOTTOM) / (TOWER_TOP - TOWER_BOTTOM);
            proposedBrightness = (int)(BRIGHTNESS_BOTTOM + (BRIGHTNESS_TOP - BRIGHTNESS_BOTTOM) * progress);
            
            if (proposedBrightness > currentW8TowerBrightness)
            {
                setBrightness(1, proposedBrightness);
                currentW8TowerBrightness = proposedBrightness;
            }
                
            if (((u8)REG_MASTER_BRIGHT) > currentW8TowerBrightness) 
                setBrightness(1,currentW8TowerBrightness);
        }
    }
    
    return;
}
void hook_020A134C()                                        // Hook in Stage Scene dtor
{   
    doneinitialthunderstrike = false;
    MinecartHP = 1;
    ticker = 0;
}

void hook_020CC31C_ov_04()                                  // Hook in GameOverScene onCreate
{
    setBrightness(1, 0);
}
