#include "Clampy.h"
#include "game.h"
#include "sfx.h"

#define CLAMPY_BOTTOM_ID        2708 - 0x83
#define CLAMPY_TOP_ID           2709 - 0x83


bool Clampy_loadFiles()
{
	loadFileByExtId_3dModel(CLAMPY_BOTTOM_ID, 0);
	loadFileByExtId_3dModel(CLAMPY_TOP_ID, 0);
	return true;
}

bool Clampy_heapCreated(Clampy* clam)
{
	model3d_ctor(&clam->TopModel);
    model3d_ctor(&clam->BottomModel);
	return true;
}

void Clampy_setup(Clampy* clam)
{
    model3d_setup(&clam->BottomModel, getPtrToLoadedFile(CLAMPY_BOTTOM_ID), 0);
    model3d_setup(&clam->TopModel, getPtrToLoadedFile(CLAMPY_TOP_ID), 0);
    
	clam->scale.x = 0x18000;
    clam->scale.y = 0x18000;
    clam->scale.z = 0x18000;
    clam->MouthRotation.x = clam->rotation.x;
    clam->MouthRotation.y = clam->rotation.y;
    clam->MouthRotation.z = clam->rotation.z;
    clam->MouthPosition.x = clam->position.x;
    clam->MouthPosition.y = clam->position.y - 0x4000;
    clam->MouthPosition.z = clam->position.z;   
    
    
    s32 Solid_physics[] = 												
    {
        -0x18000,						    // Top Right
        0x10000,							// Bottom Left
        0x20000,			                // Top Left
        0,                                  // Bottom Right
        0,				                    // Top Callback
        0, 									// Bottom Callback
        0,									// Side Callback
        0,	
        0,
        0,
    };	

    s32 Solid_physics2[] = 												
    {
        -0x18000,						    // Top Right
        0x20000,							// Bottom Left
        0x20000,			                // Top Left
        0x10000,                            // Bottom Right
        (u32)&TopCollision,						// Top Callback
        0, 									// Bottom Callback
        0,									// Side Callback
        0,	
        0,
        0,
    };	    
					
    InitSolidCollision(&clam->solidcollision, clam, Solid_physics, 0, 0, 0);
    registerSolidCollider(&clam->solidcollision);
    InitSolidCollision(&clam->solidcollision2, clam, Solid_physics2, 0, 0, 0);
    registerSolidCollider(&clam->solidcollision2);    
    
    u32 Eat_physics[] = 										// Physics stick out a bit so Mario can actually touch the hurtbox
    {
        0x4000,		
        0x10000,	
        0x18000,	
        0x10000,	
        0x910004, 
        0x0,
       (u32)&EatCollision,
    };	
    
    initActivePhysics(&clam->activephysics, clam, Eat_physics, 0);
    
    clam->MouthRotation.x = 0x4000;
    
    clam->StateTimer = 0;
}

void Clampy_IdleState(Clampy* clam)
{
    if (clam->HasEaten)
        return;
    
    if (clam->OnGround)
    {
        if (clam->StateTimer < 100)
        {
            clam->StateTimer++;
        }
        else
        {
            PlaySNDEffect(12, &clam->position);
            clam->StateTimer = 0;
            clam->state = 1;
        }
    }
}

void Clampy_MouthOpenState(Clampy* clam)
{

    if (clam->StateTimer < 20)
    {
        clam->MouthPosition.y -= 0x200;
        clam->MouthRotation.x -= 0x200;
    }
    else if (clam->StateTimer < 300)
    {
    }

    else if (clam->StateTimer < 305)
    {
        clam->MouthRotation.x += 0x200;
    }   
    
    else if (clam->StateTimer < 310)
    {
        clam->MouthRotation.x -= 0x200;
    }    
    
    else if (clam->StateTimer < 320)
    {
    }      
    
    else if (clam->StateTimer < 330)
    {
        clam->MouthPosition.y += 0x400;
        clam->MouthRotation.x += 0x400; 
    }       
    
   
    if (clam->StateTimer == 21)
        UnregisterSolidCollision(&clam->solidcollision2);    
    
    
    
    if (clam->StateTimer == 325)
    {
        PlaySNDEffect(13, &clam->position);
        registerCollider(&clam->activephysics);
    }
    
    
    if (clam->StateTimer == 330)
    {
        UnregisterActivePhysics(&clam->activephysics);
        registerSolidCollider(&clam->solidcollision2);   
        clam->StateTimer = -1;
        clam->state = 0;
    }
    
    clam->StateTimer++;
}

void TopCollision(Clampy* clam, MarioActor* Player)
	{
        if ((u32)Player != (u32)getPtrToPlayerActor())
            return;
        
        if (clam->state == 1 && clam->StateTimer < 20)
            Player->yAccel += 0x6000;
	}
    
void EatCollision(ActivePhysics* a, ActivePhysics* b)
	{
		Clampy* clam = (Clampy*) a->owner;
		MarioActor* Mario = (MarioActor*) b->owner;
		
		if (Mario->ClassID == 21) 
		{
            clam->HasEaten = true;
            PlaySNDEffect(73, &clam->position);
			Mario->Visible = false;
            Mario->DeathState = 0x2119B24;
		}
		return;
	}

void Clampy_AllStates(Clampy* clam)
{
    u32 xPos = (clam->position.x >> 12); 
    u32 yPos = ((-clam->position.y) >> 12);
    int tilebehaviour = getTileBehaviorAtPos2(xPos, yPos);
    
    if ((!(tilebehaviour >> 0x10 & 0x2)) && (tilebehaviour))
        clam->OnGround = true;
    
    if (!clam->OnGround)
    {
        clam->position.y -= 0x1000;
        clam->MouthPosition.y -= 0x1000;
        clam->solidcollision.top -= 0x1000;
        clam->solidcollision.bottom -= 0x1000;
        clam->solidcollision2.top -= 0x1000;
        clam->solidcollision2.bottom -= 0x1000;
    }
}

bool Clampy_beforeDraw(Clampy* clam)
{
	return true;
}

void G3_LoadMtx43(u32 unk);

void Clampy_onDraw(Clampy* clam)
{
	DrawAModel(&clam->TopModel, &clam->MouthPosition, &clam->MouthRotation, &clam->scale);
    DrawAModel(&clam->BottomModel, &clam->position, &clam->rotation, &clam->scale);
}

void* Clampy_dtor(Clampy* clam)
    {
        clam->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0,0,0};
        Vec2 dtor2 = {0,0};

        clam->Unk4Vec2 = dtor2;
        clam->Unk3Vec2 = dtor2;
        clam->DeleteDist = dtor2;
        clam->DrawDist = dtor2;
        clam->ExistDist = dtor2;
        clam->unkv13 = dtor;
        clam->unkv12 = dtor;
        clam->unkv11 = dtor;
        
        UnregisterSolidCollision(&clam->solidcollision);
        UnregisterSolidCollision(&clam->solidcollision2);
        UnregisterActivePhysics(&clam->activephysics);
        
        return clam;
    }

void* Clampy_dtorFree(Clampy* clam)
    {
        Clampy_dtor(clam);
		freeToGameHeap(clam);
        return clam;
    }

